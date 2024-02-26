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
	//delete this->textureLee;
}

Mesh quad;
Shader* shader[24];
Entity* entity;
Texture* texture = new Texture();
Texture* texture2 = new Texture();
Texture* texture3 = new Texture();

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
	/*
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

	*/
	

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	// Load the quad mesh
	quad.CreateQuad();

	// Load the shaders

	// Set the shaders for A-1 to A-6
	shader[0] = Shader::Get("shaders/A/A-1.vs", "shaders/A/A-1.fs");
	shader[1] = Shader::Get("shaders/A/A-2.vs", "shaders/A/A-2.fs");
	shader[2] = Shader::Get("shaders/A/A-3.vs", "shaders/A/A-3.fs");
	shader[3] = Shader::Get("shaders/A/A-4.vs", "shaders/A/A-4.fs");
	shader[4] = Shader::Get("shaders/A/A-5.vs", "shaders/A/A-5.fs");
	shader[5] = Shader::Get("shaders/A/A-6.vs", "shaders/A/A-6.fs");

	// Set the shaders for B-0 to B-6
	shader[6] = Shader::Get("shaders/B/B-1.vs", "shaders/B/B-1.fs");
	shader[7] = Shader::Get("shaders/B/B-2.vs", "shaders/B/B-2.fs");
	shader[8] = Shader::Get("shaders/B/B-3.vs", "shaders/B/B-3.fs");
	shader[9] = Shader::Get("shaders/B/B-4.vs", "shaders/B/B-4.fs");
	shader[10] = Shader::Get("shaders/B/B-5.vs", "shaders/B/B-5.fs");
	shader[11] = Shader::Get("shaders/B/B-6.vs", "shaders/B/B-6.fs");

	// Set the shaders for C-0 to C-2
	shader[12] = Shader::Get("shaders/C/C-0.vs", "shaders/C/C-0.fs");
	shader[13] = Shader::Get("shaders/C/C-1.vs", "shaders/C/C-1.fs");
	shader[14] = Shader::Get("shaders/C/C-2.vs", "shaders/C/C-2.fs");

	// Set the shaders for D-1
	shader[15] = Shader::Get("shaders/D/D-1.vs", "shaders/D/D-1.fs");


	//Check if the shaders have been loaded correctly using a for loop
	for (int i = 0; i < 6; i++) {
		if (shader[i] == nullptr) {
			char err;
			itoa(i, &err, 10);
			std::cout << "Error loading the shader" << std::endl;
			std::cout << err << std::endl;
			exit(1);
		}
	}
	// Load the texture
	Texture* texture = new Texture();  // Assuming you have a Texture class
	if (texture->Load("images/fruits.png")) {
		std::cout << "Fruits texture loaded" << std::endl;
	}
	else {
		std::cout << "Error loading Fruits image" << std::endl;
		exit(1);
	}

	// Set the texture for the shader
	//texture->Bind();  
	//shader[12]->SetTexture("u_texture", texture);
	
	

	// Load the entity
	this->meshLee.LoadOBJ("meshes/lee.obj");
	Matrix44 modelLee;
	modelLee.SetIdentity();
	modelLee.Translate(0.0, -0.3, -0.3);
	this->Lee.SetModelMatrix(modelLee);

	this->textureLee = new Texture();
	this->textureLee->Load("lee_normal.tga", true);
	this->Lee.SetTexture(textureLee);

	this->textureLeeSpecular = new Texture();
	this->textureLeeSpecular->Load("lee_color_specular.tga", true);
	this->Lee.SetTextureSpecular(textureLeeSpecular);

	this->Lee.SetMesh(this->meshLee);

	this->Lee.SetShader(shader[15]);


	/*

	// Set the camera
	camera.SetPerspective(45, static_cast<float>(framebuffer.width) / static_cast<float>(framebuffer.height), 0.01, 100);
	Vector3  eye = Vector3(0, 0.5, 1);
	Vector3 center = Vector3(0.0, 0.0, 0.0);
	Vector3 up = Vector3(0, 1.0, 0);
	camera.LookAt(eye, center, up);

	// Load the quad
	quad.CreateQuad();
	if (texture->Load("images/fruits.png") == true) {
		std::cout << "Fruits texture Loaded" << std::endl;
	}
	else {
		std::cout << "Error loading Fruits image" << std::endl;
	}
	if (texture2->Load("images/newyork.png") == true) {
		std::cout << "NY texture Loaded" << std::endl;
	}
	else {
		std::cout << "Error loading NY image" << std::endl;
	}


	//Load the entity
	shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	
	//Load the mesh
	Mesh mesh;
	mesh.LoadOBJ("meshes/lee.obj");
	//Check if the mesh has been loaded correctly
	const std::vector<Vector3>& vertices = mesh.GetVertices();
	if (vertices.size() == 0) {
		std::cout << "Error loading the mesh" << std::endl;
		exit(1);
	}
	
	//Set the entity model matrices to identity
	Matrix44 model;
	model.SetIdentity();
	
	//Set the entity with the mesh and the shader
	entity = new Entity(model, mesh, shader);

	//Load the texture
	if (texture3->Load("textures/lee_normal.tga") == true) {
		std::cout << "Lee texture Loaded" << std::endl;
	}
	else {
		std::cout << "Error loading Lee texture image" << std::endl;
	}

	*/

	std::cout << "Initiating app..." << std::endl;


}


