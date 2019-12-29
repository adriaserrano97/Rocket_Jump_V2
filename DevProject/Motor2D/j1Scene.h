#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


	// Save functions
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void OnCollision(Collider* c1, Collider* c2);

	//UI_Listener
	void ListenerUI(UIElement* UI_element);

	//Tracker of current scene
	int scene_number = 0;

	SDL_Texture* debug_tex = nullptr;

	//maps
	p2SString map1;
	p2SString map2;
	

	//Main Menu
	bool LoadIntroMenu();
	void DisplayCredits();
	p2SString intro_menu;
	
public:
	bool				load_from_save;

private:
	bool				MainMenu;
	bool				inGameMenu;
	bool				credits;
	bool				CloseGameFromMenu;
	p2SString			folder;
	UIElement*			uiElements[15];
private:
	void				ClearUIArray();
	bool				AddUIElementToArray(UIElement* element);
	int					FindElementinArray(UIElement* element); //returns position if found, -1 if not.
	int					FindElementinArrayName(p2SString* name);
};

#endif // __j1SCENE_H__