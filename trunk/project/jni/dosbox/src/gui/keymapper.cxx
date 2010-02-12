/**
 * Copyright. GPLv2
 */

#ifndef __KeyMapper_H
#define __KeyMapper_H

#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <map>
#include <SDL_keysym.h>

#include <SDL_androidvideo.h>

typedef unsigned long Android_Key;
typedef unsigned long Android_Scancode;
using std::vector;
using std::map;
using std::string;

static Android_Key scancodeLAlt = 57;
static Android_Key scancodeRAlt = 58;
static Android_Key scancodeLShift = 59;
static Android_Key scancodeRShift = 0xffff;  //FIXME

enum Android_KeyMap_Result {
    Android_Key_MC,
    Android_Key_UnMapped,
    Android_Key_Mapped
};

class AndroidMCKey;
class AndroidKMGroup;

class AndroidUtil {
public:
    static long long timeval_diff(struct timeval *end_time, struct timeval *start_time,
            struct timeval *difference = NULL) {
        struct timeval temp_diff;

        if(difference==NULL) {
            difference=&temp_diff;
        }

        difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
        difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

        /* Using while instead of if below makes the code slightly more robust. */
        while(difference->tv_usec<0) {
            difference->tv_usec+=1000000;
            difference->tv_sec -=1;
        }

        return 1000000LL*difference->tv_sec+ difference->tv_usec;

    } /* timeval_diff() */
};

/**
 * Multi-click enabled key
 */
class AndroidMCKey{
protected:
    struct timeval  lasttv;
    int             status;
    string          keyname;
    Android_Scancode scancode;

public:
    static int      clickBtwnThreshold; //in microseconds
    AndroidMCKey() {
        status = 0;
    }

    void setScancode(Android_Scancode scancode) {
        this->scancode = scancode;
    }
    Android_Scancode getScancode() {
        return this->scancode;
    }

    int getStatus() {
        return this->status;
    }

    void setKeyname (string &keyname) {
        this->keyname = keyname;
    }
    void setKeyname (const char *keyname) {
        this->keyname = keyname;
    }

    string& getKeyname() {
        return keyname;
    }

    bool doubleClick() {
        return status == 2;
    }
    bool singleClick() {
        return status == 1;
    }
    bool keyDown() {
        return status == -1;
    }

    virtual void preHandle(bool pressed) {
    }
    virtual void postHandle(bool pressed) {
    }

    bool interests(Android_Scancode scancode) {
        return this->scancode == scancode;
    }

    bool handle(Android_Scancode scancode, bool pressed) {
        printf("\nMC key handle func starts. pressed:%s\n", pressed?"yes":"no");
        if (this->scancode != scancode) return false;
        preHandle(pressed);

        struct timeval curtv;
        if (gettimeofday(&curtv, 0) != 0) {//failed
            printf("gettimeofday execution failed!!!");
            return true;
        }

        printf("\nmc key status before: %d\n", status);
        if (!pressed) { //key up
            switch (status) {
                case -3:
                    lasttv = curtv;
                    status = 0;
                    break;
                case -2:
                    lasttv = curtv;
                    status = 2;
                    break;
                case -1:
                    lasttv = curtv;
                    status = 1;
                    break;
                case 0:
                case 1:
                case 2:
                    lasttv = curtv;
                    break;
                default: //should not be here
                    break;
            }
        } else { // key down
            switch (status) {
                case -2:
                case -1:
                case -3:
                    lasttv = curtv;
                    break;
                case 0:
                    status = -1;
                    lasttv = curtv;
                    break;
                case 1: {
                    long long between = AndroidUtil::timeval_diff(&curtv, &lasttv);
                    bool isShort = between <= AndroidMCKey::clickBtwnThreshold;
                    if (isShort)
                        status = -2;
                    else
                        status = -1;
                    lasttv = curtv;
                    break;
                    }
                case 2:
                    status = -3;
                    break;
                default: //should not be here
                    break;
            }
        }
        printf("\nmc key status after: %d\n", status);

        postHandle(pressed);
        return true;
    }
};

