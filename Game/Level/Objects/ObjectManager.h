#pragma once
#include <vector>

class Camera;
class ObjectModel;
class ObjectManager
{
public:
	std::vector<ObjectModel*> MyObjects;
	ObjectManager();
	~ObjectManager();
	void Update();
	Camera* camera = nullptr;

};