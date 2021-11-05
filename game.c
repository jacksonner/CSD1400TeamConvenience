#include <stdio.h>
#include "cprocessing.h"


#define MAP_GRID_COLS 5
#define MAP_GRID_ROWS 5

#define BLOCK_INVISIBLE 6
#define BLOCK_SPAWN 5
#define BLOCK_ENEMY 3
#define BLOCK_END 2
#define BLOCK_PRESENT 1
#define BLOCK_EMPTY 0

#define WINDOW_WIDTH 800

#define TRUE 1
#define FALSE 0

#define COLOR_BLACK CP_Color_Create(0, 0, 0, 255)
#define COLOR_GREY CP_Color_Create(70, 70, 70, 255)
#define COLOR_WHITE CP_Color_Create(255, 255, 255, 255)
#define COLOR_RED CP_Color_Create(255, 0, 0, 255)
#define COLOR_GREEN CP_Color_Create(0, 255, 0, 255)
#define COLOR_BLUE CP_Color_Create(0, 0, 255, 255)
//#define COLOR_WHATEVER CP_Color_Create(25, 20, 100, 255)

/*
#define Grid_One 0
#define Grid_Two 1
int reference_grid;
int render_grid;
*/

#define X 0
#define Y 1
#define MINION_TYPE 2
#define MINION_HP 3
#define MINION_SPEED 4
#define MINION_ATTACK 5
#define MINION_MAX 7
float gMinion[MINION_MAX][6]; //array to keep track of minions
float travel_dist[MINION_MAX]; //array to keep track of dist travelled so it'll all be set distances
float gProjectile[3][2];

#define MINION_A 0
#define MINION_B 1
#define MINION_C 2


#define STOP 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4
int Current_Direction[MINION_MAX]; //array to keep track of the current direction of each minion
int Past_Direction[MINION_MAX];

int gIsPaused;
int gGrids[MAP_GRID_ROWS][MAP_GRID_COLS];

float Block_Size; //length of block square
float Enemy_Size;
float gBlockPositionX;
float gBlockPositionY;
float gEnemyPositionX;
float gEnemyPositionY;
float gMinionPositionX;
float gMinionPositionY;
float pInitialX, pInitialY;
float pDistX, pDistY;
float pNewDistX, pNewDistY;
int t_time = 0;
char buffer[60];
int level_timer = 60;
float test;
float elapsed_timer;



float pX;
float pY;
float gPauseButtonTextPositionX, gPauseButtonTextPositionY;
float gTimerPositionX, gTimerPositionY, gTimerButtonTextPositionX, gTimerButtonTextPositionY;


int minion_count;

void render_bg(void);
void render_enemy(void);
void level_1(void);
void render_minion(void);
void move_minion(void);
void check_minion_type(void);

void fire_projectile(void);
void check_distance(void);
void create_projectile(float enemyStartX, float enemyStartY);
void render_basic_projectile(float pX, float pY);
void move_projectile(int l_time);

int decide_direction(int i, float x_coordinate, float y_coordinate);
int check_up(int i, int row, int col);
int check_down(int i, int row, int col);
int check_left(int i, int row, int col);
int check_right(int i, int row, int col);

void start_timer(void);

CP_Color text_color;

float currentElapsedTime;
static float totalElapsedTime;

/* Feel free to declare your own variables here */

