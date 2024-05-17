#pragma once
#include <vector>
#include <string>
#include <random>
#include "Blit3D.h"

class Tile
{
public:
	int foregroundTileNum;
	int backgroundTileNum;
	int movementCost = 999999.f;
	float gCost = 99999.f;
	float hCost = 99999.f;
	float fCost = 99999.f;
	glm::vec2 coord;
	glm::vec2 parent;
};

class TileMap
{
public:
	//std::vector<Tile> map;
	std::vector<std::vector<Tile>> map;
	int width = 0;
	int height = 0;
	std::mt19937 rng; //for randomizing map data
	glm::vec2 goal;
	TileMap();
	void Draw();
	bool LoadMap(std::string filename);
	bool SaveMap(std::string filename);
	bool validMapPosition(glm::vec2 coord);
};