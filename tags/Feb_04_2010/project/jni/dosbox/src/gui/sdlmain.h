#include "SDL.h"

struct SDL_Block {
	bool active;							//If this isn't set don't draw
	bool updating;

	struct {
		Bit32u width;
		Bit32u height;
		Bit32u toolbarstart;
		bool fullscreen;
		bool rotateright;
		Bit32u tzx1;
		Bit32u tzy1;
		Bit32u tzx2;
		Bit32u tzy2;
	} window;

	SDL_Surface * surface;
	SDL_cond *cond;

	struct {
	       /* put emulation here ? */	       
	       bool noclick;	
		   int  delayticks;
		   bool needticks;
		   bool right;
	       Bit32u sensitivity;
	} mouse;

/* Move mouse with keys */
	struct {
		   bool on; 
		   float x;
		   float y;		
	} dpad;

/* "Touchpad" emulation */
	struct {
		   bool active;
		   bool on;
		   int x;
		   int y;
		   bool moved;		   
		   Bit32u releasedelay;
		   Bit32u clickms;
		   Bit32u rightclickdelay;	
	} tpad;

/* Toolbar */
	struct {
		   int scrollpos;
		   int oldscpos;
		   int height;
		   int vislines;
		   int prev;
		   int current;
		   int invalid;
		   int forcedsize;
		   bool scrollactive;		   
		   bool forced;        		   
		   bool transparent;
		   Bit16u gfx[320*120];
		   Bit8u keycode[320*120];
		   Bit8u pressed [255];
	} toolbar;

	//SDL_Rect updateRects[1024];

	Bitu num_joysticks;

#if defined (WIN32)
	bool using_windib;
#endif

	// state of alt-keys for certain special handlings
	Bit8u laltstate;
	Bit8u raltstate;

} __attribute__((aligned(4)));
