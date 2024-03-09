#pragma once 

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"
#include "shader.h"
#include "material.h"

class Entity {
private:
	Mesh mesh;
	Matrix44 Modelmatrix;
    Shader * shader;
public:
    enum class eRenderMode {
        POINTCLOUD,
        WIREFRAME,
        TRIANGLES,
        TRIANGLES_INTERPOLATED
    };

    bool occlusion = true;
    eRenderMode mode;
    Texture* texture;
    Texture* texture_specular;
    Material material;

    Image* ImgText;

    Entity();  //Identity matrix constructor

    Entity(const Matrix44& Modelmatrix);// Constructor with a  model matrix

    Entity(const Matrix44& Modelmatrix, const Mesh& mesh);// Constructor with model matrix and mesh

    Entity(const Mesh& mesh);// Constructor with mesh

    Entity(const Matrix44& Modelmatrix, const Mesh& mesh, Shader* shader);// Constructor with model matrix, mesh and shader

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
    void SetTexture(Texture* texture);
    void SetTextureSpecular(Texture* texture_specular);
    void SetRenderMode(eRenderMode mode);
    void SetOcclusion(bool occlusion);
    void SetShader(Shader* shader);

    const Matrix44& GetModelMatrix() const;
    const Mesh& GetMesh() const;
    const Shader* GetShader() const;
    const Texture* GetTexture() const;
    const Texture* GetTextureSpecular() const;
    eRenderMode GetRenderMode() const;
    bool GetOcclusion() const;



    void Entity::Render(Camera* camera, sUniformData uniformData);

    void Entity::Render(Image* framebuffer, Camera* camera, const Color& wireframeColor, FloatImage* zBuffer);

};
