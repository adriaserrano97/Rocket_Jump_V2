#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#define ARRAY_MAX_COLLIDERS 50
#define MAX_MAP_COLLIDERS 400  

// ----------------------------------------------------
struct Layer {
	p2SString name;
	uint width;
	uint height;
	uint* gid;
	float layer_speed;
	inline uint Get(int x, int y) const
	{
		return x + y * width;
	}
};

// ----------------------------------------------------
struct TileSet
{
	//Tileset information
	SDL_Rect TileToRect(uint tileid);
	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	int					tile_count;
	
	//Information about the texture related to tileset
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;

	//Information about the colliders associated with the tileset
	SDL_Rect			collisionBoxArray[ARRAY_MAX_COLLIDERS];
	bool				transpassable[ARRAY_MAX_COLLIDERS];
	SDL_Rect getrect(int firstgid) {}
};
// ----------------------------------------------------
enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<Layer*>		layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Go from map position to world position
	iPoint PosConverter(int x, int y);
private:

	//Information loading related functions
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetCollisions(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, Layer* layer);

public:

	MapData data;
	iPoint playerStart; //defined in each map

private:
	Collider* col[MAX_MAP_COLLIDERS];
	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__