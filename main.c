#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"



#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define TOTAL_RECT 10

typedef struct Player {
    Vector2 position;
    Rectangle rect;
    float speed;
    bool canJump;
    bool soundJump;
} Player;

typedef struct objectRect
{
    Rectangle rect;
    Color color;
    bool isExist;
    bool isSelected;
} objectRect;


typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
//void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);
void UpdatePlayer(Player *player, objectRect *objectR,float delta);
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void addObjectRect(objectRect *objectR);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

   

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");
    
    InitAudioDevice();      // Initialize audio device
    
    Sound fxWav = LoadSound("resources/smw_jump.wav");        // Load OGG audio file
    Sound boing = LoadSound("resources/mb_jump.wav");
    Sound biong = LoadSound("resources/smb_jump-small.wav");
    Sound boo = LoadSound("resources/nsmb_mini-mario_jump.wav");
    
    bool albert = false;
    bool bong = false;
    bool cock = false;
    bool dick = false;
    
    //Color boxColour;

    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.rect = (Rectangle){20,20,player.position.x,player.position.y};

    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {
        {{ 0, 0, 1000, 400 }, 0, LIGHTGRAY },
        {{ 0, 400, 1000, 200 }, 1, GRAY },
        {{ 300, 200, 400, 10 }, 1, GRAY },
        {{ 250, 300, 100, 10 }, 1, GRAY },
        {{ 650, 300, 100, 10 }, 1, GRAY }
    };


    objectRect objectR[TOTAL_RECT];

    //debug
    int mousePosx = 0;
    int recX = 0;
    bool collide = false;


    
    bool isSelecting = false;
    bool isOpenProperty = false;
    int selected = 0;
   

    //initialize objectR array

    for(int i = 0; i < TOTAL_RECT; i++)
    {
        objectR[i].rect.height = 0;
        objectR[i].rect.width = 0;
        objectR[i].rect.x = 0;
        objectR[i].rect.y = 0;

        objectR[i].color = WHITE;
        objectR[i].isExist = false;
    }

    objectR[0].rect = (Rectangle){100,500,1000,100};
    objectR[0].isExist = true;
    objectR[0].isSelected = false;
   
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

    Camera2D camera = { 0 };
    camera.target = player.position;
   
    camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f };
   
    camera.rotation = 0.0f;
    camera.zoom = 1;

    //Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
        UpdateCameraCenter,
        UpdateCameraCenterInsideMap,
        UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding,
        UpdateCameraPlayerBoundsPush
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center, but clamp to map edges",
        "Follow player center; smoothed",
        "Follow player center horizontally; updateplayer center vertically after landing",
        "Player push camera on getting too close to screen edge"
    };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

    
        UpdatePlayer(&player, objectR, deltaTime);

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.position = (Vector2){ 400, 280 };
        }

        if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1)%cameraUpdatersLength;
        
        if (IsKeyDown (KEY_A)) 
        {
            albert = true;
            bong = false;
            cock = false;
            dick = false;
        }
        
        if (IsKeyDown (KEY_S)) 
        {
            bong = true;
            albert = false;
            cock = false;
            dick = false;
        }
        
        if (IsKeyDown (KEY_D)) 
        {
            cock = true;
            albert = false;
            bong = false;
            dick = false;
        }
        
        if (IsKeyDown (KEY_F)) 
        {
            cock = false;
            albert = false;
            bong = false;
            dick = true;
        }
        
        if (albert == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (fxWav);
            }
        }
        
        if(bong == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (boing);
            }
        }
        
        if(cock == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (biong);
            }
        }
        
        if(dick == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (boo);
            }
        }
        
        //Add objects
        if (IsKeyPressed(KEY_E))
        {
            // bool foundEmptySlot = false;
            // int emptySlot;

            // //find empty spot
            // for(int i = 0;i < TOTAL_RECT; i++ && !foundEmptySlot)
            // {
            //     if(!objectR[i].isExist)
            //     {
            //         emptySlot = i;
            //         foundEmptySlot = true;
            //     }
            // }

            // if(foundEmptySlot)
            // {
            //     objectR[emptySlot].isExist = true;
            //     objectR[emptySlot].rect.height = 100;
            //     objectR[emptySlot].rect.width = 100;
            //     objectR[emptySlot].rect.x = 100;
            //     objectR[emptySlot].rect.x = 100;
            //     objectR[emptySlot].isSelected = 0;
            // }


            addObjectRect(objectR);
        }

        mousePosx = GetMousePosition().x;
        recX = objectR[0].rect.x;

        Vector2 mousePosition;
        mousePosition.x = (GetMousePosition().x - camera.offset.x)/camera.zoom + camera.target.x;
        mousePosition.y = (GetMousePosition().y - camera.offset.y)/camera.zoom + camera.target.y;
       
        
        //Highlight Object to drag
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            bool foundClick = false;
            if(!isSelecting)
            {
                for(int i = 0; i <TOTAL_RECT; i++ && !foundClick)
                {
                    if(objectR[i].isExist)
                    {
                        if(CheckCollisionPointRec(mousePosition, objectR[i].rect) && !foundClick)
                        {
                            objectR[i].isSelected = true;
                            foundClick = true;
                            selected = i;
                            isSelecting = true;
                        } 
                    }
                }
            }
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSelecting)
        {
            isSelecting = false;     
            objectR[selected].isSelected = false;    
        }

        //Moving highlighted object
        if(isSelecting && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
        {
            objectR[selected].rect.x = mousePosition.x - objectR[selected].rect.width/2  ;
            objectR[selected].rect.y = mousePosition.y - objectR[selected].rect.height/2 ;
        }

        
        //highlight object to edit
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            bool foundClick = false;
            isOpenProperty = false;   

            for(int i = 0; i <TOTAL_RECT; i++ && !foundClick)
            {
                if(objectR[i].isExist)
                {
                    if(CheckCollisionPointRec(mousePosition, objectR[i].rect) && !foundClick)
                    {
                        selected = i;
                        objectR[i].isSelected = true;
                        foundClick = true;
                        isOpenProperty = true;
                    }
                    else
                    {
                        objectR[i].isSelected = false; 
                    } 
                }
            }       
        }

        
        //Call update camera function by its pointer
        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

                for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

                for(int i = 0; i < TOTAL_RECT; i++)
                {
                    if(objectR[i].isExist)
                    {
                        if(objectR[i].isSelected)
                        {
                            DrawRectangleRec(objectR[i].rect,objectR[i].color);
                            DrawRectangleLinesEx(objectR[i].rect,5, GREEN);
                        }
                        else
                        {
                            DrawRectangleRec(objectR[i].rect,objectR[i].color); 
                        }    
                    }
                }

                //DrawRectangle(0,0,1280,720,GRAY);
                DrawCircle((GetMousePosition().x - camera.offset.x)/camera.zoom + camera.target.x ,(GetMousePosition().y - camera.offset.y)/camera.zoom +camera.target.y,5,RED);

                //Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
                DrawRectangleRec(player.rect, RED);
                
                //guiTest
                //DrawRectangle(400, 240, 40, 40, boxColour);
                
                //alpha color untuk added Box
                //boxColour.a = 255;

           EndMode2D();

            //GUI
            
            DrawText("Controls:", 20, 20, 10, BLACK);
            DrawText("- Right/Left to move", 40, 40, 10, DARKGRAY);
            DrawText("- Space to jump", 40, 60, 10, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out, R to reset ", 40, 80, 10, DARKGRAY);
            DrawText("- C to change camera mode", 40, 100, 10, DARKGRAY);
            DrawText("Current camera mode:", 20, 120, 10, BLACK);
            DrawText(cameraDescriptions[cameraOption], 40, 140, 10, DARKGRAY);
            
            //Sound
            DrawText("Sound:", 820,20,10, BLACK);
            DrawText("Press A for Sound A", 840, 40, 10, DARKGRAY);
            DrawText("Press S for Sound S", 840, 70, 10, DARKGRAY);
            DrawText("Press D for Sound D", 840, 100, 10, DARKGRAY);
            DrawText("Press F for Sound F", 840, 130, 10, DARKGRAY);
            
            //Toggle
            if (albert)
            {
                DrawText("Press A for Sound A", 840, 40, 10, RED);
            }
            else if (bong)
            {
                DrawText("Press S for Sound S", 840, 70, 10, RED);
            }
            else if (cock)
            {
                DrawText("Press D for Sound D", 840, 100, 10, RED);
            }
            else if (dick)
            {
                DrawText("Press F for Sound F", 840, 130, 10, RED);
            }


            if(isOpenProperty)
            {
                DrawRectangle(GetScreenWidth() -  GetScreenWidth()/4, 0, GetScreenWidth()/4, GetScreenHeight(), Fade(WHITE, 1));
                //Set Color
                //parametersBox color R 
                
                objectR[selected].color.r = (int)GuiSliderBar((Rectangle){ 1000, 90, 105, 20 }, "Red", NULL,  objectR[selected].color.r, 0, 255);
                
                //parametersBox color G 
                objectR[selected].color.g = (int)GuiSliderBar((Rectangle){ 1000, 120, 105, 20 }, "Green", "", objectR[selected].color.g, 0, 255);
                
                //parametersBox color B 
                objectR[selected].color.b = (int)GuiSliderBar((Rectangle){ 1000, 150, 105, 20 }, "Blue", "", objectR[selected].color.b, 0, 255);

                // //Change Size
                objectR[selected].rect.height = (int)GuiSliderBar((Rectangle){ 640, 40, 105, 20 }, "Width", NULL, objectR[selected].rect.height, 0, 1000);
                objectR[selected].rect.width = (int)GuiSliderBar((Rectangle){ 640, 70, 105, 20 }, "Height", NULL, objectR[selected].rect.width, 0, 1000);

               // recta = GuiButton((Rectangle){ 1100, 600, 105, 20 }, GuiIconText(ICON_HAND_POINTER, "ADD Objects"));
            }
           
            //Add Box
            // function add box = GuiButton((Rectangle){ 1100, 600, 105, 20 }, GuiIconText(ICON_HAND_POINTER, "ADD Objects"));
            
            // //Change Size
            // parametersBox x = (int)GuiSliderBar((Rectangle){ 640, 40, 105, 20 }, "Width", NULL, cubeSize.x, 0, 10);
            //parametersBox y = (int)GuiSliderBar((Rectangle){ 640, 70, 105, 20 }, "Height", NULL, cubeSize.y, 0, 10);
            
            
            //debug
           
            DrawText(TextFormat("Integer value: %d", mousePosx), 620, 130, 10, DARKGRAY);
            DrawText(TextFormat("Integer value: %d", recX), 620, 160, 10, DARKGRAY);

            if(collide)
            {
                DrawText(TextFormat("COLLIDE"), 620, 180, 10, DARKGRAY);    
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


void addObjectRect(objectRect *objectR)
{

    bool foundEmptySlot = false;
    int emptySlot;
    objectRect *oi;
    for(int i = 0;i < TOTAL_RECT; i++ && !foundEmptySlot)
    {
        oi = objectR + i;
        if(!oi->isExist)
        {
            emptySlot = 0;
            foundEmptySlot = true;
        }
    }
    if(foundEmptySlot)
    {
        oi[emptySlot].isExist = true;
        oi[emptySlot].rect.height = 100;
        oi[emptySlot].rect.width = 100;
        oi[emptySlot].rect.x = 0;
        oi[emptySlot].rect.x = 0;
    }

    // bool foundEmptySlot = false;
    // int emptySlot;
    // //find empty spot
    // for(int i = 0;i < TOTAL_RECT; i++ && !foundEmptySlot)
    // {
    //     if(!objectR[i]->isExist)
    //     {
    //         emptySlot = 0;
    //         foundEmptySlot = true;
    //     }
    // }
    // if(foundEmptySlot)
    // {
    //     objectR[emptySlot]->isExist = true;
    //     objectR[emptySlot]->rect.height = 100;
    //     objectR[emptySlot]->rect.width = 100;
    //     objectR[emptySlot]->rect.x = 0;
    //     objectR[emptySlot]->rect.x = 0;
    // }
}

void UpdatePlayer(Player *player, objectRect *objectR,float delta)
{
    
    
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }


    int hitObstacle = 0;
    for (int i = 0; i < TOTAL_RECT; i++)
    {
        objectRect *oi = objectR + i;
        if(CheckCollisionRecs(oi->rect,player->rect))
        {
            hitObstacle = 1;
            
            player->speed = 0.0f;

            //Up Collision
            if(oi->rect.y > player->rect.y)
            {
                player->position.y = oi->rect.y- player->rect.height;
            }

            // if(player->rect.y + player->rect.height > oi->rect.height)
            // {
            //     player->position.y =  oi->rect.y - player->rect.height;
            // }
            // else if((player->rect.y < oi->rect.y + oi->rect.height) && !(oi->rect.x > player->rect.x) && !(oi->rect.x - oi->rect.width < player->rect.x))
            // {
            //     //player->position.y = player->position.y - (oi->rect.y + oi->rect.height);
            //     player->position.y = (oi->rect.y + oi->rect.height);
            //     //player->speed = 0;
            // }

            //Left Collision
            else if(oi->rect.x < player->rect.x)
            {
                player->position.x = oi->rect.x - player->rect.width;
                
            }
            // else if(player->rect.x < oi->rect.x + oi->rect.width)
            // {
            //     player->position.x = oi->rect.x + oi->rect.width;
            // }

            //Right Collision
            else if(oi->rect.x - oi->rect.width < player->rect.x)
            {
                player->position.x = (oi->rect.x + oi->rect.width) + 1 ;
            }
            
            
        }
    } 
    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;

    }
    else player->canJump = true;


    player->rect=(Rectangle){player->position.x,player->position.y,10,10};

    // int hitObstacle = 0;
    // for (int i = 0; i < envItemsLength; i++)
    // {
    //     EnvItem *ei = envItems + i;
    //     Vector2 *p = &(player->position);
    //     if (ei->blocking &&
    //         ei->rect.x <= p->x &&
    //         ei->rect.x + ei->rect.width >= p->x &&
    //         ei->rect.y >= p->y &&
    //         ei->rect.y <= p->y + player->speed*delta)
    //     {
    //         hitObstacle = 1;
    //         player->speed = 0.0f;
    //         p->y = ei->rect.y;
    //     }
    // }

    // if (!hitObstacle)
    // {
    //     player->position.y += player->speed*delta;
    //     player->speed += G*delta;
    //     player->canJump = false;
    // }
    // else player->canJump = true;

    
}

