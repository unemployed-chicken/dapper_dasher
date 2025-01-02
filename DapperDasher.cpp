#include <iostream>
#include "raylib.h"

// Window Properties 
const int window_dimensions[2]{ 512 , 380 }; // Width * Height
const int targetFps{ 60 };

// Sprite Update Properties
const float characterSpriteSpeed{ 1.0 / 12.0 };
const float nebula_sprite_activation{ 2.0 };

float background_x{ 0.0 };
float midground_x{ 0.0 };
float foreground_x{ 0.0 };

bool isOnGround{ true };
bool hasHitNebula{ false };

// Movement Mechanics
const int gravity{ 1000 }; // pixels/sec/sec
int nebulaVelocityX{ 400 }; // pixels per frame
int scarfyVelocityY{ 0 }; // pixels per frame
const int jumpVelocity{ 600 }; // pixels/sec
const int backgroundVelocityX{ 25 }; // pixels per frame
const int midgroundVelocityX{ 80 }; // pixels per frame
const int foregroundVelocityX{ 300 }; // pixels per frame

// Game Mechanics
const bool displayValues{ false };
const int number_of_nebulae{ 6 };
const int min_space_between_nebulae{ 600 };
const int texture_count{ 5 };


// All Structures
struct AnimData {
    Rectangle rect;
    Vector2 pos;
    float right;
    float bottom;
    int frame{ 0 };
    float runTime{ 0.0 };
    float updateRate{ characterSpriteSpeed };

    AnimData() {};

    AnimData(float x, float y, float rect_width, float rect_height) {
        rect = Rectangle{ x, y, rect_width, rect_height };
        pos = Vector2{ (window_dimensions[0] / 2 - rect.width / 2), (window_dimensions[1] - rect.height) };
        right = pos.x + rect.width;
        bottom = pos.y + rect.height;
    };

    AnimData(float x, float y, float rect_width, float rect_height, float vect_width) {
        rect = Rectangle{ x, y, rect_width, rect_height };
        pos = Vector2{ vect_width, (window_dimensions[1] - rect.height) };
        right = pos.x + rect.width;
        bottom = pos.y + rect.height;
    };

};



// All Functions:
void jump(const float dT);
void applyGravity(const float bottom, const float dT);
void changePositionY(AnimData& obj, const float dT);
void changePositionX(AnimData& obj, const int velocity, const float dT);
bool setIsOnGround(const float bottom);
bool setNebulaCurrentlyExists(const float right);
void displayTestData(Vector2 scarfyPos, Vector2 nebulaPos, float scarfy_bottom_side, float nebula_right_side);
void updateAnimation(AnimData& obj, int mod);
void updateAnimations(AnimData& scarfy, AnimData nebulae[number_of_nebulae], const float dT);
void changeNebulaePosition(AnimData nebulae[number_of_nebulae], const float dT);
void drawNebulae(Texture2D nebula_text, AnimData nebulae[number_of_nebulae]);
int generateRandomPixelCount();
void unloadAllTextures(Texture2D all[texture_count]);
void drawAllBackgrounds(const Texture2D back, const Texture2D mid, const Texture2D fore, const float dT);
bool checkForCollision(AnimData scarfy, AnimData nebulae[number_of_nebulae]);



