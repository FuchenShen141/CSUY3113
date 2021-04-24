#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL_1_ENEMY_COUNT 1

unsigned int level1_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


void Level1::Initialize() {
    state.nextScene=-1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
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
    
    state.player->height=1.0f;
    state.player->width = 1.0f;
    
    state.player->jumpPower=7.0f;
    
    
    // Initialize Game Enemies
    state.enemies = new Entity[LEVEL_1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemyRed1.png");
    
    for(int i=0;i<LEVEL_1_ENEMY_COUNT;i++){
        state.enemies[i].entityType= ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 0.5;
        state.enemies[i].acceleration = glm::vec3(0,-9.81f,0);
        state.enemies[i].jumpPower =5.0f;
        state.enemies[i].isActive=true;
        
    }
    state.enemies[0].position = glm::vec3(10,3,0);
    state.enemies[0].aiType= WAITANDGO;
    state.enemies[0].aiState = WALKING;
    
    
    
    // Move over all of the player and enemy code from initialization.
    
    }
    void Level1::Update(float deltaTime) {
        state.player->Update(deltaTime, state.player, state.enemies, LEVEL_1_ENEMY_COUNT, state.map);
        for(int i=0;i<LEVEL_1_ENEMY_COUNT;i++){
            state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL_1_ENEMY_COUNT, state.map);
        }
        if(state.player->position.x >= 14){
            state.nextScene=1;
        }
    }
    
    void Level1::Render(ShaderProgram *program) { state.map->Render(program);
        state.player->Render(program);
        for(int i=0; i<LEVEL_1_ENEMY_COUNT;i++){
            state.enemies[i].Render(program);
        }
    }
