#include <stdio.h>
#include "cprocessing.h"

/*Window Dimension of Game*/
int origin_map_coordinateX; //cause I have a border around everything
int origin_map_coordinateY;
int BLOCK_SIZE; //in caps, jic people have tiny screens and block_size needs to change :(

/*Dimensions of Gameplay Map
NOTE THAT FOR X-COORDINATE REFER TO COL, Y-COORDINATE REDER TO ROW
  col 0 1 2
row 0[][][]
row 1[][][]
row 2[][][]
*/
#define MAP_GRID_COLS 12
#define MAP_GRID_ROWS 5
int array_GameMap[MAP_GRID_ROWS][MAP_GRID_COLS];
float array_Collaborative_DiffusionMap[MAP_GRID_ROWS][MAP_GRID_COLS][2];

/*Types of Blocks in Gameplay Map*/
#define BLOCK_EMPTY 0
#define BLOCK_PRESENT 1
#define BLOCK_END 2
#define BLOCK_ENEMY 3
#define BLOCK_SPAWN 4
#define BLOCK_INVISIBLE 5
#define BLOCK_ENEMY_DEAD 6

/*Different Colours in Use*/
#define COLOR_BLACK CP_Color_Create(0, 0, 0, 255)
#define COLOR_GREY CP_Color_Create(70, 70, 70, 255)
#define COLOR_WHITE CP_Color_Create(255, 255, 255, 255)
#define COLOR_RED CP_Color_Create(255, 0, 0, 255)
#define COLOR_GREEN CP_Color_Create(0, 255, 0, 255)
#define COLOR_BLUE CP_Color_Create(0, 0, 255, 255)
#define TEXT_COLOR CP_Color_Create(0, 0, 0, 255) //Text colour is just black though...?

/*Minion Stats*/
#define X 0 //x-coordinates
#define Y 1 //y-coordinates
#define MINION_TYPE 2 //is the minion a spam-type, warrior-type etc.
#define MINION_HP 3
#define MINION_MOVEMENT_SPEED 4
#define MINION_ATTACK 5
#define MINION_ATTACK_SPEED 6
#define MINION_WEIGHT 7 //used to calculate how many minions an enemy can block, some minions are considered >1
#define MINION_COST 8
#define MINION_SIZE 9 //like the radius of the minion, might be removed when actualy sprites are included?
#define MINION_DIRECTION 10
#define MINION_HEAL 11
/*can add a MINION_COLOUR too maybe?*/
#define MINION_MAX 7 //Maximum number of minions in the game at any one time
int array_MinionStats[MINION_MAX][12]; //MUST edit reset_map_and_minions() when the array size is changed

/*Types of Minions*/
#define SPAM_MINION 0 //weak everything, but low cost
#define WARRIOR_MINION 1 //decent health, decent attack
#define TANK_MINION 2 //tanky but low attack
#define WIZARD_MINION 3 //low health, high attack
#define HEALER_MINION 4 //decent health, no attack, heal other minions
#define A_MINION 5 //TBC - YC
#define B_MINION 6 //TBC - YC

/*Directions*/
#define STOP 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

/*Enemy Stats*/
#define ENEMY_ROW 0
#define ENEMY_COL 1
#define ENEMY_TYPE 2
#define ENEMY_HP 3
#define ENEMY_ATTACK 4
#define ENEMY_ATTACK_SPEED 5
#define ENEMY_BLOCK 6 //number of minions the enemy can block
#define ENEMY_SIZE 7
#define ENEMY_RANGE 8
#define ENEMY_HEAL 9
#define ENEMY_ROW_COORDINATES 10
#define ENEMY_COL_COORDINATES 11
#define ENEMY_CURRENT_MINIONS_ON_BLOCK 12

#define ENEMY_MAX 10 //How many enemies can we have at one time? 
int array_EnemyStats[ENEMY_MAX][13];

/*Used for checking if the minion will be blocked or not*/
int array_isMinionBlocked[ENEMY_MAX][MINION_MAX];

/*Types of Enemies*/
#define GUARD_ENEMY 0 //block minions
#define DAMAGE_ENEMY 1 // shorter range than tower
#define SLOW_ENEMY 2 //slow down minion
#define HEALING_TOWER 3 //heal enemies
#define RANGED_TOWER 4
#define BASE 5 //TBC
#define B_TOWER 6 //TBC