class AndroidLAltEvtMgr: public AndroidMCKey {
public:
    AndroidLAltEvtMgr() {
        this->setKeyname("lalt");
        this->setScancode(scancodeLAlt);
    }
};
class AndroidRAltEvtMgr: public AndroidMCKey  {
public:
    AndroidRAltEvtMgr() {
        this->setKeyname("ralt");
        this->setScancode(scancodeRAlt);
    }
};

class AndroidLShiftEvtMgr: public AndroidMCKey  {
public:
    AndroidLShiftEvtMgr() {
        this->setKeyname("lshift");
        this->setScancode(scancodeLShift);
    }
};

class AndroidRShiftEvtMgr: public AndroidMCKey  {
public:
    AndroidRShiftEvtMgr() {
        this->setKeyname("rshift");
        this->setScancode(scancodeRShift);
    }
};

class AndroidKM {
private:
    Android_Key from;
    Android_Key to;
public:
    bool        needShift;
    AndroidKM(Android_Key pfrom, Android_Key pto, bool pneedShift):from(pfrom), to(pto), needShift(pneedShift) {
    }

    Android_Key getFrom() {
        return from;
    }
    Android_Key getTo() {
        return to;
    }
    void setFrom(Android_Key from) {
        this->from = from;
    }
    void setTo(Android_Key to) {
        this->to = to;
    }
};

class AndroidKMGroup {
private:
    vector<AndroidKM*> keymaps;
    // Android_Scancode scancode;

public:
    /*
    AndroidKMGroup (Android_Scancode scancode) {
        this->scancode = scancode;
    }

    Android_Scancode getScancode() {
        return scancode;
    }
    */

    bool addKeyMapping(Android_Key from, Android_Key to, bool needShift) {
        vector<AndroidKM*>::iterator it = keymaps.begin();
        while (it != keymaps.end()) {
            if ((*it)->getFrom() == from)//exists already
                return false;
            ++it;
        }

        keymaps.push_back(new AndroidKM(from, to, needShift));
        return true;
    }

    Android_Key getKeyMap(Android_Key from, AndroidKM ** km) {
        bool mapped;
        return getKeyMap(from, mapped, km);
    }

    Android_Key getKeyMap(Android_Key from, bool &mapped, AndroidKM ** km) {
        vector<AndroidKM*>::iterator it = keymaps.begin();
        while (it != keymaps.end()) {
            if ((*it)->getFrom() == from) {//exists already
                mapped = true;
                *km = *it;
                return (*it)->getTo();
            }
            ++it;
        }
        mapped = false;
        *km = NULL;
        return from;
    }
};