void game_init(void)
{
    CP_Font_Set(CP_Font_GetDefault());

    gPauseButtonTextPositionX = 10.f;
    gPauseButtonTextPositionY = 10.f;

    gTimerPositionX = gPauseButtonTextPositionX + 40.f;
    gTimerPositionY = 10.f;
    gTimerButtonTextPositionX = gTimerPositionX + 5.f;
    gTimerButtonTextPositionY = gTimerPositionY + 20.f;



    text_color = CP_Color_Create(0, 0, 0, 255);

    /* Set every grids' block to empty -- empty map*/
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            gGrids[row][col] = BLOCK_EMPTY;
        }
    }

    for (int i = 0; i < MINION_MAX; ++i) {
        travel_dist[i] = 0;
        Current_Direction[i] = 1;
    }


    /* We start unpaused */
    gIsPaused = FALSE;

    /*Set up Window*/
    CP_System_SetWindowSize(WINDOW_WIDTH, WINDOW_WIDTH);
    CP_Graphics_ClearBackground(COLOR_GREY);

    Block_Size = (float)CP_System_GetWindowWidth() / MAP_GRID_COLS;
    Enemy_Size = Block_Size * 0.5f; //probably need to be changed depending on what enemy
    CP_Settings_StrokeWeight(0.6f);

    /* set framerate to 2 fps (slow)*/
    CP_System_SetFrameRate(60.0f);


    /* get dt, time elapsed from last frame*/
    currentElapsedTime = CP_System_GetDt();

    /*updating total elapsed time*/
    totalElapsedTime = 0;
    totalElapsedTime += currentElapsedTime;

    

}

void game_update(void) {
    
    float pauseButtonLimitX = gPauseButtonTextPositionX + 50.f;
    float pauseButtonLimitY = gPauseButtonTextPositionY + 50.f;

    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
    {
        if (((CP_Input_GetMouseX() >= gPauseButtonTextPositionX) && (CP_Input_GetMouseX() <= pauseButtonLimitX)) &&
            ((CP_Input_GetMouseY() >= gPauseButtonTextPositionY) && (CP_Input_GetMouseY() <= pauseButtonLimitY)))
        {
            gIsPaused = !gIsPaused;
        }
    }
    if (gIsPaused == TRUE)
    {
        level_1();
        render_bg();
        render_enemy();
        for (int i = 0; i < 3; i++)
        {
            render_basic_projectile(gProjectile[1][X], gProjectile[1][Y]);
        }
        for (int i = 0; i < minion_count; i++)
        {
            check_minion_type();
            CP_Graphics_DrawCircle(gMinion[i][X], gMinion[i][Y], Enemy_Size);

        }
        
    }
    else {
        level_1(); //probably add something for like choose level which will then toggle between the levels before rendering
        render_bg();
        render_enemy();
        fire_projectile();
        check_distance();
        

        if (minion_count > 0)
        {
            create_projectile(gEnemyPositionX, gEnemyPositionY);
            
            if (gProjectile[1][X] > gMinion[0][X])
            {
                
            }
            else
            {
                move_projectile(t_time);
                render_basic_projectile(gProjectile[1][X], gProjectile[1][Y]);
                t_time = t_time + 1;
                test = CP_System_GetDt();
                start_timer();
                snprintf(buffer, sizeof(buffer), "%d", (60 - (int)elapsed_timer));
            }
            
            printf("Min : %f, %f\n", gMinion[0][X], gMinion[0][Y]);
            printf("pX: %f, %f\n", gProjectile[1][X], gProjectile[1][Y]);
            
        }


        if (CP_Input_KeyTriggered(KEY_1)) { //KEY1 for minionA
            if (minion_count < 7) {
                gMinion[minion_count][MINION_TYPE] = MINION_A;
                render_minion();

            }
        }
        else if (CP_Input_KeyTriggered(KEY_2)) { //KEY2 for minionB
            if (minion_count < 7) {
                gMinion[minion_count][MINION_TYPE] = MINION_B;
                render_minion();
            }
        }
        else if (CP_Input_KeyTriggered(KEY_3)) { //KEY3 for minionC
            if (minion_count < 7) {
                gMinion[minion_count][MINION_TYPE] = MINION_C;
                render_minion();
            }
        }
        /*else if (CP_Input_KeyTriggered(KEY_RIGHT)) { //Key to move projectile

            t_time = t_time + 1;
        }*/

        else { //nothing happens, ideally text will let you know you have too many minions out
        }

        if (minion_count > 0) {

            move_minion();

        }
        
    }
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(gPauseButtonTextPositionX, gPauseButtonTextPositionY, 30.f, 30.f);
    CP_Graphics_DrawRect(gTimerPositionX, gTimerPositionY, 50.f, 30.f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    
    //t = clock();
    //snprintf(buffer, sizeof(buffer), "%d", (level_timer - (t/(CLOCKS_PER_SEC))));
    CP_Font_DrawText(buffer, gTimerButtonTextPositionX, gTimerButtonTextPositionY);
    
}




void start_timer()
{
    elapsed_timer += test;
}

void render_bg() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            gBlockPositionX = Block_Size * (float)row;
            gBlockPositionY = Block_Size * (float)col;
            CP_Settings_Fill(gGrids[row][col] == BLOCK_EMPTY //ternary operator
                ? COLOR_GREY
                : gGrids[row][col] == BLOCK_END //add something to include text to show that this is the end point
                ? COLOR_BLACK
                : gGrids[row][col] == BLOCK_PRESENT
                ? COLOR_WHITE
                : gGrids[row][col] == BLOCK_SPAWN
                ? COLOR_GREEN
                : gGrids[row][col] == BLOCK_INVISIBLE
                ? COLOR_GREY
                : COLOR_GREY); //BLOCK_ENEMY
            CP_Graphics_DrawRect(gBlockPositionX, gBlockPositionY, Block_Size, Block_Size);


        }
    }
}