/*Gamestate*/
#define GAMEPLAY_SCREEN 0
#define MAIN_MENU_SCREEN 1
#define WIN_SCREEN 2
#define LOSE_SCREEN 3
#define PAUSE_SCREEN 4 //like a translucent array with a giant play button?
#define LEVEL_SELECTOR_SCREEN 5
int Current_Gamestate;

/**/
#define FALSE 0
#define TRUE 1
int level_has_been_reset; //checks if level has reset so stats won't constantly be reassigned making the enemies immortal

/*Counter for no. of Minions who entered the base*/
int minions_in_base;
void minion_enter_base_counter(void);
char base_counter[10];
void display_minion_eneter_base_counter(void);

/*Levels*/
void level_1(void);
//void level_2(void);
//void level_3(void);
//void level_4(void);
//void level_5(void);
//void level_6(void);
int current_level;

/*Timer and Pause button*/
int t_time;
char buffer[60];
int level_timer = 60;
float test;
float elapsed_timer;
float gPauseButtonTextPositionX, gPauseButtonTextPositionY;
float gTimerPositionX, gTimerPositionY, gTimerButtonTextPositionX, gTimerButtonTextPositionY;
float currentElapsedTime;
static float totalElapsedTime;
float pauseButtonLimitX, pauseButtonLimitY;
float pauseButtonLimitX, pauseButtonLimitY;
int gIsPaused;
void start_timer(void);
void initialise_pause_and_timer_button(void);
void draw_timer_and_pause_button(void);

/*Variables*/
int BlockPositionX;
int BlockPositionY;
int minion_count;
int spawn_row; 
int spawn_col;
void update_variables_and_make_screen_nice(); //since it's full screen, need to update the various variables so everything still looks nice

/*Main Menu Screen*/
float button_height, button_width;
float level_selectorX, level_selectorY, start_game_buttonX, start_game_buttonY;
float start_textX, start_textY, levels_textX, levels_textY;
void main_menu_screen(void);
void main_menu_clicked(float x, float y);

/*Functions*/
void reset_map_and_minions(void);
void render_background(void); //for the gameplay_screen
void gameplay_screen(void);
void initialise_level(void); //TBC
void setup_collaborative_diffusion_map(void); //ensure no backtracking
void render_minion(void);
void move_minion(void);
void assign_minion_stats(void);
void assign_enemy_stats(void);
void render_enemy(void);
void assign_minion_color(void);

/*
void check_minion_type(void);

void fire_projectile(void);
void check_distance(void);
void move_projectile(void);
*/

void game_init(void) {
    /*Setting the FrameRate to 60fps*/
    CP_System_SetFrameRate(60.0f);

    /*Initialising variables for Fullscreen etc.*/
    CP_System_Fullscreen();
    update_variables_and_make_screen_nice(); 

    /*Initialise to Main_Menu*/
    Current_Gamestate = MAIN_MENU_SCREEN;

    /* get dt, time elapsed from last frame*/
    currentElapsedTime = CP_System_GetDt();

    /*updating total elapsed time*/
    totalElapsedTime = 0;
    totalElapsedTime += currentElapsedTime;
}

void game_update(void) {
    if (Current_Gamestate == MAIN_MENU_SCREEN) {
        main_menu_screen();
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            main_menu_clicked(CP_Input_GetMouseX(), CP_Input_GetMouseY());
        }
    }
    else if (Current_Gamestate == GAMEPLAY_SCREEN) {
        gameplay_screen();
        display_minion_eneter_base_counter();
        draw_timer_and_pause_button();
        render_enemy();
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            if (((CP_Input_GetMouseX() >= gPauseButtonTextPositionX) && (CP_Input_GetMouseX() <= pauseButtonLimitX)) &&
                ((CP_Input_GetMouseY() >= gPauseButtonTextPositionY) && (CP_Input_GetMouseY() <= pauseButtonLimitY)))
            {
                gIsPaused = !gIsPaused;
                assign_minion_color();
            }
        }
        if (gIsPaused == TRUE) {
            for (int i = 0; i < minion_count; i++) {
                array_MinionStats[minion_count][MINION_TYPE] = array_MinionStats[i][MINION_TYPE];
                assign_minion_color();
                CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
            }
        }

        else if (gIsPaused == FALSE) {
            if (CP_Input_KeyTriggered(KEY_1)) {
                array_MinionStats[minion_count][MINION_TYPE] = SPAM_MINION; //just a test thing lol
                assign_minion_stats(); //maybe can throw this function call in render_minion
                render_minion();
            }
            if (CP_Input_KeyTriggered(KEY_2)) {
                array_MinionStats[minion_count][MINION_TYPE] = WARRIOR_MINION;
                assign_minion_stats();
                render_minion();
            }
            if (CP_Input_KeyTriggered(KEY_3)) {
                array_MinionStats[minion_count][MINION_TYPE] = TANK_MINION;
                assign_minion_stats();
                render_minion();
            }

            if (minion_count > 0) {
                move_minion();
                test = CP_System_GetDt();
                start_timer();
                snprintf(buffer, sizeof(buffer), "%d", (60 - (int)elapsed_timer));
                minion_enter_base_counter();
            }
        }
    }
}

