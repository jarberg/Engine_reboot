#include "core/components.h"

#include <cmath>


void createRotationMatrixXYZ(double pitch, double yaw, double roll, float out[16]) {
    // Rotation around X-axis (pitch)
    float cx = std::cos(pitch);
    float sx = std::sin(pitch);

    // Rotation around Y-axis (yaw)
    float cy = std::cos(yaw);
    float sy = std::sin(yaw);

    // Rotation around Z-axis (roll)
    float cz = std::cos(roll);
    float sz = std::sin(roll);

    // Combined rotation matrix (Rz * Ry * Rx)
    out[0] = cy * cz;
    out[1] = sx * sy * cz + cx * sz;
    out[2] = -cx * sy * cz + sx * sz;
    out[3] = 0.0f;

    out[4] = -cy * sz;
    out[5] = -sx * sy * sz + cx * cz;
    out[6] = cx * sy * sz + sx * cz;
    out[7] = 0.0f;

    out[8] = sy;
    out[9] = -sx * cy;
    out[10] = cx * cy;
    out[11] = 0.0f;

    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = 1.0f;
}

void CharacterComponent::inputEvent(Entity owner, std::shared_ptr<KeyPressedEvent> e)
{
    unsigned int ctrlPressed = InputHandler::GetInstance()->keyStates[17];
    unsigned int altPressed = InputHandler::GetInstance()->keyStates[18];

    World* world = owner.get_world();
    float dt = world->get_delta_time();
    if (!world->has_component<RotationComponent>(owner)) {
        return;
    }

    if (ctrlPressed) {
        RotationComponent& rotComp = world->get_component<RotationComponent>(owner);
        switch (e->key) {
        case KeyCode::Left:
            rotComp.z -= 1.0f * dt;
            break;
        case KeyCode::Right:
            rotComp.z += 1.0f * dt;
            break;
        case KeyCode::Up:
            rotComp.y += 1.0f * dt;
            break;
        case KeyCode::Down:
            rotComp.y -= 1.0f * dt;
            break;
        }

        createRotationMatrixXYZ(rotComp.x, rotComp.y, rotComp.z, rotComp.matrix);
    }
    else
    {
        PositionComponent& posComp = world->get_component<PositionComponent>(owner);

        if (altPressed) {
            switch (e->key) {
            case KeyCode::Up:
                posComp.z += 1.0f * dt;

                break;
            case KeyCode::Down:
                posComp.z -= 1.0f * dt;
                break;
            }
        }
        else {
            switch (e->key) {
            case KeyCode::Left:
                posComp.x -= 1.0f * dt;
                break;
            case KeyCode::Right:
                posComp.x += 1.0f * dt;
                break;
            case KeyCode::Up:
                posComp.y += 1.0f * dt;

                break;
            case KeyCode::Down:
                posComp.y -= 1.0f * dt;
                break;
            }
        }

    }

}
