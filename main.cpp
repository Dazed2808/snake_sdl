#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <time.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const int SNAKE_SIZE = 5;
const int SNAKE_SPEED = 15;

const int APPLE_SIZE = 5;

const int DEFAULT_SNAKE_LENGTH = 5;

struct Position
{
    int x, y;
};

enum Direction
{
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

class Snake
{
public:
    Snake(SDL_Renderer* renderer, Position startPos, int length):
        m_renderer(renderer),
        m_direction(DIRECTION_RIGHT),
        m_snakeLength(length),
        m_isGameOver(false)
    {
        m_snakeSegments.push_back(startPos);
        for (int i = 1; i < length; i++) 
        {
            std::cout << "Pizda" << std::endl;
            m_snakeSegments.push_back({startPos.x - i * SNAKE_SIZE, startPos.y});
        }

        SpawnApple();
    }

    void Move()
    {
        Position newHeadPos = {m_snakeSegments.front().x, m_snakeSegments.front().y};
        switch (m_direction) 
        {
        case DIRECTION_UP:
            newHeadPos.y -= SNAKE_SIZE;
            break;
        case DIRECTION_DOWN:
            newHeadPos.y += SNAKE_SIZE;
            break;
        case DIRECTION_LEFT:
            newHeadPos.x -= SNAKE_SIZE;
            break;
        case DIRECTION_RIGHT:
            newHeadPos.x += SNAKE_SIZE;
            break;
        }

        m_snakeSegments.insert(m_snakeSegments.begin(), newHeadPos);

        if(newHeadPos.x == m_applePos.x && newHeadPos.y == m_applePos.y)
        {
            m_snakeLength++;
            SpawnApple();
        }

        if (newHeadPos.x < 0 || newHeadPos.x >= WINDOW_HEIGHT ||
            newHeadPos.y < 0 || newHeadPos.y >= WINDOW_HEIGHT ||
            CheckCollision()) 
        {
            m_isGameOver = true;
        }

        if(!m_isGameOver && m_snakeSegments.size() > static_cast<size_t>(m_snakeLength))
        {
            m_snakeSegments.pop_back();
        }
    }

    void Draw()
    {
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

        for (Position segment: m_snakeSegments) 
        {
            SDL_Rect rect = {segment.x, segment.y, SNAKE_SIZE, SNAKE_SIZE};
            SDL_RenderFillRect(m_renderer, &rect);
        }

        SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
        SDL_Rect rect = {m_applePos.x, m_applePos.y, APPLE_SIZE, APPLE_SIZE};
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void SetDirection(Direction direction)
    {
        if ((direction == DIRECTION_UP && m_direction != DIRECTION_DOWN) ||
            (direction == DIRECTION_DOWN && m_direction != DIRECTION_UP) ||
            (direction == DIRECTION_LEFT && m_direction != DIRECTION_RIGHT) ||
            (direction == DIRECTION_RIGHT && m_direction != DIRECTION_LEFT)) 
        {
            m_direction = direction;
        }
    }

    bool IsGameOver()
    {
        return m_isGameOver;
    }
private:
    void SpawnApple()
    {
        int maxX = (WINDOW_WIDTH - APPLE_SIZE) / SNAKE_SIZE;
        int maxY = (WINDOW_HEIGHT - APPLE_SIZE) / SNAKE_SIZE;
        m_applePos = {(rand() % maxX) * SNAKE_SIZE, (rand() % maxY) * SNAKE_SIZE};
    }

    bool CheckCollision()
    {
        for (size_t i = 1; i < m_snakeSegments.size(); i++) 
        {
            if (m_snakeSegments[i].x == m_snakeSegments.front().x && m_snakeSegments[i].y == m_snakeSegments.front().y) 
            {
                return true;
            }
        }
        return false;
    }

    SDL_Renderer* m_renderer;
    Direction m_direction;
    std::vector<Position> m_snakeSegments;
    Position m_applePos;
    size_t m_snakeLength;
    bool m_isGameOver;
};

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) 
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SNAKE FUCKING GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == nullptr) 
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Snake game(renderer, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}, DEFAULT_SNAKE_LENGTH);

    bool quit = false;
    SDL_Event event;
    while (!quit) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym) 
                {
                case SDLK_UP:
                    game.SetDirection(DIRECTION_UP);
                    break;
                case SDLK_DOWN:
                    game.SetDirection(DIRECTION_DOWN);
                    break;
                case SDLK_LEFT:
                    game.SetDirection(DIRECTION_LEFT);
                    break;
                case SDLK_RIGHT:
                    game.SetDirection(DIRECTION_RIGHT);
                    break;
                }
            }
        }
        
        game.Move();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        game.Draw();
        SDL_RenderPresent(renderer);
        
        if(game.IsGameOver())
        {
            quit = true;
        }

        SDL_Delay(1000 / SNAKE_SPEED);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}