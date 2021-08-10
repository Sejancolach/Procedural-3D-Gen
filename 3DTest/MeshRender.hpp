#pragma once
#include "Component.hpp"
#include "Mesh.hpp"
#include <glm/glm.hpp>

class Mesh;
namespace Component {
    class Component;

    class MeshRender :
        public Component {

    private:
        GLuint Texture1Location = 0;
        GLuint Texture2Location = 0;

    public:
        MeshRender();
        GLuint ShaderID = 0;
        GLuint TextureID = 0;
        GLuint TextureID2 = 0;
        Mesh *mesh;
        void Render(glm::mat4 mvp);
    };
}
