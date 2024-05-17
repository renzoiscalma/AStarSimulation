/*
Example program that shows how to display tile maps
*/
#include "Blit3D.h"

#include "Knight.h"
#include "TileMap.h"


Blit3D *blit3D = NULL;

//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

//GLOBAL DATA
//sprite pointers
Sprite *tileSpriteSheet = NULL;

std::vector<Sprite *> tileSpriteList;

TileMap *theMap = NULL;

Knight* knight = NULL;

enum class GameMode { PICKING, EDITING, PLAYING };
GameMode gameMode;
enum class PlayingMode { START_POSITION, GOAL_POSITION, PATH_FINDING };
PlayingMode playingMode;
enum class EditMode { BACKGROUND, FOREGROUND };
EditMode editMode;
Sprite* knightSprite = NULL;

AngelcodeFont *afont = NULL;

//mouse cursor
float cursor_x = 0;
float cursor_y = 0;

//edit tile number
int editTileNum = 0;

int tilesScrolled_X = 0;
int tilesScrolled_Y = 0;

// time based scrolling
enum class ScrollState { LEFT, RIGHT, UP, DOWN, NONE };
ScrollState scrollState = ScrollState::NONE;

float elapsedTime = 0;
// time slice of 60th of a second
float timeSlice = 1.f / 60.f;

int MAP_VIEW_SIZE = 25;
void Init()
{
	//Angelcode font
	afont = blit3D->MakeAngelcodeFontFromBinary32("Media\\Oswald_72.bin");

	//load the tilemap sprite
	tileSpriteSheet = blit3D->MakeSprite(0, 0, 480, 256, "Media\\BOF22_edited.png");

	//load individual 16x16 tiles
	for (int y = 0; y < 256 / 16; ++y)
	{
		for (int x = 0; x < 480 / 16; ++x)
		{
			tileSpriteList.push_back(blit3D->MakeSprite(x * 16, y * 16, 16, 16, "Media\\BOF22_edited.png"));
		}
	}

	knightSprite = blit3D->MakeSprite(0, 0, 52, 52, "Media\\Knight.png");

	theMap = new TileMap();
	
	//passing "random" as the map name causes the TileMap class to create a random map,
	//instead of loading one from a file.
	theMap->LoadMap("map.txt");

	gameMode = GameMode::PLAYING;
	editMode = EditMode::BACKGROUND;
}

void DeInit(void)
{
	if (theMap) delete theMap;
}

void Update(double seconds)
{
	if (seconds < 0.15) elapsedTime += static_cast<float>(seconds);
	else elapsedTime += 0.15f;

	while (elapsedTime >= timeSlice)
	{
		// update loop. every 60th of a second this loop will run
		elapsedTime -= timeSlice;
		if (knight != NULL)
		{
			knight->Update(timeSlice);
		}
		switch (scrollState)
		{
		case ScrollState::DOWN:
			if (tilesScrolled_Y < theMap->height - MAP_VIEW_SIZE)
				tilesScrolled_Y++;
			break;
		case ScrollState::UP:
			if (tilesScrolled_Y > 0)
				tilesScrolled_Y--;
			break;
		case ScrollState::RIGHT:
			if (tilesScrolled_X < theMap->width - MAP_VIEW_SIZE)
				tilesScrolled_X++;
			break;
		case ScrollState::LEFT:
			if (tilesScrolled_X > 0)
				tilesScrolled_X--;
			break;
		case ScrollState::NONE:
		default:
			break;
		}
	}
}