void render_enemy() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) { //when changed all the enemy things to array, you'll need a for loop to go through each array
            if (gGrids[row][col] == BLOCK_ENEMY) {
                CP_Settings_Fill(COLOR_RED);
                CP_Settings_RectMode(CP_POSITION_CENTER);
                gEnemyPositionX = (Block_Size * (float)row) + Enemy_Size; //all the current gPostionX etc. need to be changed into arrays
                gEnemyPositionY = (Block_Size * (float)col) + Enemy_Size;
                pNewDistX = (Block_Size * (float)(row+1)) + Enemy_Size;
                pNewDistY = (Block_Size * (float)(col)) + Enemy_Size;
                CP_Graphics_DrawRect(gEnemyPositionX, gEnemyPositionY, Enemy_Size, Enemy_Size);
            }
        }
    }
    CP_Settings_RectMode(CP_POSITION_CORNER);
}

void fire_projectile() {
    
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (gGrids[row][col] == BLOCK_ENEMY) {
                CP_Settings_Fill(COLOR_GREEN);
                pInitialX = (Block_Size * (float)row) + Enemy_Size;
                pInitialY = (Block_Size * (float)col) + Enemy_Size;
                gProjectile[0][X] = pInitialX;
                gProjectile[0][Y] = pInitialY;
            }
        }
    }
}

void check_distance()
{
    pDistX = gMinion[0][X] - gProjectile[0][X];
    pDistY = gMinion[0][Y] - gProjectile[0][Y];
    

}


void create_projectile(float enemyStartX, float enemyStartY)
{
    gProjectile[1][X] = enemyStartX;
    gProjectile[1][Y] = enemyStartY;
}
void move_projectile(int l_time)
{ 
    float vectorX = gMinion[0][X];
    float vectorY = gMinion[0][Y];
    
    gProjectile[1][X] = gProjectile[1][X] + (l_time * ((vectorX- gProjectile[1][X])/30));
    gProjectile[1][Y] = gProjectile[1][Y] + (l_time * ((vectorY- gProjectile[1][Y]) /30));
}
void render_basic_projectile(float prX, float prY)
{
    CP_Graphics_DrawRect(prX, prY, (Enemy_Size * 0.4f), (Enemy_Size * 0.2f));
}




void render_minion() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (gGrids[row][col] == BLOCK_SPAWN) {
                float SpawnX = (Block_Size * (float)row) + Enemy_Size; //using enemy size cause lazy
                float SpawnY = (Block_Size * (float)col) + Enemy_Size; //makes it spawn in the middle
                decide_direction(minion_count, SpawnX, SpawnY);        
                CP_Settings_Fill(COLOR_BLUE);              
                if (minion_count < 7) {
                    gMinion[minion_count][X] = SpawnX;
                    gMinion[minion_count][Y] = SpawnY;
                    CP_Graphics_DrawCircle(gMinion[minion_count][X], gMinion[minion_count][Y], Enemy_Size);
                }
                else if (minion_count >= 7) {
                    minion_count = 7;
                }
                ++minion_count;
                printf("%d", minion_count);
            }
        }
    }
}

