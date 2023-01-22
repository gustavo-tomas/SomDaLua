#ifndef MINION_H
#define MINION_H

#include "Component.h"
#include "GameObject.h"
#include "Vec2.h"

class Minion : public Component {
    public:
        Minion(GameObject& associated, Vec2 initialPos = Vec2(0, 0));
        void Update(float dt);
        void Render();
        bool Is(const char* type);
        void NotifyCollision(GameObject& other);
        void Shoot(Vec2 target);
};

#endif // MINION_H