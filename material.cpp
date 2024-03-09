#include "material.h"

Material::Material()
{
}

void Material::Enable(const sUniformData& uniformData) {
    Vector3 position = { 15, 15, 50 }; //Light position
    Vector3 Id = { 1,1,1 }; //Diffuse light
    Vector3 Is = { 1,1,1 }; //Specular light

    shader->Enable(); //Enable the shader
    glEnable(GL_DEPTH_TEST); //Enable the depth test

    
    glDepthFunc(GL_LEQUAL); //Z-buffer 

    
    shader->SetVector3("Ka", Ka); //Ambient light
    shader->SetVector3("Kd", Kd); //Diffuse light
    shader->SetVector3("Ks", Ks); //Specular light
    shader->SetFloat("shininess", shininess); //Shininess
    shader->SetTexture("u_color_texture", color_texture); //Diffuse texture
    shader->SetTexture("u_normal_texture", normal_texture); //Normal texture
    shader->SetMatrix44("u_modelMatrix", uniformData.modelMatrix); //Model matrix
    shader->SetMatrix44("u_viewProjection", uniformData.viewProjectionMatrix); //View projection matrix
    shader->SetVector3("cameraPosition", uniformData.cameraPosition); //Camera position
    shader->SetVector3("Ia", uniformData.Ia); //Ambient light
    shader->SetVector3("lightPosition", uniformData.scenelights[0].position); //Light position
    shader->SetVector3("Id", uniformData.scenelights[0].Id); //Diffuse light
    shader->SetVector3("Is", uniformData.scenelights[0].Is); //Specular light
    shader->SetVector3("flag", uniformData.flag); //Flag

}


void Material::Disable() {
    shader->Disable();
}