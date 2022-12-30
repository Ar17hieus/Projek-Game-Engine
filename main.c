#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"


#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define TOTAL_RECT 10

typedef struct Player {
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
    bool canBeEdited;

} objectRect;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdatePlayer(Player *player, objectRect *objectR,float delta);
void UpdateCameraCenter(Camera2D *camera, Player *player,float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, float delta, int width, int height);
void addObjectRect(objectRect *objectR);
void deleteObjectRect(objectRect *objectR, int deleteID);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

   
    InitWindow(screenWidth, screenHeight, "Game Engine Project");
    
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
    player.rect = (Rectangle){400,280,20,20};

    player.speed = 0;
    player.canJump = false;
   
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
        objectR[i].canBeEdited = true;
    }


    //Level Objects
    objectR[0].rect = (Rectangle){100,500,1000,100};
    objectR[0].isExist = true;
    objectR[0].isSelected = false;
    objectR[0].canBeEdited = false;

    objectR[1].rect = (Rectangle){100,0,100,500};
    objectR[1].isExist = true;
    objectR[1].isSelected = false;
    objectR[1].canBeEdited = false;

    objectR[2].rect = (Rectangle){1000,0,100,500};
    objectR[2].isExist = true;
    objectR[2].isSelected = false;
    objectR[2].canBeEdited = false;

    objectR[3].rect = (Rectangle){100,0,1000,100};
    objectR[3].isExist = true;
    objectR[3].isSelected = false;
    objectR[3].canBeEdited = false;

   
    Camera2D camera = { 0 };
    camera.target = (Vector2){player.rect.x,player.rect.y};
   
    camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f };
   
    camera.rotation = 0.0f;
    camera.zoom = 1;

    //Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D*, Player*, float, int, int) = {
        UpdateCameraCenter,
        UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding,
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center; smoothed",
        "Follow player center horizontally; updateplayer center vertically after landing",
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
            player.rect.x = 400;
            player.rect.y = 280;
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
                for(int i = 0; i <TOTAL_RECT && !foundClick; i++ )
                {
                    if(objectR[i].isExist && objectR[i].canBeEdited)
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

            for(int i = 0; i <TOTAL_RECT && !foundClick; i++ )
            {
                if(objectR[i].isExist && objectR[i].canBeEdited)
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

        //Delete object
        if((IsKeyPressed(KEY_DELETE)))
        {
            deleteObjectRect(objectR,selected);
        }
        
        //Call update camera function by its pointer
        cameraUpdaters[cameraOption](&camera, &player, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

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
                objectR[selected].rect.height = (int)GuiSliderBar((Rectangle){ 1000, 40, 105, 20 }, "Width", NULL, objectR[selected].rect.height, 0, 1000);
                objectR[selected].rect.width = (int)GuiSliderBar((Rectangle){ 1000, 70, 105, 20 }, "Height", NULL, objectR[selected].rect.width, 0, 1000);

                if (GuiButton((Rectangle){ 1100, 600, 105, 20 }, GuiIconText(ICON_HAND_POINTER, "ADD Objects")))
                {
                    addObjectRect(objectR);
                }
            }
           
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
    
    int i = 0;

    while(i < TOTAL_RECT && !foundEmptySlot)
    {
        objectRect *oi = objectR + i;
        if(!oi->isExist)
        {
            foundEmptySlot = true;
        }

        if(foundEmptySlot)
        {
            oi->isExist = true;
            oi->rect.height = 100;
            oi->rect.width = 100;
            oi->rect.x = 0;
            oi->rect.x = 0;
        } 
        i++;

    }
}

void deleteObjectRect(objectRect *objectR,int deleteID)
{
    objectR[deleteID].isExist = false;
    objectR[deleteID].rect.height = 0;
    objectR[deleteID].rect.width = 0;
    objectR[deleteID].rect.x = 0;
    objectR[deleteID].rect.x = 0;   
    objectR[deleteID].isSelected = 0;
    objectR[deleteID].color = WHITE;
}

void UpdatePlayer(Player *player, objectRect *objectR,float delta)
{
    if (IsKeyDown(KEY_LEFT))
    {
        player->rect.x -= PLAYER_HOR_SPD*delta;
    } 
    if (IsKeyDown(KEY_RIGHT)) 
    {
        player->rect.x += PLAYER_HOR_SPD*delta;
    }
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
                player->rect.y = oi->rect.y- player->rect.height;
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
                player->rect.x = oi->rect.x - player->rect.width;
                
            }
            // else if(player->rect.x < oi->rect.x + oi->rect.width)
            // {
            //     player->position.x = oi->rect.x + oi->rect.width;
            // }

            //Right Collision
            else if(oi->rect.x - oi->rect.width < player->rect.x)
            {
                player->rect.x = (oi->rect.x + oi->rect.width) + 1 ;
            }
            
            
        }
    } 
    if (!hitObstacle)
    {
        player->rect.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;

    }
    else player->canJump = true;


   // player->rect=(Rectangle){player->position.x,player->position.y,10,10};

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

void UpdateCameraCenter(Camera2D *camera, Player *player, float delta, int width, int height)
{
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    camera->target = (Vector2){player->rect.x,player->rect.y};
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    Vector2 diff = Vector2Subtract((Vector2){player->rect.x,player->rect.y}, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player,float delta, int width, int height)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    camera->target.x = player->rect.x;

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
        if (player->canJump && (player->speed == 0) && (player->rect.y != camera->target.y))
        {
            eveningOut = 1;
            evenOutTarget = player->rect.y;
        }
    }
}
