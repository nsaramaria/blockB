/* Hello Cubes
code adapted from from OpenGL® ES 2.0 Programming Guide
and code snippets from RPI Forum to set up Dispmanx

Presented here for Block C as a basic Framework, Rendering method is sub optimal, please take note of that, You should impliment
your own render systems and shader managment systems when you understand the methods available, workshops will be run to discuss this

Initialises a Raspberry format EGL window for rendering and provides an example a simple shader system

Provides a skeletal framework for a processing loop 
Provides input systems for mouse and keyboard (requires init to work)
(note...Linux key reads are unpredictable, if you find keys are not working use event0 or event 1 or event2.. etc
This is a known issue, we could fix with a listener on all events but other methods are being investigated)

Provides a simple Gui demonstrated on start up which allows resolution changes and info display
Provides Graphic file loading capability via MyFiles.h a wrapper class for stb_image
Provides access to a Physics Library (Bullet) but no examples of usage yet, workshops will be run
GLM is used as a standard maths library, you may use your own if you wish but why???

Recommended course..
Review ObjectModel base class, provide the functions stated
Review CubeModel as a standard object type. 
Provide init update and draw routines

Review the Draw function here in HelloCubes which is the main processing loop...make it work


Project builds as Visual GDB (Raspberry), or Visual GDB (X11)
Also C++ for Linux Builds for Raspberry  or X11
Rpi4 is considered an X11 system, see the info on CMGT regarding setup of systems/

*/
#include <X11/Xlib.h> // we now include X11 for all systems including early Rpi's so we can get access to an Xwindow cursor, but only Rpi4>> uses X11 for render
#include <X11/extensions/Xrandr.h> //this is used for our resolution systems, which are handled a little different on early Rpi's,we still need some of the info, but Rpi res data will be invalid
#include <assimp/aabb.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include "imgui/imgui_impl_opengl3.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
//#include <vector>

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include "Game/tmpl8/CubeModel.h"
//#include "ShipModel.h"
#ifdef RASPBERRY
#include "bcm_host.h"
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <vector>
// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Game/tmpl8/Graphics.h"
#include "Game/tmpl8/ObjectModel.h"

#include "Game/tmpl8/Input.h"
//#include "Game/Level/Objects/Camera/Camera.h"
#include "Level/Level.h"
#include "Level/Objects/ObjectManager.h"
#include "Game.h"

using namespace glm;

// keeping some things in global space...because....Brian does it this way :D you should consider better ways....AKA, you should consider better ways...do I have to spell it out?
//std::vector<ObjectModel*> MyObjects; // on the basis that every object is derived from ObjectModel, we keep a list of things to draw.
//Graphics Graphics;  // create a 1 time instance of a graphics class in Global space


//Target_State state; 
//Target_State* p_state = &state;
//Input* TheInput; // nice pointer to a hardware key and mouse thread system... but..it can be flakey with some wireless keyboards
//Level* level=nullptr;
Game* game = nullptr;

//Camera* thecamera = new Camera;


/*****************************************
Up date all objects shape stored in the MyObjects list

This is however not an optimal way to process objects
updating them, then doing their draw is inefficient
Consider ways to improve this
******************************************/

//void Update(Target_State *p_state)
//{
//	// Setup the viewport
//	glViewport(0, 0, p_state->width, p_state->height);
//
//	// Clear the color draw buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	
//	game->level->objmanager->Update();
//
//}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------

//
//// Our projects main  Game loop (could be put into a class, hint hint)
//void  MainLoop(Target_State *esContext)
//{
//	
//	
//}


//Program Entry

int main(int argc, char *argv[])
{
	game = new Game();
	game->Init();
	//MyFiles* Handler = new MyFiles(); // here have a nice file handler
	//ObjectData object_data;  // create a single instance of object_data to initially draw all objects with the same data.
	//
	//p_state->object_data = &object_data; // this is....inefficient... an expansion of the old hello triangle demo's consider reworking it.
	/*Target_State state;
	Target_State* p_state = &state;*/
	
	//game->graphics.GetDisplay();
	//game->graphics.esInitContext(p_state);
 //   game->graphics.init_ogl(p_state, 1024, 768,1024,768); // this is the window size we set , you can set it to anything, the 3rd and 4th params are for older pis'
	//game->graphics.GetRes(game->graphics.ResolutionX, game->graphics.ResolutionY);
	//
	
	// now go do the Game loop	
		//MainLoop(p_state);1
	//game->Loop(game->p_state);
	game->Loop();
	
// here we should do a graceful exit	
// I said graceful.....
}
