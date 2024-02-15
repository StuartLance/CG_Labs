#pragma once 

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"

class Entity {
private:
	Mesh mesh;
	Matrix44 Modelmatrix;
public:
    enum class eRenderMode {
        POINTCLOUD,
        WIREFRAME,
        TRIANGLES,
        TRIANGLES_INTERPOLATED
    };
    eRenderMode mode;

    Entity();  //Identity matrix constructor

    Entity(const Matrix44& Modelmatrix);// Constructor with a  model matrix

    Entity(const Matrix44& Modelmatrix, const Mesh& mesh);// Constructor with model matrix and mesh

    Entity(const Mesh& mesh);// Constructor with mesh

    ~Entity(); //Destructor

    void Rotate(float angle, const Vector3& axis);

    void Translate(float x, float y, float z);

    void Scale(float x, float y, float z);

    void UpdateRotate(float seconds_elapsed);

    void UpdateTranslate(float seconds_elapsed);

    void UpdateScale(float seconds_elapsed);

    void UpdateCircle(float seconds_elapsed);


    void SetModelMatrix(const Matrix44& Modelmatrix);
    void SetMesh(const Mesh& mesh);

    const Matrix44& GetModelMatrix() const;
    const Mesh& GetMesh() const;

    void Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer);

};