class AndroidLAlt2KMGroup: public AndroidKMGroup {
public:
    AndroidLAlt2KMGroup () {
        // FIXME
        //addKeyMapping(KEYCODE_H, KEYCODE_SEMICOLON, true);
        addKeyMapping(KEYCODE_H, KEYCODE_SEMICOLON, true);
        addKeyMapping(KEYCODE_J, KEYCODE_SEMICOLON, false);
        addKeyMapping(KEYCODE_K, KEYCODE_APOSTROPHE, true);
        addKeyMapping(KEYCODE_L, KEYCODE_APOSTROPHE, false);
        addKeyMapping(KEYCODE_W, KEYCODE_GRAVE, false);
        addKeyMapping(KEYCODE_F, KEYCODE_LEFT_BRACKET, true);
        addKeyMapping(KEYCODE_G, KEYCODE_RIGHT_BRACKET, true);
        addKeyMapping(KEYCODE_D, KEYCODE_BACKSLASH, false);
        addKeyMapping(KEYCODE_S, KEYCODE_BACKSLASH, true);
        addKeyMapping(KEYCODE_P, KEYCODE_EQUALS, false);
        addKeyMapping(KEYCODE_O, KEYCODE_EQUALS, true);
        addKeyMapping(KEYCODE_I, KEYCODE_MINUS, false);
        addKeyMapping(KEYCODE_E, KEYCODE_MINUS, true);
        // addKeyMapping(KEYCODE_U, KEYCODE_MINUS, false);
        addKeyMapping(KEYCODE_V, KEYCODE_LEFT_BRACKET, false);
        addKeyMapping(KEYCODE_B, KEYCODE_RIGHT_BRACKET, false);
        addKeyMapping(KEYCODE_N, KEYCODE_COMMA, true);
        addKeyMapping(KEYCODE_M, KEYCODE_PERIOD, true);
        addKeyMapping(KEYCODE_COMMA, KEYCODE_SLASH, true);
        addKeyMapping(KEYCODE_0, KEYCODE_0, true);
        addKeyMapping(KEYCODE_9, KEYCODE_9, true);
        addKeyMapping(KEYCODE_8, KEYCODE_8, true);
        addKeyMapping(KEYCODE_7, KEYCODE_7, true);
        addKeyMapping(KEYCODE_6, KEYCODE_6, true);
        addKeyMapping(KEYCODE_5, KEYCODE_5, true);
        addKeyMapping(KEYCODE_4, KEYCODE_4, true);
        addKeyMapping(KEYCODE_3, KEYCODE_3, true);
        addKeyMapping(KEYCODE_2, KEYCODE_2, true);
        addKeyMapping(KEYCODE_1, KEYCODE_1, true);
    }
};

class AndroidMCKeyMap {
protected:
    AndroidMCKey*           mckey;
    vector<AndroidKMGroup*> keymaps;
    map<int, int>           handleMap;

public:
    AndroidMCKeyMap(){
        keymaps.push_back(NULL);
        keymaps.push_back(NULL);
    }

    AndroidMCKey* getMckey() {
        return mckey;
    }

    bool mchandle(Android_Scancode scancode, bool pressed) {
        printf("\nmchandle starts\n");
        if (mckey->interests(scancode)) {
            printf("\nmchandle interest\n");
            mckey->handle(scancode, pressed);
            printf("\nmchandle interest over\n");
            return true;
        } else {
            printf("\nmchandle not interest\n");
            return false;
        }
    }

    /*
    Android_Key handle(Android_Scancode scancode, Android_Key keysym, bool pressed) {
        if (mckey->interests(scancode)) {
            mckey->handle(scancode, pressed);
            return Android_Key_MC;
        } else {
            return map(keysym);
        }
        return Android_Key_UnMapped;
    }
    */

    Android_KeyMap_Result mapKey(Android_Key from, AndroidKM **km) {
        printf("\n key mapping starts: from %d\n", from);
        int status = mckey->getStatus();
        map<int, int>::iterator loc = handleMap.find(status);
        if (loc != handleMap.end()) {
            int handlerIdx = loc->second;
            if (keymaps[handlerIdx] != NULL) {
                bool mapped = false;
                Android_Key to = keymaps[handlerIdx]->getKeyMap(from, mapped, km);
                if (mapped) { //the key is mapped
                    printf("\n key mapping over succ. to: %d\n", to);
                    return Android_Key_Mapped;
                }
            }
        }
        printf("\n key mapping over fail\n");
        return Android_Key_UnMapped;
    }

    void initHandleMap () {
        handleMap[2] = 2;
        handleMap[1] = 1;
        handleMap[0] = 0;
        handleMap[-1] = 2;
        handleMap[-2] = 2;
        handleMap[-3] = 2;
    }
};

class AndroidMCKeyLAltMap : public AndroidMCKeyMap {
public:
    AndroidMCKeyLAltMap () {
        printf("\ncreate left alt evt mgr\n");
        mckey = new AndroidLAltEvtMgr();
        printf("\ncreate left alt km group\n");
        AndroidKMGroup *kmgroup = new AndroidLAlt2KMGroup();
        keymaps.push_back(kmgroup);
        printf("\ninit handle map\n");
        initHandleMap();
    }
};