// Render one frame
void Application::Render(void)
{
	/*
	// Set background color to black
	framebuffer.Fill(Color::BLACK);

	// Set the zBuffer to max float
	zBuffer.Fill(FLT_MAX);

	// Render the entities
	Lee.Render(&framebuffer, &camera, Color::WHITE, &zBuffer);
	//Cleo.Render(&framebuffer, &camera, Color::RED);
	//Anna.Render(&framebuffer, &camera, Color::BLUE);

	

	framebuffer.Render();
	*/
	

	
	if (current_task == 1 || current_task == 2 || current_task == 3) {
		shader[current_show]->Enable();
		shader[current_show]->SetFloat("u_time", time);
		//myQuadShader->SetTexture("u_fruit", my_texture);
		quad.Render();
		shader[current_show]->Disable();
	}
	if (current_task == 4) {

		//Set camera position and target
		camera.LookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

		//Set the projection matrix to perspective
		camera.SetPerspective(45, window_width / (float)window_height, 0.1f, 10000.0f);

		//Set the projection matrix to orthographic
		//camera.SetOrthographic(-10, window_width / (float)window_height, window_height/(float)window_width, -10, 0.1f, 10000.0f);



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		shader[current_show]->SetTexture("u_lee", textureLee);
		Lee.Render(&camera);
		shader[15]->Disable();
	}


	
	
	//shader[current_show]->Enable();
	//glEnable(GL_DEPTH_TEST);
	//shader[current_show]->SetFloat("u_time", time);

	
	
	//quad.Render(GL_TRIANGLES);
	//glDisable(GL_DEPTH_TEST);
	
	
	// Clear the framebuffer and the depth buffer
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw scene
	// ...

	// Swap between front and back buffer
	// This method name changes depending on the platform
	//SDL_GL_SwapWindow(window);




	/*
	// Enable depth testing for occlusions
	glEnable(GL_DEPTH_TEST);
	// Z will pass if the Z is LESS or EQUAL to the Z of the pixel
	glDepthFunc(GL_LEQUAL);

	shader->Enable();

	// Give the letter for the fragment shader it shoud render
	shader->SetFloat("u_number", current_subtask);

	//Set uniforms from the quad

	shader->SetFloat("u_ratio", (window_width / window_height));
	shader->SetFloat("u_time", time);
	shader->SetTexture("u_texture", texture);
	shader->SetTexture("u_image", texture2);


	//Set texture for the entity
	shader->SetTexture("u_lee_texture", texture3);

	//Update camara's ratio to aboid deformations
	camera.SetAspectRatio(static_cast<float>(framebuffer.width) / static_cast<float>(framebuffer.height));

	if (current_task != 4) {
		quad.Render();
	}
	/*
	else {
		entity->SetShader(shader);
		entity->Render(&camera);
	}
	*/

	shader[current_show]->Disable();



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


	// Update the Shader based on current task
	//shader->SetFloat("u_number", current_subtask);


	if (change_task) {
		if (current_task == 1) {
			if (current_subtask == a) {
				this->current_show = 0;
				this->change_task = false;
				return;
			}
			if (current_subtask == b) {
				this->current_show = 1;
				this->change_task = false;
				return;
			}
			if (current_subtask == c) {
				this->current_show = 2;
				this->change_task = false;
				return;
			}
			if (current_subtask == d) {
				this->current_show = 3;
				this->change_task = false;
				return;
			}
			if (current_subtask == e) {
				this->current_show = 4;
				this->change_task = false;
				return;
			}
			if (current_subtask == f) {
				this->current_show = 5;
				this->change_task = false;
				return;
			}
		}

		if (current_task == 2) {
			if (current_subtask == a) {
				this->current_show = 6;
				this->change_task = false;
				return;
			}
			if (current_subtask == b) {
				this->current_show = 7;
				this->change_task = false;
				return;
			}
			if (current_subtask == c) {
				this->current_show = 8;
				this->change_task = false;
				return;
			}
			if (current_subtask == d) {
				this->current_show = 9;
				this->change_task = false;
				return;
			}
			if (current_subtask == e) {
				this->current_show = 10;
				this->change_task = false;
				return;
			}
			if (current_subtask == f) {
				this->current_show = 11;
				this->change_task = false;
				return;
			}
		}

		if (current_task == 3) {
			if (current_subtask == a) {
				this->current_show = 12;
				this->change_task = false;
				return;
			}
			if (current_subtask == b) {
				this->current_show = 13;
				this->change_task = false;
				return;
			}
			if (current_subtask == c) {
				this->current_show = 14;
				this->change_task = false;
				return;
			}
		}

		if (current_task == 4) {
			if (current_subtask == a) {
				this->current_show = 15;
				this->change_task = false;
				return;
			}
			if (current_subtask == b) {
				this->current_show = 16;
				this->change_task = false;
				return;
			}
		}

	}


}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		/* LAB 2
		
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
	/

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
		*/


			// Lab 4
			
			//Keys “1” to “4”: Show Tasks 1 to 4
			//“a” to “f” : Show subtasks a to f of the current task

		case SDLK_1:
			this->current_task = 1;
			this->current_subtask = a;
			this->change_task = true;
			break;

		case SDLK_2:
			this->current_task = 2;
			this->current_subtask = a;
			this->change_task = true;
			break;

		case SDLK_3:
			this->current_task = 3;
			this->current_subtask = a;
			this->change_task = true;
			break;

		case SDLK_4:
			this->current_task = 4;
			this->current_subtask = a;
			this->change_task = true;
			break;

		case SDLK_a:
			this->current_subtask = a;
			this->change_task = true;
			break;

		case SDLK_b:
			this->current_subtask = b;
			this->change_task = true;
			break;

		case SDLK_c:
			this->current_subtask = c;
			this->change_task = true;
			break;

		case SDLK_d:
			this->current_subtask = d;
			this->change_task = true;
			break;

		case SDLK_e:
			this->current_subtask = e;
			this->change_task = true;
			break;

		case SDLK_f:
			this->current_subtask = f;
			this->change_task = true;
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