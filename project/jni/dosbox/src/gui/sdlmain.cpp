/*
 *  Copyright (C) 2002-2009  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* $Id: sdlmain.cpp,v 1.150 2009/02/25 19:58:11 c2woody Exp $ */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#ifdef WIN32
//#include <signal.h>
#include <process.h>
#endif

#include "SDL.h"

#include "dosbox.h"
#include "video.h"
#include "mouse.h"
#include "pic.h"
#include "timer.h"
#include "setup.h"
#include "support.h"
#include "debug.h"
#include "mapper.h"
#include "vga.h"
#include "keyboard.h"
#include "cpu.h"
#include "cross.h"
#include "control.h"

#include "render.h"
extern Render_t render;

//#define DISABLE_JOYSTICK

#if !(ENVIRON_INCLUDED)
extern char** environ;
#endif

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// FIXME
// #include <windows.h>

#define STDOUT_FILE	"stdout.txt"
#define STDERR_FILE	"stderr.txt"
#define DEFAULT_CONFIG_FILE "/dosbox.conf"
#elif defined(MACOSX)
#define DEFAULT_CONFIG_FILE "/Library/Preferences/DOSBox Preferences"
#else /*linux freebsd*/
#define DEFAULT_CONFIG_FILE "/.dosboxrc"
#endif

#ifdef UNDER_CE
#undef  DEFAULT_CONFIG_FILE
#define STDOUT_FILE	"stdout.txt"
#define STDERR_FILE	"stderr.txt"
#define DEFAULT_CONFIG_FILE "dosbox.conf"
#endif

enum SCREEN_TYPES	{
	SCREEN_SURFACE,
	SCREEN_SURFACE_DDRAW,
	SCREEN_OVERLAY,
	SCREEN_OPENGL
};

#include "sdlmain.h"

SDL_Block sdl;

extern void Mouse_SetCursor(float x, float y, float xp, float yp);
extern void Mouse_MoveCursor(float x, float y);

char toolbar1[80] = "toolbar.dbk";
char toolbar2[80] = "game.dbk";
char toolbar3[80] = "set.dbk";
char mypath[260]="\0";
char fullname[260]="\0";

void GFX_SetTitle(Bit32s cycles,Bits frameskip,bool paused);
void WM_Update();
void WM_LoadToolbar(char* tbname, int tbnum);
void WM_UpdateToolbar();
void WM_PreInit();
void WM_Init();

#ifndef UNDER_CE
#define _GAPIBeginDraw() sdl.surface->pixels
#define _GAPIEndDraw() SDL_Flip(sdl.surface)
#else
#define _GAPIBeginDraw() _GetGAPIBuffer()
#define _GAPIEndDraw() _FreeGAPIBuffer()
#endif

Bit16u * GAPI_Buffer;

extern const char* RunningProgram;
extern bool CPU_CycleAutoAdjust;
//Globals for keyboard initialisation
bool startup_state_numlock=false;
bool startup_state_capslock=false;

#ifdef UNDER_CE
char cwd[MAX_PATH+1] = "";
char *getcwd(char *buffer, int maxlen)
{
	TCHAR fileUnc[MAX_PATH+1];
	char* plast;

	if(cwd[0] == 0)
	{
		GetModuleFileName(NULL, fileUnc, MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, fileUnc, -1, cwd, MAX_PATH, NULL, NULL);
		plast = strrchr(cwd, '\\');
		if(plast)
			*plast = 0;
		/* Special trick to keep start menu clean... */
		if(stricmp(cwd, "\\windows\\start menu") == 0)
			strcpy(cwd, "\\Apps");
	}
	if(buffer)
		strncpy(buffer, cwd, maxlen);
	return cwd;
}
#endif

static void WM_RenderXor(unsigned char code)
{
int x,y;

for (x=0; x<320; x++)
    for (y=0; y<sdl.toolbar.height; y++)
        if (sdl.toolbar.keycode[x+y*320]==code)
            sdl.toolbar.gfx[(y*320+x)]^=0xFF;
WM_UpdateToolbar();
}

void CPU_CycleIncrease(bool);
void CPU_CycleDecrease(bool);
void IncreaseFrameSkip(bool);
void DecreaseFrameSkip(bool);

static void Mouse_Up(bool pressed)
{
   if (pressed)
	sdl.dpad.y= -(float)(sdl.mouse.sensitivity)/100;
   else
	sdl.dpad.y=0;
}

static void Mouse_Down(bool pressed)
{
   if (pressed)
	sdl.dpad.y= (float)(sdl.mouse.sensitivity)/100;
   else
	sdl.dpad.y=0;
}

static void Mouse_Left(bool pressed)
{
   if (pressed)
	sdl.dpad.x= -(float)(sdl.mouse.sensitivity)/100;
   else
	sdl.dpad.x=0;
}

static void Mouse_Right(bool pressed)
{
   if (pressed)
	sdl.dpad.x= (float)(sdl.mouse.sensitivity)/100;
   else
	sdl.dpad.x=0;
}

static void MouseButton_Left(bool pressed)
{
   if (pressed)
   	Mouse_ButtonPressed(0);
   else
   	Mouse_ButtonReleased(0);
}

static void MouseButton_Right(bool pressed)
{
   if (pressed)
   	Mouse_ButtonPressed(1);
   else
   	Mouse_ButtonReleased(1);
}

static void MouseButton_Middle(bool pressed)
{
   if (pressed)
	Mouse_ButtonPressed(2);
   else
	Mouse_ButtonReleased(2);
}

static void Force_Toolbar(bool pressed)
{
   if (pressed) {
        sdl.toolbar.forced=!sdl.toolbar.forced;
	WM_Init();
   }
}

