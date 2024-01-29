#pragma once 

#include "framework.h"
#include "mesh.h"
#include <glm/glm.hpp>
#include "camera.h"
#include "image.h"
#include "color.h"

class Entity {
private:
	Mesh mesh;
	glm::mat4 Modelmatrix;
public:
    Entity();  //Identity matrix constructor

    Entity(const glm::mat4& Modelmatrix);// Constructor with a  model matrix

    Entity(const glm::mat4& Modelmatrix, const Mesh& mesh);// Constructor with model matrix and mesh


    void SetModelMatrix(const glm::mat4& Modelmatrix);
    void SetMesh(const Mesh& mesh);

    const glm::mat4& GetModelMatrix() const;
    const Mesh& GetMesh() const;

    void Entity::Render(Image* framebuffer, Camera* camera, const Color& wireframeColor);

};