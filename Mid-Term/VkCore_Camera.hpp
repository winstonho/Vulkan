#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace VkHelper {

    class Camera {
    public:
        void setOrthographicProjection(
            float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        void setViewDirection(
            glm::vec3 position, glm::vec3 direction, glm::vec3 up); //= glm::vec3{ 0.f, -1.f, 0.f });
        void setViewTarget(
            glm::vec3 position, glm::vec3 target, glm::vec3 up); //= glm::vec3{ 0.f, -1.f, 0.f });
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4& getProjection() const { return projectionMatrix; }
        const glm::mat4& getView() const { return viewMatrix; }

        // camera Attributes
        glm::vec3 Position{ 0.f, 0.f, .5f };
        glm::vec3 Direction{ 0.f, 0.f, 1.f };
        glm::vec3 Orientation{0.f, 0.f, -1.f};
        glm::vec3 Target{ 0.f, 1.f, 0.f };
        glm::vec3 Up{ 0.f, 1.f, 0.f };
        glm::vec3 Eye{ 0.f, 0.f, -10.f };
        glm::vec3 Centre{ 0.f, 0.f, 0.f };
        glm::vec3 Rotation{0.f, 0.f, 0.f};
        glm::vec3 Translation{};

        glm::mat4 View{ 1.f };
        glm::mat4 projection = glm::mat4(1.0f);

        int Width;
        int Height;

        // euler Angles
        float Yaw; // Left and Right , rotate about Y - Axis
        float Pitch; // Up and Down

        // camera options
        //float Speed = 0.015f;
        float moveSpeed = 10.0f;
        float lookSpeed = 1.0f;
        float Sensitivity = 100.0f;
        float Zoom = 1.0f;

        float FOV{22.5f}; //22.5
        float NearVal{ 0.1f };
        float FarVal{ 50.f };


        glm::mat4 viewMatrix{ 1.f };
        glm::mat4 projectionMatrix{ 1.f };

    private:
    };
}