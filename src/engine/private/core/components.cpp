#include "core/components.h"

#include <cmath>
#include <json.hpp>




using json = nlohmann::json;

CameraComponent::CameraComponent()
{
}

void createRotationMatrixXYZ(Vec3 rot, float out[16]) {
    // Rotation around X-axis (pitch)
    double pitch = rot.X;
    double yaw = rot.Y;
    double roll = rot.Z;
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

static inline HMM_Vec3 RotateV3ByQ(HMM_Vec3 v, HMM_Quat q)
{
    // v' = q * (v,0) * conjugate(q)
    HMM_Quat p = HMM_Q(v.X, v.Y, v.Z, 0.0f);
    HMM_Quat qc = HMM_Q(-q.X, -q.Y, -q.Z, q.W);
    HMM_Quat r = HMM_MulQ(HMM_MulQ(q, p), qc);
    return HMM_V3(r.X, r.Y, r.Z);
}

static inline HMM_Mat4 ConvertArrayToHMM_Mat4(const std::array<float, 16>& arr) {
    HMM_Mat4 mat;
    for (int i = 0; i < 4; ++i) {
        mat.Elements[i][0] = arr[i * 4 + 0];
        mat.Elements[i][1] = arr[i * 4 + 1];
        mat.Elements[i][2] = arr[i * 4 + 2];
        mat.Elements[i][3] = arr[i * 4 + 3];
    }
    return mat;
}

void CharacterComponent::cursorDeltaEvent(Entity owner, std::shared_ptr<CursorMoveEvent> e)
{
    auto& reg = owner.get_world()->get_registry();
	//std::cout << InputHandler::GetInstance()->cursorKeyHeldCode << std::endl;

    switch (KeyCode(InputHandler::GetInstance()->cursorKeyHeldCode)) {

    case KeyCode::Mouse01:
        if (auto ds = reg.try_get<DragStateComponent>(owner.get_handle()); ds && ds->active) {
            auto& p = reg.get<PositionComponent>(owner.get_handle());
            for (auto [ee, camera] : reg.view<CameraComponent>().each()) {

                int y = (camera.viewport.Y + camera.viewport.W - 1) - (int)e->posY;
                Vec3 under = unprojectAtNDC(e->posX, y, ds->zNDC, camera.viewport, camera.inverseVP);

                
                p = { under.X + ds->offset.X,
                      under.Y + ds->offset.Y,
                      under.Z + ds->offset.Z };
                break;
            }
            json j = {
                {"type", "C2S_Input"},
                {"pos", {p.x, p.y , p.z}} // array is compact for vectors
            };
 
            this->msgDispatcher->Dispatch(std::make_shared<msgEvent>(j.dump()));
        }

        break;

    case KeyCode::Mouse02:
        if (auto ds = reg.try_get<CursorDragStateComponent>(owner.get_handle()); ds && ds->active)
        {
            auto& rot = reg.get<RotationComponent>(owner.get_handle());

            // --- fetch CAMERA rotation (as a quaternion)
            HMM_Quat camQ;
            for (auto [ee, camera] : reg.view<CameraComponent>().each()) {
                camQ = HMM_M4ToQ_RH(ConvertArrayToHMM_Mat4(camera.cameraMatrix));
                break;
            }


            // ---- mouse deltas -> small angles (radians per pixel; tune these)
            const float sensX = 0.1f;
            const float sensY = 0.1f;

            const float yaw = -e->deltaX * sensX;   // left/right
            const float pitch = -e->deltaY * sensY;   // up/down (invert if you prefer)


            HMM_Vec3 camUp = HMM_NormV3(RotateV3ByQ(HMM_V3(0, 1, 0), camQ));
            HMM_Vec3 camRight = HMM_NormV3(RotateV3ByQ(HMM_V3(1, 0, 0), camQ));

            // ---- build delta rotation around camera Up/Right
            // Use _RH if your engine is right-handed; keep _LH if it's left-handed.
            HMM_Quat qYaw = HMM_QFromAxisAngle_LH(camUp, yaw);
            HMM_Quat qPitch = HMM_QFromAxisAngle_LH(camRight, pitch);

            // compose (order matters! try pitch*yaw if this feel is better)
            HMM_Quat qDelta = HMM_MulQ(qYaw, qPitch);

            // ---- apply relative to camera: new = qDelta * current
            rot.quaternion = HMM_NormQ(HMM_MulQ(qDelta, rot.quaternion));

            // ---- update rotation matrix from quaternion (ditch Euler)
            HMM_Mat4 M = HMM_QToM4(rot.quaternion);

            // Option A: memcpy (same layout: column-major, contiguous)
            memcpy(rot.matrix, &M.Elements[0][0], sizeof(float) * 16);

            std::cout << rot.quaternion.Elements << std::endl;
            // (optional) if other systems still read rot.Euler, recompute them here or stop using them.
            //rot.Euler = HMM_QV4(camRot.quaternion).XYZ; // if you add your own helper
        }
        break;
    }
}

void yawPitchToDir(float yaw, float pitch, Vec3& dir) 
{
    dir.X = cosf(pitch) * cosf(yaw);
    dir.Y = sinf(pitch);
    dir.Z = cosf(pitch) * sinf(yaw);
    dir = HMM_NormV3(dir);
}

void CharacterComponent::cursorPressedEvent(Entity owner, std::shared_ptr<CursorKeyEvent> e)
{

    auto& reg = owner.get_world()->get_registry();

    const auto pos = reg.get<PositionComponent>(owner.get_handle());
    auto rot = reg.get<RotationComponent>(owner.get_handle());

    auto& ds = reg.get_or_emplace<DragStateComponent>(owner.get_handle());
    auto& cds = reg.get_or_emplace<CursorDragStateComponent>(owner.get_handle());

    if ((InputHandler::cursorKeyHeldCode < 0)) InputHandler::cursorKeyHeldCode = int(e->key);

    switch (e->key) {
    case KeyCode::Mouse01:
        for (auto [ee, camera] : reg.view<CameraComponent>().each()) {

            ds.zNDC = objectZndc(HMM_V3((float)pos.x, (float)pos.y, (float)pos.z), camera.VP);

            int y = camera.viewport.Y + camera.viewport.W - 1 - (int)e->posY;
            Vec3 grabWS = unprojectAtNDC(e->posX, y, ds.zNDC, camera.viewport, camera.inverseVP);

            ds.offset = HMM_V3((float)pos.x - grabWS.X,
                (float)pos.y - grabWS.Y,
                (float)pos.z - grabWS.Z);
            ds.active = true;

            break;
        }
        break;
    case KeyCode::Mouse02:

        cds.offset = rot.Euler;
        cds.active = true;
        break;
    default: return;
    
    }
}

void CharacterComponent::cursorReleasedEvent(Entity owner, std::shared_ptr<CursorKeyEvent> e){

    if (!(InputHandler::cursorKeyHeldCode < 0) && InputHandler::cursorKeyHeldCode == int(e->key)) InputHandler::cursorKeyHeldCode = -1;
    auto& reg = owner.get_world()->get_registry();
    switch (e->key) {
	case KeyCode::Mouse01: 

        if (auto ds = reg.try_get<DragStateComponent>(owner.get_handle())) ds->active = false; reg.erase<DragStateComponent>(owner.get_handle());;
        break;
    case KeyCode::Mouse02: {
            if (auto ds = reg.try_get<CursorDragStateComponent>(owner.get_handle()); ds && ds->active) {
                auto& rot = reg.get<RotationComponent>(owner.get_handle());
                //createRotationMatrixXYZ(rot.Euler, rot.matrix);
                ds->active = false;
				reg.erase<CursorDragStateComponent>(owner.get_handle());
            }
        }
        break;
    }
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
            rotComp.Euler.Z -= 1.0f * dt;
            break;
        case KeyCode::Right:
            rotComp.Euler.Z += 1.0f * dt;
            break;
        case KeyCode::Up:
            rotComp.Euler.Y += 1.0f * dt;
            break;
        case KeyCode::Down:
            rotComp.Euler.Y -= 1.0f * dt;
            break;
        }

        createRotationMatrixXYZ(rotComp.Euler, rotComp.matrix);
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
