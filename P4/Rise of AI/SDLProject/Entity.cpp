#include "Entity.hpp"

Entity::Entity()
{
    position = glm::vec3(0);
    movement= glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity =glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){
    if(other==this || other==NULL)return false;
    float xdist=fabs(position.x-other->position.x)-((width + other->width)/2.0f);
    float ydist=fabs(position.y-other->position.y)-((height + other->height)/2.0f);
    if (xdist<0 && ydist<0)return true;
    return false;
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight=true;
            }
            else if (velocity.x < 0) { position.x += penetrationX;
                velocity.x = 0;
                collidedLeft=true;
            }
        }
    }
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y >0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop=true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom=true;
            }
        }
    }
}

void Entity::CheckCollisionsYForEnemy(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y >=0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop=true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom=true;
            }
        }
    }
}

void Entity::AIWalker(){
    movement=glm::vec3(-1,0,0);
}

void Entity::AIWaitAndGo(Entity *player){
    switch (aiState) {
        case IDLE:
            if (glm::distance(position, player->position)<4.0f){
                aiState=WALKING;
            }
            break;
            
        case WALKING:
            if (player->position.x < position.x){
                movement = glm::vec3(-1,0,0);
            } else{
                movement = glm::vec3(1,0,0);
            }
            break;
            
        case ATTACKING:
            break;
            
        case Patrolling:
            if (velocity.x==0){
                movement = glm::vec3(-1,0,0);
            }
            if(position.x<1){
                float penetrationX = fabs(1-position.x);
                position.x += penetrationX;
                movement.x = -movement.x;
            }else if(position.x>4){
                float penetrationX = fabs(position.x-4);
                position.x -= penetrationX;
                movement.x = -movement.x;
            }
                
            break;
    }
}

void Entity::AI(Entity *player, Entity *platforms, int platformCOunt){
    switch (aiType) {
        case WALKER:
            AIWalker();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case JUMPER:
            if(velocity.y==0){
                jump=true;
            }
            movement=glm::vec3(-0.01f,0,0);
            CheckCollisionsY(platforms, platformCOunt);
            if(collidedBottom){
                jump=true;
            }
            break;
    }
    
}

void Entity::Update(float deltaTime,Entity *player,Entity *platforms, int platformCount)
{
    if(isActive==false)return;
    
    collidedBottom=false;
    collidedLeft=false;
    collidedTop=false;
    collidedRight=false;
    
    if (entityType==ENEMY){
        AI(player,platforms,platformCount);
    }
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            
            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump){
        jump=false;
        velocity.y +=jumpPower;
    }
   
    velocity.x=movement.x*speed;
    velocity += acceleration*deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    CheckCollisionsX(platforms, platformCount);// Fix if needed
   
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    CheckCollisionsX(player,1);
    if((collidedLeft || collidedRight) && this !=player && player->velocity.y==0){
        player->fail=true;
    }else {
        CheckCollisionsYForEnemy(player,1);
        if(collidedTop && this!=platforms && player->velocity.y!=0){
            isActive=false;
            player->enemyKilled+=1;
        }
        
    }
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isActive==false)return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
