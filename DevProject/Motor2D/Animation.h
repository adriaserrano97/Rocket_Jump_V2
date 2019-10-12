#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Frame.h"
#include "SDL/include/SDL_rect.h"
#include "j1Collision.h"
#include "j1App.h"

#define MAX_FRAMES 55

class Animation
{
public:
	bool loop = true;
	Frame frames[MAX_FRAMES];

private:
	int frames_passed = 0;
	int current_frame;
	int last_frame = 0;
	int loops = 0;

public:

	Animation PushPlayerAnimation(pugi::xml_node node, p2SString name) {
		Animation aux;
		pugi::xml_node iterator = node.child("playerAnimations").first_child();
		bool ret = true;
		for (iterator; iterator && ret; iterator = iterator.next_sibling())
		{
			if (name == iterator.attribute("name").as_string())
			{
				ret = false;
				pugi::xml_node iteratorFrames = iterator.first_child();
				for (int y = 0; y < iterator.attribute("frames").as_int(); y++)
				{
					SDL_Rect rect;
					rect.x = iteratorFrames.attribute("rposX").as_int();
					rect.y = iteratorFrames.attribute("rposY").as_int();
					rect.w = iteratorFrames.attribute("rwidth").as_int();
					rect.h = iteratorFrames.attribute("rheight").as_int();

				/*if (iteratorFrames.attribute("nColliders").as_int() == 0)
					{
						SDL_Rect colRect[1]; 
						COLLIDER_TYPE colType[1]; 
						j1Module* callback[1];

						colRect[0] = { 0, 0, 20, 50 };
						colType[0] = COLLIDER_PLAYER;
						callback[0] = (j1Module*)App->player;*/
						aux.PushBack(rect, iteratorFrames.attribute("time").as_int(), { iteratorFrames.attribute("pivotX").as_int(), iteratorFrames.attribute("pivotY").as_int() });
					//}
					iteratorFrames = iteratorFrames.next_sibling();
				}
			}
		}
		aux.current_frame = 0;
		return aux;
	}
	Animation PushParticleAnimation(pugi::xml_node node, p2SString name) {
		Animation aux;
		pugi::xml_node iterator = node.child("particleAnimations").first_child();
		bool ret = true;
		for (iterator; iterator && ret; iterator = iterator.next_sibling())
		{
			if (name == iterator.attribute("name").as_string())
			{
				ret = false;
				pugi::xml_node iteratorFrames = iterator.first_child();
				for (int y = 0; y < iterator.attribute("frames").as_int(); y++)
				{
					SDL_Rect rect;
					rect.x = iteratorFrames.attribute("rposX").as_int();
					rect.y = iteratorFrames.attribute("rposY").as_int();
					rect.w = iteratorFrames.attribute("rwidth").as_int();
					rect.h = iteratorFrames.attribute("rheight").as_int();

					/*if (iteratorFrames.attribute("nColliders").as_int() == 0)
						{
							SDL_Rect colRect[1];
							COLLIDER_TYPE colType[1];
							j1Module* callback[1];

							colRect[0] = { 0, 0, 20, 50 };
							colType[0] = COLLIDER_PLAYER;
							callback[0] = (j1Module*)App->player;*/
					
							
					
					aux.PushBack(rect, iteratorFrames.attribute("time").as_int(), { iteratorFrames.attribute("pivotX").as_int(), iteratorFrames.attribute("pivotY").as_int() });
					//}
					iteratorFrames = iteratorFrames.next_sibling();
				}
			}
		}
		aux.current_frame = 0;
		return aux;
	}
	
	void PushBack(const SDL_Rect& rect, const int maxFrames, p2Point <int> pivotPosition) {
		
		frames[last_frame].frame = rect;
		frames[last_frame].maxFrames = maxFrames;
		frames[last_frame].pivotPosition = pivotPosition;

		last_frame++;
	}

	Frame& GetCurrentFrame()
	{
		if (frames_passed < frames[current_frame].maxFrames) {
			frames_passed++;
		}
		else {
			frames_passed = 0;
			current_frame++;
		}

		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[current_frame];
	}

	SDL_Rect& GetCurrentFrameBox()
	{
		return GetCurrentFrame().frame;
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void ResetAnimation()
	{
		frames_passed = 0;
		current_frame = 0;
	}
};

#endif