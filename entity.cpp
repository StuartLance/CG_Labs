#include "entity.h"
#include "camera.h"
#include "image.h"
#include "framework.h"


Entity::Entity() {
    Matrix44 Modelmatrix;
    Mesh mesh;
    mode = eRenderMode::TRIANGLES_INTERPOLATED;

    TrInfo.texture = nullptr; // Set texture to nullptr if no mesh is loaded

}

Entity::Entity(const Matrix44& Modelmatrix) {
    this->Modelmatrix = Modelmatrix;
    Mesh mesh;
    mode = eRenderMode::TRIANGLES_INTERPOLATED;

    TrInfo.texture = nullptr; // Set texture to nullptr if no mesh is loaded
}

Entity::Entity(const Matrix44& Modelmatrix, const Mesh& mesh) {
    this->Modelmatrix = Modelmatrix;
    this->mesh = mesh;
    mode = eRenderMode::TRIANGLES_INTERPOLATED;
}

//If no matrix is passed, the entity will have an identity matrix
Entity::Entity(const Mesh& mesh) {
	this->mesh = mesh;
	this->Modelmatrix.SetIdentity();
    mode = eRenderMode::TRIANGLES_INTERPOLATED;

    //TrInfo.texture = ;
}

// Destructor
Entity::~Entity() {
}

// Model matrix rotation
void Entity::Rotate(float angle, const Vector3& axis) {
	Modelmatrix.Rotate(angle, axis);
}

// Model matrix translation
void Entity::Translate(float x, float y, float z) {
	Modelmatrix.Translate(x, y, z);
}

// Model matrix scale
void Entity::Scale(float x, float y, float z) {
	Modelmatrix.m[0] *= x;
    Modelmatrix.m[5] *= y;
    Modelmatrix.m[10] *= z;
}

//Update entity with time
void Entity::UpdateRotate(float seconds_elapsed) {
    // Make the entity rotate on y axis
    Rotate(seconds_elapsed * 10.0f, Vector3(0.0f, 1.0f, 0.0f));

    // if rotate on own axis
    //Rotate(seconds_elapsed * 10.0f, use y axis of entity here)
}

// update with translation, move forward and back to the start
void Entity::UpdateTranslate(float seconds_elapsed) {

    // Calculate the position based on time
    float position = seconds_elapsed;

    // Check if the entity should move forward or back
    if (((int)seconds_elapsed % 20) > 10) {
        // Move the entity back to the start
        position = 10.0f - position;
    }

    // Translate the entity along the x-axis
    Translate(position, 0.0f, 0.0f);
}


// update with scale. Grow the object
void Entity::UpdateScale(float seconds_elapsed) {
    	// Calculate the scale based on time
	float scale = 1.0f + seconds_elapsed;

	// Check if the entity should grow or shrink
    if (((int)seconds_elapsed % 20) > 10) {
		// Shrink the entity back to its original size
		scale = 1.0f / scale;
	}

	// Scale the entity
	Scale(scale, scale, scale);
}


// update with moving in a circle
void Entity::UpdateCircle(float seconds_elapsed) {
	// Make entity move in a circle
	float radius = 5.0f;
	float speed = 1.0f;
	float angle = seconds_elapsed * speed;
	float x = radius * cos(angle);
	float y = radius * sin(angle);
	Translate(x, y, 0.0f);
}

//      SETTERS

void Entity::SetModelMatrix(const Matrix44& Modelmatrix) {
    this->Modelmatrix = Modelmatrix;
}

void Entity::SetMesh(const Mesh& mesh) {
    this->mesh = mesh;
}

void Entity::SetRenderMode(eRenderMode mode) {
	this->mode = mode;
}

void Entity::SetTexture(Image* texture) {
	TrInfo.texture = texture;
}

//      GETTERS

const Matrix44& Entity::GetModelMatrix() const {
    return Modelmatrix;
}

const Mesh& Entity::GetMesh() const {
    return mesh;
}

