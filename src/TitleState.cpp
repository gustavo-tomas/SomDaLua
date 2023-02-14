#include "../header/TitleState.h"
#include "../header/GameObject.h"
#include "../header/Sprite.h"
#include "../header/InputManager.h"
#include "../header/Game.h"
#include "../header/WorldState.h"
#include "../header/Camera.h"
#include "../header/CameraFollower.h"
#include "../header/Text.h"
#include "../header/GameData.h"
#include "../header/Sound.h"

TitleState::TitleState() : State()
{
    cout << "\nTitle State\n";
}

TitleState::~TitleState()
{
    objectArray.clear();
    cout << "TitleState deleted successfully!\n";
}

void TitleState::Start()
{
    LoadAssets();
    StartArray();
    started = true;
}

void TitleState::LoadAssets()
{
    // Music
    GameObject* bgMusic = new GameObject();
    Sound* music = new Sound(*bgMusic, "./assets/audio/musics/main_theme.mp3", 10);
    
    bgMusic->AddComponent(music);
    music->Play(1);

    backgroundMusic = AddObject(bgMusic);

    // Background
    GameObject* bgGo = new GameObject();
    Sprite* bg = new Sprite(*bgGo, "./assets/image/ui_background.jpg");
    bg->SetScale(GameData::BASE_HEIGHT * 1.0f / bg->GetUnscaledHeight(), GameData::BASE_WIDTH * 1.0f / bg->GetUnscaledWidth()); // Original resolution: 1920x1080
    bg->isProportionActive = true;
    CameraFollower* cf = new CameraFollower(*bgGo);

    bgGo->AddComponent(bg);
    bgGo->AddComponent(cf);
    AddObject(bgGo, -GameData::HEIGHT);

    // Title
    GameObject* titleGo = new GameObject();
    
    title = new Sprite(*titleGo, "./assets/image/title.png");
    title->SetScale(0.5, 0.5);

    Vec2 offset = Vec2(GameData::WIDTH / 2.0 - title->GetWidth() / 2.0, GameData::HEIGHT / 2.5 - title->GetHeight() / 2.0);
    titleCf = new CameraFollower(*titleGo, offset);
    

    titleGo->AddComponent(title);
    titleGo->AddComponent(titleCf);
    AddObject(titleGo);

    // Options
    GameObject* textGo = new GameObject();

    CameraFollower* textFollower = new CameraFollower(*textGo,Vec2(250, 400));
    textGo->AddComponent(textFollower);
    
    vector<string> options = {"NOVO JOGO", "SAIR DO JOGO"};

    for (unsigned i = 0; i < options.size(); i++)
    {
        GameObject* textGo = new GameObject();
        textGo->box.SetVec(Vec2(450, 350 + 35 * i));

        CameraFollower* textFollower = new CameraFollower(*textGo, textGo->box.GetVec());
        textGo->AddComponent(textFollower);

        const char* fontFile = "./assets/font/Inder-Regular.ttf";
        const char* textStr = options[i].c_str();
        int fontSize = 18;
        Text::TextStyle style = Text::BLENDED;
        SDL_Color color = {255, 255, 255, 255};
        
        Text* text = new Text(*textGo, fontFile, fontSize, style, textStr, color);
        textGo->AddComponent(text);
        
        AddObject(textGo);
    }

    // Cursor
    GameObject* cursor = new GameObject();
    cursor->box.SetVec(Vec2(435, 357));

    Sprite* cursorSprite = new Sprite(*cursor, "./assets/image/icons/diamond.png");
    cursorSprite->SetScale(0.06, 0.06);
    cursor->AddComponent(cursorSprite);

    this->cursor = AddObject(cursor, 20020);
}

void TitleState::Update(float dt)
{

    if(currResolution.x != GameData::WIDTH && currResolution.y != GameData::HEIGHT){
        currResolution = Vec2(GameData::WIDTH, GameData::HEIGHT);
        Vec2 offset = Vec2(GameData::WIDTH / 2.0 - title->GetWidth() / 2.0, GameData::HEIGHT / 2.5 - title->GetHeight() / 2.0);
        titleCf->setOffset(offset);
    }


    // Sets quit requested
    if (InputManager::GetInstance().KeyPress(ESCAPE_KEY) ||
        InputManager::GetInstance().QuitRequested())
    {
        quitRequested = true;
        return;
    }

    // Cursor displacement
    if (InputManager::GetInstance().KeyPress(DOWN_ARROW_KEY))
        cursor.lock().get()->box.y += 35;

    if (InputManager::GetInstance().KeyPress(UP_ARROW_KEY))
        cursor.lock().get()->box.y -= 35;

    cursor.lock().get()->box.y = max(357.f, cursor.lock().get()->box.y);
    cursor.lock().get()->box.y = min(392.f, cursor.lock().get()->box.y);

    // Creates new WorldState
    if (InputManager::GetInstance().KeyPress(ENTER_KEY) && cursor.lock().get()->box.y <= 357)
        Game::GetInstance().Push(new WorldState());
        
    // Quits
    if (InputManager::GetInstance().KeyPress(ENTER_KEY) && cursor.lock().get()->box.y >= 392)
        quitRequested = true;

    UpdateArray(dt);
}

void TitleState::Render()
{
    RenderArray();
}

void TitleState::Pause()
{
    ((Sound *) backgroundMusic.lock().get()->GetComponent("Sound"))->Stop(2500);
}

void TitleState::Resume()
{
    Camera::Reset();
}
