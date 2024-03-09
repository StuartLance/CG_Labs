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

	//Set camera position and target
	camera.LookAt(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//Set the projection matrix to perspective
	camera.SetPerspective(45, window_width / (float)window_height, 0.1f, 10000.0f);

	//Set the projection matrix to orthographic
	//camera.SetOrthographic(-10, window_width / (float)window_height, window_height/(float)window_width, -10, 0.1f, 10000.0f);
	

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	// Load the quad mesh
	quad.CreateQuad();

	// Load the shaders
	
	GShader = Shader::Get("shaders/gourand.vs", "shaders/gourand.fs");
	PShader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");



	//Check if the shaders have been loaded correctly 
	if (GShader == nullptr) {
		std::cout << "Error loading Gouraud Shader" << std::endl;
		exit(1);
	}
	if (PShader == nullptr) {
		std::cout << "Error loading Phong Shader" << std::endl;
		exit(1);
	}
	
	

	// Load the texture
	this->texture->Load("images/fruits.png");
	
	
	
	
	

	// Load the entity
	this->meshLee.LoadOBJ("meshes/lee.obj");
	Matrix44 modelLee;
	modelLee.SetIdentity();
	modelLee.Translate(0.0, -0.3, -0.3);
	this->Lee.SetModelMatrix(modelLee);

	this->textureLee = new Texture();
	this->textureLee->Load("textures/lee_normal.tga", true);
	this->Lee.SetTexture(textureLee);

	this->textureLeeSpecular = new Texture();
	this->textureLeeSpecular->Load("textures/lee_color_specular.tga");
	this->Lee.SetTextureSpecular(textureLeeSpecular);

	this->Lee.SetMesh(this->meshLee);

	material = new Material();
	material->color_texture = textureLeeSpecular;
	material->normal_texture = textureLee;

	material->shininess = 100;

	uniData.viewProjectionMatrix = camera.viewprojection_matrix;
	uniData.flag = Vector3(0.0, 0.0, 0.0);

	Ia = Vector3(0.1, 0.1, 0.1);

	material->shader = GShader;

	//Print all current variables
	printf("Current task: %d\n", current_task);
	printf("Current subtask: %d\n", current_subtask);
	printf("Current show: %d\n", current_show);
	printf("Current property: %d\n", current_property);
	printf("Current Pbool: %d\n", Pbool);
	printf("Current spec_or_normal: %d\n", spec_or_normal);
	printf("Current cam: %d\n", cam);
	printf("Current time: %f\n", time);
	printf("Current window width: %d\n", window_width);
	printf("Current window height: %d\n", window_height);
	printf("Current change task: %d\n", change_task);
	printf("Current Ia: %f\n", Ia.x);
	printf("Current Ia: %f\n", Ia.y);
	printf("Current Ia: %f\n", Ia.z);
	printf("Current flag: %f\n", flags[0]);
	printf("Current flag: %f\n", flags[1]);
	printf("Current flag: %f\n", flags[2]);
	printf("Current light position: %f\n", lights[0].position.x);
	printf("Current light position: %f\n", lights[0].position.y);
	printf("Current light position: %f\n", lights[0].position.z);
	printf("Current light Id: %f\n", lights[0].Id.x);
	printf("Current light Id: %f\n", lights[0].Id.y);
	printf("Current light Id: %f\n", lights[0].Id.z);
	printf("Current light Is: %f\n", lights[0].Is.x);
	printf("Current light Is: %f\n", lights[0].Is.y);
	printf("Current light Is: %f\n", lights[0].Is.z);
	printf("Current light position: %f\n", lights[1].position.x);
	printf("Current light position: %f\n", lights[1].position.y);
	printf("Current light position: %f\n", lights[1].position.z);
	printf("Current light Id: %f\n", lights[1].Id.x);
	printf("Current light Id: %f\n", lights[1].Id.y);
	printf("Current light Id: %f\n", lights[1].Id.z);
	printf("Current light Is: %f\n", lights[1].Is.x);
	printf("Current light Is: %f\n", lights[1].Is.y);
	printf("Current light Is: %f\n", lights[1].Is.z);
	printf("Current shininess: %f\n", material->shininess);
	printf("Current color_texture: %f\n", material->color_texture);
	printf("Current normal_texture: %f\n", material->normal_texture);
	printf("Current shader: %f\n", material->shader);
	printf("Current modelMatrix: %f\n", uniData.modelMatrix);


	


	

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

	
	
	
	//shader[current_show]->SetFloat("u_time", time);
	//shader[current_show]->SetFloat("u_ratio", (window_width / window_height)); // Not needed, Don't resize the window
	//shader[current_show]->SetFloat("u_rotationAngle", 30*DEG2RAD);

	lights[0].Id = Vector3(1, 1, 1);
	lights[0].Is = Vector3(1, 1, 1);
	lights[0].position = Vector3(1.5, 1.0, 1.5);

	lights[1].Id = Vector3(1, 1, 1);
	lights[1].Is = Vector3(1, 1, 1);
	lights[1].position = Vector3(-1.5, 1.0, 1.5);

	uniData.scenelights[0] = lights[0];
	uniData.scenelights[1] = lights[1];

	uniData.modelMatrix = Lee.GetModelMatrix();
	uniData.viewProjectionMatrix = camera.viewprojection_matrix;


	

	uniData.Ia = Ia;
	material->color_texture = textureLeeSpecular;
	material->normal_texture = textureLee;

	if (Pbool == false) {
		material->shader = GShader;
	}
	else {
		material->shader = PShader;
	}

	uniData.flag = { flags[0],flags[1],flags[2]};

	printf("x of flag %f", uniData.flag.x);
	printf("y of flag %f", uniData.flag.y);
	printf("z of flag %f", uniData.flag.z);

	


	uniData.cameraPosition = camera.eye;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//enShader->SetTexture("u_lee", u_lee);
	Lee.Render(&camera, uniData);
	Lee.material.shader->Disable();

	

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