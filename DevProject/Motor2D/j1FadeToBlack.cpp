#include <math.h>
#include "j1App.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Player.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

j1FadeToBlack::j1FadeToBlack()
{
	LOG("FadeToBlack constructor activated");
}


j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeToBlack::PostUpdate() {

	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			if (to_disable != nullptr) {
				to_disable->Disable();
			}
			App->frames = 0;

			if (to_enable != nullptr)
			{
				to_enable->Enable();
			}
			
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time) {
			current_step = fade_step::none;
			App->player->godMode = false;
			App->player->freeze = false;
		}
	} break;
	}
	
	// Finally render the black square with alpha on the screen
	int screen_width = +(App->win->width);	//conversion needed since win uses these values as uint and SDL_Rect only works with int
	int screen_height = +(App->win->height);
	screen = { 0, 0, screen_width, screen_height}; 
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black the screen. At mid point, deactivate one module, then activate the other.
void j1FadeToBlack::FadeToBlack(j1Module* module_off, j1Module* module_on, float time)
{

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		to_disable = module_off;
		to_enable = module_on;

		App->player->godMode = true;
		App->player->freeze = true;
	}


}

//Fade without messing with modules. Used to "reset" positions in a visually smooth way
void j1FadeToBlack::FadeToBlack(float time) {

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		to_disable = nullptr;
		to_enable = nullptr;

		App->player->godMode = true;
		App->player->freeze = true;
	}


}

bool j1FadeToBlack::CleanUp() {
	
	to_disable = nullptr;
	to_enable = nullptr;

	return true;
}