void WM_HandleToolbar(int x, int y, bool isdown)
{

        if ((sdl.window.height-sdl.window.toolbarstart)<2) return;

        int B=x;
        int L=y-(sdl.window.height-sdl.toolbar.vislines);

        //Option to hide toolbar?
        if(B<0 || B>320 || L<0 || L>sdl.toolbar.vislines)
        return;

        char code=sdl.toolbar.keycode[B+(L+sdl.toolbar.scrollpos)*320];
        if (code!=KBD_SC) sdl.toolbar.scrollactive=false;
        if (code==0) return;
        bool LockKey=(code&KBD_LOCKKEY);
        code=code&0x7F;
        char ascii;
        //Here need caps, shift, numlock aknowledgement...
        if (isdown)
        {
        switch(code)
         {
            case KBD_capslock:   WM_RenderXor(code); break;
            case KBD_numlock:    WM_RenderXor(code); break;
            case KBD_scrolllock: WM_RenderXor(code); break;
         }
        }
        switch(code)
        {
            case KBD_CCI:
                        WM_RenderXor(code);
                        if (isdown) CPU_CycleIncrease(true);
                        break;
            case KBD_CCD:
                        WM_RenderXor(code);
                        if (isdown) CPU_CycleDecrease(true);
                        break;
            case KBD_IFS:
                        WM_RenderXor(code);
                        if (isdown) IncreaseFrameSkip(true);
                        break;
            case KBD_DFS:
                        WM_RenderXor(code);
                        if (isdown) DecreaseFrameSkip(true);
                        break;
            case KBD_EXIT:
                        if (isdown) E_Exit("Exit button pressed");
                        break;
            case KBD_NT:
                        if (isdown)
                        {
                        sdl.toolbar.current++;
                        if (sdl.toolbar.current>2) sdl.toolbar.current=0;
                        WM_Init();
                        }
                        break;
            case KBD_PT:
                        if (isdown)
                        {
                        sdl.toolbar.current--;
                        if (sdl.toolbar.current<0) sdl.toolbar.current=2;
                        WM_Init();
                        }
                        break;
            case KBD_RMC:
                        if (isdown) Mouse_ButtonPressed(1); else Mouse_ButtonReleased(1);
                        break;
            case KBD_LMC:
                        if (isdown) Mouse_ButtonPressed(0); else Mouse_ButtonReleased(0);
                        break;
            case KBD_MMC:
                        if (isdown) Mouse_ButtonPressed(2); else Mouse_ButtonReleased(2);
                        break;
            case KBD_MNC:
                        if (isdown) {WM_RenderXor(code); sdl.mouse.noclick=!sdl.mouse.noclick;}
                        break;

            case KBD_SC:
                        sdl.toolbar.oldscpos=L;
                        sdl.toolbar.scrollactive=isdown;
                        break;


        };

        if(code<KBD_LAST)
        {
                if(LockKey)
                {
                        if (isdown)
                        {
                            if(sdl.toolbar.pressed[code])
                                KEYBOARD_AddKey((KBD_KEYS)code,false);
                                else
                                KEYBOARD_AddKey((KBD_KEYS)code,true);
                                WM_RenderXor(code|0x80);
                                sdl.toolbar.pressed[code]=!sdl.toolbar.pressed[code];

                        }
                } else
                {
                        if (isdown) KEYBOARD_AddKey((KBD_KEYS)code,true);
                        else KEYBOARD_AddKey((KBD_KEYS)code,false);
                }
        }
}

static void WM_ClearToolbarArea()
{
bool Close;
Bit16u * tempbuf;
int i,j;

  Close = false;
  if (!GAPI_Buffer) {
	GAPI_Buffer = (Bit16u*)_GAPIBeginDraw();
	Close = true;
  }

  if (!sdl.window.rotateright) {
  tempbuf = GAPI_Buffer + (319*240) + sdl.window.toolbarstart; //correct pointer to where upper-left corner should be
  for (i=0; i<sdl.window.height-sdl.window.toolbarstart; i++) {
	for (j=0; j<320; j++) {
		tempbuf[0]=0;
		tempbuf+=(-240);
		}
  	tempbuf+=(319*240+240+1);
  	}
  } else {
  tempbuf = GAPI_Buffer + sdl.window.height - sdl.window.toolbarstart - 1; //correct pointer to where upper-left corner should be
  for (i=0; i<sdl.window.height-sdl.window.toolbarstart; i++) {
	for (j=0; j<320; j++) {
		tempbuf[0]=0;
		tempbuf+=(240);
		}
  	tempbuf=tempbuf-(320*240)-1;
  	}
  }
  if (Close) {
  	_GAPIEndDraw();
  	GAPI_Buffer = 0;
  }
}


static void WM_DrawToolbar()
{
Bit32u i,j,k;
Bit16u tmp;
Bit16u * tempbuf;
Bit16u * tbbuf;
int offset = 0;
bool Close;

  if ((sdl.window.height-sdl.window.toolbarstart)<2) return;

  Close = false;
  if (!GAPI_Buffer) {
	GAPI_Buffer = (Bit16u*)_GAPIBeginDraw();
	Close = true;
  }

  if (sdl.toolbar.transparent) offset = render.scale.height - sdl.window.toolbarstart;
  if (offset < 0) offset=0;

  k=(sdl.toolbar.scrollpos+offset);
  k*=320;
  tbbuf=(Bit16u*)sdl.toolbar.gfx;

  if (!sdl.window.rotateright) {
  tempbuf = GAPI_Buffer + (319*240) + (sdl.window.toolbarstart+offset); //correct pointer to where upper-left corner should be
  for (i=0; i<(sdl.toolbar.vislines-offset); i++)
  {
	for (j=0; j<320; j++) {
		tmp=tbbuf[k];
		tempbuf[0]=tmp;
		tempbuf+=(-240);
		k++;
	}
  tempbuf+=(319*240+240+1);
  }

  } else {
  tempbuf = GAPI_Buffer + sdl.window.height - (sdl.window.toolbarstart+offset) - 1; //correct pointer to where upper-left corner should be
  for (i=0; i<(sdl.toolbar.vislines-offset); i++)
  {
	for (j=0; j<320; j++) {
		tmp=tbbuf[k];
		tempbuf[0]=tmp;
		tempbuf+=(240);
		k++;
	}
  tempbuf=tempbuf-(320*240)-1;
  }
  }
  if (Close) {
  	_GAPIEndDraw();
  	GAPI_Buffer = 0;
  }
}

