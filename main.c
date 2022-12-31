#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"


#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define TOTAL_OBJECT 20


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

typedef struct objectCirc
{
    Vector2 center;
    float radius;
    Color color;
    bool isExist;
    bool isSelected;

}objectCirc;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdatePlayer(Player *player, objectRect *objectR,float delta);
void UpdateCameraCenter(Camera2D *camera, Player *player,float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, float delta, int width, int height);
void UpdateCameraFree(Camera2D *camera,Player *player, float delta,int width, int height);
void addObjectRect(objectRect *objectR);
void addObjectCirc(objectCirc *objectC);
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
    

    Player player = { 0 };
    player.rect = (Rectangle){400,280,20,20};

    player.speed = 0;
    player.canJump = false;
   
    objectRect objectR[TOTAL_OBJECT];
    objectCirc objectC[TOTAL_OBJECT];

    //selection variable
    bool isSelecting = false;
    bool isOpenProperty = false;
    int selectedEdit = 0;
    int selectedMove = 0;
    int selectedType; //0-Rectangle  1-Circle
   

    //initialize objectR array

    for(int i = 0; i < TOTAL_OBJECT; i++)
    {
        objectR[i].rect.height = 0;
        objectR[i].rect.width = 0;
        objectR[i].rect.x = 0;
        objectR[i].rect.y = 0;

        objectR[i].color = WHITE;
        objectR[i].isExist = false;
        objectR[i].canBeEdited = true;
    }

    //initialize ObjectC array
    for(int i = 0; i < TOTAL_OBJECT; i++)
    {
        objectC[i].center = (Vector2){0,0};
        objectC[i].radius = 0;
        objectC[i].color = WHITE;
        objectC[i].isExist = false;
        objectC[i].isSelected = false;
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


    //Initialize Camera
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
        UpdateCameraFree,
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center; smoothed",
        "Follow player center horizontally; updateplayer center vertically after landing",
        "Free Camera"
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


        //Camera Zoom
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
        if (IsKeyPressed(KEY_Q))
        {
            addObjectCirc(objectC);
        }


        Vector2 mousePosition;
        mousePosition.x = (GetMousePosition().x - camera.offset.x)/camera.zoom + camera.target.x;
        mousePosition.y = (GetMousePosition().y - camera.offset.y)/camera.zoom + camera.target.y;
       
        
        //Highlight Object to drag
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if(!isSelecting)
            {
                for(int i = 0; i <TOTAL_OBJECT; i++ )
                {
                    //Collision Rectangle
                    if(objectR[i].isExist && objectR[i].canBeEdited)
                    {
                        if(CheckCollisionPointRec(mousePosition, objectR[i].rect) )
                        {
                            selectedMove = i;
                            isSelecting = true;
                            selectedType = 0;
                        } 
                    }

                    //Collision Circle
                    else if(objectC[i].isExist)
                    {
                        if(CheckCollisionPointCircle(mousePosition, objectC[i].center, objectC[i].radius) )
                        {
                            selectedMove = i;
                            isSelecting = true; 
                            selectedType = 1;
                        } 
                    }
                }
            }
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSelecting)
        {
            isSelecting = false;     
            objectR[selectedMove].isSelected = false;  
            objectC[selectedMove].isSelected = false;  
        }

        //Moving highlighted object
        if(isSelecting && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
        {
            if(selectedType == 0)
            {
                objectR[selectedMove].rect.x = mousePosition.x - objectR[selectedMove].rect.width/2  ;
                objectR[selectedMove].rect.y = mousePosition.y - objectR[selectedMove].rect.height/2 ;

            }
            else if (selectedType == 1)
            {
                objectC[selectedMove].center = (Vector2){ mousePosition.x,mousePosition.y};
            } 
        }

        
        //highlight object to edit
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            isOpenProperty = false;   
            bool found = false;
            for(int i = 0; i <TOTAL_OBJECT ; i++ )
            {
                objectR[i].isSelected = false; 
                objectC[i].isSelected = false; 
                
                //Highlight Rectangle
                if(objectR[i].isExist && objectR[i].canBeEdited && !found)
                {
                    if(CheckCollisionPointRec(mousePosition, objectR[i].rect))
                    {
                        selectedEdit = i;
                        objectR[i].isSelected = true;
                        isOpenProperty = true;
                        selectedType = 0;
                        found = true;
                    }
                    else
                    {
                        objectR[i].isSelected = false; 
                    } 
                }
                //Highlight Circle
                else if(objectR[i].isExist && !found) 
                {
                    if(CheckCollisionPointCircle(mousePosition, objectC[i].center, objectC[i].radius) )
                    {
                        selectedEdit = i;
                        objectC[i].isSelected = true;
                        isOpenProperty = true;
                        selectedType = 1;
                        found = true;
                    } 
                    else
                    {
                        objectC[i].isSelected = false; 
                    } 
                }
            }       
        }

        //Delete object
        if((IsKeyPressed(KEY_DELETE)))
        {
            deleteObjectRect(objectR,selectedEdit);
        }
        
        //Call update camera function by its pointer
        cameraUpdaters[cameraOption](&camera, &player, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);
                
                for(int i = 0; i < TOTAL_OBJECT; i++)
                {
                    //Draw Rectangle
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

                    //Draw Circle
                    if(objectC[i].isExist)
                    {
                        if(objectC[i].isSelected)
                        {
                            DrawCircle(objectC[i].center.x,objectC[i].center.y,objectC[i].radius,objectC[i].color);
                            DrawCircleLines(objectC[i].center.x,objectC[i].center.y,objectC[i].radius,GREEN);
                        }
                        else
                        {
                            DrawCircle(objectC[i].center.x,objectC[i].center.y,objectC[i].radius,objectC[i].color);
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
                 
                //Rectangle Properties
                if(selectedType == 0)
                {
                   //parametersBox color R
                    objectR[selectedEdit].color.r = (int)GuiSliderBar((Rectangle){ 1000, 90, 105, 20 }, "Red", NULL,  objectR[selectedEdit].color.r, 0, 255);
                
                    //parametersBox color G 
                    objectR[selectedEdit].color.g = (int)GuiSliderBar((Rectangle){ 1000, 120, 105, 20 }, "Green", "", objectR[selectedEdit].color.g, 0, 255);
                    
                    //parametersBox color B 
                    objectR[selectedEdit].color.b = (int)GuiSliderBar((Rectangle){ 1000, 150, 105, 20 }, "Blue", "", objectR[selectedEdit].color.b, 0, 255);

                    // //Change Size
                    objectR[selectedEdit].rect.width = (int)GuiSliderBar((Rectangle){ 1000, 40, 105, 20 }, "Width", NULL, objectR[selectedEdit].rect.width, 0, 1000);
                    objectR[selectedEdit].rect.height = (int)GuiSliderBar((Rectangle){ 1000, 70, 105, 20 }, "Height", NULL, objectR[selectedEdit].rect.height, 0, 1000);
                }
                
                else if (selectedType == 1)
                {
                    //parametersBox color R
                    objectC[selectedEdit].color.r = (int)GuiSliderBar((Rectangle){ 1000, 90, 105, 20 }, "Red", NULL,  objectC[selectedEdit].color.r, 0, 255);
                
                    //parametersBox color G 
                    objectC[selectedEdit].color.g = (int)GuiSliderBar((Rectangle){ 1000, 120, 105, 20 }, "Green", "", objectC[selectedEdit].color.g, 0, 255);
                    
                    //parametersBox color B 
                    objectC[selectedEdit].color.b = (int)GuiSliderBar((Rectangle){ 1000, 150, 105, 20 }, "Blue", "", objectC[selectedEdit].color.b, 0, 255);

                    objectC[selectedEdit].radius = (int)GuiSliderBar((Rectangle){ 1000, 40, 105, 20 }, "Radius", NULL, objectC[selectedEdit].radius, 0, 1000);

                }
                
                if (GuiButton((Rectangle){ 1100, 600, 105, 20 }, GuiIconText(ICON_HAND_POINTER, "ADD Objects")))
                {
                    addObjectRect(objectR);
                }
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

    while(i < TOTAL_OBJECT && !foundEmptySlot)
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
            oi->isSelected = false;
        } 
        i++;

    }
}

