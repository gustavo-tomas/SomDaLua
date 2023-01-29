#include "../header/WorldState.h"
#include "../header/Camera.h"
#include "../header/CameraFollower.h"
#include "../header/Collider.h"
#include "../header/Collision.h"
#include "../header/Game.h"
#include "../header/GameData.h"
#include "../header/InputManager.h"
#include "../header/Minion.h"
#include "../header/Player.h"
#include "../header/Sprite.h"
#include "../header/Sound.h"
#include "../header/StageState.h"
#include <fstream>
#include <string>

WorldState::WorldState() : State()
{
    
}

WorldState::~WorldState()
{
    objectArray.clear();
    cout << "WorldState deleted successfully!\n";
}

void WorldState::Start()
{
    LoadAssets();
    StartArray();
    started = true; 
}

void WorldState::LoadAssets()
{
    // Background
    GameObject* bgGo = new GameObject();
    Sprite* bg = new Sprite(*bgGo, "./assets/image/background.png");
    bg->SetScale(4.0, 4.0);
    CameraFollower* cf = new CameraFollower(*bgGo);

    bgGo->AddComponent(bg);
    bgGo->AddComponent(cf);
    AddObject(bgGo);

    // Player
    playerGo = new GameObject();
    Player* player = new Player(*playerGo);
    playerGo->box.SetVec(Vec2(1650, 350));
    playerGo->AddComponent(player);
    AddObject(playerGo, 1);

    // Camera
    Camera::Follow(playerGo);

    // World Objects
    vector<WorldObject> objects = {};

    ifstream mappingFile; 
    mappingFile.open("./assets/map/world_objects.txt");

    string buff;
    getline(mappingFile, buff);

    // Format: < name, position, scale, >
    while (!getline(mappingFile, buff).eof())
    {
        size_t pos = 0;
        bool nameFound = false;
        string name;
        vector<float> values;

        while ((pos = buff.find(",")) != string::npos)
        {
            string token = buff.substr(0, pos);
            
            if (!nameFound)
            {
                name = token;
                nameFound = true;
            }

            else
                values.push_back(stof(token));

            buff.erase(0, pos + 1);
        }

        objects.push_back({
            .name = name,
            .position = Vec2(values[0], values[1]),
            .scale = Vec2(values[2], values[3])
        });
    }

    mappingFile.close();

    // @TODO Add a collider field < name, position, scale, colliderScale, colliderOffset > ?
    for (auto object : objects)
    {
        GameObject* objectGo = new GameObject();

        Sprite* objectSprite = new Sprite(*objectGo, (GameData::objectsPath + object.name).c_str());
        objectSprite->SetScale(object.scale.x, object.scale.y);
        objectGo->AddComponent(objectSprite);

        Collider* objectCollider = new Collider(*objectGo, {0.9, 0.9}, {0, 5});
        objectGo->AddComponent(objectCollider);
        
        objectGo->box.SetVec(object.position);
        AddObject(objectGo);
    }
}

void WorldState::Update(float dt)
{
    if (QuitRequested() || PopRequested())
        return;

    // Sets quit requested
    if (InputManager::GetInstance().QuitRequested())
    {
        quitRequested = true;
        return;
    }

    // Returns to title screen
    if (InputManager::GetInstance().KeyPress(ESCAPE_KEY))
    {
        popRequested = true;
        return;
    }

    // Creates new StageState
    if (InputManager::GetInstance().MousePress(LEFT_MOUSE_BUTTON))
        Game::GetInstance().Push(new StageState());

    // Updates the camera
    Camera::Update(dt);

    // Updates GOs
    UpdateArray(dt);

    // Deletes GOs
    for (uint32_t i = 0; i < objectArray.size(); i++)
        for (uint32_t j = 0; j < objectArray[i].size(); j++)
            if (objectArray[i][j]->IsDead()) 
                objectArray[i].erase(objectArray[i].begin() + j);

    // Checks for colisions
    for (uint32_t i = 0; i < colliderArray.size(); i++)
    {
        for (uint32_t j = i + 1; j < colliderArray.size(); j++)
        {
            if(colliderArray[i].expired() || colliderArray[j].expired())
                continue;
            
            auto weakColliderA = colliderArray[i].lock().get();
            auto weakColliderB = colliderArray[j].lock().get();

            Collider* colliderA = (Collider*) weakColliderA->GetComponent("Collider");
            Collider* colliderB = (Collider*) weakColliderB->GetComponent("Collider");

            if (colliderA == nullptr || colliderB == nullptr)
                continue;

            if (!Collision::IsColliding(colliderA->box, colliderB->box, weakColliderA->angleDeg, weakColliderB->angleDeg))
                continue;
            
            weakColliderA->NotifyCollision(*weakColliderB);
            weakColliderB->NotifyCollision(*weakColliderA);
            Collision::ResolveCollision(*colliderA, *colliderB);
            
            // Update collisions before next frame
            colliderA->ResolveCollisionUpdate(dt);
            colliderB->ResolveCollisionUpdate(dt);
        }
    }
}

void WorldState::AddColliderObject(weak_ptr<GameObject>& object)
{
    colliderArray.emplace_back(object);
}

void WorldState::Render()
{
    RenderArray();
}

void WorldState::Pause()
{
    Sound::StopAllSounds();
}

void WorldState::Resume()
{
    Camera::Follow(playerGo);
}
