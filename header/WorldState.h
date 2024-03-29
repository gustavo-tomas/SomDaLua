#ifndef WORLDSTATE_H
#define WORLDSTATE_H

#include "GameObject.h"
#include "State.h"

using namespace std;

typedef struct WorldObject {
    string name;
    Vec2 position = {0, 0};
    Vec2 scale = {1, 1};
    Vec2 colliderScale = {1, 1};
    Vec2 colliderOffset = {0, 0};
} WorldObject;

class WorldState : public State {
    public:
        WorldState();
        ~WorldState();
        void LoadAssets();
        void Update(float dt);
        void Render();
        void Start();
        void Pause();
        void Resume();
        weak_ptr<GameObject> backgroundMusic;
        static unsigned int collectedSongs;

    private:
        weak_ptr<GameObject> fpsCounter; // for tests
        weak_ptr<GameObject> player, counterDialog;
};

#endif // WORLDSTATE_H