void WM_InvalidateToolbar()
{
	sdl.toolbar.invalid = sdl.window.toolbarstart;
	render.frameskip.count = render.frameskip.max;
}

void WM_UpdateToolbar()
{
    if (sdl.toolbar.forced && sdl.toolbar.transparent && render.scale.height > sdl.window.toolbarstart) {
      WM_InvalidateToolbar();
    }

    if ((sdl.window.height-sdl.window.toolbarstart)<2) return;

    WM_DrawToolbar();
}

extern Bit8u int10_font_08[256 * 8];

void DrawToolbarChar(unsigned char ch, Bits x, Bits y)
{
  Bit8u c, *pc;
  Bits xx, yy, i;
  Bit16u bg = *(Bit16u*)(&sdl.toolbar.gfx[0]);
  pc=int10_font_08 + ch*8;
  for (yy=0; yy<8; yy++) {
    c=*pc;
    i=((y + yy)*320 + x);
    for (xx=0; xx<8; xx++) {
      if (c & 0x80)
        *(Bit16u*)(&sdl.toolbar.gfx[i])=0;
      else
        *(Bit16u*)(&sdl.toolbar.gfx[i])=bg;
      c=c << 1;
      i++;
    }
    pc++;
  }
}

void DrawToolbarText(char* Text, Bits x, Bits y, Bits cnt)
{
  Bits xx = x;
  while (*Text || cnt) {
    if (*Text != '\n') {
      DrawToolbarChar(*Text, xx, y);
      cnt--;
      xx+=8;
    }
    else
      xx=320;
    if (*Text)
      Text++;
    if (xx > 295) {
      xx=x;
      y+=8;
      if (y > sdl.toolbar.height-8)
        break;
    }
  }
}


void WM_Init()
{
	WM_InvalidateToolbar();
	switch (sdl.toolbar.current)	{
		case 0: WM_LoadToolbar(toolbar1,0); break;
		case 1: WM_LoadToolbar(toolbar2,1); break;
		case 2: WM_LoadToolbar(toolbar3,2); break;
	}

    if (sdl.toolbar.forced) {
	sdl.window.toolbarstart = sdl.window.height - sdl.toolbar.forcedsize;
    } else {
	sdl.window.toolbarstart = render.scale.height;
    };
	if (sdl.toolbar.height <= (sdl.window.height-sdl.window.toolbarstart) )
		sdl.window.toolbarstart = sdl.window.height - sdl.toolbar.height;
	sdl.toolbar.vislines = sdl.window.height-sdl.window.toolbarstart;
        if (sdl.toolbar.vislines>(sdl.toolbar.height-sdl.toolbar.scrollpos)) sdl.toolbar.scrollpos = sdl.toolbar.height-sdl.toolbar.vislines;
	if (sdl.toolbar.current == 2) GFX_SetTitle(-1,-1,false);
	WM_UpdateToolbar();
}

extern Bits cpuLoadDisp;
extern Bits cyclesDisp;

void GFX_SetTitle(Bit32s cycles,Bits frameskip,bool paused){
	char title[200]={0};
	static Bit32s internal_cycles=0;
	static Bits internal_frameskip=0;
	if(cycles != -1) internal_cycles = cycles;
	if(frameskip != -1) internal_frameskip = frameskip;

	if(CPU_CycleAutoAdjust) {
		if (internal_cycles>=100)
			sprintf(title,"DOSBox %s, C: max, FS %2d, P: %8s",VERSION,internal_frameskip,RunningProgram);
		else
			sprintf(title,"DOSBox %s, C: [%3d%%], FS %2d, P: %8s",VERSION,internal_cycles,internal_frameskip,RunningProgram);
	} else {
		sprintf(title,"DOSBox %s, C: %8d, FS %2d, P: %8s",VERSION,internal_cycles,internal_frameskip,RunningProgram);
	}

	if(paused) strcat(title," PAUSED");
	SDL_WM_SetCaption(title,VERSION);

  if (sdl.toolbar.current == 2) {
    sprintf(title,"FS: %d",internal_frameskip);
    DrawToolbarText(title, 135, 4, 20);
    sprintf(title,"Cycles: %d",internal_cycles);
    DrawToolbarText(title, 135, 4+8, 20);
//    sprintf(title,"CPU load: %d.%d%%",cpuLoadDisp/10,cpuLoadDisp % 10);
//    DrawToolbarText(title, 135, 4+16, 20);
    WM_UpdateToolbar();
  }
}

