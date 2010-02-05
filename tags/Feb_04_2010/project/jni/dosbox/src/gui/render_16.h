/*
 *  Copyright (C) 2002-2005  The DOSBox Team
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

#define PSIZE 2
#define PTYPE Bit16u
#define redMask		0xF800
#define greenMask	0x07E0
#define blueMask	0x001F

#define redblueMask (redMask | blueMask)

#if SBPP == 8
#define PMAKE(_VAL) render.pal.lut[_VAL]
#endif

#if SBPP == 9
#define PMAKE(_VAL) render.pal.lut[_VAL]
#endif

#if SBPP == 15
#define PMAKE(_VAL) (((_VAL) & 31) | ((_VAL) & ~31) << 1)
#endif

#if SBPP == 16
#define PMAKE(_VAL) (_VAL)
#define DATATYPE Bit32u
#define DSTDATATYPE Bit16u
#endif

#if SBPP == 32
#define PMAKE(_VAL) (PTYPE)(((_VAL&(31<<19))>>8)|((_VAL&(63<<10))>>4)|((_VAL&(31<<3))>>3))
#endif

#define Scaler_DstWrite render.scale.outWrite
#define Scaler_DstPitch render.scale.outPitch
#define Scaler_DstCache render.scale.outCache
#define Scaler_SrcWidth render.src.width
#define ModeWidth render.scale.width
#define MH sdl.window.toolbarstart
#define ForcedKeyboard sdl.toolbar.forced
#define FKTransparent sdl.toolbar.transparent
#define Scaler_PaletteChanged render.pal.changed
#define Scaler_PaletteStatus render.pal.modified
#define tbscrollpos sdl.toolbar.scrollpos
#define rotateright sdl.window.rotateright
#define Scaler_Line render.scale.outLine
#define toolb sdl.toolbar.gfx
extern Bit16u Scaler_LineCache[];
extern Bit32u ScreenCache[];

#define BeginScaler(DataType, DstDataType) \
	Bitu process = 0; \
	if (ForcedKeyboard && Scaler_Line >= MH) if (FKTransparent) process = 2; else return; \
	Bitu x = render.src.width*Scaler_Line/2; \
	DataType *Cache = (DataType*)&ScreenCache[0] + x; \
	DataType *SrcData = (DataType*)src; \
	if (sdl.toolbar.invalid) process |= 8; \
	x = 0; \
	DstDataType *dst; \
	if (process & 2) \
	  dst=(DstDataType*)&Scaler_LineCache; \
	else \
	  dst=(DstDataType*)Scaler_DstWrite;

#define EndScalerCommon(Toolbar) \
	Toolbar \
	Scaler_DstWrite+=Scaler_DstPitch; \
	Scaler_Line++;
	
#define EndScaler EndScalerCommon(DrawToolbar)

#define CheckPaletteBit8u Scaler_PaletteStatus[out]
#define CheckPaletteBit16u Scaler_PaletteStatus[out & 0xff] || Scaler_PaletteStatus[out >> 8]
#define CheckPaletteBit32u Scaler_PaletteStatus[out & 0xff] || Scaler_PaletteStatus[out >> 8 & 0xff] || Scaler_PaletteStatus[out >> 16 & 0xff] || Scaler_PaletteStatus[out >> 24]

#define GetSourceData(DataType, CheckPalette) \
	DataType out=*(SrcData++); \
	if (out == *Cache && !(process & 8) ) { \
		Cache++; \
		process &= ~1L; \
	} \
	else { \
		*(Cache++)=out; \
		process |= 1; \
		if (process & 2) process |= 8; \
	}

#define BeginScalerGX(DataType, DstDataType) \
  BeginScaler(DataType, DstDataType) \
	Bits Pitch; \
	if (!(process & 2)) { \
		if (rotateright) \
			Pitch = 240; \
		else \
		  Pitch = -240; \
	} \
	else \
		Pitch=1;

#define DrawToolbarGX \
	if ((process & 2)&&(process & 8)) { \
		if (rotateright) \
			Pitch = 240; \
		else \
		        Pitch = -240; \
		Bit32u* src = (Bit32u*)&Scaler_LineCache; \
		Bit16u* dst = (Bit16u*)Scaler_DstWrite; \
		Bit32u* tb = (Bit32u*)&toolb + (tbscrollpos + Scaler_Line - MH)*320/2; \
		x=320/2; \
   	do { \
   	  Bit32u px=((*(src++) & 0xF7DEF7DE)>>1) + ((*(tb++) & 0xF7DEF7DE)>>1); \
   	  *dst=px & 0xFFFF; \
			dst+=Pitch; \
   	  *dst=px >> 16; \
			dst+=Pitch; \
	  } while (--x); \
	}

#define EndScalerGX EndScalerCommon(DrawToolbarGX)

static void conc3d(NormalGX,SBPP,DBPP) (const void * src) {
	BeginScalerGX(DATATYPE, DSTDATATYPE)
	while (x < ModeWidth) {
		GetSourceData(DATATYPE, CHKPAL)
		x+=2;
		if (!(process & 3))
			dst+=Pitch*2;
		else {
			*dst=PMAKE(out & 0xFFFF);
			dst+=Pitch;
			*dst=PMAKE((out >> 16) & 0xFFFF);
			dst+=Pitch;
		}
	}
	EndScalerGX
}

static void conc3d(HalfGX,SBPP,DBPP) (const void * src) {
	BeginScalerGX(DATATYPE, DSTDATATYPE)
	while (x < ModeWidth) {
		GetSourceData(DATATYPE, CHKPAL)
		x+=1;
		if (!(process & 3))
			dst+=Pitch*1;
		else {
			*dst=((PMAKE(out & 0xFFFF) & 0xF7DE) + (PMAKE((out >> 16) & 0xFFFF) & 0xF7DE)) >> 1;
			dst+=Pitch;
		}
	}
	EndScalerGX
}


static void conc3d(SpecialGX,SBPP,DBPP) (const void * src) {
	BeginScalerGX(DATATYPE, DSTDATATYPE)
	Bitu dx = (Scaler_SrcWidth - ModeWidth);
	Bitu xx = 0;
	while (x < ModeWidth) {
		xx+=2*dx;
		GetSourceData(DATATYPE, SCHKPAL)
		if (process & 3) {
			Bit32u px1 = PMAKE(out & 0xFFFF);
			Bit32u px2 = PMAKE(out >> 16);
			if (xx >= ModeWidth) {
				*dst=((px1 & 0xF7DE) + (px2 & 0xF7DE)) >> 1;
				dst+=Pitch;
				xx-=ModeWidth + dx;
				x++;
			}
			else {
				*dst=px1;
				dst+=Pitch;
				*dst=px2;
				dst+=Pitch;
				x+=2;
			}
		}
		else {
			if (xx >= ModeWidth) {
				dst+=Pitch;
				xx-=ModeWidth + dx;
				x++;
			}
			else {
				dst+=Pitch*2;
				x+=2;
			}
		}
	}
	EndScalerGX
}

static void conc3d(HalfSpecialGX,SBPP,DBPP) (const void * src) {
	BeginScalerGX(DATATYPE, DSTDATATYPE)
	Bitu dx = (Scaler_SrcWidth/2 - ModeWidth);
	Bitu xx = 0;
	while (x < ModeWidth) {
		xx+=2*dx;
		GetSourceData(DATATYPE, SCHKPAL)
		if (process & 3) {
			if (xx >= ModeWidth) {
				xx-=ModeWidth + dx;
			}
			else {
				*dst=((PMAKE(out & 0xFFFF) & 0xF7DE) + (PMAKE((out >> 16) & 0xFFFF) & 0xF7DE)) >> 1;
				dst+=Pitch;
				x++;
			}
		}
		else {
			if (xx >= ModeWidth) {
				xx-=ModeWidth + dx;
			}
			else {
				dst+=Pitch;
				x++;
			}
		}
	}
	EndScalerGX
}


#undef LINEPITCH
#undef PIXELPITCH


#undef PSIZE
#undef PTYPE
#undef PMAKE
#undef redMask
#undef greenMask
#undef blueMask
#undef redblueMask
#undef SRCTYPE

#undef DATATYPE
#undef DSTDATATYPE

