#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Module.h"

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Blit
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX, bool flip = false) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);
	iPoint ScreenToWorld(int x, int y) const;

	//General lerp function
	int Lerp(int a, int b, float dt);
	int Full_Lerp(int a, int b, float lerp, float dt);

	//UI_Listener
	void ListenerUI(UIElement* UI_element);

private:
	//Camera control functions
	void AdjustCamera(float dt);
	void CD_Manager(float dt);
	void AdjustAnchorPoints();
	int GetSideOfScreen(int x);
	int CamLerp(int a, int b, float dt = 1.f); //adjusted to work with our camera, with our prefered default value
	bool manual = false; //manual camera control - automatic camera control

	void SnapAxis(float dt);
	void Vertical_Look(float dt);

public:

	SDL_Renderer*	renderer = nullptr;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
	
	// Variables used in camera control
	iPoint auxCam;
	int left_trigger_camera; 
	int right_trigger_camera;
	int left_trigger_change;
	int right_trigger_change;
	int up_trigger;
	int down_trigger;
	int camera_speed;
	bool snapping_to_right = false;
	bool snapping_to_left = false;
	int cameraCD;
	float lerp;//default lerp value. All lerps shall default to this value unless specified otherwise
	float fast_lerp; //value to use for faster movement

	enum PLAYER_SIDE
	{
		LEFT,
		RIGHT,
		NONE
	};

	enum SNAP_STATE
	{
		SNAP_TO_RIGHT,
		SNAP_TO_LEFT,
		SNAP_NONE
	};

	SNAP_STATE snap_state = SNAP_NONE;

	bool vsinc = false;

};

#endif // __j1RENDER_H__