void game_exit(void) {

}

void main_menu_screen(void) {
    //const char* main_menu_image = "./Assets/main_menu.png";
    CP_Graphics_ClearBackground(COLOR_GREY);
    /*Buttons*/
    CP_Settings_Fill(COLOR_WHITE);
    
    float quarter_blockX = (float)CP_System_GetDisplayWidth() / 4;
    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 4;
    button_height = 120.f;
    button_width = 300.f;
    level_selectorX = (quarter_blockX * 3) - button_width;
    start_game_buttonX = quarter_blockX;
    level_selectorY = start_game_buttonY = quarter_blockY * 2.8f;
    CP_Graphics_DrawRect(level_selectorX, level_selectorY, button_width, button_height);
    CP_Graphics_DrawRect(start_game_buttonX, start_game_buttonY, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(80);
    CP_Settings_Fill(COLOR_BLACK);
    start_textX = start_game_buttonX + 50;
    start_textY = start_game_buttonY + 80;
    levels_textX = level_selectorX + 40;
    levels_textY = level_selectorY + 80;
    CP_Font_DrawText("START", start_textX, start_textY);
    CP_Font_DrawText("LEVELS", levels_textX, levels_textY);
    /*For the bg image*//*
    CP_Settings_RectMode(CP_POSITION_CORNER);
    static float middleX, middleY, width, height;
    width = (float)(CP_System_GetWindowWidth());
    height = (float)(CP_System_GetWindowHeight());
    middleX =  width / 2.0f;
    middleY =  height / 2.0f;
    CP_Image_Draw(main_menu_image, middleX, middleY, width, height, 30);
    */

}

void main_menu_clicked(float x, float y) {
    //float button_height, button_width;
    //float level_selectorX, level_selectorY, start_game_buttonX, start_game_buttonY;
    /*Play button clicked*/
    if (x >= start_game_buttonX && x <= (start_game_buttonX + button_width) &&
        y >= start_game_buttonY && y <= start_game_buttonY + button_height) {
        Current_Gamestate = GAMEPLAY_SCREEN;

        /*initialise for gameplay screen*/
        minion_count = 0;
        reset_map_and_minions();
        initialise_level();
        gIsPaused = FALSE;
        minions_in_base = 0;

        initialise_pause_and_timer_button();
    }
    /*Level selector button clicked*/
    else if (x >= level_selectorX && x <= (level_selectorX + button_width) &&
        y >= level_selectorY && y <= level_selectorY + button_height) {
        //Current_Gamestate = LEVEL_SELECTOR_SCREEN;
        /*pending level_selector_screen completion*/
    }

}
/*Updates the new origin depending on what the full screen size is*/
void update_variables_and_make_screen_nice() {
    int map_border_width, window_width, window_height; //map_border_height;
    BLOCK_SIZE = 150; //the ideal width/length
    window_width = CP_System_GetWindowWidth();
    window_height = CP_System_GetWindowHeight();
    if (window_width < BLOCK_SIZE * MAP_GRID_COLS || window_height < BLOCK_SIZE * MAP_GRID_ROWS) {//if you have tiny screen
        BLOCK_SIZE = 100; //should probably make this a variable that depends on window_width and height but brain dead
    }
    map_border_width = (window_width - (BLOCK_SIZE * MAP_GRID_COLS)) / 2;
    //map_border_height = ((window_height - (BLOCK_SIZE * MAP_GRID_ROWS) - BLOCK_SIZE) / 2);
    origin_map_coordinateX = map_border_width;
    origin_map_coordinateY = map_border_width;
}

/*Sets entire map to BLOCK_EMPTY so it can be initialised later according to the level*/
void reset_map_and_minions(void) {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            array_GameMap[row][col] = BLOCK_EMPTY;
        }
    }
    for (int row = 0; row < MINION_MAX; ++row) {
        for (int col = 0; col < 12; ++col) {
            array_MinionStats[row][col] = 0;
        }
    }
    for (int row = 0; row < ENEMY_MAX; ++row) {
        for (int col = 0; col < MINION_MAX; ++col) {
            array_isMinionBlocked[row][col] = 0;
        }
    }
    level_has_been_reset = TRUE;
}

