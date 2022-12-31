#include "ObjectManager.h"
#include "Camera/Camera.h"
//#include <vector>
#include "../../tmpl8/ObjectModel.h"

ObjectManager::ObjectManager()
{
	camera = new Camera();
	
 }

void ObjectManager::Update()
{
	for (size_t i = 0; i < MyObjects.size(); i++)
	{
		//update the object, in this demo, we just up its position and transform data but you should consider logic
		MyObjects[i]->Update(camera->GetViewMatrix());
		// the draw routine is the responsbility of the object itself, thats not an ideal system, consider how to improve
		MyObjects[i]->Draw();
	}
}