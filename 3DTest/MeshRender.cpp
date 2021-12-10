#include "MeshRender.hpp"
#include <GL/glew.h>
#include "Transform.hpp"
#include "Shader.hpp"

bool Component::MeshRender::IsVisibleToCamera(glm::mat4 mvp) {
    glm::vec4 c = mvp * glm::vec4(gameObject->transform->getPosition(), 1);
    return  (abs(c.x)-512) < c.w &&
            (abs(c.z)-512) < c.w;
}

Component::MeshRender::MeshRender() {
    ShaderID = util::Shader::GetDefaultShader();
    mesh = nullptr;
}

void Component::MeshRender::Render(glm::mat4 mvp) {
    if(!IsVisibleToCamera(mvp)) return;
    if(lastUsedShader != ShaderID) {
        glUseProgram(ShaderID);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, TextureID2);
        lastUsedShader = ShaderID;
    }
    if(Texture1Location == 0) {
        Texture1Location = glGetUniformLocation(ShaderID, "TextureSampler1");
        Texture2Location = glGetUniformLocation(ShaderID, "TextureSampler2");
        glUniform1i(Texture1Location, 8);
        glUniform1i(Texture2Location, 9);
    }

    mvp = mvp * gameObject->transform->getMatrix();
    glUniform3f(glGetUniformLocation(ShaderID, "diffuseColor"), .8f,.6f,.5f);
    for(int i = 0; i < mesh->indices.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glUniformMatrix4fv(glGetUniformLocation(ShaderID, "MVP"), 1, GL_FALSE, &mvp[0][0]);
        glm::vec3 pos = gameObject->transform->getPosition();
        glUniform3f(glGetUniformLocation(ShaderID, "worldPosition"), pos.x, pos.y, pos.z);
        if(!mesh->normals.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBuffer);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceBuffer[i]);
        glDrawElements(GL_TRIANGLES, mesh->indices[i].size(), GL_UNSIGNED_INT, (void*)0);
        glUniform3f(glGetUniformLocation(ShaderID, "diffuseColor"), .6f, .85f, .65f);
    }
    
    if(DrawBBDMesh) {
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mvp = mvp * glm::scale(glm::vec3{ 1.0f, 1.0f, 1.0f });
        glUniformMatrix4fv(glGetUniformLocation(ShaderID, "MVP"), 1, GL_FALSE, &mvp[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, DebugDrawMesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, DebugDrawMesh->normalBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DebugDrawMesh->indiceBuffer[0]);
        glDrawElements(GL_TRIANGLES, DebugDrawMesh->indices[0].size(), GL_UNSIGNED_INT, (void*)0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Component::MeshRender::ShadowRender(glm::mat4 mvp) { 
    if(!IsVisibleToCamera(mvp)) return;

    mvp = mvp * gameObject->transform->getMatrix();
    for(int i = 0; i < mesh->indices.size(); i++) {
        glUniformMatrix4fv(glGetUniformLocation(DepthShaderID, "MVP"), 1, GL_FALSE, &mvp[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceBuffer[i]);
        glDrawElements(GL_TRIANGLES, mesh->indices[i].size(), GL_UNSIGNED_INT, (void*)0);
    }

    if(DrawBBDMesh) {
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mvp = mvp * glm::scale(glm::vec3{ 1.0f, 1.0f, 1.0f });
        glUniformMatrix4fv(glGetUniformLocation(ShaderID, "MVP"), 1, GL_FALSE, &mvp[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, DebugDrawMesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DebugDrawMesh->indiceBuffer[0]);
        glDrawElements(GL_TRIANGLES, DebugDrawMesh->indices[0].size(), GL_UNSIGNED_INT, (void*)0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glDisableVertexAttribArray(0);
}
GLuint Component::MeshRender::lastUsedShader = 0xFFFF;
GLuint Component::MeshRender::DepthShaderID = 0xFFFF;
Mesh* Component::MeshRender::DebugDrawMesh = nullptr;