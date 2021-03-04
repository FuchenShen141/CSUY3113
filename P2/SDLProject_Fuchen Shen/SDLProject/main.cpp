// project 1 by Fuchen Shen fs1470
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Renderer* renderer;
SDL_Window* displayWindow;
bool gameIsRunning = true;

SDL_Rect l_paddle, r_paddle, ball;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;

// Start at 0, 0, 0

// Don’t go anywhere (yet).
float player1_movement = 0;
float player2_movement = 0;
float ball_movement_x = 0;
float ball_movement_y = 0;

float player1_speed = 128;
float player2_speed = 128;
float ball_speed = 128;

bool paddle_enable=true;




GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(displayWindow, -1,0);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    //create paddles and ball
    l_paddle.h = r_paddle.h= 480/4;
    l_paddle.w = r_paddle.w=8;
    l_paddle.x=0;
    l_paddle.y=r_paddle.y=480/2 - 480/4/2;
    r_paddle.x=640-16;
    ball.h=ball.w=16;
    ball.x= 640/2 - 16;
    ball.y= 480/2 - 16;
    
    
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(0.5f, 0.5f, 1.0f, 0.75f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

}

void ProcessInput() {
    player1_movement=0;
    player2_movement=0;
    SDL_Event event;
    
    if(ball.x<= 0 || ball.x>= (640-16)){
        ball_movement_x=0;
        ball_movement_y=0;
        paddle_enable=false;
    }
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        // Move the player left
                        break;
                        
                    case SDLK_DOWN:
                        // Move the player right
                        break;
                    case SDLK_SPACE:
                        ball_movement_x=1.0;
                        ball_movement_y=1.0;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W] && l_paddle.y>=0 && paddle_enable) {
        player1_movement=2.0;
    }
    else if (keys[SDL_SCANCODE_S] && l_paddle.y<=480-480/4 && paddle_enable) {
        player1_movement=-2.0;
    }
    
    if (keys[SDL_SCANCODE_UP] && r_paddle.y>=0 && paddle_enable) {
        player2_movement=2.0;
    }
    else if (keys[SDL_SCANCODE_DOWN] && r_paddle.y<=480-480/4 && paddle_enable) {
        player2_movement=-2.0;
    }
    
}

float lastTicks=0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    // Add (direction * units per second * elapsed time)
    l_paddle.y -= player1_movement * player1_speed * deltaTime;
    r_paddle.y -= player2_movement * player2_speed * deltaTime;
    ball.x -= ball_movement_x * ball_speed * deltaTime;
    ball.y -= ball_movement_y * ball_speed * deltaTime;
    if(ball.y<=0 || ball.y >= 480-16){
        ball_movement_y=-ball_movement_y;
    }
    if(SDL_HasIntersection(&ball, &l_paddle)){
        ball_movement_x=-ball_movement_x;
        ball_movement_y=-ball_movement_y;
    }
    if(SDL_HasIntersection(&ball, &r_paddle)){
        ball_movement_x=-ball_movement_x;
        ball_movement_y=-ball_movement_y;
    }
    
    
    
    
}

void Render() {
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &r_paddle);
    SDL_RenderFillRect(renderer, &ball);
    SDL_RenderPresent(renderer);
    SDL_GL_SwapWindow(displayWindow);

}

void Shutdown() {
    SDL_DestroyWindow(displayWindow);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