void Draw(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);	//clear colour: r,g,b,a 	
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here
	switch (gameMode)
	{
	//case GameMode::EDITING:
	//	//loop over our map and draw it
	//	for (int y = 0; y < MAP_VIEW_SIZE + tilesScrolled_Y; ++y)
	//	{
	//		for (int x = 0; x < MAP_VIEW_SIZE + tilesScrolled_X; ++x)
	//		{
	//			tileSpriteList[theMap->map[y * theMap->width + x].backgroundTileNum]->Blit((x - tilesScrolled_X) * 16 + 8, blit3D->screenHeight - ((y - tilesScrolled_Y) * 16 + 8));

	//			//foreground tile
	//			if (theMap->map[y*theMap->width + x].foregroundTileNum != -1)
	//				tileSpriteList[theMap->map[y*theMap->width + x].foregroundTileNum]->Blit((x - tilesScrolled_X) * 16 + 8, blit3D->screenHeight - ((y - tilesScrolled_Y) * 16 + 8));

	//		}
	//	}

	//	if(editTileNum != -1) tileSpriteList[editTileNum]->Blit(500, blit3D->screenHeight - 300);
	//	else afont->BlitText(500, blit3D->screenHeight - 300, "Empty FG");

	//	//draw mouse coords - this is here to help us debug issues with our logic
	//	{
	//		int y = (cursor_y / 16) + tilesScrolled_Y;
	//		int x = (cursor_x / 16) + tilesScrolled_X;
	//		std::string coords;
	//		coords = "X:" + std::to_string((int)cursor_x);
	//		afont->BlitText(500, blit3D->screenHeight - 40, coords);
	//		coords = "Y:" + std::to_string((int)cursor_y);
	//		afont->BlitText(500, blit3D->screenHeight - 120, coords);
	//		if (cursor_y < 0 || cursor_y > MAP_VIEW_SIZE * 16 ||
	//			cursor_x < 0 || cursor_x > MAP_VIEW_SIZE * 16)
	//			coords = "tile #: error";
	//		else
	//			coords = "tile #: " + std::to_string((int)(y * theMap->width) + x);
	//		afont->BlitText(430, blit3D->screenHeight - 200, coords);
	//	}

	//	switch (editMode)
	//	{
	//	case EditMode::BACKGROUND:
	//		afont->BlitText(10, 80, "Editing background tiles");
	//		break;

	//	case EditMode::FOREGROUND:
	//		afont->BlitText(10, 80, "Editing foreground tiles");
	//		break;
	//	}
	//	break;

	//case GameMode::PICKING:
	//	//draw our tilemap
	//	tileSpriteSheet->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);

	//	afont->BlitText(10, 80, "Pick a tile to draw with.");

	//	//draw mouse coords - this is here to help us debug issues with our logic
	//	{
	//		std::string coords;
	//		coords = "X:" + std::to_string((int)cursor_x);
	//		afont->BlitText(70, blit3D->screenHeight - 10, coords);
	//		coords = "Y:" + std::to_string((int)cursor_y);
	//		afont->BlitText(250, blit3D->screenHeight - 10, coords);

	//		// for debugging purposes
	//		int tile_x = -(int)(blit3D->screenWidth / 2) + (int)(480 / 2) + cursor_x;
	//		int tile_y = -(int)(blit3D->screenHeight / 2) + (int)(256 / 2) + cursor_y;
	//		bool outOfBounds = (tile_x < 0 || tile_x > 480) || (tile_y < 0 || tile_y > 256);
	//		int tile = ((tile_y / 16) * 30) + (tile_x / 16);

	//		if (!outOfBounds)
	//			coords = "tile #:" + std::to_string((int)tile);
	//		else if (outOfBounds && editMode == EditMode::FOREGROUND)
	//			coords = "tile #: Empty FG";
	//		else
	//			coords = "tile #: error";
	//		afont->BlitText(450, blit3D->screenHeight - 10, coords);
	//	}
	//	break;
	case GameMode::PLAYING:
		theMap->Draw();
		if (knight != NULL)
		{
			knight->Draw();
		}
		//draw mouse coords - this is here to help us debug issues with our logic
		{
			int y = (cursor_y / 16) + tilesScrolled_Y;
			int x = (cursor_x / 16) + tilesScrolled_X;
			std::string coords;
			coords = "X:" + std::to_string((int)x);
			afont->BlitText(500, blit3D->screenHeight - 40, coords);
			coords = "Y:" + std::to_string((int)y);
			afont->BlitText(500, blit3D->screenHeight - 120, coords);
			if (y < 0 || y > theMap->height - 1 ||
				x < 0 || x > theMap->width - 1)
				coords = "tile #: error";
			else
				coords = "tile score: " + std::to_string(theMap->map[(int)y][(int)x].movementCost);
			afont->BlitText(430, blit3D->screenHeight - 200, coords);
		}
		afont->BlitText(10, 80, "Arrow keys to scroll through the map.");
		if (playingMode == PlayingMode::START_POSITION) {
			afont->BlitText(10, 160, "Pick a tile for the start position.");
		}
		else if (playingMode == PlayingMode::GOAL_POSITION) {
			afont->BlitText(10, 160, "Pick a tile for the goal position.");
		}
		break;
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{	
	//std::cout << "input here " + key;
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		switch (gameMode)
		{
		case GameMode::EDITING:
			gameMode = GameMode::PICKING;
			break;

		case GameMode::PICKING:
			gameMode = GameMode::EDITING;
			break;
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_RELEASE)
	{
		//make a random map when they press R
		theMap->LoadMap("map.txt");
	}

	if (key == GLFW_KEY_L && action == GLFW_RELEASE)
	{
		//load an actual map from a file if they press L
		theMap->LoadMap("map.txt");
	}

	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		//save the current map to a file
		//theMap->SaveMap("mapfile.dat");
	}

	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
	{
		editMode = EditMode::BACKGROUND;
		//prevent crash when switching from drawing no FG to drawing BG
		if (editTileNum == -1) editTileNum = 0;
	}

	if (key == GLFW_KEY_F2 && action == GLFW_RELEASE)
	{
		editMode = EditMode::FOREGROUND;
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		scrollState = ScrollState::RIGHT;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
	{
		scrollState = ScrollState::NONE;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		scrollState = ScrollState::LEFT;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
	{
		scrollState = ScrollState::NONE;
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		scrollState = ScrollState::DOWN;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		scrollState = ScrollState::NONE;
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		scrollState = ScrollState::UP;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
	{
		scrollState = ScrollState::NONE;
	}


}

void DoCursor(double x, double y)
{
	//convert values in case we are in a "fake" fullscreen,
	//becuase the OS reports mouse positions based on ACTUAL screen resolution.
	cursor_x = blit3D->screenWidth / blit3D->trueScreenWidth * (float)x;
	cursor_y = blit3D->screenHeight / blit3D->trueScreenHeight * (float)y;

	//we will leave the y position rleative to 0 at the top of the screen
	//for now. To invert it, do the following:

	//cursor_y = blit3D->screenHeight - cursor_y;
}

void DoMouseButton(int button, int action, int mods)
{
	int tile_x, tile_y;
	switch (gameMode)
	{
		//case GameMode::EDITING:
		//	tile_y = (cursor_y / 16) + tilesScrolled_Y;
		//	tile_x = (cursor_x / 16) + tilesScrolled_X;
		//	// check if out of bounds
		//	if (cursor_y < 0 || cursor_y > MAP_VIEW_SIZE * 16 ||
		//		cursor_x < 0 || cursor_x > MAP_VIEW_SIZE * 16)
		//		break;
		//	
		//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		//	{	
		//		switch (editMode)
		//		{
		//		case EditMode::BACKGROUND:
		//			//set the background tile number at the current location to the edit tile number
		//			theMap->map[(tile_y * theMap->width) + tile_x].backgroundTileNum = editTileNum;
		//			break;

		//		case EditMode::FOREGROUND:
		//			//set the foreground tile number at the current location to the edit tile number
		//			theMap->map[(tile_y * theMap->width) + tile_x].foregroundTileNum = editTileNum;
		//			break;
		//		}
		//	}

		//	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		//	{
		//		switch (editMode)
		//		{
		//		case EditMode::BACKGROUND:
		//			//set the edit tile number to the current tile under the cursor:
		//			//we can pick tiles "up" off the map to draw with
		//			editTileNum = theMap->map[(tile_y * theMap->width) + tile_x].backgroundTileNum;
		//			break;

		//		case EditMode::FOREGROUND:
		//			//set the edit tile number to the current tile under the cursor:
		//			//we can pick tiles "up" off the map to draw with
		//			editTileNum = theMap->map[(tile_y * theMap->width) + tile_x].foregroundTileNum;
		//			break;

		//		}
		//	}
		//	break;

		//case GameMode::PICKING:
		//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		//	{
		//		// set the edit tile number to the current tile number under the cursor
		//		// convert cursor placement to actual map placement by removing offset
		//		tile_x = -(int)(blit3D->screenWidth / 2) + (int)(480 / 2) + cursor_x;
		//		tile_y = -(int)(blit3D->screenHeight / 2) + (int)(256 / 2) + cursor_y;

		//		// check if out of bounds
		//		bool outOfBounds = (tile_x < 0 || tile_x > 480) || (tile_y < 0 || tile_y > 256);
		//		if (!outOfBounds) {
		//			editTileNum = ((tile_y / 16) * 30) + (tile_x / 16);
		//			gameMode = GameMode::EDITING;
		//		} else if (outOfBounds && editMode == EditMode::FOREGROUND) {
		//			editTileNum = -1;
		//			gameMode = GameMode::EDITING;
		//		}
		//	}
		//	break;

	case GameMode::PLAYING:
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			int y = (cursor_y / 16) + tilesScrolled_Y;
			int x = (cursor_x / 16) + tilesScrolled_X;
			if (y < 0 || y > theMap->height - 1 || x < 0 || x > theMap->width - 1)
			{
				break;
			}
			switch (playingMode) {
				case PlayingMode::START_POSITION:
					knight = new Knight(x, y);
					playingMode = PlayingMode::GOAL_POSITION;
					break;
				case PlayingMode::GOAL_POSITION:
					theMap->goal = vec2(y, x);
					theMap->map[y][x].foregroundTileNum = 478;
					playingMode = PlayingMode::PATH_FINDING;
					knight->pathfind(theMap->goal);
					break;
			}
		}
		break;
	}
}

//called whenever the user resizes the window
void DoResize(int width, int height)
{
	blit3D->trueScreenWidth = blit3D->screenWidth = static_cast<float>(width);
	blit3D->trueScreenHeight = blit3D->screenHeight = static_cast<float>(height);
	blit3D->Reshape(blit3D->shader2d);
}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1280, 720);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);
	blit3D->SetDoResize(DoResize);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}