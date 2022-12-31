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
    bool haveGravity;
    float speed;

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
void UpdateRect(objectRect *objectR,float delta, int rectID);
void UpdateCameraCenter(Camera2D *camera, Player *player,float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, float delta, int width, int height);
void UpdateCameraFree(Camera2D *camera,Player *player, float delta,int width, int height);
void addObjectRect(objectRect *objectR);
void addObjectCirc(objectCirc *objectC);
void deleteObjectRect(objectRect *objectR, int deleteID);
void deleteObjectCirc(objectCirc *objectC,int deleteID);

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
    
    bool sound1 = false;
    bool sound2 = false;
    bool sound3 = false;
    bool sound4 = false;
    

    Player player = { 0 };
    player.rect = (Rectangle){400,280,20,20};

    player.speed = 0;
    player.canJump = false;
   
    objectRect objectR[TOTAL_OBJECT];
    objectCirc objectC[TOTAL_OBJECT];

    //selection variable
    bool isSelecting = false;
    bool isOpenProperty = false;
    bool isHelpOpen = false;
    int selectedEdit = 0;
    int selectedMove = 0;
    int selectedTypeEdit; //0-Rectangle  1-Circle
    int selectedTypeMove;
   

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
        objectR[i].speed = 0;
        objectR[i].haveGravity = false;
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
    objectR[0].rect = (Rectangle){100,500,2000,100};
    objectR[0].isExist = true;
    objectR[0].isSelected = false;
    objectR[0].canBeEdited = false;

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

    int cameraOption = 3;
    
    char *cameraDescriptions[] = {
        "Follow Player",
        "Follow Player Smooth",
        "Follow Player Vertical",
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

        if (IsKeyPressed(KEY_C))
        {
            if(cameraOption <3)
            {
                cameraOption ++;;
            }
            else
            {
                cameraOption = 0;   
            }
        } 
        
        if (IsKeyDown (KEY_ONE)) 
        {
            sound1 = true;
            sound2 = false;
            sound3 = false;
            sound4 = false;
        }
        
        if (IsKeyDown (KEY_TWO)) 
        {
            sound2 = true;
            sound1 = false;
            sound3 = false;
            sound4 = false;
        }
        
        if (IsKeyDown (KEY_THREE)) 
        {
            sound3 = true;
            sound1 = false;
            sound2 = false;
            sound4 = false;
        }
        
        if (IsKeyDown (KEY_FOUR)) 
        {
            sound3 = false;
            sound1 = false;
            sound2 = false;
            sound4 = true;
        }
        
        if (sound1 == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (fxWav);
            }
        }
        
        if(sound2 == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (boing);
            }
        }
        
        if(sound3 == true)
        {
            if(IsKeyDown (KEY_SPACE))
            {
                PlaySound (biong);
            }
        }
        
        if(sound4 == true)
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

        // Get True Mouse Position
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
                            selectedTypeMove = 0;
                        } 
                    }

                    //Collision Circle
                    else if(objectC[i].isExist)
                    {
                        if(CheckCollisionPointCircle(mousePosition, objectC[i].center, objectC[i].radius) )
                        {
                            selectedMove = i;
                            isSelecting = true; 
                            selectedTypeMove = 1;
                        } 
                    }
                }
            }
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            isSelecting = false;     
            objectR[selectedMove].isSelected = false;  
            objectC[selectedMove].isSelected = false;  
        }

        //Moving highlighted object
        if(isSelecting && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
        {
            if(selectedTypeMove == 0)
            {
                objectR[selectedMove].rect.x = mousePosition.x - objectR[selectedMove].rect.width/2  ;
                objectR[selectedMove].rect.y = mousePosition.y - objectR[selectedMove].rect.height/2 ;

            }
            else if (selectedTypeMove == 1)
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
                        selectedTypeEdit = 0;
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
                        selectedTypeEdit = 1;
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
            if(selectedTypeEdit == 0 && objectR[selectedEdit].canBeEdited)
            {
                deleteObjectRect(objectR,selectedEdit);
                isOpenProperty = false;
            }
            else if(selectedTypeEdit == 1)
            {
                deleteObjectCirc(objectC,selectedEdit);
                isOpenProperty = false;
            }
            
        }
        
        //Update Rect
        for(int i = 0; i <TOTAL_OBJECT ; i++ )
        {
            if(objectR[i].isExist && objectR[i].haveGravity )
            {
                UpdateRect(objectR,deltaTime,i);
            }
        
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


                DrawCircle((GetMousePosition().x - camera.offset.x)/camera.zoom + camera.target.x ,(GetMousePosition().y - camera.offset.y)/camera.zoom +camera.target.y,5,RED);

                DrawRectangleRec(player.rect, RED);
            
           EndMode2D();

            //GUI
            if(GuiButton((Rectangle){ 0, 0, 105, 40 }, GuiIconText(ICON_HELP, "Controls")))
            {
                if(isHelpOpen)
                {
                    isHelpOpen = false;
                }
                else if(!isHelpOpen)
                {
                    isHelpOpen = true;
                }

            }

            if(isHelpOpen)
            {
                DrawRectangle(0, 40, GetScreenWidth()/4, GetScreenHeight()/3 + 260, Fade(GRAY, 0.3f));

                DrawText("Character:", 5, 55, 25, DARKBLUE);
                DrawText("[<-][->]    - Move Left/Right", 5, 85, 20, BLUE);
                DrawText("[SPACE]  - Jump", 5, 110, 20, BLUE);
                DrawText("[1][2][3][4] - Change Sound", 5, 135, 20, BLUE);

                DrawText("Camera:", 5, 175, 25, PURPLE);
                DrawText("[C]  - Change Camera ", 5, 205, 20, DARKPURPLE);
                DrawText("[W][A][S][D]- Move Free Camera ", 5, 235, 20, DARKPURPLE);
                DrawText("[Mouse Wheel] - Adjust Zoom ", 5, 265, 20, DARKPURPLE);

                DrawText("MISC:", 5, 305, 25, DARKBROWN);
                DrawText("[MOUSE RIGHT]- Select Object  ", 5, 335, 20, BROWN);
                DrawText("[MOUSE LEFT]- Move Object ", 5, 365, 20, BROWN);
                DrawText("[Q] - Add Rectangle ", 5, 395, 20, BROWN);
                DrawText("[E] - Add Circle ", 5, 425, 20, BROWN);
                DrawText("[DELETE] - Delete Object ", 5, 455, 20, BROWN);
                DrawText("[R] - Reset ", 5, 485, 20, BROWN);
                DrawText("[Q] - Add Rectangle ", 5, 515, 20, BROWN);
            }
            
            
            DrawText("Control Panels", 1000, 500, 30, BLACK);
            DrawText("Sounds:", 1000, 550, 20, BLACK);
            DrawText("1", 1100, 550, 20, DARKGRAY);
            DrawText("2", 1120, 550, 20, DARKGRAY);
            DrawText("3", 1140, 550, 20, DARKGRAY);
            DrawText("4", 1160, 550, 20, DARKGRAY);

            DrawText("Camera:", 1000, 580, 20, BLACK);
            DrawText(cameraDescriptions[cameraOption], 1100, 583, 15, DARKGRAY);

            if (sound1)
            {
                DrawText("1", 1100, 550, 20, RED);
            }
            else if (sound2)
            {
                DrawText("2", 1120, 550, 20, RED);
            }
            else if (sound3)
            {
                DrawText("3", 1140, 550, 20, RED);
            }
            else if (sound4)
            {
                DrawText("4", 1160, 550, 20, RED);
            }

            DrawRectangle(GetScreenWidth() -  GetScreenWidth()/4, GetScreenHeight() - GetScreenHeight()/3, GetScreenWidth()/4, GetScreenHeight()/3, Fade(GRAY, 0.3f));
            if (GuiButton((Rectangle){ 1060, 630, 105, 30 }, GuiIconText(ICON_FILE_ADD, "ADD Rectangle")))
            {
                addObjectRect(objectR);
            }
            if (GuiButton((Rectangle){ 1060, 670, 105, 30 }, GuiIconText(ICON_FILE_ADD, "ADD Circle")))
            {
                addObjectCirc(objectC);
            }
            if(isOpenProperty)
            {
                DrawRectangle(GetScreenWidth() -  GetScreenWidth()/4, 0, GetScreenWidth()/4, 2*GetScreenHeight()/3, Fade(WHITE, 0.3));
                
                DrawText("Edit Properties", 1000, 30, 30, BLACK);
                DrawText("Colours", 1080, 80, 20, BLACK);
                

                //Set Color
                //Rectangle Properties
                if(selectedTypeEdit == 0)
                {
                    DrawText("Colours", 1080, 80, 20, BLACK);
                   //parametersBox color R
                    objectR[selectedEdit].color.r = (int)GuiSliderBar((Rectangle){ 1025, 100, 200, 20 }, "Red", NULL,  objectR[selectedEdit].color.r, 0, 255);
                
                    //parametersBox color G 
                    objectR[selectedEdit].color.g = (int)GuiSliderBar((Rectangle){ 1025, 140, 200, 20 }, "Green", NULL, objectR[selectedEdit].color.g, 0, 255);
                    
                    //parametersBox color B 
                    objectR[selectedEdit].color.b = (int)GuiSliderBar((Rectangle){ 1025, 180, 200, 20 }, "Blue", NULL, objectR[selectedEdit].color.b, 0, 255);

                    
                    //Change Size
                    DrawText("Size", 1100, 250, 20, BLACK);
                    objectR[selectedEdit].rect.width = (int)GuiSliderBar((Rectangle){ 1025, 270, 200, 20 }, "Width", NULL, objectR[selectedEdit].rect.width, 0, 1000);
                    objectR[selectedEdit].rect.height = (int)GuiSliderBar((Rectangle){ 1025, 310, 200, 20 }, "Height", NULL, objectR[selectedEdit].rect.height, 0, 1000);
                    
                    objectR[selectedEdit].haveGravity =  GuiCheckBox((Rectangle){ 1025, 340, 20, 20 }, "Enable Gravity", objectR[selectedEdit].haveGravity);

                    if (GuiButton((Rectangle){ 1080, 380, 105, 30 }, GuiIconText(ICON_BIN, "Delete Object")))
                    {
                        deleteObjectRect(objectR,selectedEdit);
                        isOpenProperty = false;
                    }
                }
                
                else if (selectedTypeEdit == 1)
                {
                    
                    DrawText("Colours", 1080, 80, 20, BLACK);

                    //parametersBox color R
                    objectC[selectedEdit].color.r = (int)GuiSliderBar((Rectangle){ 1025, 100, 200, 20 }, "Red", NULL,  objectC[selectedEdit].color.r, 0, 255);

                    //parametersBox color G 
                    objectC[selectedEdit].color.g = (int)GuiSliderBar((Rectangle){ 1025, 140, 200, 20 }, "Green", NULL, objectC[selectedEdit].color.g, 0, 255);

                    //parametersBox color B 
                    objectC[selectedEdit].color.b = (int)GuiSliderBar((Rectangle){ 1025, 180, 200, 20 }, "Blue", NULL, objectC[selectedEdit].color.b, 0, 255);

                    
                    // //Change Size
                    DrawText("Size", 1100, 250, 20, BLACK);
                    objectC[selectedEdit].radius = (int)GuiSliderBar((Rectangle){ 1025, 270, 200, 20 }, "Radius", NULL, objectC[selectedEdit].radius, 0, 500);

                    
                    if (GuiButton((Rectangle){ 1080, 380, 105, 30 }, GuiIconText(ICON_BIN, "Delete Object")))
                    {
                        deleteObjectCirc(objectC,selectedEdit);
                        isOpenProperty = false;
                    }

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
            oi->rect.x = 100;
            oi->rect.x = 100;
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

void deleteObjectCirc(objectCirc *objectC,int deleteID)
{
    objectC[deleteID].center = (Vector2){0,0};
    objectC[deleteID].radius = 0;
    objectC[deleteID].color = WHITE;
    objectC[deleteID].isExist = false;
    objectC[deleteID].isSelected = false;    
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

            //Left Collision
            else if(oi->rect.x > player->rect.x)
            {
                player->rect.x = oi->rect.x - player->rect.width;   
            }
         
            //Right Collision
            else if(oi->rect.x - oi->rect.width < player->rect.x)
            {
                player->rect.x = (oi->rect.x + oi->rect.width) ;
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
}

void UpdateRect(objectRect *objectR, float delta ,int rectID)
{
    bool hitObstacle = false;

    for (int i = 0; i < TOTAL_OBJECT; i++)
    {
        objectRect *oi = objectR + i;
        if(CheckCollisionRecs(oi->rect,objectR[rectID].rect))
        {
            //Up Collision
            if(oi->rect.y > objectR[rectID].rect.y)
            {
                objectR[rectID].rect.y = oi->rect.y- objectR[rectID].rect.height;
                
                hitObstacle = true;
                objectR[rectID].speed = 0;
            }
        }
    } 
    
    if (!hitObstacle)
    {
        objectR[rectID].rect.y +=  objectR[rectID].speed*delta;
        objectR[rectID].speed += G*delta;    
    }
}

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
