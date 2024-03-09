
#pragma once 

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"
#include "shader.h"



struct sLight {
    Vector3 position = { 15, 15, 50 }; //Light position

    Vector3 Id; //Diffuse light
    Vector3 Is; //Specular light
    // Ambient light is not a property of the light, but of the scene
};

struct sUniformData {
    Matrix44 modelMatrix;
    Matrix44 viewProjectionMatrix;
    Vector3 cameraPosition;
    Vector3 Ia; //Ambient light
    sLight scenelights[2]; //Light properties
    Vector3 flag; //Used for different purposes
};


class Material {

public:
    Texture* color_texture; //Diffuse texture
    Texture* normal_texture; //Normal texture
    Shader* shader; //Shader program
    Vector3 Ka = { 1.0,1.0,1.0 }; //Ambient light
    Vector3 Kd = { 0.8,0.8,0.8 }; //Diffuse light
    Vector3 Ks = { 0.5,0.5,0.5 }; //Specular light
    float alpha = 1.0; //Alpha, used for transparency
    float shininess; //Shininess
     

    Material(); //Constructor

    void Material::Enable(const sUniformData& uniformData); //Enable the material
    void Material::Disable(); //Disable the material
};

