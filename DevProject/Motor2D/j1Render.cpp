#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Render.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}
	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	//defining our anchor points for camera control
	AdjustAnchorPoints();
	return true;
}

bool j1Render::Update(float dt)
{
	return true;
}

bool j1Render::PostUpdate()
{
	//Make Camera movement
	AdjustCamera();

	//Draw everything
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	
	renderer = nullptr;

	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");


	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivot_x, int pivot_y, bool flip) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	if (speed != NULL)
	{
		rect.x = (int)(camera.x * speed) + x * scale;
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	else
	{
		rect.x = (int)(camera.x * 1.0f) + x * scale;
		rect.y = (int)(camera.y * 1.0f) + y * scale;
	}
	

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, NULL, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Camera control functions
void j1Render::AdjustCamera() {

	auxCam.x = -camera.x; //camera has axis flipped in relation to player position
	auxCam.y = -camera.y;
	

	//Change X
	//---------------------------------

	if (snap_state != SNAP_NONE) { SnapAxis(); }
	switch (GetSideOfScreen(App->player->position.x)) { //where is the player?

	case LEFT://player on left side
		//going right
		if (App->player->position.x > left_trigger_camera) {
			auxCam.x += abs(CamLerp(App->player->position.x, left_trigger_camera));
		} //follow player

		//going left enough
		if (App->player->position.x <= left_trigger_change) {
			snap_state = SNAP_TO_RIGHT;//produce change of perspective
		}
		break;

	case RIGHT://player on right side
		//going left
		if (App->player->position.x < right_trigger_camera) {
			auxCam.x -= abs(CamLerp(right_trigger_camera, App->player->position.x));
		}//follow player

		//going right enough
		if (App->player->position.x >= right_trigger_change) {
			snap_state = SNAP_TO_LEFT;//produce change of perspective
		}

		break;
	}	
	
	//respect margins
	if (auxCam.x < 0) {
		auxCam.x = 0;
	}
	if (auxCam.x + App->win->width > App->map->data.width*App->map->data.tile_width) {
		auxCam.x = App->map->data.width*App->map->data.tile_width - App->win->width;
	}


	//Change Y
	//---------------------------------

	//adjust up
	if (App->player->position.y < up_trigger) { //high enough
		auxCam.y -= abs(CamLerp(App->player->position.y, up_trigger));//adjust down
	} 
	
	if ((App->player->position.y + App->player->collider->rect.h) > down_trigger) {//low enough
		auxCam.y += abs(CamLerp(App->player->position.y + App->player->collider->rect.h, down_trigger));//adjust high
	} 

	//respect margins	
	if (auxCam.y < 0) {
		auxCam.y = 0;
	}
	if (App->map->data.height*App->map->data.tile_height <auxCam.y + App->win->height) {
		auxCam.y = App->map->data.height*App->map->data.tile_height - App->win->height;
	}
	

	//Assign proper values
	camera.x = -auxCam.x;
	camera.y = -auxCam.y;
}

void j1Render::Vertical_Look() {
	if (App->player->godMode && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		camera.y++;
	}

	//TODO

}

//Returns lerp% distance from a to b
int j1Render::CamLerp(int a, int b) {
	if (abs(b - a) >= 5)return (int)(lerp * (b - a));
	else return (b-a);
	//if destination is close enough, snap to it
}

//General Lerp function
int j1Render::Lerp(int a, int b, float t){
		return (int)(a + lerp * (b - a));
}

//Define where does our camera have triggers to change perspective
void j1Render::AdjustAnchorPoints() {
	
	//remember, camera axis are flipped, hence the (-1)*

	left_trigger_camera = (-1)*(int)(camera.x - (App->win->width / 4));	

	right_trigger_camera = (-1)*(int)(camera.x - (3 * App->win->width / 4)); 

	left_trigger_change = (-1)*(int)(camera.x - (App->win->width / 12));

	right_trigger_change = (-1)*(int)(camera.x - (11 * App->win->width / 12));

	up_trigger = (-1)*(int)(camera.y - App->win->height / 3);

	down_trigger = (-1)*(int)(camera.y - (2 * App->win->height / 3));
}


int j1Render::GetSideOfScreen(int x){

	uint distance_to_L = abs(x - left_trigger_camera);
	uint distance_to_R = abs(x - right_trigger_camera);

	if (distance_to_L < distance_to_R) { 
		return LEFT; 
	}

	else if (distance_to_L > distance_to_R) { 
		return RIGHT;
	}

	else { 
		LOG("Unable to assert position of object on screen"); 
		return NONE; 
	}
}


void j1Render::SnapAxis() {
	
	//Check if we need to change camera perspective

	switch (snap_state) {

	case SNAP_TO_RIGHT:
	
		auxCam.x -= abs(CamLerp(right_trigger_camera,left_trigger_change));
		if (App->player->position.x >= right_trigger_camera) { 
			snap_state = SNAP_NONE;
		}
		break;

	case SNAP_TO_LEFT:

		auxCam.x += abs(CamLerp(right_trigger_change, left_trigger_camera));
		if (App->player->position.x <= left_trigger_camera) { 
			snap_state = SNAP_NONE; 
		}
		break;	
	}
}