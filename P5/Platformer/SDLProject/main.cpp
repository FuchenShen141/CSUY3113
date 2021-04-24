#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>


#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


GLuint fontTextureID;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music *music_playing;
Mix_Chunk *bounce;

enum GameMode {GAME_LEVEL, GAME_OVER};
GameMode mode = GAME_LEVEL;


Scene *currentScene;
Scene *sceneList[3];

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}


int playerLives=3;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music_playing = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music_playing, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTextureID =Util::LoadTexture("font1.png");
    
    
    
    sceneList[0] = new Level1();
    sceneList[1] = new Level2();
    sceneList[2] = new Level3();
    SwitchToScene(sceneList[0]);
    
}

void ProcessInputGameLevel() {
    if(playerLives<=0 || currentScene->state.player->success==true){
        currentScene->state.nextScene=-1;
        mode=GAME_OVER;
        currentScene->state.player->fail=false;
    }
    if(currentScene->state.player->fail==true){
        currentScene->state.player->fail=false;
        playerLives-=1;
        SwitchToScene(sceneList[0]);
        
    }
    currentScene->state.player->movement = glm::vec3(0);
    currentScene->state.player->acceleration=glm::vec3(0,-9.81f,0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        
                        if(currentScene->state.player->collidedBottom){
                            currentScene->state.player->jump=true;
                            Mix_PlayChannel(-1, bounce, 0);
                        }
                        
                        break;
                         
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_A]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_D]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    /*
    else if (keys[SDL_SCANCODE_W]) {
        state.player->movement.y = 1.0f;
        state.player->animIndices = state.player->animUp;
    }
    else if (keys[SDL_SCANCODE_S]) {
        state.player->movement.y =-1.0f;
        state.player->animIndices = state.player->animDown;
    }
     */
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    
}

void ProcessInputGameOver(){
    
    currentScene->state.player->movement = glm::vec3(0);
    currentScene->state.player->acceleration=glm::vec3(0,0,0);
    currentScene->state.player->velocity=glm::vec3(0);
    currentScene->state.nextScene=-1;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_RETURN:
                        mode=GAME_LEVEL;
                        currentScene->state.player->fail=false;
                        currentScene->state.player->success=false;
                        playerLives=3;
                        SwitchToScene(0);
                        
                        for(int i=0;i<3;i++){
                            currentScene->state.enemies[i].isActive=true;
                        }
                        currentScene->state.player->enemyKilled=0;
                        
                        break;
                        
                }
                break; // SDL_KEYDOWN
        }
    }
}
    


void ProcessInput(){
    switch(mode){
        case GAME_LEVEL:
            ProcessInputGameLevel();
            break;
        case GAME_OVER:
            ProcessInputGameOver();
            break;
    }
}



#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
        
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
    if (currentScene->state.player->position.y < -5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x+6, -currentScene->state.player->position.y-3.75, 0));}
    
    if(currentScene->state.player->enemyKilled==3){
        currentScene->state.player->success=true;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
   
    if(playerLives<=0){
        Util::DrawText(&program, fontTextureID, "You Lose!", 1, -0.5f, glm::vec3(currentScene->state.player->position.x+1,-2,0));
    }
    else if (currentScene->state.player->success){
        Util::DrawText(&program, fontTextureID, "You Win!", 1, -0.5f, glm::vec3(8,-2,0));
    }
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

void Menu(){
    bool start=false;
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    Util::DrawText(&program, fontTextureID, "Platformer Game", 1, -0.5f,glm::vec3(-3, 1,0));
    Util::DrawText(&program, fontTextureID, "Press Enter To Start", 1, -0.5f,glm::vec3(-5,-1.5,0));
    SDL_GL_SwapWindow(displayWindow);
    while(start!=true){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    gameIsRunning = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_LEFT:
                            // Move the player left
                            break;
                            
                        case SDLK_RIGHT:
                            // Move the player right
                            break;
                            
                        case SDLK_RETURN:
                            start=true;
                            
                            break;
                            
                    }
                    break; // SDL_KEYDOWN
            }
    }
    }
}

int main(int argc, char* argv[]) {
    Initialize();
    Menu();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        
        if(currentScene->state.nextScene >=0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        Render();
    }
    
    Shutdown();
    return 0;
}
