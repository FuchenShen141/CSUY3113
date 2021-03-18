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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.hpp"

#define PLATFORM_COUNT 2
#define WALL_COUNT 26

GLuint fontTextureID;

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *walls;
};
enum GameMode {GAME_LEVEL, GAME_OVER};
GameMode mode = GAME_LEVEL;

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
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

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), { u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f); modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID); glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTextureID = LoadTexture("font1.png");
    
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0,3.5f,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-0.2f,0);
    
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("playerShip2_red.png");
    /*
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    */
    state.player->height=1.0f;
    state.player->width = 1.0f;
    
    state.player->jumpPower=5.0f;
    
    state.platforms =new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID= LoadTexture("platformPack_tile036.png");
    state.platforms[0].textureID=platformTextureID;
    state.platforms[0].position= glm::vec3(-3,-3.5f,0);
    
    state.platforms[1].textureID=platformTextureID;
    state.platforms[1].position= glm::vec3(-2.0,-3.5f,0);
    
    for(int i=0;i<PLATFORM_COUNT;i++){
        state.platforms[i].Update(0,NULL,NULL,0,0);
    }
    
    state.walls =new Entity[WALL_COUNT];
    
    GLuint wallTextureID= LoadTexture("platformPack_tile030.png");
    
    state.walls[0].textureID=wallTextureID;
    state.walls[0].position= glm::vec3(-5.0,-3.5,0);
    
    state.walls[1].textureID=wallTextureID;
    state.walls[1].position= glm::vec3(-5.0,-2.5f,0);
    
    state.walls[2].textureID=wallTextureID;
    state.walls[2].position= glm::vec3(-5.0,-1.5f,0);
    
    state.walls[3].textureID=wallTextureID;
    state.walls[3].position= glm::vec3(-5.0,-0.5f,0);
    
    state.walls[4].textureID=wallTextureID;
    state.walls[4].position= glm::vec3(-5.0,0.5f,0);
    
    state.walls[5].textureID=wallTextureID;
    state.walls[5].position= glm::vec3(-5.0,1.5f,0);
    
    state.walls[6].textureID=wallTextureID;
    state.walls[6].position= glm::vec3(-5.0,2.5f,0);
    
    state.walls[7].textureID=wallTextureID;
    state.walls[7].position= glm::vec3(-5.0,3.5f,0);
    
    state.walls[8].textureID=wallTextureID;
    state.walls[8].position= glm::vec3(5.0,2.5f,0);
    
    state.walls[9].textureID=wallTextureID;
    state.walls[9].position= glm::vec3(5.0,1.5f,0);
    
    state.walls[10].textureID=wallTextureID;
    state.walls[10].position= glm::vec3(5.0,-0.5f,0);
    
    state.walls[11].textureID=wallTextureID;
    state.walls[11].position= glm::vec3(5.0,-1.5f,0);
    
    state.walls[12].textureID=wallTextureID;
    state.walls[12].position= glm::vec3(4.0,-1.5f,0);
    
    state.walls[13].textureID=wallTextureID;
    state.walls[13].position= glm::vec3(3.0,-1.5f,0);
    
    state.walls[14].textureID=wallTextureID;
    state.walls[14].position= glm::vec3(2.0,-1.5f,0);
    
    state.walls[15].textureID=wallTextureID;
    state.walls[15].position= glm::vec3(1.0,-1.5f,0);
    
    state.walls[16].textureID=wallTextureID;
    state.walls[16].position= glm::vec3(-4.0,-3.5f,0);
    
    state.walls[17].textureID=wallTextureID;
    state.walls[17].position= glm::vec3(-1.0,-3.5f,0);
    
    state.walls[18].textureID=wallTextureID;
    state.walls[18].position= glm::vec3(0,-3.5f,0);
    
    state.walls[19].textureID=wallTextureID;
    state.walls[19].position= glm::vec3(1,-3.5f,0);
    
    state.walls[20].textureID=wallTextureID;
    state.walls[20].position= glm::vec3(2,-3.5f,0);
    
    state.walls[21].textureID=wallTextureID;
    state.walls[21].position= glm::vec3(3,-3.5f,0);
    
    state.walls[22].textureID=wallTextureID;
    state.walls[22].position= glm::vec3(4,-3.5f,0);
    
    state.walls[23].textureID=wallTextureID;
    state.walls[23].position= glm::vec3(5,-3.5f,0);
    
    state.walls[24].textureID=wallTextureID;
    state.walls[24].position= glm::vec3(-4,1.5f,0);
    
    state.walls[25].textureID=wallTextureID;
    state.walls[25].position= glm::vec3(-3,1.5f,0);
    
    
    
    
    
    for(int i=0;i<WALL_COUNT;i++){
        state.walls[i].Update(0,NULL,NULL,0,0);
    }
    
    
}

void ProcessInputGameLevel() {
    if(state.player->fail || state.player->success){
        mode=GAME_OVER;
    }
    state.player->movement = glm::vec3(0);
    state.player->acceleration=glm::vec3(0,-0.2f,0);
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
                        /*
                        if(state.player->collided){
                            state.player->jump=true;
                        }
                         */
                        break;
                         
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_A]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
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
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
    
}

void ProcessInputGameOver(){
    
    state.player->movement = glm::vec3(0);
    state.player->acceleration=glm::vec3(0,0,0);
    state.player->velocity=glm::vec3(0);
    
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
                        mode=GAME_LEVEL;
                        state.player->fail=false;
                        state.player->success=false;
                        state.player->position = glm::vec3(0,3.5f,0);
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
        state.player->Update(FIXED_TIMESTEP,state.platforms, state.walls,PLATFORM_COUNT,WALL_COUNT);
        deltaTime -= FIXED_TIMESTEP;
        
    }
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(int i=0; i<PLATFORM_COUNT;i++){
        state.platforms[i].Render(&program);
    }
    
    for(int i=0; i<WALL_COUNT;i++){
        state.walls[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if(state.player->fail){
        DrawText(&program, fontTextureID, "Mission Failed!", 1, -0.5f, glm::vec3(-3.5,0,0));
        DrawText(&program, fontTextureID, "Press Space to Restart", 0.9, -0.5f, glm::vec3(-4.5,-1.0,0));
    }
    else if (state.player->success){
        DrawText(&program, fontTextureID, "Mission Success!", 1, -0.5f, glm::vec3(-3.5,0,0));
        DrawText(&program, fontTextureID, "Press Space to Restart", 0.9, -0.5f, glm::vec3(-4.5,-1.0,0));
    }
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
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
