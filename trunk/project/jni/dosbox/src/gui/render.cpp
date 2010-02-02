/*
 *  Copyright (C) 2002-2008  The DOSBox Team
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

/* $Id: render.cpp,v 1.58 2009/02/01 14:24:36 qbix79 Exp $ */

#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <math.h>

#include "dosbox.h"
#include "video.h"
#include "render.h"
#include "setup.h"
#include "control.h"
#include "mapper.h"
#include "cross.h"
#include "hardware.h"
#include "support.h"

#include "render_scalers.h"

#include "sdlmain.h"
extern SDL_Block sdl;

Render_t render;
ScalerLineHandler_t RENDER_DrawLine;
Bit8u Scaler_Aspect[SCALER_MAXHEIGHT];
Bit32u ScreenCache[320*240];
Bit32u Scaler_LineCache[320];

static void RENDER_CallBack( GFX_CallBackFunctions_t function );

static void Check_Palette(void) {
	/* Clean up any previous changed palette data */
	if (render.pal.changed) {
		memset(render.pal.modified, 0, sizeof(render.pal.modified));
		render.pal.changed = false;
	}
	if (render.pal.first>render.pal.last) 
		return;
	Bitu i;
		for (i=render.pal.first;i<=render.pal.last;i++) {
			Bit8u r=render.pal.rgb[i].red;
			Bit8u g=render.pal.rgb[i].green;
			Bit8u b=render.pal.rgb[i].blue;
			Bit16u newPal = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
			if (newPal != render.pal.lut[i]) {
				render.pal.changed = true;
				render.pal.modified[i] = 1;
				render.pal.lut[i] = newPal;
 				}
		}
	/* Setup pal index to startup values */
	render.pal.first=256;
	render.pal.last=0;
}

void RENDER_SetPal(Bit8u entry,Bit8u red,Bit8u green,Bit8u blue) {
	render.pal.rgb[entry].red=red;
	render.pal.rgb[entry].green=green;
	render.pal.rgb[entry].blue=blue;
	if (render.pal.first>entry) render.pal.first=entry;
	if (render.pal.last<entry) render.pal.last=entry;
}

static void RENDER_EmptyLineHandler(const void * src) {
}

static void RENDER_Line(const void * src)
{
        if (Scaler_Aspect[render.scale.inLine]) {
		if (render.pal.changed) render.scale.linePalHandler (src); else render.scale.lineHandler (src);
	};
	render.scale.inLine++;
}


bool RENDER_StartUpdate(void) {
	if (GCC_UNLIKELY(render.updating))
		return false;
	if (GCC_UNLIKELY(!render.active))
		return false;
	if (GCC_UNLIKELY(render.frameskip.count<render.frameskip.max)) {
		render.frameskip.count++;
		return false;
	}
	render.frameskip.count=0;
	if (render.scale.inMode == scalerMode8) {
		Check_Palette();
	}

	render.scale.inLine = 0;
	render.scale.outLine = 0;
	render.scale.outWrite = 0;
	render.scale.outPitch = 0;

	if (!GFX_StartUpdate( render.scale.outWrite, render.scale.outPitch )) {
		RENDER_DrawLine = RENDER_EmptyLineHandler;
		return false;
	}
        
	RENDER_DrawLine = RENDER_Line;
	render.updating = true;
	return true;
}

static void RENDER_Halt( void ) {
	RENDER_DrawLine = RENDER_EmptyLineHandler;
	GFX_EndUpdate( 0 );
	render.updating=false;
	render.active=false;
}

extern Bitu PIC_Ticks;
void RENDER_EndUpdate( void ) {
	if (GCC_UNLIKELY(!render.updating))
		return;
	sdl.toolbar.invalid = 0;
	RENDER_DrawLine = RENDER_EmptyLineHandler;
	if ( render.scale.outWrite ) GFX_EndUpdate( 0 );
	render.updating=false;
}

static Bitu MakeAspectTable(Bitu SourceHeight, Bitu AimHeight) {
	Bitu Step = (AimHeight<<16)/SourceHeight;
	Bitu Acc = Step;
	Bitu i=SourceHeight-1;
	do {
	    Scaler_Aspect[i] = Acc>>16;
	    Acc=Acc&0x0000FFFF;
	    Acc+=Step;
	} while (i--);
}


