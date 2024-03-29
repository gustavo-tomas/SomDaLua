#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
#include "Vec2.h"

class Camera {
    public:
        static void Follow(GameObject* newFocus);
        static void Unfollow();
        static void Update(float dt);
        static void Reset();
        static Vec2 pos;
        static Vec2 speed;
        static bool movement;

    private:
        static GameObject* focus;
};

#endif // CAMERA_H
