#ifndef __FRAME_H__
#define __FRAME_H__

#include "p2Point.h"
#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

enum COLLIDER_TYPE;

class Frame
{
public:
	float maxFrames;
	SDL_Rect frame;
	p2Point <int> pivotPosition;

private:
	int last_collider = 0;

public:
	

};

#endif
