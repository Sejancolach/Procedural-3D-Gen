#include "Transform.hpp"
#include <glm/ext/quaternion_common.hpp>

void Component::Transform::UpdateMatrix() {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_pos);
    glm::mat4 rotate = glm::mat4_cast(m_rot);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);
    matrix = translate * rotate * scale;
}

Component::Transform::Transform() {
    m_pos = glm::vec3(0.0f);
    m_rot = glm::quat(1.0, 0.0, 0.0, 0.0);
    m_scale = glm::vec3(1.0f);
    UpdateMatrix();
}

Component::Transform::Transform(const glm::vec3& pos = glm::vec3(0.0f), const glm::quat& rot = glm::quat(1.0, 0.0, 0.0, 0.0), const glm::vec3& scale = glm::vec3(1.0f)) {
    m_pos = pos;
    m_rot = rot;
    m_scale = scale;
    UpdateMatrix();
}

void Component::Transform::SetPosition(const glm::vec3& pos) {
    m_pos = pos;
    UpdateMatrix();
}

void Component::Transform::AddPosition(const glm::vec3& pos) {
    m_pos += pos;
    UpdateMatrix();
}

glm::vec3 Component::Transform::getPosition(void) {
    return m_pos;
}

glm::mat4x4 Component::Transform::getMatrix() {
    return matrix;
}
