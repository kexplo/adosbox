/* This file should be edited using Touchscreen Editor utility */
#ifndef _TOUCHSCREENFONT_H_
#define _TOUCHSCREENFONT_H_
#include <SDL_types.h>
typedef struct font_line_t { Uint8 x1, y1, x2, y2; } font_line_t;
enum { FONT_MAX_LINES_PER_CHAR = 5 };
static font_line_t font[11][FONT_MAX_LINES_PER_CHAR] = {
	{
		{  27, 136, 182, 136 },
		{  27, 136,  94, 104 },
		{  27, 136,  89, 162 },
		{    0,   0,   0,   0 },
		{    0,   0,   0,   0 }
	},
	{
		{ 192, 123,  61, 123 },
		{ 150, 103, 192, 123 },
		{ 192, 123, 155, 144 },
		{    0,   0,   0,   0 },
		{    0,   0,   0,   0 }
	},
	{
		{ 118,  43, 118, 188 },
		{ 118,  43, 131,  88 },
		{ 118,  43, 104,  87 },
		{    0,   0,   0,   0 },
		{    0,   0,   0,   0 }
	},
	{
		{ 122,  45, 122, 200 },
		{ 122, 200, 138, 156 },
		{ 122, 200, 106, 158 },
		{    0,   0,   0,   0 },
		{    0,   0,   0,   0 }
	},
	{
		{  75,  78, 201, 183 },
		{ 198,  66,  86, 193 },
		{  61, 135, 220, 128 },
		{ 135,  59, 145, 199 },
		{    0,   0,   0,   0 }
	},
	{
		{  60, 200, 126,  50 },
		{ 126,  50, 172, 199 },
		{ 172, 199,  46,  85 },
		{  46,  85, 199,  85 },
		{ 199,  85,  60, 200 }
	},
	{
		{  77, 204, 132,  64 },
		{ 132,  64, 175, 200 },
		{ 175, 200,  76, 204 },
		{    0,   0,   0,   0 },
		{    0,   0,   0,   0 }
	},
	{
		{  74, 190,  74,  73 },
		{  74,  73, 189,  74 },
		{ 189,  74, 189, 180 },
		{ 189, 180,  74, 190 },
		{    0,   0,   0,   0 }
	},
	{
		{ 127,  56, 127, 116 },
		{  69, 119, 186, 115 },
		{  94, 140, 161, 138 },
		{ 114, 162, 138, 161 },
		{    0,   0,   0,   0 }
	},
	{
		{ 121,  77,  81, 126 },
		{  81, 126, 119, 178 },
		{ 119, 178, 160, 127 },
		{ 160, 127, 121,  77 },
		{    0,   0,   0,   0 }
	},
	{
		{ 185,  81,  75,  81 },
		{  75,  81, 124, 158 },
		{ 124, 158, 185,  81 },
		{ 180, 158,  74, 158 },
		{    0,   0,   0,   0 }
	}
};
#endif
