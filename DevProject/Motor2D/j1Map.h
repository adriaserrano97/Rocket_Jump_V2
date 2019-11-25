#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


/*
Okay. This one is gonna be a long explanation.

First things first: what represents Mongolia to us? It all starts in the 13th century. Under the command of Genghis Khan (born as Temüjin) the Mongol Empire ruled over the vast
majority of the asian continent. In further expansions, both under his command and the command of his heirs (once he joined the departed), Mongol Empire expanded both West
to Europe and South. Why is this important? Well, because the first Empire that was taken over was... China. If not for that, Mongol Empire would have never been the powerhouse that ended up beign.

China was and has always beign a cultural reference in antique times, starting from times way before the Roman Empire. Once Mongol Empire took over, they adopted all cultural and structural organization
methods that China had been developing for the past 3000+ years. Used to rule a vast territory from a centralized capital, China had all the structural and politicial tools needed to, in conjunction with
a most impressive burocratial flowchart of control, rule in perfect coordination all his provinces. In contrast, Mongols had always function in small tribes, living as nomads. Thus, their power structures had
nothing to do with controlling territory, as there was no territory to control (Mongol steppe is famously harsh, so keeping on foot is almost mandatory back at the time).

For all those reasons, historians and traditional mongol culture see their invasion of China as a golden chapter in Mongol History. A perfect oportunity for a underdeveloped political empire to
adquire HUGE knowledge and technological development, sustained by a militar prowess (the horde of Genghis Khan, which would later be Ödegai, the last great khan before the great division of the Empire, 
which give birth to the chinease descendents of the Yuren-Chin and Kiri-Kithian, and the heirs of the mongols which stayed in the steppe, the two sister tribes of Black Sheep and White Sheep, which would later
merge into the Golden Horde which would raid Europe from the 14ths to the late 15ths).

Thus, from a historical point of view, "leaving Mongolia" represented a move which granted great knowledge. 
It represents the action of adquiring greater wisdom. 
A golden oportunity to ascend to a higher level of understanding.

This is us, now.
The next includes is our own "leaving Mongolia".
It represents the culmination of our knowledge, focused in two of the most unlikely and bizarre lines of code you would ever see.

Rejoyce, and ascend with us to a higher plane of understanding.

Take our had,
	Leave Mongolia

*/
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )//Leaving Mongolia
#include "SDL_image/include/SDL_image.h"//Leaving Mongolia



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
	int					type[ARRAY_MAX_COLLIDERS];
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
	void Unload();

	// Go from map position to world position
	iPoint PosConverter(int x, int y);
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
private:

	//Information loading related functions
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetCollisions(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadEnemies(pugi::xml_node& tileset_node, Layer* set);
	bool LoadLayer(pugi::xml_node& node, Layer* layer);
	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:
	Collider* col[MAX_MAP_COLLIDERS];
	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__