void current_minion() {
    for (int i = 0; i < minion_count; i++) {
        if (gMinion[i][MINION_TYPE] == MINION_A) {
            CP_Settings_Fill(COLOR_BLUE);
        }
        else if (gMinion[i][MINION_TYPE] == MINION_B) {
            CP_Settings_Fill(COLOR_RED);
        }
    }
}


void move_minion() {
    float minion_speed, move_left, move_right, move_up, move_down;
    minion_speed = 4.0f; //should probably declare it in global
    for (int i = 0; i < minion_count; i++) {
        move_left = gMinion[i][X] - minion_speed;
        move_right = gMinion[i][X] + minion_speed;
        move_up = gMinion[i][Y] - minion_speed;
        move_down = gMinion[i][Y] + minion_speed;
        travel_dist[i] = travel_dist[i] + minion_speed;
        if (travel_dist[i] <= Block_Size) {
            Current_Direction[i] = Current_Direction[i];
        }
        else {
            travel_dist[i] = 0;
            Current_Direction[i] = decide_direction(i, gMinion[i][X], gMinion[i][Y]);
        }
        if (Current_Direction[i] == UP || Current_Direction[i] == DOWN) {
            gMinion[i][X] = gMinion[i][X];
            gMinion[i][Y] = (Current_Direction[i] == UP
                ? move_up
                : move_down);
        }
        else if (Current_Direction[i] == LEFT || Current_Direction[i] == RIGHT) {
            gMinion[i][Y] = gMinion[i][Y];
            gMinion[i][X] = (Current_Direction[i] == LEFT
                ? move_left
                : move_right);
        }
        else if (Current_Direction[i] == STOP) {
            gMinion[i][X] = gMinion[i][X];
            gMinion[i][Y] = gMinion[i][Y];
            decide_direction(i, gMinion[i][X], gMinion[i][Y]);
        }
    
      
        CP_Graphics_DrawCircle(gMinion[i][X], gMinion[i][Y], Enemy_Size);
    }
    check_minion_type();
}

void check_minion_type() {
    for (int i = 0; i < minion_count; i++) {
        if (gMinion[i][MINION_TYPE] == MINION_A) {
            CP_Settings_Fill(COLOR_BLUE);
        }
        else if (gMinion[i][MINION_TYPE] == MINION_B) {
            CP_Settings_Fill(COLOR_RED);
        }
        else if (gMinion[i][MINION_TYPE] == MINION_C) {
            CP_Settings_Fill(COLOR_GREEN);
        }

        CP_Graphics_DrawCircle(gMinion[i][X], gMinion[i][Y], Enemy_Size);
    }
}



int decide_direction(int i, float x_coordinate, float y_coordinate) {
    float x = x_coordinate / Block_Size;
    float y = y_coordinate / Block_Size;
    int row = (int)x;
    int col = (int)y; //find out which box we're located in and then do checks?
    Past_Direction[i] = Current_Direction[i];
    if (Current_Direction[i] == UP) {  //3
        Current_Direction[i] = (check_left(i, row, col) == LEFT
            ? LEFT
            : check_right(i, row, col) == RIGHT
            ? RIGHT
            : check_up(i, row, col) == UP
            ? UP
            : STOP);
    }
    else if (Current_Direction[i] == DOWN) { //4
        Current_Direction[i] = (check_left(i, row, col) == LEFT
            ? LEFT
            : check_right(i, row, col) == RIGHT
            ? RIGHT
            : check_down(i, row, col) == DOWN
            ? DOWN
            : STOP);
    }
    else if (Current_Direction[i] == LEFT) { //1
        Current_Direction[i] = (check_left(i, row, col) == LEFT
            ? LEFT
            : check_up(i, row, col) == UP
            ? UP
            : check_down(i, row, col) == DOWN
            ? DOWN
            : STOP);
    }
    else if (Current_Direction[i] == RIGHT) { //2
        Current_Direction[i] = (check_right(i, row, col) == RIGHT
            ? RIGHT
            : check_up(i, row, col) == UP
            ? UP
            : check_down(i, row, col) == DOWN
            ? DOWN
            : STOP);
    }
    else if (Current_Direction[i] == STOP) { //2
        Current_Direction[i] = Past_Direction[i];
    }

    return Current_Direction[i];
}