/*
1)Get the vertices of the mesh and iterate them

2)In each iteration :
    a) Transform the local space of the vertices to world space using the model matrix of the entity
    b) Project each of the world space vertices to clip space positions using your current camera
    c) Before drawing each of the triangle lines, convert the clip space positions to screen space using the framebuffer width and height.
*/
void Entity::Render(Image* framebuffer, Camera* camera, const Color& wireframeColor, FloatImage* zBuffer) {
    const std::vector<Vector3>& vertices = mesh.GetVertices();
    const std::vector<Vector2>& uvs = mesh.GetUVs(); // Lab 3, get the uvs of the mesh

    for (size_t i = 0; i < vertices.size(); i += 3) {
        // Get the vertices of the current triangle
        TrInfo.vertices[0] = vertices[i];
        TrInfo.vertices[1] = vertices[i + 1];
        TrInfo.vertices[2] = vertices[i + 2];

        // Transform vertices from local space to world space
        TrInfo.vertices[0] = Modelmatrix * TrInfo.vertices[0];
        TrInfo.vertices[1] = Modelmatrix * TrInfo.vertices[1];
        TrInfo.vertices[2] = Modelmatrix * TrInfo.vertices[2];

        // Project vertices to clip space. `negZ` will be true if the vertex is behind the camera, so we can skip rendering it
        bool negZ0, negZ1, negZ2;

        // Project vertices to clip space with the current camera
        TrInfo.vertices[0] = camera->ProjectVector(TrInfo.vertices[0], negZ0);
        TrInfo.vertices[1] = camera->ProjectVector(TrInfo.vertices[1], negZ1);
        TrInfo.vertices[2] = camera->ProjectVector(TrInfo.vertices[2], negZ2);

        // Check if any vertex is outside the camera frustum
        if (negZ0 || negZ1 || negZ2) {
            // Skip rendering this triangle
            continue;
        }

        // Convert clip space positions to screen space. Clip space is between -1 and 1, screen space is between [0, width] and [0, height]
        // So we need to add 1 to the x and y coordinates and multiply by 0.5 widths to get the screen space coordinates
        // We also DON'T need to flip the y coordinate because the screen space origin is at the BOTTOM left corner, not the TOP left corner

        // LAB 2 Slide 30: x:[0, W-1], y:[0, H-1]
        TrInfo.vertices[0] = Vector3((TrInfo.vertices[0].x + 1.0f) * 0.5f * (framebuffer->width - 1), (1.0f + TrInfo.vertices[0].y) * 0.5f * (framebuffer->height - 1), TrInfo.vertices[0].z);
        TrInfo.vertices[1] = Vector3((TrInfo.vertices[1].x + 1.0f) * 0.5f * (framebuffer->width - 1), (1.0f + TrInfo.vertices[1].y) * 0.5f * (framebuffer->height - 1), TrInfo.vertices[1].z);
        TrInfo.vertices[2] = Vector3((TrInfo.vertices[2].x + 1.0f) * 0.5f * (framebuffer->width - 1), (1.0f + TrInfo.vertices[2].y) * 0.5f * (framebuffer->height - 1), TrInfo.vertices[2].z);


        if (mode == eRenderMode::POINTCLOUD)
        {
			// Draw the vertices of the mesh
			framebuffer->SetPixelSafe(TrInfo.vertices[0].x, TrInfo.vertices[0].y, wireframeColor);
			framebuffer->SetPixelSafe(TrInfo.vertices[1].x, TrInfo.vertices[1].y, wireframeColor);
			framebuffer->SetPixelSafe(TrInfo.vertices[2].x, TrInfo.vertices[2].y, wireframeColor);
		}
        else if (mode == eRenderMode::WIREFRAME)
        {
			// Draw the wireframe of the mesh
			framebuffer->DrawLineDDA(TrInfo.vertices[0].x, TrInfo.vertices[0].y, TrInfo.vertices[1].x, TrInfo.vertices[1].y, wireframeColor);
			framebuffer->DrawLineDDA(TrInfo.vertices[1].x, TrInfo.vertices[1].y, TrInfo.vertices[2].x, TrInfo.vertices[2].y, wireframeColor);
			framebuffer->DrawLineDDA(TrInfo.vertices[2].x, TrInfo.vertices[2].y, TrInfo.vertices[0].x, TrInfo.vertices[0].y, wireframeColor);
		}
        else if (mode == eRenderMode::TRIANGLES)
        {
			// Draw the filled triangle
			framebuffer->DrawTriangle(TrInfo.vertices[0].GetVector2(), TrInfo.vertices[1].GetVector2(), TrInfo.vertices[2].GetVector2(), wireframeColor, true, wireframeColor);
		}
        else if (mode == eRenderMode::TRIANGLES_INTERPOLATED)
        {
            // Convert texture coordinates from 0-1 range to 0 - width-1 and 0 - height-1 range
            TrInfo.uvs[0] = uvs[i];
            TrInfo.uvs[1] = uvs[i + 1];
            TrInfo.uvs[2] = uvs[i + 2];

            TrInfo.uvs[0].x = TrInfo.uvs[0].x * (TrInfo.texture->width - 1);
            TrInfo.uvs[0].y = TrInfo.uvs[0].y * (TrInfo.texture->height - 1);

            TrInfo.uvs[1].x = TrInfo.uvs[1].x * (TrInfo.texture->width - 1);
            TrInfo.uvs[1].y = TrInfo.uvs[1].y * (TrInfo.texture->height - 1);

            TrInfo.uvs[2].x = TrInfo.uvs[2].x * (TrInfo.texture->width - 1);
            TrInfo.uvs[2].y = TrInfo.uvs[2].y * (TrInfo.texture->height - 1);

            // Get colour of the vertices from the texture, using the UVs
            Color c0 = TrInfo.texture->GetPixel(TrInfo.uvs[0].x, TrInfo.uvs[0].y);
            Color c1 = TrInfo.texture->GetPixel(TrInfo.uvs[1].x, TrInfo.uvs[1].y);
            Color c2 = TrInfo.texture->GetPixel(TrInfo.uvs[2].x, TrInfo.uvs[2].y);

			// Draw the interpolated triangle with red, green and blue vertices, using new DrawTriangleInterpolated function
			
            if (TrInfo.texture != nullptr)
            {
				framebuffer->DrawTriangleInterpolated(&TrInfo, zBuffer);
			}
            else
            {
				//framebuffer->DrawTriangleInterpolated(TrInfo.vertices[0], TrInfo.vertices[1], TrInfo.vertices[2], Color::RED, Color::GREEN, Color::BLUE, zBuffer);
			}
            
            //framebuffer->DrawTriangleInterpolated(TrInfo.vertices[0], TrInfo.vertices[1], TrInfo.vertices[2], Color::RED, Color::GREEN, Color::BLUE, zBuffer);
		}
        //Change to wireframe render
        //framebuffer->DrawLineDDA(screenPos0.x, screenPos0.y, screenPos1.x, screenPos1.y, wireframeColor);
        //framebuffer->DrawLineDDA(screenPos1.x, screenPos1.y, screenPos2.x, screenPos2.y, wireframeColor);
        //framebuffer->DrawLineDDA(screenPos2.x, screenPos2.y, screenPos0.x, screenPos0.y, wireframeColor);

        
        // Draw the interpolated triangle with red, green and blue vertices, using new DrawTriangleInterpolated function
        //framebuffer->DrawTriangleInterpolated(screenPos0, screenPos1, screenPos2, Color::RED, Color::GREEN, Color::BLUE);


    }
}
