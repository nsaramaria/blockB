#include "Game.h"
#include "Level/Level.h"
#include "tmpl8/MyFiles.h"
#include "Level/Objects/ObjectManager.h"
#include "tmpl8/CubeModel.h"
#include "Level/Objects/World/Map.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "Level/Objects/World/Map.h"
#include "Level/Objects/Camera/Camera.h"
Game::Game()
{
	level = new Level();
	Handler = new MyFiles();
}

void Game::Init()
{   
	graphics.GetDisplay();
    graphics.esInitContext(p_state);
	graphics.init_ogl(p_state, 1024, 768, 1024, 768); // this is the window size we set , you can set it to anything, the 3rd and 4th params are for older pis'
	graphics.GetRes(graphics.ResolutionX, graphics.ResolutionY);

	//imGui init
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 300 es");
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(1024, 768);
	
}
void Game::Update(float DeltaTime)
{   
	// Setup the viewport
	glViewport(0, 0, p_state->width, p_state->height);
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.75f, 1.0f, 0.0f);

	Window window_returned;
	int root_x;
	int root_y;
	int win_x;
	int win_y;
	uint mask_return;
	bool result = XQueryPointer(
	    graphics.x_display,
		graphics.win,
		&window_returned,
		&window_returned,

		&root_x,
		&root_y,
		&win_x,
		&win_y,
		&mask_return);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(1024, 768);
	io.DeltaTime = DeltaTime;
	//we should also pass on mouse and key info into the IO here
	//UserController::GetMouseScreenPos().x;
	io.MousePos = ImVec2(root_x, root_y);
	io.MouseDown[0] = mask_return & (1 << 8); // left
	io.MouseDown[1] = mask_return & (1 << 10); // right

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame(); // do this at the start of any frame

	ImGui::Begin("saraaaaaaalala");
	ImGui::Text("ik ben sara");

	//ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color
	//ImGui::ColorEdit3("tint color", reinterpret_cast<float*>(&tint_color)); // Edit 3 floats representing a color
	//ImGui::SliderFloat3("translationA", &translationA.x, -1000.0f, 1960.0f);
	//ImGui::SliderFloat3("translationB", &translationB.x, -1000.0f, 1960.0f);

	//ImGui::SliderFloat3("cameraPos", &cameraPos.x, -1000.0f, 1000.0f);
	//ImGui::SliderFloat3("cameraRotation", &cameraRotation.x, -1.0f, 1.0f);

	map->Update(level->objmanager->camera->GetViewMatrix());
	map->Draw();
    level->objmanager->Update();


	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Clear the color draw buffer
	glFlush();
	eglSwapBuffers(p_state->display, p_state->surface);
}
	

void Game::Loop()
{    
	level->input->Init();

	ObjectModel* T2;   // so both types even though quite different use the same base to create them

	/////OBJECT 1
	//T2 = new CubeModel(Handler); // make a new cube
	glm::vec3 Pos = glm::vec3(10.0f, 0.0f, 0.0f); // set a position
	//T2->Scales = glm::vec3(0.1f, 0.1f, 0.1f);	// a cube is actually quite large (screen size) so shrink it down
	//T2->SetPosition(Pos);
	//level->objmanager->MyObjects.push_back(T2); // store in the vector ready for the game loop to process
	//T2->StoreGraphicClass(&graphics); // make sure it knows the where the graphics class is, (for now it contains our attribute/uniform info)
	//graphics.Init(T2); // set it up

	// repeat for a different kind of model	this time a full Obj model
	//T2 = new Map(Handler);
	//map = new Map(Handler);
	map = new Map(Handler);
	Pos = glm::vec3(-8.0f, 0.0f, 0.0f);
	map->Scales = glm::vec3(0.8f, 0.8f, 0.8f);
	map->SetPosition(Pos);
	map->StoreGraphicClass(&graphics);
	graphics.Init(map);

	level->objmanager->MyObjects.push_back(map);

	struct timeval t1, t2;
	struct timezone tz;
	float deltatime;
	float totaltime = 0.0f;
	unsigned int frames = 0;

	gettimeofday(&t1, &tz);


	int Counter = 0;
	while (true) // for a timed loop check the counter otherwise find another exit condition
	{
		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 0.0000001f);
		t1 = t2;
		Update(deltatime);  // this is where the action happens
		level->CameraUpdate();

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui::NewFrame(); // do this at the start of any frame
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//glFlush();
		//eglSwapBuffers(esContext->display, esContext->surface);

		//ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color
		//ImGui::ColorEdit3("tint color", reinterpret_cast<float*>(&tint_color)); // Edit 3 floats representing a color
		//ImGui::SliderFloat3("translationA", &translationA.x, -1000.0f, 1960.0f);
		//ImGui::SliderFloat3("translationB", &translationB.x, -1000.0f, 1960.0f);

		//ImGui::SliderFloat3("cameraPos", &cameraPos.x, -1000.0f, 1000.0f);
		//ImGui::SliderFloat3("cameraRotation", &cameraRotation.x, -1.0f, 1.0f);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		//ImGui::GetIO().Framerate);

		// report frame rate
		totaltime += deltatime;
		frames++;
		if (totaltime > 1.0f)
		{
			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);  // uncomment to see frames
			totaltime -= 1.0f;
			frames = 0;
		}
	}
}