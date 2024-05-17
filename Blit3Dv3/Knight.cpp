#include "Knight.h"

extern Sprite* knightSprite;
extern int tilesScrolled_Y;
extern int tilesScrolled_X;
extern Blit3D* blit3D;
extern TileMap* theMap;

Knight::Knight(int x, int y)
{
	position = glm::vec2(y, x);
	startPosition = glm::vec2(y, x);
	pathfound = false;
	followingPath = false;
	finished = false;
}

void Knight::Draw()
{
	int tileXPosition = (position.y - tilesScrolled_X) * 16 + 8;
	int tileYPosition = blit3D->screenHeight - ((position.x - tilesScrolled_Y) * 16 + 8);
	knightSprite->Blit(tileXPosition, tileYPosition);
}

bool sortByF(Tile* A, Tile* B)
{
	if (A->fCost < B->fCost) return 1;

	return 0;
}

std::vector<glm::vec2> direction = {
	glm::vec2(-1, 0),		// direction for UP
	glm::vec2(1, 0),		// direction for DOWN
	glm::vec2(0, -1),		// direction for LEFT
	glm::vec2(0, 1),		// direction for RIGHT
};

float computeHeuristicCost(glm::vec2 coord1, glm::vec2 coord2)
{
	return abs(coord1.x - coord2.x) + abs(coord1.y - coord2.y);
}

bool inVector(Tile* t, std::vector<Tile*> v)
{
	for (int i = 0; i < v.size(); i++) {
		if (t == v[i]) return true;
	}
	return false;
}

void deleteInVector(Tile* t, std::vector<Tile*> v) 
{
	for (int i = 0; i < v.size(); i++)
	{
		if (t == v[i]) {
			v.erase(v.begin() + i);
			return;
		}
	}
	std::cout << "not deleted";
}

void Knight::Update(double seconds)
{
	if ((pathfound && !followingPath) && pathIdx >= 0)
	{
		currWalktimer += seconds;
		if (currWalktimer > walkTimer)
		{
			position = path[pathIdx]->coord;
			currWalktimer = 0.f;
			pathIdx--;
			if (pathIdx < 0) {
				finished = true;
			}
		}
	}
}

void Knight::pathfind(glm::vec2 goal)
{
	bool finished = false;
	std::vector<Tile*> openList;
	Tile* initialPos = &(theMap->map[startPosition.x][startPosition.y]);
	initialPos->gCost = 0;
	initialPos->hCost = computeHeuristicCost(initialPos->coord, goal);
	initialPos->fCost = initialPos->gCost + initialPos->hCost;
	openList.push_back(initialPos);
	std::vector<Tile*> closedList;
	while (!finished)
	{
		std::sort(openList.begin(), openList.end(), sortByF);
		Tile* current = openList[0];
		openList.erase(openList.begin());
		closedList.push_back(current);
		if (current->coord.y == goal.y && current->coord.x == goal.x)
		{
			current->foregroundTileNum = 2;
			finished = true;
			break;
		}
		for (int i = 0; i < 4; i++) {
			glm::vec2 neighborCoord = current->coord + direction[i];
			//std::cout << neighborCoord.x << " " << neighborCoord.y << std::endl;
			if (!theMap->validMapPosition(neighborCoord)) continue;

			Tile* neighbor = &(theMap->map[neighborCoord.x][neighborCoord.y]);
			//theMap->map[neighborCoord.x][neighborCoord.y].foregroundTileNum = 450;
			//theMap->map[current->coord.x][current->coord.y].foregroundTileNum = 450;
			float cost = (current->gCost) + neighbor->movementCost;
			if (inVector(neighbor, openList) && cost < neighbor->gCost)
			{
				deleteInVector(neighbor, openList);
			}
			if (inVector(neighbor, closedList) && cost < neighbor->gCost)
			{
				deleteInVector(neighbor, closedList);
				std::cout << "well, this happened";
			}
			if (!inVector(neighbor, closedList) && !inVector(neighbor, openList))
			{
				neighbor->gCost = cost;
				neighbor->fCost = neighbor->gCost + computeHeuristicCost(neighborCoord, goal);
				neighbor->parent = current->coord;
				openList.push_back(neighbor);
			}
		}
	}
	pathfound = true;
	// reconstruct path to vector
	Tile* t = &(theMap->map[goal.x][goal.y]);
	path.push_back(t);
	while (computeHeuristicCost(t->coord, startPosition) != 0) {
		std::cout << t->parent.x << " " << t->parent.y << ": g = " << t->gCost << std::endl;
		theMap->map[t->coord.x][t->coord.y].foregroundTileNum = 123;
		t = &(theMap->map[t->parent.x][t->parent.y]);
		path.push_back(t);
	}
	theMap->map[t->coord.x][t->coord.y].foregroundTileNum = 123;
	std::cout << t->parent.x << " " << t->parent.y << ": g = " << t->gCost << std::endl;
	pathIdx = path.size() - 1;
}