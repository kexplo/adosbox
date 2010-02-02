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


//TODO:
//Maybe just do the cache checking back into the simple scalers so they can 
//just handle it all in one go, but this seems to work well enough for now

#include "dosbox.h"
#include "render.h"
#include <string.h>

#include "sdlmain.h"
extern SDL_Block sdl;

#define _conc2(A,B) A ## B
#define _conc3(A,B,C) A ## B ## C
#define _conc4(A,B,C,D) A ## B ## C ## D
#define _conc5(A,B,C,D,E) A ## B ## C ## D ## E

#define conc2(A,B) _conc2(A,B)
#define conc3(A,B,C) _conc3(A,B,C)
#define conc4(A,B,C,D) _conc4(A,B,C,D)
#define conc2d(A,B) _conc3(A,_,B)
#define conc3d(A,B,C) _conc5(A,_,B,_,C)

/* Include the different rendering routines */
#define SBPP 8
#define DBPP 16
#include "render_8.h"
#undef SBPP
#undef DBPP

#define SBPP 9
#define DBPP 16
#include "render_9.h"
#undef SBPP
#undef DBPP

#define SBPP 15
#define DBPP 16
#include "render_16.h"
#undef SBPP
#undef DBPP

#define SBPP 16
#define DBPP 16
#include "render_16.h"
#undef SBPP
#undef DBPP

#define SBPP 32
#define DBPP 16
#include "render_32.h"
#undef SBPP
#undef DBPP

ScalerLineBlock_t ScaleNormalGX = {
	NormalGX_8_16,NormalGX_15_16,NormalGX_16_16,NormalGX_32_16,NormalGX_9_16
};

ScalerLineBlock_t ScaleHalfGX = {
	HalfGX_8_16,HalfGX_15_16,HalfGX_16_16,HalfGX_32_16,HalfGX_9_16
};

ScalerLineBlock_t ScaleSpecialGX= {
	SpecialGX_8_16,SpecialGX_15_16,SpecialGX_16_16,SpecialGX_32_16,SpecialGX_9_16
};

ScalerLineBlock_t ScaleHalfSpecialGX= {
	HalfSpecialGX_8_16,HalfSpecialGX_15_16,HalfSpecialGX_16_16,HalfSpecialGX_32_16,HalfSpecialGX_9_16
};
