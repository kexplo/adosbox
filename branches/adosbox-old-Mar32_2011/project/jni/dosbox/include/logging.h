#ifndef DOSBOX_LOGGING_H
#define DOSBOX_LOGGING_H
enum LOG_TYPES {
	LOG_ALL,
	LOG_VGA, LOG_VGAGFX,LOG_VGAMISC,LOG_INT10,
	LOG_SB,LOG_DMACONTROL,
	LOG_FPU,LOG_CPU,LOG_PAGING,
	LOG_FCB,LOG_FILES,LOG_IOCTL,LOG_EXEC,LOG_DOSMISC,
	LOG_PIT,LOG_KEYBOARD,LOG_PIC,
	LOG_MOUSE,LOG_BIOS,LOG_GUI,LOG_MISC,
	LOG_IO,
	LOG_MAX
};

enum LOG_SEVERITIES {
	LOG_NORMAL,
	LOG_WARN,
	LOG_ERROR
};

#if C_DEBUG

#ifndef ANDROID_DEBUG //FIXME
class LOG
{
	LOG_TYPES       d_type;
	LOG_SEVERITIES  d_severity;
public:

	LOG (LOG_TYPES type , LOG_SEVERITIES severity):
		d_type(type),
		d_severity(severity)
		{}
	void operator() (char const* buf, ...) GCC_ATTRIBUTE(__format__(__printf__, 2, 3));  //../src/debug/debug_gui.cpp

};

void DEBUG_ShowMsg(char const* format,...) GCC_ATTRIBUTE(__format__(__printf__, 1, 2));
#define LOG_MSG DEBUG_ShowMsg

#else //ANDROID_DEBUG
class LOG
{
	LOG_TYPES       d_type;
	LOG_SEVERITIES  d_severity;
public:

	LOG (LOG_TYPES type , LOG_SEVERITIES severity):
		d_type(type),
		d_severity(severity)
		{}
	void operator() (char const* buf, ...) {
        char buffer[512];
        va_list msg;
        va_start(msg,buf);
        vsprintf(buffer,buf,msg);
        va_end(msg);

        const char *type = NULL;
        switch (d_type) {
            case LOG_ALL:
                type = "ALL";
                break;
            case LOG_VGA:
                type = "VGA";
                break;
            case LOG_VGAGFX:
                type = "VGAGFX";
                break;
            case LOG_VGAMISC:
                type = "VGAMISC";
                break;
            case LOG_INT10:
                type = "INT10";
                break;
            case LOG_SB:
                type = "SB";
                break;
            case LOG_DMACONTROL:
                type = "DMACONTROL";
                break;
            case LOG_FPU:
                type = "FPU";
                break;
            case LOG_CPU:
                type = "CPU";
                break;
            case LOG_PAGING:
                type = "PAGING";
                break;
            case LOG_FCB:
                type = "FCB";
                break;
            case LOG_FILES:
                type = "FILES";
                break;
            case LOG_IOCTL:
                type = "IOCTL";
                break;
            case LOG_EXEC:
                type = "EXEC";
                break;
            case LOG_DOSMISC:
                type = "DOSMISC";
                break;
            case LOG_PIT:
                type = "PIT";
                break;
            case LOG_KEYBOARD:
                type = "KEYBOARD";
                break;
            case LOG_PIC:
                type = "PIC";
                break;
            case LOG_MOUSE:
                type = "MOUSE";
                break;
            case LOG_BIOS:
                type = "BIOS";
                break;
            case LOG_GUI:
                type = "GUI";
                break;
            case LOG_MISC:
                type = "MISC";
                break;
            case LOG_IO:
                type = "IO";
                break;
            case LOG_MAX:
                type = "MAX";
                break;
            default:
                type = "UNKNOWN";
                break;
        }
        switch (d_severity) {
            case LOG_NORMAL:
                ALOG_DEBUG("%s: %s", type, buffer);
                break;
            case LOG_WARN:
                ALOG_WARN("%s: %s", type, buffer);
                break;
            case LOG_ERROR:
                ALOG_ERROR("%s: %s", type, buffer);
                break;
            default:
                ALOG_DEBUG("%s: %s", type, buffer);
                break;
        }
    }
};

void DEBUG_ShowMsg(char const* format,...);

#define LOG_MSG DEBUG_ShowMsg

#endif //ANDROID_DEBUG

#else  //C_DEBUG

struct LOG
{
	LOG(LOG_TYPES , LOG_SEVERITIES )										{ }
	void operator()(char const* )													{ }
	void operator()(char const* , double )											{ }
	void operator()(char const* , double , double )								{ }
	void operator()(char const* , double , double , double )					{ }
	void operator()(char const* , double , double , double , double )					{ }
	void operator()(char const* , double , double , double , double , double )					{ }

	void operator()(char const* , char const* )									{ }
	void operator()(char const* , char const* , double )							{ }
	void operator()(char const* , char const* , double ,double )				{ }
	void operator()(char const* , double , char const* )						{ }
	void operator()(char const* , double , double, char const* )						{ }
	void operator()(char const* , char const*, char const*)				{ }


}; //add missing operators to here
	//try to avoid anything smaller than bit32...
void GFX_ShowMsg(char const* format,...) GCC_ATTRIBUTE(__format__(__printf__, 1, 2));
#define LOG_MSG GFX_ShowMsg

#endif //C_DEBUG


#endif //DOSBOX_LOGGING_H
