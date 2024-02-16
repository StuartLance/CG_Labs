#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "camera.h"
#include "entity.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
	// ADd zBuffer
	this->zBuffer.Resize(w, h);
}

Application::~Application()
{
	delete this->textureLee;
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	//Set camera position and target
	camera.LookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//Set the projection matrix to perspective
	camera.SetPerspective(45, window_width / (float)window_height, 0.1f, 10000.0f);

	//Set the projection matrix to orthographic
	//camera.SetOrthographic(-10, window_width / (float)window_height, window_height/(float)window_width, -10, 0.1f, 10000.0f);



	//Load meshes from file
	this->meshLee.LoadOBJ("meshes/lee.obj");
	this->meshCleo.LoadOBJ("meshes/cleo.obj");
	this->meshAnna.LoadOBJ("meshes/anna.obj");

	// Check if the meshes has been loaded correctly
	const std::vector<Vector3>& verticesL = meshLee.GetVertices();
	if (verticesL.size() == 0) {
		std::cout << "Error loading Lee mesh" << std::endl;
		exit(1);
	}

	const std::vector<Vector3>& verticesC = meshCleo.GetVertices();
	if (verticesC.size() == 0) {
		std::cout << "Error loading the mesh" << std::endl;
		exit(1);
	}

	const std::vector<Vector3>& verticesA = meshAnna.GetVertices();
	if (verticesA.size() == 0) {
		std::cout << "Error loading the mesh" << std::endl;
		exit(1);
	}

	//Set the entity model matrices to identity
	Matrix44 modelLee;
	modelLee.SetIdentity();
	modelLee.Translate(0.0, -0.3, -0.3);
	this->Lee.SetModelMatrix(modelLee);
	
	this->textureLee = new Image();
	this->textureLee->LoadTGA("lee_normal.tga", true);
	this->Lee.SetTexture(textureLee);

	if (Lee.GetTexture() == textureLee) {
		std::cout << "Texture set successfully on the entity." << std::endl;
	}
	else {
		std::cout << "Failed to set texture on the entity." << std::endl;
	}

	//this->Lee.SetTexture(nullptr);
	
	//this->Cleo.SetModelMatrix(Matrix44());
	//this->Anna.SetModelMatrix(Matrix44());

	//Set the entity meshes
	this->Lee.SetMesh(this->meshLee);
	//this->Cleo.SetMesh(this->meshCleo);
	//this->Anna.SetMesh(this->meshAnna);
}

// Render one frame
// Render one frame
void Application::Render(void)
{
	// Set background color to black
	framebuffer.Fill(Color::BLACK);

	// Set the zBuffer to max float
	zBuffer.Fill(FLT_MAX);

	// Render the entities
	Lee.Render(&framebuffer, &camera, Color::WHITE, &zBuffer);
	//Cleo.Render(&framebuffer, &camera, Color::RED);
	//Anna.Render(&framebuffer, &camera, Color::BLUE);

	

	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
	
	//Lee.UpdateRotate(0.5*seconds_elapsed);
	//Cleo.UpdateTranslate(seconds_elapsed);
	//Anna.UpdateScale(0.1*seconds_elapsed);


	// Update the camera
	//if (cam ==1) camera.SetPerspective(camera.fov, camera.aspect, camera.near_plane, camera.far_plane);
	//else if (cam == 0) camera.SetOrthographic(camera.left, camera.right, camera.top, camera.bottom, camera.near_plane, camera.far_plane);

}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		// LAB 2
		
	case SDLK_o:  camera.SetOrthographic(-1, window_width / (float)window_height, window_height / (float)window_width, -1, 0.1f, 10000.0f); 
		this->cam = 0;
		break; // aspect ratio for right and then the inverse for top
	case SDLK_p:  camera.SetPerspective(45, window_width / (float)window_height, 0.1f, 10000.0f); 
		this->cam = 1;
		break;
	
	case SDLK_n:  this->current_property = NEAR; break;
	case SDLK_f:  this->current_property = FAR; break;
	case SDLK_v:  this->current_property = FOV; break;



	case SDLK_PLUS:
		if (current_property == NEAR) {
			this->camera.near_plane += 0.1;
		}

		else if (current_property == FAR) {
			this->camera.far_plane += 1000;
		}
		else if (current_property == FOV) {
			this->camera.fov += 15;
			clamp(this->camera.fov, 10.0f, 179.0f);
		}
		
		break;

	case SDLK_MINUS:
		if (current_property == NEAR) {
			this->camera.near_plane -= 0.1;
			break;
		}
		else if (current_property == FAR) {
		this->camera.far_plane -= 1000;
		break;
		}
		else if (current_property == FOV) {
		this->camera.fov -= 15;
		clamp(this->camera.fov, 10.0f, 179.0f);
		break;
		}
		break;
		
	/* LAB 3 -
		“C”	Toggle(activate / deactivate) between PLAIN COLOR / INTERPOLATED vertex colors. i.e. use Color::WHITE or the interpolated color
		“Z”	Toggle between OCCLUSIONS and NO OCCLUSIONS
		“T”	Toggle between USE MESH TEXTURE and USE PLAIN COLOR colors
	*/

	case SDLK_z: 
		if (Lee.GetOcclusion() == true) {
		Lee.SetOcclusion(false);
	}
		else {
		Lee.SetOcclusion(true);
	}
	break;

	case SDLK_c: 
		// Cycle through all 4 modes
		if (Lee.GetRenderMode() == Entity::eRenderMode::TRIANGLES_INTERPOLATED) {
			Lee.SetRenderMode(Entity::eRenderMode::POINTCLOUD);
		}
		else if (Lee.GetRenderMode() == Entity::eRenderMode::POINTCLOUD) {
			Lee.SetRenderMode(Entity::eRenderMode::WIREFRAME);
		}
		else if (Lee.GetRenderMode() == Entity::eRenderMode::WIREFRAME) {
			Lee.SetRenderMode(Entity::eRenderMode::TRIANGLES);
		}
		else if (Lee.GetRenderMode() == Entity::eRenderMode::TRIANGLES) {
			Lee.SetRenderMode(Entity::eRenderMode::TRIANGLES_INTERPOLATED);
		}
		break;

	case SDLK_t: 
		if (Lee.GetTexture() == nullptr) {
			Lee.SetTexture(textureLee);
		}
		else {
			Lee.SetTexture(nullptr);
		}
		break;

	}


}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
		//this->camera.Rotate(2 * DEG2RAD, Vector3(0, 1, 0));
	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{	
	if (event.button == SDL_BUTTON_LEFT) {
		this->camera.Orbit(-mouse_delta.x * 0.01, Vector3::UP);
		this->camera.Orbit(-mouse_delta.y * 0.01, Vector3::RIGHT);
	}
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;
	this->camera.Zoom(dy < 0 ? 1.1 : 0.9);
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}