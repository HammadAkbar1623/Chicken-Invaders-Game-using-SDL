#include <iostream>
#include <SDL.h>
#include <ctime>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

using namespace std;
#undef main

class Game
{
public:
    Game();
    ~Game();
    bool Init();
    void Run();

private:
    struct Egg
    {
        int x;
        int y;
        bool active;
    };

    enum class GameState
    {
        MENU,
        PLAYING,
        INFO
    };

    SDL_Window *CreateWindow();
    SDL_Texture *LoadTexture(const char *path);
    SDL_Texture *LoadTextTexture(const string &text, SDL_Color textColor);
    bool LoadFont();
    void HandleEvents();
    void Update();
    void Render();
    void Menu();
    void BackToMenu();
    void Info();
    void Cleanup();
    bool CheckCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
    void DropEgg();

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    SDL_Texture *chicken;
    SDL_Texture *spaceship;
    SDL_Texture *bullet;
    SDL_Texture *StartButton;
    SDL_Texture *InfoButton;
    SDL_Texture *ExitButton;
    SDL_Texture *egg;
    SDL_Texture *Heart;
    SDL_Texture *textTexture;
    SDL_Texture *Title;
    TTF_Font *gameFont;

    bool gameIsRunning;
    bool gameIsPaused;
    bool isFiring;
    bool specialMovement;
    int spaceshipX;
    int spaceshipY;
    int bulletX;
    int bulletY;
    int chickenXPos;
    int chickenDirection;
    int chickenWidth;
    int chickenHeight;
    int numChickens;
    int spacing;
    int totalWidth;
    int score;
    int Health;

    int chickenCounter;
    const int BulletWidth = 24;
    const int BulletHeight = 10;
    const int EggWidth = 16;
    const int EggHeight = 24;
    bool *chickenAlive;
    vector<Egg> eggs;

    SDL_Color textColor = {255, 255, 255, 255}; // White color

    int eggDropInterval;
    int timeSinceLastEggDrop;

    GameState gameState;
};

Game::Game()
    : window(nullptr), renderer(nullptr), background(nullptr), chicken(nullptr),
      spaceship(nullptr), bullet(nullptr), egg(nullptr), textTexture(nullptr), gameFont(nullptr), Title(nullptr),
      gameIsRunning(true), gameIsPaused(false), isFiring(false), specialMovement(false), gameState(GameState::MENU),
      spaceshipX(0), spaceshipY(0), bulletX(0), bulletY(0), chickenXPos(0), chickenDirection(1),
      chickenWidth(35), chickenHeight(35), numChickens(12), spacing(10), score(0), Health(3), chickenCounter(12),
      eggDropInterval(2000), timeSinceLastEggDrop(0)
{
    chickenAlive = new bool[numChickens];
    for (int i = 0; i < numChickens; ++i)
    {
        chickenAlive[i] = true;
    }
    srand(time(nullptr)); // Seed the random number generator
}

Game::~Game()
{
    Cleanup();
    delete[] chickenAlive;
}

bool Game::Init()
{
    window = CreateWindow();
    if (!window)
        return false;

    background = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/backgroundd.bmp");
    if (!background)
        return false;

    chicken = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/Chicken.png");
    if (!chicken)
        return false;

    spaceship = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/sss.png");
    if (!spaceship)
        return false;

    Heart = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/Health.png");

    egg = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/egg.png");
    if (!egg)
    {
        cout << "Failed to load egg texture";
        return false;
    }

    bullet = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/bullet 2.png");
    if (!bullet)
        return false;

    StartButton = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/StartButton1.png");
    if (!StartButton)
        return false;

    InfoButton = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/InfoButton1.png");
    if (!InfoButton)
        return false;

    ExitButton = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/ExitButton1.png");
    if (!ExitButton)
        return false;

    Title = LoadTexture("D:/C++ Projects/17th- Chicken Invaders Game/images/MenuText.png");
    if (!Title)
        return false;

    if (TTF_Init() == -1)
    {
        cout << "TTF could not initialize! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }

    if (!LoadFont())
        return false;

    spaceshipX = (626 - 64) / 2; // Center the spaceship horizontally
    spaceshipY = 417 - 64;       // Place the spaceship at the bottom of the window

    totalWidth = numChickens * (chickenWidth + spacing) - spacing;
    chickenXPos = (626 - totalWidth) / 2;

    return true;
}

SDL_Window *Game::CreateWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return nullptr;
    }

    SDL_Window *window = SDL_CreateWindow("Chicken Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 626, 417, SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return nullptr;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return nullptr;
    }

    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags)
    {
        cout << "SDL_Image format is not found. SDL_Error: " << IMG_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return nullptr;
    }

    return window;
}

SDL_Texture *Game::LoadTexture(const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        cout << "Unable to load image! SDL_Error: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture)
    {
        cout << "Unable to create texture from surface! SDL_Error: " << SDL_GetError() << endl;
        return nullptr;
    }
    return texture;
}