int main() {
    InitWindow(window_dimensions[0], window_dimensions[1], "Dapper Dasher");
    SetTargetFPS(targetFps);

    // Game Properties
    int frame{ 0 };
    float seconds_since_animation_update{ 0.0 };
    float seconds_since_last_nebula{ 0.0 };

    // Background Textures
    Texture2D background = LoadTexture("textures\\far-buildings.png");
    Texture2D middleground = LoadTexture("textures\\back-buildings.png");
    Texture2D foreground = LoadTexture("textures\\foreground.png");


    // SCARFY 
    Texture2D scarfy_text = LoadTexture("textures\\scarfy.png");
    AnimData scarfy = AnimData(0.0, 0.0, scarfy_text.width / 6, scarfy_text.height);

    // NEBULA
    Texture2D nebula_text = LoadTexture("textures\\12_nebula_spritesheet.png");
    AnimData nebulae[number_of_nebulae];
    
    // All Textures stored for removal at end
    Texture2D all_textures[texture_count] = { background, middleground, foreground, scarfy_text,  nebula_text };


    for (int i{ 0 }; i < number_of_nebulae; ++i) {
        nebulae[i] = AnimData(0.0, 0.0, nebula_text.width / 8, nebula_text.height / 8, (window_dimensions[0] + generateRandomPixelCount() * i));
    }
   
    float finish_line{ nebulae[number_of_nebulae - 1].pos.x + 500 };

    while (!WindowShouldClose()) {
        // Time from last frame
        const float dT{ GetFrameTime() };

        // Update all animations
        updateAnimations(scarfy, nebulae, dT);

        // Start Drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // Draw the background
        drawAllBackgrounds(background, middleground, foreground, dT);

        // Update velocity of Scarfy
        applyGravity(scarfy.bottom, dT);
        
        if (IsKeyPressed(KEY_SPACE) && isOnGround) {
            jump(dT);
        }

        if (finish_line <= scarfy.right) {
            DrawText("VICTORY!!!!!", window_dimensions[0] * .25, window_dimensions[1] / 2, 50, DARKPURPLE);
        }
        else if (!hasHitNebula) {
            // Change Character Position
            changePositionY(scarfy, dT);

            // Change Nebula Position
            changeNebulaePosition(nebulae, dT);

            // Change Finishline Position
            finish_line -= nebulaVelocityX * dT;

            // Draw Scarfy
            DrawTextureRec(scarfy_text, scarfy.rect, scarfy.pos, WHITE);

            // Draw Nebulae
            drawNebulae(nebula_text, nebulae);

            isOnGround = setIsOnGround(scarfy.bottom);
            hasHitNebula = checkForCollision(scarfy, nebulae);
        }
        else {
            DrawText("GAME OVER", window_dimensions[0] * .25, window_dimensions[1] / 2, 50, DARKPURPLE);
        }

        // For testing purposes:
        if (displayValues) {
            displayTestData(scarfy.pos, nebulae[0].pos, scarfy.bottom, nebulae[0].right);
        }

        EndDrawing();
    }

    unloadAllTextures(all_textures);
    CloseWindow();
}




void jump(const float dT) {
    scarfyVelocityY -= jumpVelocity ;
}

void applyGravity(const float bottom, const float dT) {
    scarfyVelocityY += (gravity * dT);
}

void changePositionX(AnimData& obj, const int velocity, const float dT) {
    // X-Coordinate Change
    obj.pos.x -= velocity * dT;
    obj.right = obj.pos.x + obj.rect.width;
}

void changePositionY(AnimData& obj, const float dT) {
    // Y-Coordinate Change
    if (obj.pos.y + obj.rect.height + scarfyVelocityY * dT > window_dimensions[1]) {
        obj.pos.y = window_dimensions[1] - obj.rect.height;
    }
    else {
        obj.pos.y += scarfyVelocityY * dT;
    } 
    obj.bottom = obj.pos.y + obj.rect.height;
}

bool setIsOnGround(const float bottom) {
    if (bottom == window_dimensions[1]) {
        scarfyVelocityY = 0;
        return true;
    }
    return false;
}

bool setNebulaCurrentlyExists(const float right) {
    if (right >= 0) {
        return true;
    }
    return false;
}


void displayTestData(Vector2 scarfyPos, Vector2 nebulaPos, float scarfy_bottom_side, float nebula_right_side) {
    char scarftopbase[] = "ScarfyRec.Y: ";
    char scarfY[50];
    int top = scarfyPos.y;
    sprintf_s(scarfY, "%s%d", scarftopbase, top);
    DrawText(scarfY, 20, 25, 20, BLACK);

    char scarfbottombase[] = "Scarfy Feet: ";
    char scarfB[50];
    int bot = scarfy_bottom_side;
    sprintf_s(scarfB, "%s%d", scarfbottombase, bot);
    DrawText(scarfB, 20, 50, 20, BLACK);

    DrawText("-------------------------", 20, 75, 20, BLACK);

    char nebulaLeftBase[] = "nebulaRec.X: ";
    char neb[50];
    int left = nebulaPos.x;
    sprintf_s(neb, "%s%d", nebulaLeftBase, left);
    DrawText(neb, 20, 100, 20, BLACK);

    char nebularightBase[] = "nebula Right: ";
    char nebR[50];
    int right = nebula_right_side;
    sprintf_s(nebR, "%s%d", nebularightBase, right);
    DrawText(nebR, 20, 125, 20, BLACK);

    DrawText("-------------------------", 20, 150, 20, BLACK);

    char yVelBase[] = "scarfyVelocityY: ";
    char yvel[50];
    sprintf_s(yvel, "%s%i", yVelBase, scarfyVelocityY);
    DrawText(yvel, 20, 175, 20, BLACK);

    char xVelBase[] = "nebulaVelocityX: ";
    char xvel[50];
    sprintf_s(xvel, "%s%i", xVelBase, nebulaVelocityX);
    DrawText(xvel, 20, 200, 20, BLACK);

    DrawText("-------------------------", 20, 225, 20, BLACK);

    char isOnGroundBase[] = "isOnGround: ";
    char ground[50];
    sprintf_s(ground, "%s%i", isOnGroundBase, isOnGround);
    DrawText(ground, 20, 250, 20, BLACK);

}


