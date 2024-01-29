#include "entity.h"
#include <glm/glm.hpp>
#include "camera.h"
#include "image.h"
#include "color.h"
#include "framework.h"


Entity::Entity() : Modelmatrix(glm::mat4(1.0f)), mesh() {}

Entity::Entity(const glm::mat4& Modelmatrix) : Modelmatrix(Modelmatrix), mesh() {}

Entity::Entity(const glm::mat4& Modelmatrix, const Mesh& mesh) : Modelmatrix(Modelmatrix), mesh(mesh) {}

void Entity::SetModelMatrix(const glm::mat4& Modelmatrix) {
    this->Modelmatrix = Modelmatrix;
}

void Entity::SetMesh(const Mesh& mesh) {
    this->mesh = mesh;
}

const glm::mat4& Entity::GetModelMatrix() const {
    return Modelmatrix;
}

const Mesh& Entity::GetMesh() const {
    return mesh;
}
void Entity::Render(Image* framebuffer, Camera* camera, const Color& wireframeColor) {
    const std::vector<Vector3>& vertices = GetVertices();

    for (size_t i = 0; i < vertices.size(); i += 3) {
        // Get the vertices of the current triangle
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        // Transform vertices from local space to world space
        v0 = glm::vec3(GetModelMatrix() * glm::vec4(v0, 1.0f));
        v1 = glm::vec3(GetModelMatrix() * glm::vec4(v1, 1.0f));
        v2 = glm::vec3(GetModelMatrix() * glm::vec4(v2, 1.0f));

        // Project vertices to clip space
        bool negZ0, negZ1, negZ2;
        glm::vec4 clipPos0 = camera->ProjectVector(v0, negZ0);
        glm::vec4 clipPos1 = camera->ProjectVector(v1, negZ1);
        glm::vec4 clipPos2 = camera->ProjectVector(v2, negZ2);

        // Check if any vertex is outside the camera frustum
        if (negZ0 || negZ1 || negZ2) {
            // Skip rendering this triangle
            continue;
        }

        // Convert clip space positions to screen space
        glm::vec3 screenPos0 = glm::vec3((clipPos0.x + 1.0f) * 0.5f * framebuffer->GetWidth(),
            (1.0f - clipPos0.y) * 0.5f * framebuffer->GetHeight(),
            clipPos0.z);
        glm::vec3 screenPos1 = glm::vec3((clipPos1.x + 1.0f) * 0.5f * framebuffer->GetWidth(),
            (1.0f - clipPos1.y) * 0.5f * framebuffer->GetHeight(),
            clipPos1.z);
        glm::vec3 screenPos2 = glm::vec3((clipPos2.x + 1.0f) * 0.5f * framebuffer->GetWidth(),
            (1.0f - clipPos2.y) * 0.5f * framebuffer->GetHeight(),
            clipPos2.z);

        framebuffer->DrawLine(screenPos0.x, screenPos0.y, screenPos1.x, screenPos1.y, wireframeColor);
        framebuffer->DrawLine(screenPos1.x, screenPos1.y, screenPos2.x, screenPos2.y, wireframeColor);
        framebuffer->DrawLine(screenPos2.x, screenPos2.y, screenPos0.x, screenPos0.y, wireframeColor);
    }
}