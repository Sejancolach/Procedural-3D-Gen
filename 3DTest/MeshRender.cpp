#include "MeshRender.hpp"
#include <GL/glew.h>
#include "Transform.hpp"
#include "Shader.hpp"

Component::MeshRender::MeshRender() { 
    ShaderID = 0;
    mesh = nullptr;
}

void Component::MeshRender::Render(glm::mat4 mvp, glm::mat4 depthBiasMVP) {
    if(Texture1Location == 0) {
        Texture1Location = glGetUniformLocation(ShaderID, "TextureSampler1");
        Texture2Location = glGetUniformLocation(ShaderID, "TextureSampler2");

        glUseProgram(ShaderID);
        glUniform1i(Texture1Location, 1);
        glUniform1i(Texture2Location, 2);
    }
    mvp = mvp * gameObject->transform->getMatrix();
    depthBiasMVP = depthBiasMVP * gameObject->transform->getMatrix();
    glUseProgram(ShaderID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute 0 vertex Position
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glUniformMatrix4fv(glGetUniformLocation(ShaderID, "MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ShaderID, "LightBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
    glm::vec3 pos = gameObject->transform->getPosition();
    glUniform3f(glGetUniformLocation(ShaderID, "worldPosition"), pos.x, pos.y, pos.z);
  
    if(!mesh->normals.empty()) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBuffer);
        glVertexAttribPointer(
            2,              // attribute 2 vertex Normal
            3,              // size
            GL_FLOAT,       // type
            GL_TRUE,        // normalized
            0,              // stride
            (void*)0        // offset
        );
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureID2);

    if(mesh->indices.empty()) {
        glDrawArrays(GL_TRIANGLES, 0, mesh->triangleCount);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceBuffer);
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Component::MeshRender::ShadowRender(glm::mat4 mvp) { 
    mvp = mvp * gameObject->transform->getMatrix();
    //glUseProgram(util::Shader::CompileShader("simpleDepthShader"));
    glUniformMatrix4fv(glGetUniformLocation(util::Shader::CompileShader("simpleDepthShader"), "MVP"), 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute 0 vertex Position
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    if(mesh->indices.empty()) {
        glDrawArrays(GL_TRIANGLES, 0, mesh->triangleCount);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceBuffer);
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }
    glDisableVertexAttribArray(0);
}
