#pragma once

class ObjectManager;
class Input;
class Level
{
public:
	Level();
	~Level();
	void CameraUpdate();
	ObjectManager* objmanager=nullptr;
	Input* input=nullptr;
};