void initialise_pause_and_timer_button(void) {
    
    gPauseButtonTextPositionX = 10.f;
    gPauseButtonTextPositionY = 10.f;
    gTimerPositionX = gPauseButtonTextPositionX + 40.f;
    gTimerPositionY = 10.f;
    gTimerButtonTextPositionX = gTimerPositionX + 5.f;
    gTimerButtonTextPositionY = gTimerPositionY + 20.f;
    pauseButtonLimitX = gPauseButtonTextPositionX + 100.f;
    pauseButtonLimitY = gPauseButtonTextPositionY + 50.f;
}

void draw_timer_and_pause_button(void) {
    CP_Font_Set(CP_Font_GetDefault());
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(gPauseButtonTextPositionX, gPauseButtonTextPositionY, 30.f, 30.f);
    CP_Graphics_DrawRect(gTimerPositionX, gTimerPositionY, 50.f, 30.f);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(20);
    CP_Font_DrawText(buffer, gTimerButtonTextPositionX, gTimerButtonTextPositionY);
}

void start_timer(void) {
    elapsed_timer += test;
}

void gameplay_screen() {
    //initialise_level();
    render_background();
    int options_boxX, options_boxY, box_width, box_length;
    box_length = MAP_GRID_COLS * BLOCK_SIZE;
    box_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    options_boxX = origin_map_coordinateX;
    options_boxY = origin_map_coordinateY + MAP_GRID_ROWS * BLOCK_SIZE;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect((float)options_boxX, (float)options_boxY, (float)box_length, (float)box_width);
    int minion_buttons_width, minion_buttons_height, minion_boxX, minion_boxY;
    minion_buttons_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    minion_buttons_height = BLOCK_SIZE;
    for (int i = 1; i < 6; i++) {
        minion_boxX = options_boxX + (i * minion_buttons_width);
        minion_boxY = options_boxY + box_width / 2;
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Graphics_DrawRect((float)minion_boxX, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
    }
    CP_Settings_RectMode(CP_POSITION_CORNER);

}

void initialise_level() {
    level_1();
    //chooses level to initialise
    /*
    if (current_level == 1) {
        level_1();
    }
    else if (current_level == 1) {
        //level_2();
    }
    */
    setup_collaborative_diffusion_map();
}

void render_background() {
    CP_Graphics_ClearBackground(COLOR_BLACK);
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            BlockPositionX = origin_map_coordinateX + BLOCK_SIZE * col;
            BlockPositionY = origin_map_coordinateY + BLOCK_SIZE * row;
            CP_Settings_Fill(array_GameMap[row][col] == BLOCK_EMPTY //ternary operator
                ? COLOR_GREY
                : array_GameMap[row][col] == BLOCK_END //add something to include text to show that this is the end point
                ? COLOR_BLACK
                : array_GameMap[row][col] == BLOCK_PRESENT
                ? COLOR_WHITE
                : array_GameMap[row][col] == BLOCK_SPAWN
                ? COLOR_GREEN
                : array_GameMap[row][col] == BLOCK_INVISIBLE
                ? COLOR_GREY
                : COLOR_GREY); //BLOCK_ENEMY
            CP_Graphics_DrawRect((float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
        }
    }
}

void setup_collaborative_diffusion_map() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_END) {
                array_Collaborative_DiffusionMap[row][col][1] = 1; //when 1 it means value has been filled and done
                array_Collaborative_DiffusionMap[row][col][0] = 1000.f;
                spawn_row = row;
                spawn_col = col;
            }
            else if (array_GameMap[row][col] == BLOCK_PRESENT || array_GameMap[row][col] == BLOCK_INVISIBLE ||
                array_GameMap[row][col] == BLOCK_SPAWN) {
                array_Collaborative_DiffusionMap[row][col][1] = 1;
                array_Collaborative_DiffusionMap[row][col][0] = 0;
            }
            else {
                array_Collaborative_DiffusionMap[row][col][1] = 0;
            }
        }
    }
    for (int row = spawn_row; row < MAP_GRID_ROWS; ++row) {
        for (int col = spawn_col; col < MAP_GRID_COLS; ++col) {
            if (array_Collaborative_DiffusionMap[row][col][1] == 1) {
                if ((col - 1) >= 0 && array_Collaborative_DiffusionMap[row][col - 1][1] == 0) {
                    if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row][col - 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row][col - 1][1] = 1;
                    }
                }
                if ((col + 1) < MAP_GRID_COLS && array_Collaborative_DiffusionMap[row][col + 1][1] == 0) {
                    if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row][col + 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row][col + 1][1] = 1;
                    }
                }
                if ((row - 1) >= 0 && array_Collaborative_DiffusionMap[row - 1][col][1] == 0) {
                    if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row - 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row - 1][col][1] = 1;
                    }
                }
                if ((row + 1) < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[row + 1][col][1] == 0) {
                    if (array_GameMap[row + 1][col] == BLOCK_EMPTY || array_GameMap[row + 1][col] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row + 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row + 1][col][1] = 1;
                    }
                }
            }
        }
    }
    for (int row = 0; row < spawn_row; ++row) {
        for (int col = 0; col < spawn_col; ++col) {
            if (array_Collaborative_DiffusionMap[row][col][1] == 1) {
                if ((col - 1) >= 0 && array_Collaborative_DiffusionMap[row][col - 1][1] == 0) {
                    if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row][col - 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row][col - 1][1] = 1;
                    }
                }
                if ((col + 1) < MAP_GRID_COLS && array_Collaborative_DiffusionMap[row][col + 1][1] == 0) {
                    if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row][col + 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row][col + 1][1] = 1;
                    }
                }
                if ((row - 1) >= 0 && array_Collaborative_DiffusionMap[row - 1][col][1] == 0) {
                    if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row - 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row - 1][col][1] = 1;
                    }
                }
                if ((row + 1) < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[row + 1][col][1] == 0) {
                    if (array_GameMap[row + 1][col] == BLOCK_EMPTY || array_GameMap[row + 1][col] == BLOCK_ENEMY) {
                        array_Collaborative_DiffusionMap[row + 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                        array_Collaborative_DiffusionMap[row + 1][col][1] = 1;
                    }
                }
            }
        }
    }
}