static void PauseDOSBox(bool pressed) {
	if (!pressed)
		return;
	GFX_SetTitle(-1,-1,true);
	bool paused = true;
	KEYBOARD_ClrBuffer();
	SDL_Delay(500);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// flush event queue.
	}

	while (paused) {
		SDL_WaitEvent(&event);    // since we're not polling, cpu usage drops to 0.
		switch (event.type) {

			case SDL_QUIT:
                // FIXME
                printf("bad modification!!!");
                exit(1);
                // throw(0);
                break;
			case SDL_KEYDOWN:   // Must use Pause/Break Key to resume.
			case SDL_KEYUP:
			if(event.key.keysym.sym==SDLK_PAUSE) {

				paused=false;
				GFX_SetTitle(-1,-1,false);
				break;
			}
		}
	}
}

#if defined (WIN32)
bool GFX_SDLUsingWinDIB(void) {
	return sdl.using_windib;
}
#endif

/* Reset the screen with current values in the sdl structure */
Bitu GFX_GetBestMode(Bitu flags) {
	return GFX_LOVE_16|GFX_CAN_16;
}


void GFX_ResetScreen(void) {
	GFX_Stop();
	GFX_Start();
	CPU_Reset_AutoAdjust();
}

static int int_log2 (int val) {
    int log = 0;
    while ((val >>= 1) != 0)
	log++;
    return log;
}


Bitu GFX_SetSize(Bitu width,Bitu height,Bitu flags) {
	if (sdl.updating)
		GFX_EndUpdate( 0 );
	GFX_Start();
	WM_Init();
	return GFX_CAN_16;
}

void GFX_SwitchFullScreen(void) {
	sdl.window.fullscreen=!sdl.window.fullscreen;
	GFX_ResetScreen();
}

static void SwitchFullScreen(bool pressed) {
	if (!pressed)
		return;
	GFX_SwitchFullScreen();
}


bool GFX_StartUpdate(Bit8u * & pixels,Bitu & pitch) {
	if (!sdl.active || sdl.updating)
		return false;
	GAPI_Buffer = (Bit16u*)_GAPIBeginDraw();
	if (sdl.window.rotateright) {
		pixels = (Bit8u*)GAPI_Buffer + (239)*2;
		pitch = -2;
	} else {
		pixels = (Bit8u*)GAPI_Buffer + (319*240)*2;
		pitch = 2;
	}
	sdl.updating=true;
	return true;
}


void GFX_EndUpdate( const Bit16u *changedLines ) {
	if (!sdl.updating)
		return;
	sdl.updating=false;
        _GAPIEndDraw();
	GAPI_Buffer = 0;
}

void WM_PreInit()
{
memset(&sdl,0,sizeof(sdl));
sdl.toolbar.invalid=0;
sdl.toolbar.prev=100;
sdl.toolbar.forcedsize=40;
}

void WM_LoadToolbar(char* tbname, int tbnum)
{
if (sdl.toolbar.prev!=tbnum) {
	sprintf(fullname,"%s\\%s",mypath,tbname);
	FILE * hand = fopen(fullname,"rb");
	if (!hand) return;
	fread(&sdl.toolbar.height,1, 4, hand);
	fread(&sdl.toolbar.gfx[0],1,sdl.toolbar.height*320*2, hand);
	fread(&sdl.toolbar.keycode[0],1,320*sdl.toolbar.height, hand);
	fclose(hand);
	int x;
	for (x=0;x<255;x++) sdl.toolbar.pressed[x]=false;
	sdl.mouse.noclick=false;
	sdl.toolbar.prev=tbnum;
	sdl.toolbar.scrollpos=0;
	WM_ClearToolbarArea();
	}
sdl.toolbar.scrollactive=false;
}

int  update_counter = 100;
void WM_Update()
{

	if (sdl.mouse.needticks) sdl.mouse.delayticks++;

	if (sdl.tpad.releasedelay&&sdl.tpad.on)	{
		sdl.tpad.releasedelay--;
		if (!sdl.tpad.releasedelay) {
			if (!sdl.mouse.right) Mouse_ButtonReleased(0); else Mouse_ButtonReleased(1);
			sdl.mouse.right=false; }
	}

	if (update_counter--) return;
	update_counter = 100;
	if (sdl.dpad.on) {
	    if (sdl.dpad.x!=0 | sdl.dpad.y!=0) {
		Mouse_MoveCursor(sdl.dpad.x,sdl.dpad.y);
		sdl.dpad.x=sdl.dpad.x+sdl.dpad.x/10;
		sdl.dpad.y=sdl.dpad.y+sdl.dpad.y/10;
		}
	}
}

void GFX_SetPalette(Bitu start,Bitu count,GFX_PalEntry * entries) {
/* no support */
}

Bitu GFX_GetRGB(Bit8u red,Bit8u green,Bit8u blue) {
		return SDL_MapRGB(sdl.surface->format,red,green,blue);
}

void GFX_Stop() {
	if (sdl.updating)
		GFX_EndUpdate( 0 );
	sdl.active=false;
}

void GFX_Start() {
	GAPI_Buffer = (Bit16u*)_GAPIBeginDraw();
	memset(GAPI_Buffer,0,320*240*2);
	sdl.active=true;
	_GAPIEndDraw();
        GAPI_Buffer = 0;
}

static void GUI_ShutDown(Section * /*sec*/) {
	GFX_Stop();
	if (sdl.window.fullscreen) GFX_SwitchFullScreen();
}

static void KillSwitch(bool pressed) {
	if (!pressed)
		return;
    // FIXME
	//throw 1;
    printf("bad modification!!!");
}

static unsigned char logo[32*32*4]= {
#include "dosbox_logo.h"
};

