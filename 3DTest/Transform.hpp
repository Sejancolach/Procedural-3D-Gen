#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include "Component.hpp"

namespace Component {
    class Component;

    class Transform :
        public Component {

    public:
        glm::vec3 m_pos;
        glm::quat m_rot;
        glm::vec3 m_scale;

        glm::mat4 matrix;

    public:
        Transform();
        Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);

        void SetPosition(const glm::vec3& pos);
        void AddPosition(const glm::vec3& pos);
        glm::vec3 getPosition(void);
        glm::mat4 getMatrix(void);

    private:
        void UpdateMatrix();
    };
}

