/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "mesh.h"
#include "entity.h"
#include "camera.h"

#define NEAR 1
#define FAR 2
#define FOV 3

class Application
{
private:
	
public:

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	int cam = 1;

	int current_property = 0;
	//Declare 3 entities and 3 meshes

	Entity Lee; // Declare entity as a member variable
	Mesh meshLee; // Declare mesh as a member variable
	Image* textureLee; // Declare texture as a member variable


	Entity Cleo; // Declare entity as a member variable
	Mesh meshCleo; // Declare mesh as a member variable

	Entity Anna; // Declare entity as a member variable
	Mesh meshAnna; // Declare mesh as a member variable


	Camera camera; // Declare camera as a member variable

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;

	// CPU GLobal z-Buffer
	FloatImage zBuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
};