// void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
// {
//     if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
//     if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
//     if (IsKeyDown(KEY_SPACE) && player->canJump)
//     {
//         player->speed = -PLAYER_JUMP_SPD;
//         player->canJump = false;
//     }

//     int hitObstacle = 0;
//     for (int i = 0; i < envItemsLength; i++)
//     {
//         EnvItem *ei = envItems + i;
//         Vector2 *p = &(player->position);
//         if (ei->blocking &&
//             ei->rect.x <= p->x &&
//             ei->rect.x + ei->rect.width >= p->x &&
//             ei->rect.y >= p->y &&
//             ei->rect.y <= p->y + player->speed*delta)
//         {
//             hitObstacle = 1;
//             player->speed = 0.0f;
//             p->y = ei->rect.y;
//         }
//     }

//     if (!hitObstacle)
//     {
//         player->position.y += player->speed*delta;
//         player->speed += G*delta;
//         player->canJump = false;
//     }
//     else player->canJump = true;
// }

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    camera->target = player->position;
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = player->position;
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);

    if (max.x < width) camera->offset.x = width - (max.x - width/2);
    if (max.y < height) camera->offset.y = height - (max.y - height/2);
    if (min.x > 0) camera->offset.x = width/2 - min.x;
    if (min.y > 0) camera->offset.y = height/2 - min.y;
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    camera->target.x = player->position.x;

    if (eveningOut)
    {
        if (evenOutTarget > camera->target.y)
        {
            camera->target.y += evenOutSpeed*delta;

            if (camera->target.y > evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
        else
        {
            camera->target.y -= evenOutSpeed*delta;

            if (camera->target.y < evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
    }
    else
    {
        if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
        {
            eveningOut = 1;
            evenOutTarget = player->position.y;
        }
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*width, (1 - bbox.y)*0.5f*height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*width, (1 + bbox.y)*0.5f*height }, *camera);
    camera->offset = (Vector2){ (1 - bbox.x)*0.5f * width, (1 - bbox.y)*0.5f*height };

    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}