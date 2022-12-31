#pragma once
#include "tmpl8/Graphics.h"

//cannot acces member variables with forward declaration
class Level;
class MyFiles;
class Map;
class Game
{
public:
	Target_State state;
	Target_State* p_state = &state;
	Map* map;
	Game();
	~Game();
	void Update(float DeltaTime);
	void Loop();
	void Init();
	Graphics graphics;
	Level* level = nullptr;
	MyFiles* Handler = nullptr;

};