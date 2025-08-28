#pragma once

#include <cmath>
#include <array>

#include "HandmadeMath.h"

using Vec2 = HMM_Vec2;
using Vec3 = HMM_Vec3;
using Vec4 = HMM_Vec4;
using Mat4 = HMM_Mat4;
using Quat = HMM_Quat;




inline std::array<float, 16> lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {

    Vec3 f = HMM_NormV3(target - eye);
    Vec3 s = HMM_NormV3(HMM_Cross(f, up));
    Vec3 u = HMM_Cross(s, f);

    std::array<float, 16> m;
    m[0] = s.X; m[4] = s.Y; m[8] = s.Z;  m[12] = -HMM_DotV3(s, eye);
    m[1] = u.X; m[5] = u.Y; m[9] = u.Z;  m[13] = -HMM_DotV3(u, eye);
    m[2] = -f.X; m[6] = -f.Y; m[10] = -f.Z; m[14] = HMM_DotV3(f, eye);
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
    return m;
}


// Load a column-major float[16] (OpenGL-style) into an HMM_Mat4
inline Mat4 M4FromColumnMajor(const float* m) {
    Mat4 M{};
    M.Columns[0] = HMM_V4(m[0], m[1], m[2], m[3]);
    M.Columns[1] = HMM_V4(m[4], m[5], m[6], m[7]);
    M.Columns[2] = HMM_V4(m[8], m[9], m[10], m[11]);
    M.Columns[3] = HMM_V4(m[12], m[13], m[14], m[15]);
    return M;
}

// Pixel -> NDC (OpenGL: origin bottom-left). If your input is top-left, flip Y first (see below).
inline void pixelToNDC(double mx, double my, const Vec4& vp, float& xN, float& yN) {
    xN = float((mx - vp.X) / double(vp.Z)) * 2.f - 1.f;
    yN = float((my - vp.Y) / double(vp.W)) * 2.f - 1.f;
}

// World -> NDC.z in [-1,1]
inline float objectZndc(const Vec3& objWS, const Mat4& VP) {
    Vec4 clip = HMM_MulM4V4(VP, HMM_V4(objWS.X, objWS.Y, objWS.Z, 1.f));
    return float(clip.Z / clip.W);
}

// Unproject at specific NDC z
inline Vec3 unprojectAtNDC(double mx, double my, float zNDC,
    const Vec4& vp, const Mat4& invVP) {
    float xN, yN; pixelToNDC(mx, my, vp, xN, yN);
    Vec4 ndc = HMM_V4(xN, yN, zNDC, 1.f);
    Vec4 world = HMM_MulM4V4(invVP, ndc);
    float iw = 1.f / world.W;
    return HMM_V3(world.X * iw, world.Y * iw, world.Z * iw);
}

// Build VP and its inverse with HMM
inline Mat4 makeVP(const float* projColMajor16, const float* viewColMajor16) {
    Mat4 P = M4FromColumnMajor(projColMajor16);
    Mat4 V = M4FromColumnMajor(viewColMajor16);
    return HMM_MulM4(P, V); // column-major, column vectors
}
inline bool makeInvVP(const float* projColMajor16, const float* viewColMajor16, Mat4& invVP) {
    Mat4 VP = makeVP(projColMajor16, viewColMajor16);
    // HandmadeMath gives a general 4x4 inverse:
    invVP = HMM_InvGeneralM4(VP);
    // If you want a safety check, multiply VP*invVP and inspect; otherwise just return true.
    return true;
}

struct CameraOrbit {
    Vec3  target{ 0.0f, 0.0f, 0.0f };
    Vec3  up{ 0.0f, 1.0f, 0.0f };

    float radius = 5.0f;
    float yaw = 0.0f;
    float pitch = 0.3f;
    float minRadius = 0.5f, maxRadius = 50.0f;

    CameraOrbit(Vec3 up, Vec3 target) : up(up), target(target) {};

    Vec3 eye() const {
        float cp = std::cos(pitch), sp = std::sin(pitch);
        float cy = std::cos(yaw), sy = std::sin(yaw);
        Vec3 offset{ radius * cp * cy, radius * sp, radius * cp * sy };
        return target + offset;
    }

    std::array<float, 16> view(Vec3& up) const {
        return lookAt(eye(), target, up);
    }

    void drag(float dx_pixels, float dy_pixels, float sensitivity = 0.005f) {
        yaw -= dx_pixels * sensitivity;
        pitch -= dy_pixels * sensitivity;

        const float eps = 0.001f;
        const float halfPi = 1.57079632679f;
        if (pitch > halfPi - eps) pitch = halfPi - eps;
        if (pitch < -halfPi + eps) pitch = -halfPi + eps;

        const float twoPi = 6.28318530718f;
        if (yaw > twoPi) yaw -= twoPi;
        if (yaw < 0.0f) yaw += twoPi;
    }

    void dolly(float wheelDelta, float speed = 0.1f) {
        float r = radius * (1.0f - wheelDelta * speed);
        if (r < minRadius) r = minRadius;
        if (r > maxRadius) r = maxRadius;
        radius = r;
    }

    void pan(float dx_pixels, float dy_pixels, float pixelsPerUnit = 200.0f) {
        //Vec3 f = HMM_NormV3(target - eye);
        //Vec3 s = HMM_NormV3(HMM_Cross(f, up)); 
        //Vec3 u = HMM_Cross(r, f)(r, f);
        //float s = radius / pixelsPerUnit;
        //target = target + (r * (-dx_pixels * s)) + (u * (dy_pixels * s));
    }
};
