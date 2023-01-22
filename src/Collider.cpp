#include "../header/Collider.h"

// Debug 
#include "../header/Camera.h"
#include "../header/Game.h"
#include "../header/InputManager.h"
#include <SDL2/SDL.h>

bool Collider::debug = true;

Collider::Collider(GameObject& associated, Vec2 scale, Vec2 offset) : Component(associated)
{
    this->scale = scale;
    this->offset = offset;
}

void Collider::Update(float dt)
{
    // Debug is toggled ON/OFF by pressing Tab :)
    if (InputManager::GetInstance().IsKeyDown(TAB_KEY))
        debug = !debug;

    // This section was moved from Update to better reflect changes in the hitbox
    box.SetVec(associated.box.GetCenter() + offset.GetRotated(associated.angleDeg));
    if (scale.x > 0)
    {
        box.w = associated.box.w * scale.x;
        box.x -= (associated.box.w * scale.x) / 2.0;
    }
    
    if (scale.y > 0)
    {
        box.h = associated.box.h * scale.y;
        box.y -= (associated.box.h * scale.y) / 2.0;
    }
    // Section ends here

    box.SetVec(box.GetVec() + velocity * dt);
    associated.box.SetVec(box.GetVec());
}

void Collider::ResolveCollisionUpdate(float dt)
{
    // Apply correction (if any) and update the associated box
    box.SetVec(box.GetVec() + correction);
    associated.box.SetVec(box.GetVec());
    correction = Vec2();
}

void Collider::Render() {

    if (!debug)
        return;

    Vec2 center(box.GetCenter());
    SDL_Point points[5];

    Vec2 point = (Vec2(box.x, box.y) - center).GetRotated(associated.angleDeg) + center - Camera::pos;
    points[0] = { (int) point.x, (int) point.y };
    points[4] = { (int) point.x, (int) point.y };
    
    point = (Vec2(box.x + box.w, box.y) - center).GetRotated(associated.angleDeg) + center - Camera::pos;
    points[1] = { (int) point.x, (int) point.y };
    
    point = (Vec2(box.x + box.w, box.y + box.h) - center).GetRotated(associated.angleDeg) + center - Camera::pos;
    points[2] = { (int) point.x, (int) point.y };
    
    point = (Vec2(box.x, box.y + box.h) - center).GetRotated( associated.angleDeg) + center - Camera::pos;
    points[3] = { (int) point.x, (int) point.y };

    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLines(Game::GetInstance().GetRenderer(), points, 5);
}

bool Collider::Is(const char* type)
{
    string str_type = type;
    return str_type == "Collider";
}

void Collider::ApplyImpulse(Vec2 impulse)
{
    float inv = 1.f / this->mass;
    this->velocity = this->velocity + (impulse * inv);
}

void Collider::SetScale(Vec2 scale)
{
    this->scale = scale;
}

void Collider::SetOffset(Vec2 offset)
{
    this->offset = offset;
}

void Collider::SetVelocity(Vec2 velocity)
{
    this->velocity = velocity;
}

void Collider::SetRestitution(float restitution)
{
    this->restitution = restitution;
}

void Collider::SetMass(float mass)
{
    this->mass = mass;
    this->invMass = 1.f / mass;
}