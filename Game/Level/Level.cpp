#include "Level.h"
#include "Objects/ObjectManager.h"
#include "../tmpl8/Input.h"
#include "Objects/Camera/Camera.h"
#include <glm.hpp>

Level::Level()
{
	objmanager = new ObjectManager();
    input = new Input();
}

void Level::CameraUpdate()
{
    if (input->TestKey(KEY_A))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x - 1, objmanager->camera->Position.y, objmanager->camera->Position.z);
    }

    if (input->TestKey(KEY_D))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x + 1, objmanager->camera->Position.y, objmanager->camera->Position.z);
    }

    if (input->TestKey(KEY_W))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x, objmanager->camera->Position.y, objmanager->camera->Position.z - 1);
    }

    if (input->TestKey(KEY_S))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x, objmanager->camera->Position.y, objmanager->camera->Position.z + 1);
    }

    if (input->TestKey(KEY_O))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x, objmanager->camera->Position.y+1, objmanager->camera->Position.z - 1);
    }

    if (input->TestKey(KEY_L))
    {
        objmanager->camera->Position = glm::vec3(objmanager->camera->Position.x, objmanager->camera->Position.y-1, objmanager->camera->Position.z + 1);
    }
    if (input->TestKey(KEY_RIGHT))
    {
        objmanager->camera->ProcessCameraView(7, 0, true);
    }
    if (input->TestKey(KEY_LEFT))
    {
        objmanager->camera->ProcessCameraView(-7, 0, true);
    }
    if (input->TestKey(KEY_UP))
    {
        objmanager->camera->ProcessCameraView(0, 7, true);
    }
    if (input->TestKey(KEY_DOWN))
    {
        objmanager->camera->ProcessCameraView(0, -7, true);
    }
   

  // objmanager->camera->ProcessMouseMovement(input->TheMouse.RelX, input->TheMouse.RelY, true);

}