#include "TileMap.h"
#include <fstream>
#include <iostream>

extern int tilesScrolled_Y;
extern int tilesScrolled_X;
extern std::vector<Sprite*> tileSpriteList;
extern Blit3D* blit3D;

TileMap::TileMap()
{
	//seed our rng
	std::random_device rd;
	rng.seed(rd());
}

void TileMap::Draw()
{
	//loop over our map and draw it
	for (int y = 0; y < 25 + tilesScrolled_Y; ++y)
	{
		for (int x = 0; x < 25 + tilesScrolled_X; ++x)
		{
			int tileXPosition = (x - tilesScrolled_X) * 16 + 8;
			int tileYPosition = blit3D->screenHeight - ((y - tilesScrolled_Y) * 16 + 8);
			if (validMapPosition(glm::vec2(y, x))) {
				tileSpriteList[map[y][x].backgroundTileNum]->Blit(tileXPosition, tileYPosition);
				if (map[y][x].foregroundTileNum != -1) {
					tileSpriteList[map[y][x].foregroundTileNum]->Blit(tileXPosition, tileYPosition);
				}
			}

			//foreground tile
			/*if (theMap->map[y * theMap->width + x].foregroundTileNum != -1)
				tileSpriteList[theMap->map[y * theMap->width + x].foregroundTileNum]->Blit((x - tilesScrolled_X) * 16 + 8, blit3D->screenHeight - ((y - tilesScrolled_Y) * 16 + 8));*/

		}
	}
}

bool TileMap::LoadMap(std::string filename)
{
	if (filename == "random")
	{
		//distribution for creating a map
		std::uniform_int_distribution<int> dist07(0, 7);

		width = 50;
		height = 50;

		// initialize map2d with height and width sizes
		map = std::vector<std::vector<Tile>>(
			height,
			std::vector<Tile>(width));

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				Tile t;
				t.foregroundTileNum = -1;
				t.backgroundTileNum = dist07(rng);
				t.movementCost = t.backgroundTileNum;
				map[x][y] = t;
			}
		}
		return true;
	}

	// load file
	std::ifstream mapFile;
	mapFile.open(filename);

	if (!mapFile.is_open()) {
		std::cout << "Can't open map file!" << std::endl;
		exit(-1);
	}

	std::string currLine;
	mapFile >> width;
	mapFile >> height;
	// initialize map2d with height and width sizes
	map = std::vector<std::vector<Tile>>(
		height,
		std::vector<Tile>(width));

	// load background tiles
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Tile t;
			t.foregroundTileNum = -1;
			mapFile >> t.backgroundTileNum;
			t.movementCost = t.backgroundTileNum;
			t.coord = glm::vec2(i, j);
			map[i][j] = t;
		}
	}

	mapFile.close();
	return true;
}

bool TileMap::SaveMap(std::string filename)
{
	//std::ofstream mapFile;
	//mapFile.open("mapfile.dat");
	//
	//if (!mapFile.is_open()) {
	//	std::cout << "Can't open map file!" << std::endl;
	//	exit(-1);
	//}

	//mapFile << width << std::endl;
	//mapFile << height << std::endl;

	//// write background tiles
	//for (int i = 0; i < height; i++) {
	//	for (int j = 0; j < width; j++) {
	//		mapFile << map[(i * width) + j].backgroundTileNum << " ";
	//	}
	//	mapFile << std::endl;
	//}

	//// write foregroundTile
	//for (int i = 0; i < height; i++) {
	//	for (int j = 0; j < width; j++) {
	//		mapFile << map[(i * width) + j].foregroundTileNum << " ";
	//	}
	//	mapFile << std::endl;
	//}

	//mapFile.close();

	return true;
}

bool TileMap::validMapPosition(glm::vec2 coord) {
	return (coord.y >= 0 && coord.x >= 0 && coord.x < width && coord.y < height);
}