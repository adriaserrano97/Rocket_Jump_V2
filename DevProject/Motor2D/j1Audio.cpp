#include "p2Defs.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "p2List.h"
#include "UIElement.h"
#include "Brofiler/Brofiler/Brofiler.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.create("audio");
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	BROFILER_CATEGORY("Audio Awake", Profiler::Color::AliceBlue)

	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	fxFolder.create(config.child("folder").child_value());

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	//Load SFX needed. The Load function returns the int we need to call it in PlayFx, so we store it for easier use.
	jump_sound =		LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("jump").as_string()));
	bomb_sound =		LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("bomb").as_string()));
	button_1 =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("button_1").as_string())); 
	button_3 =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("button_3").as_string()));
	coin =				LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("coin").as_string()));
	powerup =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("powerup").as_string()));
	option_denied = LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_denied").as_string()));
	/*
	
	option_denied_2 =	LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_denied_2").as_string()));
	option_denied_3 =	LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_denied_3").as_string()));
	option_denied_4 =	LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_denied_4").as_string()));
	option_selected =	LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_selected").as_string()));
	option_selected_2 = LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_selected_2").as_string()));
	option_selected_3 = LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("option_selected_3").as_string()));
	//button_2 =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("button_2").as_string()));
	powerup_2 =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("powerup_2").as_string()));
	powerup_3 =			LoadFx(PATH(fxFolder.GetString(), config.child("fx").attribute("powerup_3").as_string()));
	*/
	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	p2List_item<Mix_Chunk*>* item;
	for(item = fx.start; item != NULL; item = item->next)
		Mix_FreeChunk(item->data);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.add(chunk);
		ret = fx.count();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;
	id += 0;
	if(!active)
		return false;

	if(id > 0 && id <= fx.count())
	{
		Mix_PlayChannel(-1, fx[id - 1], repeat);
		//Input explanation: first argument is channel, -1  meanining first free channel. Second argument is a pointer to the chunk to play.
		//3rd argument is number of loops. if you want in once, put 0. -1 plays it "infinite" times.
	}

	return ret;
}

void j1Audio::ListenerUI(UIElement * UI_element)
{

	if(UI_element->name == "VerticalVolumeScrollBar") {
		
		int value = -UI_element->local_position.y * MAX_VOLUME / (UI_element->father->my_box->h - UI_element->my_box->h);

		Mix_VolumeMusic(value + MAX_VOLUME);
		Mix_Volume(-1, value + MAX_VOLUME);
	}

	else if (UI_element->name == "HorizontalVolumeScrollBar") {

		int value = -UI_element->local_position.x * MAX_VOLUME / (UI_element->father->my_box->w - UI_element->my_box->w);

		Mix_VolumeMusic(value + MAX_VOLUME);
		Mix_Volume(-1, value + MAX_VOLUME);
	}
}
