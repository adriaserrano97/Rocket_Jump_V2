#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "Brofiler/Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Scene.h"


j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

//called at start
bool j1PathFinding::Start() {
	BROFILER_CATEGORY("Pathfinding Start", Profiler::Color::DarkOrange)

	return true;
}

// Called before quitting
bool j1PathFinding::CleanUp()
{

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

void j1PathFinding::PrintLastPath() {

	//Print the path, just debug
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->PosConverter(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
	}

}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) 
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() 
{
	p2List_item<PathNode>* ret = NULL;
	float min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(float g, float h, const iPoint& pos, PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode( PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill)
{
	iPoint cell;

	bool north = false;
	bool south = false;
	bool east = false;
	bool west = false;

	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
		north = true;
	}

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
		south = true;
	}

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
		east = true;
	}

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)){
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
		west = true;
	}


	
	if (north)
	{
		// north-east
		if (east)
		{
			cell.create(pos.x + 1, pos.y + 1);
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
			}
		}

		// north-west
		if (west)
		{
			cell.create(pos.x - 1, pos.y + 1);
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
			}
		}
		
	}

	if (south)
	{
		// south-east
		if (east)
		{
			cell.create(pos.x + 1, pos.y - 1);
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
			}
		}

		// south-west
		if (west)
		{
			cell.create(pos.x - 1, pos.y - 1);
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
			}
		}

	}
	

	

	

	

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
float PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
float PathNode::CalculateF(const iPoint& destination)
{
	if (pos.x != parent->pos.x && pos.y != parent->pos.y) {

		g = parent->g + 1.5;
	}

	else {
		g = parent->g + 1;
	}

	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("Pathfinder create path", Profiler::Color::Orchid)
	// TODO 1: if origin or destination are not walkable, return -1
	if (IsWalkable(origin) == false || IsWalkable(destination) == false) {
		return -1;
	}
	// TODO 2: Create two lists: open, close

	last_path.Clear();

	PathList open; PathList closed;
	// Add the origin tile to open
	open.list.add(PathNode(0, origin.DistanceTo(destination), origin, NULL));
	// Iterate while we have tile in the open list

	PathNode* current_node;

	while (open.GetNodeLowestScore() != NULL)
	{	
		current_node = new PathNode(open.GetNodeLowestScore()->data);
		// TODO 3: Move the lowest score cell from open list to the closed list
		closed.list.add(*current_node);
		open.list.del(open.Find(current_node->pos));
	// TODO 4: If we just added the destination, we are done!
	// Backtrack to create the final path
	// Use the Pathnode::parent and Flip() the path when you are finish... �ed?
		if (current_node->pos == destination) {
			
			PathNode* iterator = current_node;

			for (iterator; iterator->pos != origin; iterator = iterator->parent) {
				last_path.PushBack(iterator->pos);

			}
			last_path.PushBack(origin);

			last_path.Flip();
			return 0;
			
		}
	
	// TODO 5: Fill a list of all adjancent nodes
		PathList Adjacent_list;
		uint limit = current_node->FindWalkableAdjacents(Adjacent_list);
	// TODO 6: Iterate adjancent nodes:	
		for (uint i = 0; i < limit; i++) {
			// ignore nodes in the closed list <======> do things only if we didnt find them
			if ((closed.Find(Adjacent_list.list[i].pos)) == NULL) {
				// If it is NOT found, calculate its F and add it to the open list
				if ((open.Find(Adjacent_list.list[i].pos)) == NULL) {
					Adjacent_list.list[i].CalculateF(destination);
					open.list.add(Adjacent_list.list[i]);
				}
				else { // If it is already in the open list, check if it is a better path (compare G)
					Adjacent_list.list[i].CalculateF(destination);
					if ( Adjacent_list.list[i].g < open.Find(Adjacent_list.list[i].pos)->data.g) {
						// If it is a better path, Update the parent
						//open.Find(Adjacent_list.list[i].pos)->data.parent = Adjacent_list.list[i].parent;
						
						open.list.del(open.Find(Adjacent_list.list[i].pos));
						open.list.add(Adjacent_list.list[i]);
					}
				} 
			}
		}
	}
}
