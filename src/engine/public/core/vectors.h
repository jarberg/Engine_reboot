#pragma once

#include <cmath>
#include <array>

struct Vec3 {
    float x, y, z;

};
inline Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x + b.x,a.y + b.y,a.z + b.z }; }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x - b.x,a.y - b.y,a.z - b.z }; }
inline Vec3 operator*(const Vec3& a, float s) { return { a.x * s,a.y * s,a.z * s }; }

inline Vec3 normalize(const Vec3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / len, v.y / len, v.z / len };
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline std::array<float, 16> lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 f = normalize(target - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    std::array<float, 16> m;
    m[0] = s.x; m[4] = s.y; m[8] = s.z;  m[12] = -dot(s, eye);
    m[1] = u.x; m[5] = u.y; m[9] = u.z;  m[13] = -dot(u, eye);
    m[2] = -f.x; m[6] = -f.y; m[10] = -f.z; m[14] = dot(f, eye);
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
    return m;
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
        Vec3 f = normalize(target - eye());
        Vec3 r = normalize(cross(f, up));
        Vec3 u = cross(r, f);
        float s = radius / pixelsPerUnit;
        target = target + (r * (-dx_pixels * s)) + (u * (dy_pixels * s));
    }
};