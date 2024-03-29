#include "../header/Bullet.h"
#include "../header/Sprite.h"
#include "../header/Collider.h"
#include "../header/Sound.h"
#include <cmath>

Bullet::Bullet(GameObject& associated, float angle, float speed,
    int damage, float maxDistance, const char* sprite,
    int frameCount, float frameTime, bool targetsPlayer, string soundFile)
    : Component(associated)
{
    Sprite* bulletSprite = new Sprite(associated, sprite, frameCount, 1, frameTime);
    bulletSprite->SetScale(0.2, 0.2);
    associated.AddComponent(bulletSprite);

    Collider* collider = new Collider(associated, {1, 1}, {0, 0}, false);
    collider->kinematic = true;
    associated.AddComponent(collider);

    this->targetsPlayer = targetsPlayer;
    this->speed = Vec2(speed, speed).GetRotated(angle);
    this->damage = damage;
    this->distanceLeft = maxDistance;

    if (soundFile != "")
    {
        Sound* sound = new Sound(associated, soundFile.c_str());
        associated.AddComponent(sound);
        sound->Play();
    }
}

void Bullet::Update(float dt)
{
    if (distanceLeft <= 0)
    {
        associated.RemoveComponent(associated.GetComponent("Collider"));
        associated.RemoveComponent(associated.GetComponent("Sprite"));
        
        Sound* sound = (Sound *) associated.GetComponent("Sound");
        if (sound != nullptr && sound->IsOpen())
            return;

        associated.RequestDelete();
        return;
    }
    
    Vec2 deltaS = speed * dt;
    distanceLeft -= Vec2(deltaS.x, deltaS.y).GetMagnitude();

    Collider* collider = (Collider*) associated.GetComponent("Collider");
    if (collider != nullptr)
        collider->velocity = speed;

    else
        associated.box.SetVec(associated.box.GetVec() + deltaS);
}

void Bullet::Render()
{

}

bool Bullet::Is(const char* type)
{
    string str_type = type;
    return str_type == "Bullet";
}

void Bullet::NotifyCollision(GameObject& other)
{
    if (other.GetComponent("Bullet") == nullptr)
    {
        associated.RemoveComponent(associated.GetComponent("Collider"));
        associated.RemoveComponent(associated.GetComponent("Sprite"));
    }
}

int Bullet::GetDamage()
{
    return damage;
}