void addObjectCirc(objectCirc *objectC)
{
    bool foundEmptySlot = false;
    
    int i = 0;

    while(i < TOTAL_OBJECT && !foundEmptySlot)
    {
        objectCirc *oi = objectC + i;
        if(!oi->isExist)
        {
            foundEmptySlot = true;
        }

        if(foundEmptySlot)
        {
            oi->center = (Vector2){100,100};
            oi->radius = 50;
            oi->color = WHITE;
            oi->isExist = true;
            oi->isSelected = false;        
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
    objectR[deleteID].isSelected = false;
    objectR[deleteID].color = WHITE;
}

void UpdatePlayer(Player *player, objectRect *objectR,float delta)
{
    

    if (IsKeyDown(KEY_LEFT))
    {
        player->rect.x = player->rect.x - PLAYER_HOR_SPD*delta;
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
    for (int i = 0; i < TOTAL_OBJECT; i++)
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

void UpdateCameraFree(Camera2D *camera,Player *player, float delta, int width, int height)
{
    
    int cameraSpeed = 200;
    if (IsKeyDown(KEY_W))
    {
        camera->target.y -= cameraSpeed*delta;
    } 
    if (IsKeyDown(KEY_S)) 
    {
        camera->target.y += cameraSpeed*delta;
    }
    if (IsKeyDown(KEY_A))
    {
       camera->target.x -= cameraSpeed*delta;
    }    
    if (IsKeyDown(KEY_D))
    {
       camera->target.x += cameraSpeed*delta;
    }  

}
