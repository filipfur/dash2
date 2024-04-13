#pragma once

#include "glm/glm.hpp"
#include "glnode.h"
#include "gluniformbuffer.h"


class Camera : public lix::Node
{
public:
    Camera(const glm::mat4& projection)
        : _block{projection, glm::mat4{1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}}
    {

    }

    void setTarget(const glm::vec3& target)
    {
        _target = target;
        _targetInvalid = true;
    }

    void setupUBO(const std::vector<lix::Element*>& shaders)
    {
        _ubo.reset(new lix::UniformBuffer(
            sizeof(CameraBlock), (void*)&_block, "CameraBlock", 0, GL_STATIC_DRAW
        ));
        _ubo->bindShaders(shaders);
    }

    void refresh(float dt)
    {
        bool positionUpdated{false};
        if(updateModelMatrix() || updateGlobalMatrix())
        {
            const glm::mat4& glob = globalMatrix();
            _targetPosition = glm::vec3(glob[3]);
            positionUpdated = true;
        }
        if(positionUpdated || _targetInvalid)
        {
            _block.view = glm::lookAt(_block.position, _target, Camera::UP);
            _targetInvalid = false;
            positionUpdated = true;
        }

        glm::vec3 d{_block.position - _targetPosition};
        if(length(d) > 0)
        {
            _block.position = glm::mix(_block.position, _targetPosition, 4.0f * dt);
            positionUpdated = true;
        }

        if(_viewInvalid || positionUpdated)
        {
            _ubo->bufferSubData(sizeof(glm::mat4), sizeof(glm::mat4) + sizeof(glm::vec3)); // view + position
            _viewInvalid = false;
        }
    }

    const glm::vec3& position() const
    {
        return _block.position;
    }

private:
    struct CameraBlock {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 position;
        float padding;
    } _block;

    bool _viewInvalid{true};

    std::shared_ptr<lix::UniformBuffer> _ubo;

    glm::vec3 _targetPosition{0.0f, 0.0f, 0.0f};

    glm::vec3 _target{0.0f, 0.0f, 0.0f};
    bool _targetInvalid{false};

    static constexpr glm::vec3 UP{0.0f, 0.0f, 1.0f};

};