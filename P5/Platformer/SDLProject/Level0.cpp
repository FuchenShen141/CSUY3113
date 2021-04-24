#include "Level0.h"

void Level0::Initialize() {}
void Level0::Update(float deltaTime) {}
 void Level0::Render(ShaderProgram *program) {
     Util::DrawText(&program, fontTextureID, "Platformer Game", 1, -0.5f, glm::vec3(-3.5,0,0));
     Util::DrawText(&program, fontTextureID, "Press Enter to Restart", 0.9, -0.5f, glm::vec3(-4.5,-1.0,0));
 }

