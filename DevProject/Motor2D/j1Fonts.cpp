#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "Brofiler/Brofiler/Brofiler.h"

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

j1Fonts::j1Fonts() : j1Module()
{
	name.create("fonts");
}

// Destructor
j1Fonts::~j1Fonts()
{}

// Called before render is available
bool j1Fonts::Awake(pugi::xml_node& conf)
{
	BROFILER_CATEGORY("Fonts Awake", Profiler::Color::DarkGray)
	
	bool ret = true;

	if (TTF_Init() == -1)
	{
		ret = false;
	}
	else
	{
		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		default = Load(path, size);
	}

	return ret;
}

// Called before quitting
bool j1Fonts::CleanUp()
{
	p2List_item<TTF_Font*>* item;

	for (item = fonts.start; item != NULL; item = item->next)
	{
		TTF_CloseFont(item->data);
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const j1Fonts::Load(const char* path, int size)
{
	TTF_Font* font = TTF_OpenFont(path, size);

	if (font == NULL)
	{
	}
	else
	{
		fonts.add(font);
	}

	return font;
}

// Print text using font
SDL_Texture* j1Fonts::Print(const char* text, SDL_Color color, TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default, text, color);

	if (surface == NULL)
	{
	}
	else
	{
		ret = App->tex->LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool j1Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret;

	if (TTF_SizeText((font) ? font : default, text, & width, & height) != 0)
		ret = false;
	else
		ret = true;

	return ret;
}