static void GUI_StartUp(Section * sec) {
	sec->AddDestroyFunction(&GUI_ShutDown);
	Section_prop * section=static_cast<Section_prop *>(sec);
	sdl.active=false;
	sdl.updating=false;

#if !defined(MACOSX)
	/* Set Icon (must be done before any sdl_setvideomode call) */
	/* But don't set it on OS X, as we use a nicer external icon there. */
#if WORDS_BIGENDIAN
	SDL_Surface* logos= SDL_CreateRGBSurfaceFrom((void*)logo,32,32,32,128,0xff000000,0x00ff0000,0x0000ff00,0);
#else
	SDL_Surface* logos= SDL_CreateRGBSurfaceFrom((void*)logo,32,32,32,128,0x000000ff,0x0000ff00,0x00ff0000,0);
#endif
	SDL_WM_SetIcon(logos,NULL);
#endif

	sdl.window.fullscreen=section->Get_bool("fullscreen");
	sdl.window.rotateright=section->Get_bool("rotateright");
	sdl.mouse.sensitivity=section->Get_int("sensitivity");
	sdl.tpad.on=section->Get_bool("touchpadmouse");
	sdl.tpad.clickms=section->Get_int("clickms");
	sdl.tpad.rightclickdelay=section->Get_int("rightclickdelay");
	sdl.dpad.on=section->Get_bool("dpadmouse");
/* toolbars */
	const char * tb1=section->Get_string("kbdtoolbar");
	const char * tb2=section->Get_string("gametoolbar");
	const char * tb3=section->Get_string("settoolbar");
	if (tb1>"") strcpy(toolbar1,tb1);
	if (tb2>"") strcpy(toolbar2,tb2);
	if (tb3>"") strcpy(toolbar3,tb3);
/* forced toolbar */
	sdl.toolbar.forced=section->Get_bool("forcetoolbar");
	sdl.toolbar.transparent=section->Get_bool("forcetransparent");
	sdl.toolbar.forcedsize=section->Get_int("forceheight");
	const char * ftz=section->Get_string("forcezone");
/* awfully dumb parser */
	int i,val;
	i=0;
	val=0; while (ftz[i]!=0x2C) {val=val*10+(ftz[i++]-0x30); }; sdl.window.tzx1=val; i++;
	val=0; while (ftz[i]!=0x2C) {val=val*10+(ftz[i++]-0x30); }; sdl.window.tzy1=val; i++;
	val=0; while (ftz[i]!=0x2C) {val=val*10+(ftz[i++]-0x30); }; sdl.window.tzx2=val+sdl.window.tzx1; i++;
	val=0; while (ftz[i]!=0x00) {val=val*10+(ftz[i++]-0x30); }; sdl.window.tzy2=val+sdl.window.tzy1;

	//LOG_MSG("%d,%d,%d,%d",sdl.window.tzx1,sdl.window.tzy1,sdl.window.tzx2,sdl.window.tzy2);

/* Our window */
	sdl.window.width=320;
	sdl.window.height=240;

/* fixme */
//	sdl.surface=SDL_SetVideoMode(sdl.window.width,sdl.window.height,16,0);

if (!sdl.window.fullscreen)
	sdl.surface=SDL_SetVideoMode(240,320,16,0);
else
	sdl.surface=SDL_SetVideoMode(240,320,16,SDL_FULLSCREEN);

	if (sdl.surface == NULL) E_Exit("Could not initialize video: %s",SDL_GetError());

	GFX_Stop();
/* Get some Event handlers */
	MAPPER_AddHandler(KillSwitch,MK_f9,MMOD1,"shutdown","ShutDown");
	MAPPER_AddHandler(SwitchFullScreen,MK_return,MMOD2,"fullscr","Fullscreen");
#if C_DEBUG
	/* Pause binds with activate-debugger */
#else
	MAPPER_AddHandler(&PauseDOSBox, MK_pause, MMOD2, "pause", "Pause");
#endif
	/* Get Keyboard state of numlock and capslock */
	SDLMod keystate = SDL_GetModState();

/* n0p - our specials */
	MAPPER_AddHandler(MouseButton_Left, (MapKeys)0xC2,0,"mbtn_left","Left mouse button");
	MAPPER_AddHandler(MouseButton_Right,(MapKeys)0xC3,0,"mbtn_right","Right mouse button");
	MAPPER_AddHandler(MouseButton_Middle, (MapKeys)0xC4,0,"mbtn_mdl","Middle mouse button");
	MAPPER_AddHandler(Force_Toolbar,(MapKeys)0xC1,0,"force_tlbr","Force toolbar display");
	MAPPER_AddHandler(Mouse_Up,(MapKeys)0xC1,0,"mm_up","Move mouse up");
	MAPPER_AddHandler(Mouse_Down,(MapKeys)0xC1,0,"mm_down","Move mouse down");
	MAPPER_AddHandler(Mouse_Left,(MapKeys)0xC1,0,"mm_left","Move mouse left");
	MAPPER_AddHandler(Mouse_Right,(MapKeys)0xC1,0,"mm_right","Move mouse right");

	if(keystate&KMOD_NUM) startup_state_numlock = true;
	if(keystate&KMOD_CAPS) startup_state_capslock = true;
}

void Mouse_AutoLock(bool enable) {
/* dummy */
}


