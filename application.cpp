#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);
	this->key = 0;
	int w,h;
	SDL_GetWindowSize(window,&w,&h);
	this->borderWidth = 25;
	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
	key = 0;
}

// Render one frame
void Application::Render(void)
{
	// ...
	framebuffer.Fill(Color::BLACK);
	
	float x = 50;
	float y = 10;
	Color lineColour = Color::BLUE;
	framebuffer.DrawLineDDA(x, y, x + 100 * cos(time), y + 100 * sin(time), lineColour);
	
	framebuffer.DrawRect(x, y, 400, 200, Color::GREEN);

	int x1 = 300;
	int y1 = 200;
	framebuffer.DrawRect(x1, y1, x1 + y1, x1 + y1, Color::BLUE, 50, false, Color::BLUE);
	
	int x2 = 600;
	int y2 = 100;
	framebuffer.DrawRect(x2, y2, x1 + y2, x1 + y2, Color::GREEN, 50, true, Color::BLUE);
	
	Vector2 p1(50, 120);
	Vector2 p2(200, 250);
	Vector2 p3(120, 100);

	framebuffer.DrawTriangle(p1, p2, p3, Color::BLUE, true, Color::GREEN);
	
	framebuffer.DrawCircle(600, 100, 50, Color::CYAN, 10, false, Color::BLUE);
	//framebuffer.DrawCircle(600, 400, 100, Color::CYAN, 10, true, Color::PURPLE);

	

	framebuffer.DrawTriangle(p1, p2, p3, Color::BLUE, false, Color::GREEN);
	//framebuffer.DrawTriangle(p1, p2, p3, Color::BLUE, true, Color::GREEN);

	if (key = 1) framebuffer.DrawCircle(mouse_position.x, mouse_position.y, 100, Color::CYAN, 10, true, Color::PURPLE);


	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

	case SDLK_p:
		borderWidth = std::max(borderWidth - 1, 1);
		key = 1;
		break;

	case SDLK_m:
		borderWidth = std::min(borderWidth + 1, 100);
		break;
	}
}


void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
		framebuffer.DrawCircle(mouse_position.x, mouse_position.y, 100, Color::CYAN, 10, true, Color::PURPLE);
	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}