void render_enemy() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_ENEMY) {
                for (int i = 0; i < ENEMY_MAX; ++i) {
                    if (level_has_been_reset == TRUE) {
                        assign_enemy_stats();
                        level_has_been_reset = FALSE;
                    }
                    CP_Settings_Fill(COLOR_RED);
                    CP_Settings_RectMode(CP_POSITION_CENTER);
                    array_EnemyStats[i][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[i][ENEMY_SIZE];
                    array_EnemyStats[i][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[i][ENEMY_SIZE];
                    CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[i][ENEMY_COL_COORDINATES], (float)array_EnemyStats[i][ENEMY_SIZE], (float)array_EnemyStats[i][ENEMY_SIZE]);
                    CP_Settings_RectMode(CP_POSITION_CORNER); 
                }
            }
        }
    }
}

void render_minion() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_SPAWN) {
                int SpawnX = origin_map_coordinateX + (BLOCK_SIZE * col) + (BLOCK_SIZE/2);
                int SpawnY = origin_map_coordinateY + (BLOCK_SIZE * row) + (BLOCK_SIZE/2); //makes it spawn in the middle of the spawn block
                if (minion_count < 7) {
                    array_MinionStats[minion_count][X] = SpawnX;
                    array_MinionStats[minion_count][Y] = SpawnY;
                    CP_Graphics_DrawCircle((float)array_MinionStats[minion_count][X], (float)array_MinionStats[minion_count][Y], (float)array_MinionStats[minion_count][MINION_SIZE]);
                    ++minion_count;
                }
                else if (minion_count >= 7) {
                    minion_count = 7;
                }
            }
        }
    }
}