static void HandleMouseMotion(SDL_MouseMotionEvent * motion) {
	int c;
	int mx = motion->x;
	int my = motion->y;

if (!sdl.window.rotateright) {
	c = my;
	my = mx;
	mx = 320-1-c;
} else {
	c = my;
	my = 240-1-mx;
	mx = c;
}

      if(my>sdl.window.toolbarstart) {
           sdl.tpad.active=false;
           if (sdl.toolbar.scrollactive) {
           	int L=(my-(sdl.window.height-sdl.toolbar.vislines));
           	sdl.toolbar.scrollpos-=(L-sdl.toolbar.oldscpos);
           	sdl.toolbar.oldscpos=L;
           	if (sdl.toolbar.scrollpos<0) sdl.toolbar.scrollpos=0;
           	if (sdl.toolbar.scrollpos>(sdl.toolbar.height-sdl.toolbar.vislines)) sdl.toolbar.scrollpos=sdl.toolbar.height-sdl.toolbar.vislines;
           WM_UpdateToolbar();
           }
        return;
        }

        sdl.toolbar.scrollactive=false;

    if (!sdl.tpad.on)
	Mouse_SetCursor((mx*render.src.width)/render.scale.width,(my*render.src.height)/render.scale.height,((float)mx*100)/(float)render.scale.width, ((float)my*100)/(float)render.scale.height );

	if (sdl.tpad.on) {
            if (sdl.tpad.active) {
				if (sdl.tpad.x!=mx) sdl.tpad.moved=true;
				if (sdl.tpad.y!=my) sdl.tpad.moved=true;
				Mouse_MoveCursor(float((mx-sdl.tpad.x)*float(sdl.mouse.sensitivity))/100, float((my-sdl.tpad.y)*float(sdl.mouse.sensitivity))/100);
				sdl.tpad.x=mx; sdl.tpad.y=my;
				}
        return;
        }

}

static void HandleMouseButton(SDL_MouseButtonEvent * button) {

	int c;
	int bx = button->x;
	int by = button->y;

if (!sdl.window.rotateright) {
	c = by;
	by = bx;
	bx = 320-1-c;
} else {
	c = by;
	by = 240-1-bx;
	bx = c;
}

    if ((by<=sdl.window.tzy2)&&(by>=sdl.window.tzy1)&&(bx<=sdl.window.tzx2)&&(bx>=sdl.window.tzx1)) { Force_Toolbar(button->state==SDL_PRESSED); return; }

    if(by>sdl.window.toolbarstart) {
        WM_HandleToolbar(bx, by, button->state==SDL_PRESSED);
        return; };

    if (!sdl.tpad.on)
	Mouse_SetCursor((bx*render.src.width)/render.scale.width,(by*render.src.height)/render.scale.height,((float)bx*100)/(float)render.scale.width, ((float)by*100)/(float)render.scale.height );

	switch (button->state) {
	case SDL_PRESSED:

    if (sdl.tpad.on) {
       if (!sdl.tpad.active) {
          sdl.tpad.active=true;
          sdl.tpad.x=bx;
          sdl.tpad.y=by;
          sdl.tpad.moved=false;
		  sdl.mouse.needticks=true;
		  sdl.mouse.delayticks=0;
          }
          return;
     }

	if (sdl.mouse.noclick) return;

		switch (button->button) {
		case SDL_BUTTON_LEFT:
			Mouse_ButtonPressed(0);
			break;
		case SDL_BUTTON_RIGHT:
			Mouse_ButtonPressed(1);
			break;
		case SDL_BUTTON_MIDDLE:
			Mouse_ButtonPressed(2);
			break;
		}
		break;
	case SDL_RELEASED:

		if (sdl.tpad.on) {
                  if (sdl.tpad.active)
				  if (!sdl.tpad.moved) {
				     if (sdl.tpad.rightclickdelay)
					if (sdl.mouse.delayticks>sdl.tpad.rightclickdelay) sdl.mouse.right=true; else sdl.mouse.right=false;
					    if (!sdl.mouse.right) Mouse_ButtonPressed(0); else Mouse_ButtonPressed(1);
						sdl.tpad.releasedelay=sdl.tpad.clickms;
                      }
                sdl.tpad.active=false;
				sdl.mouse.needticks=false;
				sdl.mouse.delayticks=0;
                return;
                }

        if (sdl.mouse.noclick) return;

		switch (button->button) {
		case SDL_BUTTON_LEFT:
			Mouse_ButtonReleased(0);
			break;
		case SDL_BUTTON_RIGHT:
			Mouse_ButtonReleased(1);
			break;
		case SDL_BUTTON_MIDDLE:
			Mouse_ButtonReleased(2);
			break;
		}
		break;
	}
}

void GFX_LosingFocus(void) {
	sdl.laltstate=SDL_KEYUP;
	sdl.raltstate=SDL_KEYUP;
	MAPPER_LosingFocus();
}

void GFX_Events() {
	SDL_Event event;
#if defined (REDUCE_JOYSTICK_POLLING)
	static int poll_delay=0;
	int time=GetTicks();
	if (time-poll_delay>20) {
		poll_delay=time;
		if (sdl.num_joysticks>0) SDL_JoystickUpdate();
		MAPPER_UpdateJoysticks();
	}
#endif
	WM_Update();
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_ACTIVEEVENT:
			if (event.active.state & SDL_APPINPUTFOCUS) {
				if (event.active.gain) {
					CPU_Disable_SkipAutoAdjust();
				} else {
						if (sdl.window.fullscreen) {
							VGA_KillDrawing();
							sdl.window.fullscreen=false;
							GFX_ResetScreen();
						}
					GFX_LosingFocus();
					CPU_Enable_SkipAutoAdjust();
				}
			}
			break;
		case SDL_MOUSEMOTION:
			HandleMouseMotion(&event.motion);
			break;
		case SDL_MOUSEBUTTONDOWN:
			HandleMouseMotion(&event.motion);
            		HandleMouseButton(&event.button);
            		break;
		case SDL_MOUSEBUTTONUP:
			HandleMouseMotion(&event.motion);
			HandleMouseButton(&event.button);
			break;
		case SDL_QUIT:
            // FIXME
            printf("bad modification!!!");
            exit(1);
			// throw(0);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		        /* Rotate D-Pad */
			if (!sdl.window.rotateright)
			{
				if (event.key.keysym.sym==SDLK_LEFT) event.key.keysym.sym=SDLK_UP; else
				if (event.key.keysym.sym==SDLK_RIGHT) event.key.keysym.sym=SDLK_DOWN; else
				if (event.key.keysym.sym==SDLK_UP) event.key.keysym.sym=SDLK_RIGHT; else
				if (event.key.keysym.sym==SDLK_DOWN) event.key.keysym.sym=SDLK_LEFT;
			} else {
				if (event.key.keysym.sym==SDLK_LEFT) event.key.keysym.sym=SDLK_DOWN; else
				if (event.key.keysym.sym==SDLK_RIGHT) event.key.keysym.sym=SDLK_UP; else
				if (event.key.keysym.sym==SDLK_UP) event.key.keysym.sym=SDLK_LEFT; else
				if (event.key.keysym.sym==SDLK_DOWN) event.key.keysym.sym=SDLK_RIGHT;
			}
			// ignore event alt+tab
			if (event.key.keysym.sym==SDLK_LALT) sdl.laltstate = event.key.type;
			if (event.key.keysym.sym==SDLK_RALT) sdl.raltstate = event.key.type;
			if (((event.key.keysym.sym==SDLK_TAB)) &&
				((sdl.laltstate==SDL_KEYDOWN) || (sdl.raltstate==SDL_KEYDOWN))) break;
		default:
			void MAPPER_CheckEvent(SDL_Event * event);
			MAPPER_CheckEvent(&event);
		}
	}
}

