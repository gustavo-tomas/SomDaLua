#include "../header/WorldState.h"
#include "../header/Camera.h"
#include "../header/CameraFollower.h"
#include "../header/Collider.h"
#include "../header/Collision.h"
#include "../header/Game.h"
#include "../header/GameData.h"
#include "../header/InputManager.h"
#include "../header/Player.h"
#include "../header/Sprite.h"
#include "../header/Sound.h"
#include "../header/StageState.h"
#include "../header/TestBox.h"
#include "../header/NPC.h"
#include "../header/DialogBox.h"
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
    // Music
    backgroundMusic = Music("./assets/audio/musics/background.mp3", 15);
    backgroundMusic.Play();

    // Background
    GameObject* bgGo = new GameObject();
    Sprite* bg = new Sprite(*bgGo, "./assets/image/background.png");
    bg->SetScale(4.0, 4.0);
    CameraFollower* cf = new CameraFollower(*bgGo);

    bgGo->AddComponent(bg);
    bgGo->AddComponent(cf);
    AddObject(bgGo, -GameData::HEIGHT);

    // Player
    playerGo = new GameObject();
    Player* player = new Player(*playerGo);
    playerGo->box.SetVec(Vec2(1650, 350));
    playerGo->AddComponent(player);
    AddObject(playerGo, 10020);

    // NPC 1
    GameObject* npcGo = new GameObject();
    Sprite npcSprite = Sprite(*npcGo, "./assets/image/250_scout.png");
    npcSprite.SetScale(0.5, 0.5);
    NPC* npc = new NPC(*npcGo, "Scout", Vec2(1800, 500), npcSprite);
    npc->AddSpeech("Ola!");
    npc->AddSpeech("Bom Dia!");

    
    npcGo->AddComponent(npc);
    AddObject(npcGo, 10020);

    // NPC 2

    GameObject* npcGo2 = new GameObject();
    NPC* npc2 = new NPC(*npcGo2, "Mage", Vec2(850, 300), Sprite(*npcGo2, "./assets/image/mage-1-85x94.png", 4, 2, 0.2));
    npc2->AddSpeech("Lorem ipsum dolor amet."
                    " Eu esqueci o resto da frase."
                    " Aqui vai uma receita de bolo entao: "
                    " ... eu nao sei fazer bolo :(");
    
    npcGo2->AddComponent(npc2);
    AddObject(npcGo2, 10020);

    // Camera
    Camera::Follow(playerGo);

    // Dialog
    GameObject* dialogGo = new GameObject();
    DialogBox* dialog = new DialogBox(*dialogGo, "Sua missão", 
                                                            "Encontre todas as partituras "
                                                            "Para aprender a tocar a sua flauta. "
                                                            "Com isso venca as batalhas musicais "
                                                            "para se tornar a flautista oficial da banda!!",
                                                            Vec2(GameData::WIDTH - 250, 20));
    dialogGo->AddComponent(dialog);
    AddObject(dialogGo, 20002);

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
            .position = {values[0], values[1]},
            .scale = {values[2], values[3]},
            .colliderScale = {values[4], values[5]},
            .colliderOffset = {values[6], values[7]}
        });
    }

    mappingFile.close();

    for (auto object : objects)
    {
        GameObject* objectGo = new GameObject();

        Sprite* objectSprite = new Sprite(*objectGo, (GameData::objectsPath + object.name).c_str());
        objectSprite->SetScale(object.scale.x, object.scale.y);
        objectGo->AddComponent(objectSprite);

        Collider* objectCollider = new Collider(*objectGo, object.colliderScale, object.colliderOffset);
        objectGo->AddComponent(objectCollider);
        
        objectGo->box.SetVec(object.position);
        AddObject(objectGo, 10000);
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

    // Updates GOs
    UpdateArray(dt);

    // Deletes GOs
    for (uint32_t i = 0; i < objectArray.size(); i++)
        if (objectArray[i]->IsDead())
            objectArray.erase(objectArray.begin() + i--);

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

    // Updates the camera
    Camera::Update(dt);

    // Updates camera followers (o fix mais feio da história)
    for(uint32_t i = 0; i < objectArray.size(); i++)
        if ((CameraFollower *) objectArray[i]->GetComponent("CameraFollower") != nullptr)
            objectArray[i]->GetComponent("CameraFollower") ->Update(dt);
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