void move_minion() {
    int move_left, move_right, move_up, move_down;
    for (int i = 0; i < minion_count; i++) {
        int current_boxCOL, current_boxROW;
        move_left = array_MinionStats[i][X] - array_MinionStats[i][MINION_MOVEMENT_SPEED];
        move_right = array_MinionStats[i][X] + array_MinionStats[i][MINION_MOVEMENT_SPEED];
        move_up = array_MinionStats[i][Y] - array_MinionStats[i][MINION_MOVEMENT_SPEED];
        move_down = array_MinionStats[i][Y] + array_MinionStats[i][MINION_MOVEMENT_SPEED];
        current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX + BLOCK_SIZE/2 - 1) / BLOCK_SIZE; //x
        current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY + BLOCK_SIZE/2 - 1) / BLOCK_SIZE; //y
        /*now we check, we want to move in the direction of the one with the highest value*/
        array_MinionStats[i][MINION_DIRECTION] = (current_boxCOL + 1 < MAP_GRID_COLS && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL + 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0])
            ? RIGHT
            : (current_boxCOL - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL - 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0])
            ? LEFT
            : (current_boxROW + 1 < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[current_boxROW + 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0])
            ? DOWN
            : (current_boxROW - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW - 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0])
            ? UP
            : STOP;
        int Past_Direction = array_MinionStats[i][MINION_DIRECTION];
        if (array_GameMap[current_boxROW][current_boxCOL] == BLOCK_ENEMY) {  
            int row_enemy, col_enemy;
            row_enemy = current_boxROW;
            col_enemy = current_boxCOL;
            int correct_enemy;
            correct_enemy = 0;
            for (int r = 0; r < ENEMY_MAX; r++) {
                /*Finds out which is the right enemy, since there can be 10 enemies at a time*/
                if ((array_EnemyStats[r][ENEMY_ROW] == row_enemy) && (array_EnemyStats[r][ENEMY_COL] == col_enemy)) {
                    correct_enemy = r;
                    /*Guard blocking minion*/
                    if (array_isMinionBlocked[correct_enemy][i] == 0) {
                        array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] += array_MinionStats[i][MINION_WEIGHT];
                        array_isMinionBlocked[correct_enemy][i] = 1;
                    }
                    if (array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] > array_EnemyStats[correct_enemy][ENEMY_BLOCK]) {
                        array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] -= array_MinionStats[i][MINION_WEIGHT];
                        array_MinionStats[i][MINION_DIRECTION] = Past_Direction;
                        array_isMinionBlocked[correct_enemy][i] = 0;
                    }
                    /*Minion attacking ground guard if blocked and guard attacking minion in return*/
                    else {
                        if (array_EnemyStats[correct_enemy][ENEMY_HP] > 0) {
                            array_MinionStats[i][MINION_DIRECTION] = STOP;
                            if (array_MinionStats[i][MINION_HP] > 0) {
                                array_EnemyStats[correct_enemy][ENEMY_HP] = array_EnemyStats[correct_enemy][ENEMY_HP] - array_MinionStats[i][MINION_ATTACK];
                                array_MinionStats[i][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];   
                            }
                            if (array_MinionStats[i][MINION_HP] <= 0) {
                                array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] -= array_MinionStats[i][MINION_WEIGHT];
                                array_isMinionBlocked[correct_enemy][i] = 0;
                            }
                        }
                        else if (array_EnemyStats[correct_enemy][ENEMY_HP] <= 0) {
                            array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] = 0;
                            array_GameMap[row_enemy][col_enemy] = BLOCK_EMPTY;
                            array_MinionStats[i][MINION_DIRECTION] = Past_Direction;
                        }
                        //printf("1 - %d |", array_EnemyStats[0][ENEMY_CURRENT_MINIONS_ON_BLOCK]);
                        //printf("2 - %d |", array_EnemyStats[1][ENEMY_CURRENT_MINIONS_ON_BLOCK]);
                        //printf("3 - %d \n", array_EnemyStats[2][ENEMY_CURRENT_MINIONS_ON_BLOCK]);
                    }
                }
            }
        }
        if (array_MinionStats[i][MINION_DIRECTION] == UP || array_MinionStats[i][MINION_DIRECTION] == DOWN) {
            array_MinionStats[i][X] = array_MinionStats[i][X];
            array_MinionStats[i][Y] = (array_MinionStats[i][MINION_DIRECTION] == UP
                ? move_up
                : move_down);
        }
        else if (array_MinionStats[i][MINION_DIRECTION] == LEFT || array_MinionStats[i][MINION_DIRECTION] == RIGHT) {
            array_MinionStats[i][Y] = array_MinionStats[i][Y];
            array_MinionStats[i][X] = (array_MinionStats[i][MINION_DIRECTION] == LEFT
                ? move_left
                : move_right);
        }
        else if (array_MinionStats[i][MINION_DIRECTION] == STOP) {
            array_MinionStats[i][X] = array_MinionStats[i][X];
            array_MinionStats[i][Y] = array_MinionStats[i][Y];
        }
        assign_minion_color();
        if (array_MinionStats[i][MINION_HP] > 0) { //only live minion are drawn
            CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
        }
    }
}