SDL_Texture *Game::LoadTextTexture(const string &text, SDL_Color textColor)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(gameFont, text.c_str(), textColor);
    if (!textSurface)
    {
        cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!texture)
    {
        cout << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << endl;
    }
    return texture;
}

bool Game::LoadFont()
{
    gameFont = TTF_OpenFont("D:/C++ Projects/17th- Chicken Invaders Game/Fonts/font.ttf", 25); // Specify your font path and size
    if (gameFont == nullptr)
    {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }
    return true;
}

void Game::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            gameIsRunning = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (gameState == GameState::MENU)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    // Check if Start button is clicked
                    if (x >= 250 && x <= 250 + 150 && y >= 150 && y <= 150 + 50)
                    {
                        gameState = GameState::PLAYING;
                    }
                    else if (x >= 250 && x <= 250 + 150 && y >= 220 && y <= 220 + 50)
                    {
                        // Info button clicked
                        gameState = GameState::INFO;
                    }

                    else if (x >= 250 && x <= 250 + 150 && y >= 290 && y <= 290 + 50)
                    {
                        // Exit button clicked
                        gameIsRunning = false;
                    }
                }
                else
                {
                    if (!isFiring)
                    {
                        isFiring = true;
                        bulletX = spaceshipX + 32 - 12;
                        bulletY = spaceshipY;
                    }
                }
            }
            break;
        case SDL_MOUSEMOTION:
            if (gameState == GameState::PLAYING)
            {
                spaceshipX = event.motion.x - 32;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                gameIsRunning = false;
            }
            else if (event.key.keysym.sym == SDLK_p)
            {
                gameIsPaused = !gameIsPaused;
            }
            break;
        default:
            break;
        }
    }
}

void Game::DropEgg()
{
    int currentTime = SDL_GetTicks();
    if (currentTime - timeSinceLastEggDrop >= eggDropInterval)
    {
        timeSinceLastEggDrop = currentTime;

        // Find a random active chicken
        vector<int> activeChickens;
        for (int i = 0; i < numChickens; ++i)
        {
            if (chickenAlive[i])
            {
                activeChickens.push_back(i);
            }
        }

        if (!activeChickens.empty())
        {
            int randomIndex = rand() % activeChickens.size();
            int chickenIndex = activeChickens[randomIndex];
            int eggX = chickenXPos + chickenIndex * (chickenWidth + spacing) + (chickenWidth - EggWidth) / 2;
            int eggY = chickenHeight;

            eggs.push_back({eggX, eggY, true});
        }

        // Set a new random interval for the next egg drop
        eggDropInterval = 1000 + rand() % 2000;
    }
}