/*
class AndroidKMMgr {
private:
    vector<AndroidKMGroup*> keygroups;

public:
    bool existsKeyGroup(Android_Scancode scancode) {
        if (getKMGroup(scancode))
            return true;
        else
            return false;
    }

    AndroidKMGroup* getKMGroup(Android_Scancode scancode) {
        vector<AndroidKMGroup*>::iterator it = keygroups.begin();
        while(it != keygroups.end()) {
            if ((*it)->getScancode() == scancode)
                return *it;
            ++it;
        }
        return NULL;
    }

    bool addKeyGroup(Android_Scancode scancode) {
        if (existsKeyGroup(scancode))
            return false;
        AndroidKMGroup group = new AndroidKMGroup(scancode);
        keygroups.push_back(scancode);
        return true;
    }

    bool addKeyMapping(Android_Scancode scancode, Android_Key from, Android_Key to) {
        addKeyGroup(scancode);
        AndroidKMGroup *group = getKMGroup(scancode);
        assert(group!=NULL)
        return group->addKeyMapping(from, to);
    }
}
*/

/**
 * Singleton object
 */
class AndroidKeyEvtMgr {
protected:

    AndroidKeyEvtMgr () {
        printf("\n add Left Alt key handler\n");
        AndroidMCKeyLAltMap *lAltMCKey = new AndroidMCKeyLAltMap();
        mckeys.push_back(lAltMCKey);
        printf("\n add Left Alt key handler over\n");
    }

    static AndroidKeyEvtMgr     *androidEvtMgr;
    vector<AndroidMCKeyMap*>    mckeys;

public:
    static AndroidKeyEvtMgr *getInstance() {
        printf("\ngetInstance AndroidKeyEvtMgr\n");
        if (!AndroidKeyEvtMgr::androidEvtMgr)
            AndroidKeyEvtMgr::androidEvtMgr = new AndroidKeyEvtMgr();
        printf("\ngetInstance AndroidKeyEvtMgr succeeded\n");
        return AndroidKeyEvtMgr::androidEvtMgr;
    }
    static void destroyInstance() {
        if (AndroidKeyEvtMgr::androidEvtMgr)
            delete AndroidKeyEvtMgr::androidEvtMgr;
        AndroidKeyEvtMgr::androidEvtMgr = NULL;
    }

    enum Android_KeyMap_Result dispatch (Android_Key scancode,
            Android_Key keysym, bool pressed, AndroidKM **km) {
        printf("\n dispatch starts: scancode: %d, keysym: %d, pressed:%s\n", scancode, keysym, pressed?"yes":"no");
        vector<AndroidMCKeyMap*>::iterator it = mckeys.begin();
        while (it != mckeys.end()) {
            printf("interesting mc key: %s\n", (*it)->getMckey()->getKeyname().c_str());
            if ((*it)->mchandle(scancode, pressed)) {
                printf("\n MC key\n");
                return Android_Key_MC;
            }
            ++it;
        }

        printf("\n not MC key\n");
        // if (!pressed) return Android_Key_UnMapped;

        it = mckeys.begin();
        while (it != mckeys.end()) {
            printf("2 interesting mc key: %s\n", (*it)->getMckey()->getKeyname().c_str());
            Android_KeyMap_Result to = (*it)->mapKey(keysym, km);
            if (to == Android_Key_Mapped) {
                printf("\n key mapped\n");
                return Android_Key_Mapped;
            }
            ++it;
        }
        printf("\n key unmapped\n");
        return Android_Key_UnMapped;
    }
};

AndroidKeyEvtMgr* AndroidKeyEvtMgr::androidEvtMgr = NULL;
int AndroidMCKey::clickBtwnThreshold = 1000 * 1000; //1 second


/*
int main() {
}
*/

#endif
