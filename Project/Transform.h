#include <glm/ext/matrix_float4x4.hpp>

class Transform
{
private:
    glm::mat4 m_WorldMatrix{glm::mat4(1)};
    glm::mat4 m_RotationMatrix{glm::mat4(1.f)};
    glm::mat4 m_TranslationMatrix{glm::mat4(1.f)};
    glm::mat4 m_ScaleMatrix{glm::mat4(1.f)};

    bool m_NeedsUpdate = true;
    glm::vec3 m_RotationPerSecond{0.f};
public:
    void Update();
    void SetRotationPerSecond(const glm::vec3& rotationPerSecond);
    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& rotation);
    void Scale(const glm::vec3& scale);
    [[nodiscard]] glm::mat4 GetWorldMatrix();
};