/* static variable to show wether there is not a valid stdout.
 * Fixes some bugs when -noconsole is used in a read only directory */
static bool no_stdout = false;

void GFX_ShowMsg(char const* format,...) {
	char buf[512];
	va_list msg;
	va_start(msg,format);
	vsprintf(buf,format,msg);
        strcat(buf,"\n");
	va_end(msg);
	if(!no_stdout) printf("%s",buf); //Else buf is parsed again.
}


void Config_Add_SDL() {
	Section_prop * sdl_sec=control->AddSection_prop("sdl",&GUI_StartUp);
	sdl_sec->AddInitFunction(&MAPPER_StartUp);
	Prop_bool* Pbool;
	Prop_string* Pstring;
	Prop_int* Pint;
	Prop_multival* Pmulti;

	Pbool = sdl_sec->Add_bool("fullscreen",Property::Changeable::Always,false);
	Pbool->Set_help("Start dosbox directly in fullscreen.");

	Pbool = sdl_sec->Add_bool("rotateright",Property::Changeable::Always,false);
	Pbool->Set_help("Alternate screen rotation.");

	Pstring = sdl_sec->Add_path("mapperfile",Property::Changeable::Always,"mapper.txt");
	Pstring->Set_help("File used to load/save the key/event mappings from.");

	Pstring = sdl_sec->Add_string("kbdtoolbar",Property::Changeable::Always,"toolbar.dbk");
	Pstring->Set_help("First toolbar.");
	Pstring = sdl_sec->Add_string("gametoolbar",Property::Changeable::Always,"game.dbk");
	Pstring->Set_help("Second toolbar.");
	Pstring = sdl_sec->Add_string("settoolbar",Property::Changeable::Always,"set.dbk");
	Pstring->Set_help("Third toolbar. Note: frameskip and cycles values are drawn there.");

	Pbool = sdl_sec->Add_bool("usescancodes",Property::Changeable::Always,false);
	Pbool->Set_help("Avoid usage of symkeys, might not work on all operating systems.");

	Pint = sdl_sec->Add_int("sensitivity",Property::Changeable::Always,100);
	Pint->SetMinMax(1,1000);
	Pint->Set_help("Mouse sensitivity.");

	Pbool = sdl_sec->Add_bool("touchpadmouse",Property::Changeable::Always,false);
	Pbool->Set_help("Emulate mouse with stylus moves on screen. Can fix mouse behavior in games that expect only relative mouse movement.");

	Pint = sdl_sec->Add_int("clickms",Property::Changeable::Always,50);
	Pint->SetMinMax(10,500);
	Pint->Set_help("When in touchpad mode, left-click is sent after single-tap. This sets the delay in milliseconds between emulated mouse button down and up events.");

	Pint = sdl_sec->Add_int("rightclickdelay",Property::Changeable::Always,700);
	Pint->SetMinMax(0,10000);
	Pint->Set_help("Touchpad mode. If you tap and hold stylus for this milliseconds, right-click will be send instead of left. 0 to disable.");

	Pbool = sdl_sec->Add_bool("dpadmouse",Property::Changeable::Always,false);
	Pbool->Set_help("Emulate mouse with D-Pad. Make sure to set corresponding mapper values for this to work. Up, down, left and right keys are autorotated.");

	Pbool = sdl_sec->Add_bool("forcetoolbar",Property::Changeable::Always,false);
	Pbool->Set_help("Sometimes where's no space on screen to fit even a part of toolbar. This forces it to display. Also a key and a zone can be set for toggling.");

	Pbool = sdl_sec->Add_bool("forcetransparent",Property::Changeable::Always,true);
	Pbool->Set_help("Forced toolbar area, overlapping active screen part will be semitransparent.");

	Pstring = sdl_sec->Add_string("forcezone",Property::Changeable::Always,"0,0,10,5");
	Pstring->Set_help("X, Y, width and height of 'force toolbar display' tapzone. No spaces please. See also hand_force_tlbr setting in mapper.txt");

	Pint = sdl_sec->Add_int("forceheight",Property::Changeable::Always,40);
	Pint->Set_help("Forced toolbar will have this height.");

}

