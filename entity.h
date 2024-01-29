#pragma once 

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"
#include "color.h"

class Entity {
private:
	Mesh mesh;
	Matrix44 Modelmatrix;
public:
    Entity();  //Identity matrix constructor

    Entity(const Matrix44& Modelmatrix);// Constructor with a  model matrix

    Entity(const Matrix44& Modelmatrix, const Mesh& mesh);// Constructor with model matrix and mesh


    void SetModelMatrix(const Matrix44& Modelmatrix);
    void SetMesh(const Mesh& mesh);

    const Matrix44& GetModelMatrix() const;
    const Mesh& GetMesh() const;

    void Entity::Render(Image* framebuffer, Camera* camera, const Color& wireframeColor);

};
