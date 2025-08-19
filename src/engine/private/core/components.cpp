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

CameraComponent::CameraComponent()
{
}
    
void CharacterComponent::inputEvent(Entity owner, std::shared_ptr <KeyEvent> e)
{   
    switch (e->action) {
    case KeyAction::Press: 
        inputPressedEvent(owner, e); break;
	case KeyAction::Repeat:
		inputHeldEvent(owner, e); break;
    case KeyAction::Release: 
		inputReleaseEvent(owner, e); break;
    }
}

void CharacterComponent::cursorDeltaEvent(Entity owner, std::shared_ptr<CursorMoveEvent> e)
{
    auto& reg = owner.get_world()->get_registry();
    if (auto ds = reg.try_get<DragStateComponent>(owner.get_handle()); ds && ds->active) {
        for (auto [ee, camera] : reg.view<CameraComponent>().each()) {

            int y = (camera.viewport.Y + camera.viewport.W - 1) - (int)e->posY; 
            Vec3 under = unprojectAtNDC(e->posX, y, ds->zNDC, camera.viewport, camera.inverseVP);

            auto& p = reg.get<PositionComponent>(owner.get_handle());
            p = { under.X + ds->offset.X,
                  under.Y + ds->offset.Y,
                  under.Z + ds->offset.Z };
            break;
        }
    }
}

void CharacterComponent::cursorPressedEvent(Entity owner, std::shared_ptr<CursorKeyEvent> e)
{
    auto& reg = owner.get_world()->get_registry();
    auto& ds = reg.get_or_emplace<DragStateComponent>(owner.get_handle());
    const auto pos = reg.get<PositionComponent>(owner.get_handle());

    for (auto [ee, camera] : reg.view<CameraComponent>().each()) {

        ds.zNDC = objectZndc(HMM_V3((float)pos.x, (float)pos.y, (float)pos.z), camera.VP);

        int y = camera.viewport.Y + camera.viewport.W - 1 - (int)e->posY;
        Vec3 grabWS = unprojectAtNDC(e->posX, y, ds.zNDC, camera.viewport, camera.inverseVP);

        ds.offset = HMM_V3( (float)pos.x - grabWS.X,
                            (float)pos.y - grabWS.Y,
                            (float)pos.z - grabWS.Z);
        ds.active = true;

        break;
    }
}

void CharacterComponent::cursorReleasedEvent(Entity owner, std::shared_ptr<CursorKeyEvent> e){

    if (e->key != KeyCode::Mouse01) return;
    auto& reg = owner.get_world()->get_registry();
    if (auto ds = reg.try_get<DragStateComponent>(owner.get_handle())) ds->active = false;
}

void CharacterComponent::cursorKeyEvent(Entity owner, std::shared_ptr<CursorKeyEvent> e)
{

    switch (e->action) {
        case KeyAction::Press: cursorPressedEvent(owner, e); break;
        case KeyAction::Release: cursorReleasedEvent(owner, e); break;
        case KeyAction::Repeat:;
    }
}
void CharacterComponent::inputHeldEvent(Entity owner, std::shared_ptr<KeyEvent> e)
{
    const bool ctrlPressed = (e->mod & 1u) != 0;
    const bool altPressed = (e->mod & 2u) != 0;

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
                posComp.x += 1.0f * dt;
                break;
            case KeyCode::Right:
                posComp.x -= 1.0f * dt;
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

void CharacterComponent::inputReleaseEvent(Entity owner, std::shared_ptr<KeyEvent> e)
{
	std::cout << "key Released " << int(e->key) << std::endl;
    switch (e->key) {
    case KeyCode::Mouse01:
        std::cout << "Mouse button 1 released" << std::endl;
        break;
    }
}

void CharacterComponent::inputPressedEvent(Entity owner, std::shared_ptr<KeyEvent> e)
{
    std::cout << "key pressed " << int(e->key) << std::endl;

}