void updateAnimation(AnimData& obj, int mod) {
    obj.rect.x = obj.rect.width * (obj.frame % mod);
    obj.runTime = 0.0;
    ++obj.frame;
}

void updateAnimations( AnimData& scarfy, AnimData nebulae[number_of_nebulae], const float dT) {
    scarfy.runTime += dT;
    if (scarfy.runTime >= scarfy.updateRate && isOnGround) {
        updateAnimation(scarfy, 6);
    }

    for (int i = 0; i < number_of_nebulae; ++i) {
        nebulae[i].runTime += dT;
        if (nebulae[i].runTime >= nebulae[i].updateRate) {
            updateAnimation(nebulae[i], 8);
        }
    }   
}

void changeNebulaePosition(AnimData nebulae[number_of_nebulae], const float dT) {
    for (int i = 0; i < number_of_nebulae; ++i) {
        changePositionX(nebulae[i], nebulaVelocityX, dT);
    }
}


void drawNebulae(Texture2D nebula_text, AnimData nebulae[number_of_nebulae]) {
    for (int i = 0; i < number_of_nebulae; ++i) {
        DrawTextureRec(nebula_text, nebulae[i].rect, nebulae[i].pos, WHITE);
    }
}



// Future Work: Make a short and long randomizer. Randomly choose if it is a long or short pause
int generateRandomPixelCount() {
    srand(time(0));
    int i = rand() % ( 12 ); // 12 too long
    return (i * 100) + min_space_between_nebulae;
}


void unloadAllTextures(Texture2D all[texture_count]) {
    for (int i = 0; i < texture_count; ++i) {
        UnloadTexture(all[i]);
    }
}

void drawAllBackgrounds(const Texture2D back, const Texture2D mid, const Texture2D fore,  const float dT) {
    background_x -= backgroundVelocityX * dT;
    midground_x -= midgroundVelocityX * dT;
    foreground_x -= foregroundVelocityX * dT;

    if (background_x <= -back.width * 2) { background_x = 0.0; }
    if (midground_x <= -mid.width * 2) { midground_x = 0.0; }
    if (foreground_x <= -fore.width * 2) { foreground_x = 0.0; }

    // Draw Background
    Vector2 background_1_pos{ background_x, 0.0 };
    Vector2 background_2_pos{ background_x + back.width * 2, 0.0 };
    DrawTextureEx(back, background_1_pos, 0.0, 2.0, WHITE);
    DrawTextureEx(back, background_2_pos, 0.0, 2.0, WHITE);

    // Draw Midground
    Vector2 midground_1_pos{ midground_x, 0.0 };
    Vector2 midground_2_pos{ midground_x + mid.width * 2, 0.0 };
    DrawTextureEx(mid, midground_1_pos, 0.0, 2.0, WHITE);
    DrawTextureEx(mid, midground_2_pos, 0.0, 2.0, WHITE);

    // Draw Foreground
    Vector2 foreground_1_pos{ foreground_x, 0.0 };
    Vector2 foreground_2_pos{ foreground_x + fore.width * 2, 0.0 };
    DrawTextureEx(fore, foreground_1_pos, 0.0, 2.0, WHITE);
    DrawTextureEx(fore, foreground_2_pos, 0.0, 2.0, WHITE);
}

bool checkForCollision(AnimData scarfy, AnimData nebulae[number_of_nebulae]) {
    // We do not need to remove padding from Scarfy because the reference images does not have much white space around the sprite
    Rectangle scarfyRec{
        scarfy.pos.x,
        scarfy.pos.y,
        scarfy.rect.width,
        scarfy.rect.height
    };
    
    for (int i = 0; i < number_of_nebulae; ++i) {
        // Used to remove white space surrounding Nebula
        float pad{ 50 }; 
        Rectangle nebRec{
            // Removes space on the left and top of nebula got
            nebulae[i].pos.x + pad,
            nebulae[i].pos.y + pad,
            
            // Removes Space on bottom and right. This is multiplied by 2 because we take 20 off the top AND 20 off the bottom
            nebulae[i].rect.width - 2 * pad,
            nebulae[i].rect.height - 2 * pad
        };

        if (CheckCollisionRecs(scarfyRec, nebRec)) {
            return true;
        }
    }
    return false;
}