static void printconfiglocation() {
	std::string path,file;
	Cross::CreatePlatformConfigDir(path);
	Cross::GetPlatformConfigName(file);
	path += file;
	FILE* f = fopen(path.c_str(),"r");
	if(!f && !control->PrintConfig(path.c_str())) {
		printf("tried creating %s. but failed",path.c_str());
		exit(1);
	}
	if(f) fclose(f);
	printf("%s\n",path.c_str());
	exit(0);
}


#if 0
//extern void UI_Init(void);
#undef main
int main(int argc, char* argv[]) {

getcwd(mypath,MAX_PATH);

	try {
		CommandLine com_line(argc,argv);
		Config myconf(&com_line);
		control=&myconf;
		/* Init the configuration system and add default values */
		Config_Add_SDL();
		DOSBOX_Init();

		/* Redirect standard input and standard output */
		sprintf(fullname,"%s\\%s",mypath,STDOUT_FILE);
		if(freopen(fullname, "w", stdout) == NULL)
			no_stdout = true; // No stdout so don't write messages
		sprintf(fullname,"%s\\%s",mypath,STDERR_FILE);
		freopen(fullname, "w", stderr);
		setvbuf(stdout, NULL, _IOLBF, BUFSIZ);	/* Line buffered */
		//setbuf(stderr, NULL);					/* No buffering */

		if (control->cmdline->FindExist("-version") ||
		    control->cmdline->FindExist("--version") ) {
			printf("\nDOSBox version %s, copyright 2002-2009 DOSBox Team.\n\n",VERSION);
			printf("DOSBox is written by the DOSBox Team (See AUTHORS file))\n");
			printf("DOSBox comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
			printf("and you are welcome to redistribute it under certain conditions;\n");
			printf("please read the COPYING file thoroughly before doing so.\n\n");
			return 0;
		}
		if(control->cmdline->FindExist("-printconf")) printconfiglocation();

#if C_DEBUG
		DEBUG_SetupConsole();
#endif

	/* Display Welcometext in the console */
	LOG_MSG("DOSBox version %s",VERSION);
	LOG_MSG("Copyright 2002-2009 DOSBox Team, published under GNU GPL.");
	LOG_MSG("---");

	/* Init SDL */
	if ( SDL_Init( SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE
		) < 0 ) E_Exit("Can't init SDL %s",SDL_GetError());

#ifndef DISABLE_JOYSTICK
	//Initialise Joystick seperately. This way we can warn when it fails instead
	//of exiting the application
	if( SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0 ) LOG_MSG("Failed to init joystick support");
#endif

	WM_PreInit();

	sdl.laltstate = SDL_KEYUP;
	sdl.raltstate = SDL_KEYUP;

	sdl.num_joysticks=SDL_NumJoysticks();

	/* Parse configuration files */
	std::string config_file,config_path;
	bool parsed_anyconfigfile = false;
	//First Parse -conf switches
	while(control->cmdline->FindString("-conf",config_file,true))
		if (control->ParseConfigFile(config_file.c_str())) parsed_anyconfigfile = true;

	//if none found => parse localdir conf

	sprintf(fullname,"%s\\%s",mypath,"dosbox.conf");
	config_file = fullname;
	if (!parsed_anyconfigfile && control->ParseConfigFile(config_file.c_str())) parsed_anyconfigfile = true;

	//if none found => parse userlevel conf
	if(!parsed_anyconfigfile) {
		config_file.clear();
		Cross::GetPlatformConfigDir(config_path);
		Cross::GetPlatformConfigName(config_file);
		sprintf(fullname,"%s\\%s",mypath,config_file.c_str());
		config_path = fullname;
		if(control->ParseConfigFile(config_path.c_str())) parsed_anyconfigfile = true;
	}

	if(!parsed_anyconfigfile) {
		//Try to create the userlevel configfile.
		config_file.clear();
		Cross::CreatePlatformConfigDir(config_path);
		Cross::GetPlatformConfigName(config_file);
		sprintf(fullname,"%s\\%s",mypath,config_file.c_str());
		config_path = fullname;
		if(control->PrintConfig(config_path.c_str())) {
			LOG_MSG("CONFIG: Generating default configuration.\nWriting it to %s",config_path.c_str());
			//Load them as well. Makes relative paths much easier
			control->ParseConfigFile(config_path.c_str());
		} else {
			LOG_MSG("CONFIG: Using default settings. Create a configfile to change them");
		}
	}


#if (ENVIRON_LINKED)
		control->ParseEnv(environ);
#endif
		/* Init all the sections */
		control->Init();
		/* Some extra SDL Functions */
		Section_prop * sdl_sec=static_cast<Section_prop *>(control->GetSection("sdl"));

		if (control->cmdline->FindExist("-fullscreen") || sdl_sec->Get_bool("fullscreen")) {
			if(!sdl.window.fullscreen) { //only switch if not allready in fullscreen
				GFX_SwitchFullScreen();
			}
		}

		/* Init the keyMapper */
		MAPPER_Init();
		if (control->cmdline->FindExist("-startmapper")) MAPPER_Run(false);
		/* Start up main machine */
		control->StartUp();
		/* Shutdown everything */
	} catch (char * error) {
		GFX_ShowMsg("Exit to error: %s",error);
		fflush(NULL);
	}
	catch (int){
		;//nothing pressed killswitch
	}
	catch(...){
		//Force visible mouse to end user. Somehow this sometimes doesn't happen
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(SDL_ENABLE);
		throw;//dunno what happened. rethrow for sdl to catch
	}
	//Force visible mouse to end user. Somehow this sometimes doesn't happen
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(SDL_ENABLE);

	SDL_Quit();//Let's hope sdl will quit as well when it catches an exception
	return 0;
};

#endif
