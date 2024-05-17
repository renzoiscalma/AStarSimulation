#pragma once
#include <algorithm> //need this to use std::sort
#include <vector>
#include "TileMap.h"

class Knight
{
public:
	glm::vec2 position;
	glm::vec2 startPosition;
	std::vector<Tile *> path;
	int pathIdx;
	bool pathfound;
	bool followingPath;
	bool finished;
	float walkTimer = 0.3f;
	float currWalktimer = 0.f;
	Knight(int x, int y);
	void Draw();
	void pathfind(glm::vec2 goal);
	void Update(double seconds);
};