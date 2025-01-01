#include <iostream>
#include "raylib.h"

// Window Properties
const int windowWidth{ 512 };
const int windowHeight{ 380 };
const int targetFps{ 60 };

// Sprite Update Properties
const float characterSpriteSpeed{ 1.0 / 12.0 };
const float nebula_sprite_activation{ 5.0 };

bool isOnGround{ true };
bool nebula_currently_exists{ false };

// Movement Mechanics
const int gravity{ 1000 }; // pixels/sec/sec
int xVelocity{ 400 }; // pixels per frame
int yVelocity{ 0 }; // pixels per frame
const int jumpVelocity{ 600 }; // pixels/sec

const bool displayValues{ false };

void jump(const float dT);
void applyGravity(const float bottom, const float dT);
void changePositionY(Vector2& scarfyPos, float& bottom, Rectangle& scarfyRec, const float dT);
void changePositionX(Vector2& vect, float& right, Rectangle& rect, const float dT);
bool setIsOnGround(const float bottom);
bool setNebulaCurrentlyExists(const float right);
void displayTestData(Vector2 scarfyPos, Vector2 nebulaPos, float scarfy_bottom_side, float nebula_right_side);
void updateScarfyAnimation(Rectangle& scarfyRec, int frame);
void updateNebulaAnimation(Rectangle& nebulaRec, int frame);
void restartNebulaAnimation(Vector2& nebulaPos, float& seconds_since_last_nebula);
void updateAnimations(
    Rectangle& scarfyRec, Rectangle& nebulaRec, Vector2& nebulaPos, int& frame, float& seconds_since_animation_update, float& seconds_since_last_nebula
);


int main() {
    InitWindow(windowWidth, windowHeight, "Dapper Dasher");
    SetTargetFPS(targetFps);

    // Game Properties
    int frame{ 0 };
    float seconds_since_animation_update{ 0.0 };
    float seconds_since_last_nebula{ 0.0 };


    // SCARFY 
    Texture2D scarfy = LoadTexture("textures\\scarfy.png");
    Rectangle scarfyRec{0.0, 0.0, scarfy.width / 6, scarfy.height };
    Vector2 scarfyPos{ (windowWidth / 2 - scarfyRec.width / 2), (windowHeight - scarfyRec.height) };

        // Scarfy's Screen Location
    float scarfy_right_side = scarfyPos.x + scarfyRec.width;
    float scarfy_bottom_side = scarfyPos.y + scarfyRec.height;


    // NEBULA
    Texture2D nebula = LoadTexture("textures\\12_nebula_spritesheet.png");
    Rectangle nebulaRec{ 0, 0, nebula.width / 8,  nebula.height / 8 };
    Vector2 nebulaPos{ windowWidth, (windowHeight - nebulaRec.height) };

        // Nebulas's Screen Location
    float nebula_right_side = nebulaPos.x + nebulaRec.width;
    float nebula_bottom_side = nebulaPos.y + nebulaRec.height;



    //////////////////////////////////////////
    //////////////// GAMEPLAY ////////////////
    //////////////////////////////////////////

    while (!WindowShouldClose()) {
        // Time from last frame
        const float dT{ GetFrameTime() };
        seconds_since_animation_update += dT;
        seconds_since_last_nebula += dT;

        updateAnimations(scarfyRec, nebulaRec, nebulaPos, frame, seconds_since_animation_update, seconds_since_last_nebula);

        BeginDrawing();
        ClearBackground(WHITE);

        applyGravity(scarfy_bottom_side, dT);
        
        if (IsKeyPressed(KEY_SPACE) && isOnGround) {
            jump(dT);
        }

        // Change Character Position
        changePositionY(scarfyPos, scarfy_bottom_side, scarfyRec, dT);

        // Change Nebula Position
        changePositionX(nebulaPos, nebula_right_side, nebulaRec, dT);

        DrawTextureRec(scarfy, scarfyRec, scarfyPos, WHITE);
        DrawTextureRec(nebula, nebulaRec, nebulaPos, WHITE);
        
        isOnGround = setIsOnGround(scarfy_bottom_side);
        nebula_currently_exists = setNebulaCurrentlyExists(nebula_right_side);

        // For testing purposes:
        if (displayValues) {
            displayTestData(scarfyPos, nebulaPos, scarfy_bottom_side, nebula_right_side);
        }

        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    CloseWindow();
}

void jump(const float dT) {
    yVelocity -= jumpVelocity ;
}

void applyGravity(const float bottom, const float dT) {
    yVelocity += (gravity * dT);
}

void changePositionX(Vector2& vect, float& right, Rectangle& rect, const float dT) {
    // X-Coordinate Change
    vect.x -= xVelocity * dT;
    //vect.x -= xVelocity;
    right = vect.x + rect.width;
}

void changePositionY(Vector2& scarfyPos, float& bottom, Rectangle& scarfyRec, const float dT) {
    // Y-Coordinate Change
    if (scarfyPos.y + scarfyRec.height + yVelocity * dT > windowHeight) {
        scarfyPos.y = windowHeight - scarfyRec.height;
    }
    else {
        scarfyPos.y += yVelocity * dT;
    } 
    bottom = scarfyPos.y + scarfyRec.height;
}

bool setIsOnGround(const float bottom) {
    if (bottom == windowHeight) {
        yVelocity = 0;
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

    char yVelBase[] = "yVelocity: ";
    char yvel[50];
    sprintf_s(yvel, "%s%i", yVelBase, yVelocity);
    DrawText(yvel, 20, 175, 20, BLACK);

    char xVelBase[] = "xVelocity: ";
    char xvel[50];
    sprintf_s(xvel, "%s%i", xVelBase, xVelocity);
    DrawText(xvel, 20, 200, 20, BLACK);

    DrawText("-------------------------", 20, 225, 20, BLACK);

    char isOnGroundBase[] = "isOnGround: ";
    char ground[50];
    sprintf_s(ground, "%s%i", isOnGroundBase, isOnGround);
    DrawText(ground, 20, 250, 20, BLACK);

    char isNebulaPresent[] = "nebula_currently_exists: ";
    char isNeb[50];
    sprintf_s(isNeb, "%s%i", isNebulaPresent, nebula_currently_exists);
    DrawText(isNeb, 20, 275, 20, BLACK);
}


void updateScarfyAnimation(Rectangle& scarfyRec, int frame) {
    scarfyRec.x = scarfyRec.width * (frame % 6);
}

void updateNebulaAnimation(Rectangle& nebulaRec, int frame) {
    nebulaRec.x = nebulaRec.width * (frame % 8);
}


void restartNebulaAnimation(Vector2& nebulaPos, float& seconds_since_last_nebula) {
    seconds_since_last_nebula = 0.0;
    nebulaPos.x = windowWidth;
}

void updateAnimations(
    Rectangle& scarfyRec, Rectangle& nebulaRec, Vector2& nebulaPos, int& frame, float& seconds_since_animation_update, float& seconds_since_last_nebula
) {
    if (seconds_since_animation_update >= characterSpriteSpeed) {
        if (isOnGround) {
            updateScarfyAnimation(scarfyRec, frame);
        }
        if (nebula_currently_exists) {
            updateNebulaAnimation(nebulaRec, frame);
        }
        seconds_since_animation_update = 0.0;
        ++frame;
    }
        

    if (seconds_since_last_nebula >= nebula_sprite_activation && !nebula_currently_exists) {
        restartNebulaAnimation(nebulaPos, seconds_since_last_nebula);
    }
    
}