static void RENDER_Reset( void ) {
	Bitu width=render.src.width;
	Bitu height=render.src.height;

	switch (render.src.bpp) {
	case 8:
		render.src.start = ( render.src.width * 1) / sizeof(Bitu);
			break;
	case 15:
		render.src.start = ( render.src.width * 2) / sizeof(Bitu);
			break;
	case 16:
		render.src.start = ( render.src.width * 2) / sizeof(Bitu);
			break;
	case 32:
		render.src.start = ( render.src.width * 4) / sizeof(Bitu);
			break;
	}

	/* FIXME */

	render.scale.width=320;
	render.scale.height=render.src.height;

	if (render.src.height>240) render.scale.height=render.src.height>>1;
	if (render.src.height==400) render.scale.height=200;
	if (render.src.height==480) render.scale.height=240;

	//LOG_MSG("%dx%d:%dx%d",render.src.width,render.src.height,render.scale.width,render.scale.height);	
	//ScreenCache=(Bit32u*)realloc(ScreenCache, render.src.width*render.scale.height*render.src.bpp/8);

        memset(&ScreenCache[0], 0, 320*240*4);

	ScalerLineBlock_t *lineBlock;

	if (render.src.width == 320) {
			lineBlock=&ScaleNormalGX;
	}
	else
		if (width % 320 == 0) {
				lineBlock=&ScaleHalfGX;
		}
		else
			if (width > 320) {
				if (width>640) lineBlock=&ScaleHalfSpecialGX; else lineBlock=&ScaleSpecialGX;
			}

	MakeAspectTable( render.src.height, render.scale.height );

/*
	int accc=0;
	for (int i=0; i<render.src.height; i++) {
	         fprintf(stdout,"%d ",Scaler_Aspect[i]);
		 accc+=Scaler_Aspect[i];
	}
	fprintf(stdout,"/n%d ",accc);
*/

	GFX_SetSize(width,height,16);

	switch (render.src.bpp) {
	case 8:
		render.scale.lineHandler = lineBlock->Linear[0];
		render.scale.linePalHandler = lineBlock->Linear[4];
		render.scale.inMode = scalerMode8;
		render.scale.cachePitch = render.src.width * 1;
		break;
	case 15:
		render.scale.lineHandler = lineBlock->Linear[1];
		render.scale.linePalHandler = 0;
		render.scale.inMode = scalerMode15;
		render.scale.cachePitch = render.src.width * 2;
		break;
	case 16:
		render.scale.lineHandler = lineBlock->Linear[2];
		render.scale.linePalHandler = 0;
		render.scale.inMode = scalerMode16;
		render.scale.cachePitch = render.src.width * 2;
		break;
	case 32:
		render.scale.lineHandler = lineBlock->Linear[3];
		render.scale.linePalHandler = 0;
		render.scale.inMode = scalerMode32;
		render.scale.cachePitch = render.src.width * 4;
		break;
	default:
		E_Exit("RENDER:Wrong source bpp %d", render.src.bpp );
	}
	/* Reset the palette change detection to it's initial value */
	render.pal.first= 0;
	render.pal.last = 255;
	render.pal.changed = false;
	memset(render.pal.modified, 0, sizeof(render.pal.modified));
	//Finish this frame using a copy only handler
	RENDER_DrawLine = RENDER_EmptyLineHandler;
	render.scale.outWrite = 0;
	/* Signal the next frame to first reinit the cache */
	render.active=true;
}

void RENDER_SetSize(Bitu width,Bitu height,Bitu bpp,float fps,double ratio,bool dblw,bool dblh) {
	RENDER_Halt( );
	if (!width || !height || width > SCALER_MAXWIDTH || height > SCALER_MAXHEIGHT) { 
		return;	
	}
	render.src.width=width;
	render.src.height=height;
	render.src.bpp=bpp;
	render.src.fps=fps;
	RENDER_Reset( );
}

extern void GFX_SetTitle(Bit32s cycles, Bits frameskip,bool paused);
void IncreaseFrameSkip(bool pressed) {
	if (!pressed)
		return;
	if (render.frameskip.max<10) render.frameskip.max++;
	LOG_MSG("Frame Skip at %d",render.frameskip.max);
	GFX_SetTitle(-1,render.frameskip.max,false);
}

void DecreaseFrameSkip(bool pressed) {
	if (!pressed)
		return;
	if (render.frameskip.max>0) render.frameskip.max--;
	LOG_MSG("Frame Skip at %d",render.frameskip.max);
	GFX_SetTitle(-1,render.frameskip.max,false);
}

void RENDER_Init(Section * sec) {
	Section_prop * section=static_cast<Section_prop *>(sec);

	//For restarting the renderer.
	static bool running = false;

	render.pal.first=256;
	render.pal.last=0;
	render.frameskip.max=section->Get_int("frameskip");
	render.frameskip.count=0;

	/* Maybe scaler = qvga ? */	
#if 0
	std::string cline;
	std::string scaler;   
	Prop_multival* prop = section->Get_multival("scaler");
	scaler = prop->GetSection()->Get_string("type");
	if(f == "forced") render.scale.forced = true;
	if (scaler == "none") { render.scale.op = scalerOpNormal;render.scale.size = 1; }

	//If something changed that needs a ReInit
	// Only ReInit when there is a src.bpp (fixes crashes on startup and directly changing the scaler without a screen specified yet)
	if(running && render.src.bpp && ((render.aspect != aspect) || (render.scale.op != scaleOp) || 
				  (render.scale.size != scalersize) || (render.scale.forced != scalerforced) ||
				   render.scale.forced))
		RENDER_CallBack( GFX_CallBackReset );
#endif

	if(!running) render.updating=true;
	running = true;

	MAPPER_AddHandler(DecreaseFrameSkip,MK_f7,MMOD1,"decfskip","Dec Fskip");
	MAPPER_AddHandler(IncreaseFrameSkip,MK_f8,MMOD1,"incfskip","Inc Fskip");
	GFX_SetTitle(-1,render.frameskip.max,false);
}