void Game::Update()
{
    if (!gameIsPaused && gameState == GameState::PLAYING)
    {
        // Move bullet if firing
        if (isFiring)
        {
            bulletY -= 10;
            if (bulletY < 0)
            {
                isFiring = false;
            }
            else
            {
                for (int i = 0; i < numChickens; ++i)
                {
                    if (chickenAlive[i])
                    {
                        int chickenX = chickenXPos + i * (chickenWidth + spacing);
                        int chickenY = 0;

                        if (CheckCollision(bulletX, bulletY, BulletWidth, BulletHeight, chickenX, chickenY, chickenWidth, chickenHeight))
                        {
                            if (bulletY <= chickenHeight)
                            {
                                chickenAlive[i] = false;
                                score++; // Increment the score when a chicken is hit
                                chickenCounter--;
                                isFiring = false;
                                cout << "Hit! Chicken " << i << " is down. Score: " << score << endl;
                                specialMovement = true; // Enable special movement when any chicken is hit
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Update chicken positions
        chickenXPos += chickenDirection;
        if (chickenXPos <= 0 || chickenXPos >= 626 - totalWidth)
        {
            chickenDirection = -chickenDirection;
            chickenXPos += chickenDirection;
        }

        // Update egg dropping
        timeSinceLastEggDrop += 16; // Assuming a 60 FPS frame rate
        if (timeSinceLastEggDrop > eggDropInterval)
        {
            DropEgg();
            timeSinceLastEggDrop = 0;
        }

        // Update eggs
        for (auto &egg : eggs)
        {
            if (egg.active)
            {
                egg.y += 5; // Move eggs downwards
                if (egg.y > 417)
                {
                    egg.active = false; // Deactivate eggs that reach bottom of the window
                }
                else
                {
                    // Check collision with spaceship
                    if (CheckCollision(egg.x, egg.y, EggWidth, EggHeight, spaceshipX, spaceshipY, 64, 64))
                    {
                        egg.active = false; // Deactivate egg if it hits the spaceship
                        Health--;           // Decrement health on collision
                    }
                }
            }
        }

        // Check game over condition
        if (Health <= 0)
        {
            gameState = GameState::MENU;
            score = 0;
            Health = 3;
            // Reset chickens
            for (int i = 0; i < numChickens; ++i)
            {
                chickenAlive[i] = true;
            }
        }
    }
}

void Game::Render()
{
    SDL_RenderClear(renderer);

    int textWidth, textHeight;

    // Render background
    SDL_RenderCopy(renderer, background, nullptr, nullptr);

    if (gameState == GameState::MENU)
    {
        Menu();
    }
    else
    {
        // Render chicken invaders
        for (int i = 0; i < numChickens; ++i)
        {
            if (chickenAlive[i])
            {
                SDL_Rect chickenRect = {chickenXPos + i * (chickenWidth + spacing), 0, chickenWidth, chickenHeight};
                SDL_RenderCopy(renderer, chicken, NULL, &chickenRect);
            }
        }

        // Render spaceship
        SDL_Rect spaceshipRect = {spaceshipX, spaceshipY, 64, 64};
        SDL_RenderCopy(renderer, spaceship, nullptr, &spaceshipRect);

        // Render bullets
        if (isFiring)
        {
            SDL_Rect bulletRect = {bulletX, bulletY, BulletWidth, BulletHeight};
            SDL_RenderCopy(renderer, bullet, nullptr, &bulletRect);
        }

        // Render eggs
        for (const auto &egg : eggs)
        {
            if (egg.active)
            {
                SDL_Rect eggRect = {egg.x, egg.y, EggWidth, EggHeight};
                SDL_RenderCopy(renderer, this->egg, nullptr, &eggRect);
            }
        }

        // Render score
        textTexture = LoadTextTexture("Score: " + to_string(score), textColor);
        if (textTexture)
        {
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
            SDL_Rect textRect = {10, 50, textWidth, textHeight};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }

        // Render You Won text after killing all the chickens
        if (chickenCounter == 0)
        {
            textTexture = LoadTextTexture("You Won....", textColor);
            if (textTexture)
            {
                SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
                SDL_Rect textRect = {200, 170, 200, 45};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }

        // Render health
        for (int i = 0; i < Health; ++i)
        {
            SDL_Rect heartRect = {550 + i * (20 + 6), 60, 20, 20};
            SDL_RenderCopy(renderer, Heart, NULL, &heartRect);
        }
    }
    // Render the info screen
    if (gameState == GameState::INFO)
    {
        Info();
    }
    SDL_RenderPresent(renderer);
}

void Game::Menu()
{
    // Render menu buttons
    SDL_Rect startButtonRect = {200, 3, 200, 400};
    SDL_RenderCopy(renderer, StartButton, nullptr, &startButtonRect);

    // Render Info Button
    SDL_Rect infoButtonRect = {200, 60, 200, 400};
    SDL_RenderCopy(renderer, InfoButton, nullptr, &infoButtonRect);

    // Render Exit Button
    SDL_Rect exitButtonRect = {200, 120, 200, 400};
    SDL_RenderCopy(renderer, ExitButton, nullptr, &exitButtonRect);

    // Render Title
    SDL_Rect TitleRect = {170, 10, 270, 120};
    SDL_RenderCopy(renderer, Title, nullptr, &TitleRect);
}

void Game::Info()
{
    if (gameState == GameState::INFO)
    {
        // Load and render the background
        SDL_RenderCopy(renderer, background, nullptr, nullptr);

        // Load and render the info image
        SDL_Surface *infoSurface = SDL_LoadBMP("D:/C++ Projects/17th- Chicken Invaders Game/images/InfoImage.bmp");
        if (!infoSurface)
        {
            cout << "Failed to load info image! SDL_Error: " << SDL_GetError() << endl;
            return;
        }

        SDL_Texture *infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
        SDL_FreeSurface(infoSurface);

        if (!infoTexture)
        {
            cout << "Failed to create texture from info surface! SDL_Error: " << SDL_GetError() << endl;
            return;
        }

        // Get dimensions of the info image
        int infoImageWidth, infoImageHeight;
        SDL_QueryTexture(infoTexture, nullptr, nullptr, &infoImageWidth, &infoImageHeight);

        // Center the info image on the screen
        SDL_Rect infoRect = {0, 0, 626, 417};
        SDL_RenderCopy(renderer, infoTexture, nullptr, &infoRect);

        SDL_DestroyTexture(infoTexture);
    }
}

void Game ::BackToMenu()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.key.keysym.sym == SDLK_b)
        {
            gameState = GameState::MENU;
        }
    }
}

bool Game::CheckCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

void Game::Run()
{
    if (!Init())
    {
        cout << "Failed to initialize!" << endl;
        return;
    }

    SDL_Event event;
    int frameStart;
    int frameTime;

    while (gameIsRunning)
    {
        frameStart = SDL_GetTicks();

        HandleEvents();
        Update();
        Render();
        Info();
        BackToMenu();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16)
        {
            SDL_Delay(16 - frameTime);
        }
    }
}

void Game::Cleanup()
{
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(chicken);
    SDL_DestroyTexture(spaceship);
    SDL_DestroyTexture(bullet);
    SDL_DestroyTexture(StartButton);
    SDL_DestroyTexture(InfoButton);
    SDL_DestroyTexture(ExitButton);
    SDL_DestroyTexture(egg);
    SDL_DestroyTexture(Heart);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(gameFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main()
{
    Game game;
    game.Run();
    return 0;
}