void minion_enter_base_counter() {
    for (int i = 0; i < MINION_MAX; i++) {
        int current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
        int current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
        if (array_GameMap[current_boxROW][current_boxCOL] == BLOCK_END && array_MinionStats[i][MINION_HP] > 0) {
            minions_in_base++;
            array_MinionStats[i][MINION_HP] = 0; //so essentially the minion dies, and amirah's code should come into effect here to essentially recycle the minion
        }
    }  
}

void display_minion_eneter_base_counter() {
    float counter_X, counter_Y, counter_width, counter_height;
    counter_height = 80;
    counter_width = (float)BLOCK_SIZE - 20;
    counter_X = (float)CP_System_GetWindowWidth() - (float)origin_map_coordinateX - (float)BLOCK_SIZE + 10;
    counter_Y = (float)origin_map_coordinateY + (float)MAP_GRID_ROWS * (float)BLOCK_SIZE + 10;
    snprintf(base_counter, sizeof(base_counter), "%d", minions_in_base);
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(counter_X, counter_Y, counter_width, counter_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(50);
    CP_Font_DrawText(base_counter, (counter_X + 17), (counter_Y + 55));
}

void assign_minion_stats() {
    if (array_MinionStats[minion_count][MINION_TYPE] == SPAM_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 50;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 8; //original speed was 8
        array_MinionStats[minion_count][MINION_ATTACK] = 4;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 1;
        array_MinionStats[minion_count][MINION_SIZE] = 50;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WARRIOR_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 130;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 6;
        array_MinionStats[minion_count][MINION_ATTACK] = 5;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 3;
        array_MinionStats[minion_count][MINION_SIZE] = 80;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == TANK_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 240;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 3; //original speed was 3
        array_MinionStats[minion_count][MINION_ATTACK] = 1;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 5;
        array_MinionStats[minion_count][MINION_SIZE] = 120;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WIZARD_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 80;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 40;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 4;
        array_MinionStats[minion_count][MINION_SIZE] = 100;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == HEALER_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 120;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 0;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 5;
        array_MinionStats[minion_count][MINION_SIZE] = 100;
    }
    /*
    if (array_MinionStats[minion_count][MINION_TYPE] == ) {

    } 
    */
}

void assign_minion_color() {
    if (array_MinionStats[minion_count][MINION_TYPE] == SPAM_MINION) {
        CP_Settings_Fill(COLOR_BLUE);
    }
}

void assign_enemy_stats() {
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 150;
            array_EnemyStats[i][ENEMY_ATTACK] = 2;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2; //idk how attack_speed works yet
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 100;
            array_EnemyStats[i][ENEMY_ATTACK] = 30;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 150;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 4;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == BASE) {
            CP_Settings_Fill(COLOR_RED);
            array_EnemyStats[i][ENEMY_HP] = 250;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
        /*
        if (array_EnemyStats[i][MINION_TYPE] == ) {

        }
        */
    }
}

void level_1() {
    array_GameMap[1][0] = BLOCK_END;
    array_GameMap[4][11] = BLOCK_SPAWN;
    array_GameMap[4][7] = BLOCK_ENEMY;
        array_EnemyStats[0][ENEMY_ROW] = 4;
        array_EnemyStats[0][ENEMY_COL] = 7;
        array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;
    array_GameMap[3][3] = BLOCK_ENEMY;
        array_EnemyStats[1][ENEMY_ROW] = 3;
        array_EnemyStats[1][ENEMY_COL] = 3;
        array_EnemyStats[1][ENEMY_TYPE] = GUARD_ENEMY;
    array_GameMap[3][0] = BLOCK_ENEMY;
        array_EnemyStats[2][ENEMY_ROW] = 3;
        array_EnemyStats[2][ENEMY_COL] = 0;
        array_EnemyStats[2][ENEMY_TYPE] = GUARD_ENEMY;
        array_GameMap[4][6] = BLOCK_PRESENT;
}

