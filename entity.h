#pragma once 

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"


/* Create a struct containing all the information needed to raster a triangle (e.g. sTriangleInfo):
       3 Vertices
       3 UVs
       3 Colors
       A pointer to an Image, the texture
 */
struct sTriangleInfo {
    Vector3 vertices[3];
    Vector2 uvs[3];
    Color colors[3];
    class Image* texture;
};

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

   
    sTriangleInfo TrInfo;
    


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
    void SetRenderMode(eRenderMode mode);
    void SetTexture(class Image* texture);

    const Matrix44& GetModelMatrix() const;
    const Mesh& GetMesh() const;
    const class Image* GetTexture() const;

    void Entity::Render(class Image* framebuffer, class Camera* camera, const Color& wireframeColor, class FloatImage* zBuffer);

};