int check_up(int i, int row, int col) {
    int surrounding_x, surrounding_y;
    if ((surrounding_x = row) >= 0 && (surrounding_y = col - 1) >= 0) {
        if (gGrids[surrounding_x][surrounding_y] == BLOCK_EMPTY
            || gGrids[surrounding_x][surrounding_y] == BLOCK_END) {   //UP have no block
            Current_Direction[i] = UP;
        }
        else {
            Current_Direction[i] = STOP;
        }
    }
    else {
        Current_Direction[i] = STOP;
    }
    return Current_Direction[i];
}

int check_down(int i, int row, int col) {
    int surrounding_x, surrounding_y;
    if ((surrounding_x = row) >= 0 && (surrounding_y = col + 1) < MAP_GRID_COLS) {
        if (gGrids[surrounding_x][surrounding_y] == BLOCK_EMPTY
            || gGrids[surrounding_x][surrounding_y] == BLOCK_END) {   //DOWN have no block
            Current_Direction[i] = DOWN;
        }
        else {
            Current_Direction[i] = STOP;
        }
    }
    else {
        Current_Direction[i] = STOP;
    }
    return Current_Direction[i];
}

int check_left(int i, int row, int col) {
    int surrounding_x, surrounding_y;
    if ((surrounding_x = row - 1) >= 0 && (surrounding_y = col) >= 0) {
        if (gGrids[surrounding_x][surrounding_y] == BLOCK_EMPTY
            || gGrids[surrounding_x][surrounding_y] == BLOCK_END) {   //LEFT have no block
            Current_Direction[i] = LEFT;
        }
        else {
            Current_Direction[i] = STOP;
        }
    }
    else {
        Current_Direction[i] = STOP;
    }
    return Current_Direction[i];
}

int check_right(int i, int row, int col) {
    int surrounding_x, surrounding_y;
    if ((surrounding_x = row + 1) < MAP_GRID_ROWS && (surrounding_y = col) >= 0) {
        if (gGrids[surrounding_x][surrounding_y] == BLOCK_EMPTY
            || gGrids[surrounding_x][surrounding_y] == BLOCK_END) {   //RIGHT have no block
            Current_Direction[i] = RIGHT;
        }
        else {
            Current_Direction[i] = STOP;
        }
    }
    else {
        Current_Direction[i] = STOP;

    }
    return Current_Direction[i];
}

void level_1() {
    gGrids[0][0] = BLOCK_INVISIBLE;
    gGrids[0][1] = BLOCK_INVISIBLE;
    gGrids[0][2] = BLOCK_INVISIBLE; //cause aesthetics
    gGrids[0][3] = BLOCK_PRESENT;
    gGrids[0][4] = BLOCK_PRESENT;
    gGrids[1][3] = BLOCK_PRESENT;
    gGrids[1][4] = BLOCK_PRESENT;
    gGrids[2][1] = BLOCK_PRESENT;
    gGrids[3][1] = BLOCK_PRESENT;
    gGrids[3][2] = BLOCK_PRESENT;
    gGrids[3][3] = BLOCK_PRESENT;
    gGrids[4][3] = BLOCK_PRESENT;
    gGrids[4][2] = BLOCK_END;
    gGrids[1][2] = BLOCK_ENEMY;
    gGrids[4][4] = BLOCK_SPAWN;
}

void game_exit(void)
{

}