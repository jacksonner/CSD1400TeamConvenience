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
#define BLOCK_TOWER_ENEMY 7 //used to mark location for tower enemies

/*Different Colours in Use*/
#define COLOR_BLACK CP_Color_Create(0, 0, 0, 255)
#define COLOR_GREY CP_Color_Create(70, 70, 70, 255)
#define COLOR_WHITE CP_Color_Create(255, 255, 255, 255)
#define COLOR_RED CP_Color_Create(255, 0, 0, 255)
#define COLOR_GREEN CP_Color_Create(0, 255, 0, 255)
#define COLOR_SEAGREEN CP_Color_Create(46, 139, 87, 255)
#define COLOR_BLUE CP_Color_Create(30, 144, 255, 255)
#define TEXT_COLOR CP_Color_Create(0, 0, 0, 255) //Text colour is just black though...?
#define COLOR_BROWN CP_Color_Create(165, 42, 42, 255)
#define COLOR_CYAN CP_Color_Create(0, 255, 255, 255)
#define COLOR_PURPLE CP_Color_Create(138, 43, 226, 255)
#define TRANSLUCENT_WHITE CP_Color_Create(255, 255, 255, 100)

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
#define MINION_TOTAL_STATS 12
int array_MinionStats[MINION_MAX][MINION_TOTAL_STATS]; //MUST edit reset_map_and_minions() when the array size is changed

#define MINION_CHARGE_TIME 0
#define MINION_CURRENT_CHARGE 1
float array_MinionCurrentCharge[MINION_MAX][2];

/*Types of Minions*/
#define SPAM_MINION 0 //weak everything, but low cost
#define WARRIOR_MINION 1 //decent health, decent attack
#define TANK_MINION 2 //tanky but low attack //targets tower but very low damage rip
#define WIZARD_MINION 3 //low health, after x amount of time, lightning from the skies hits all enemies BUT not so high damage ||
                               //limited range BUT higher damage //targets towers
#define HEALER_MINION 4 //decent health, no attack, heal other minions, relatively ex

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
#define ENEMY_TOTAL_STATS 13
int array_EnemyStats[ENEMY_MAX][ENEMY_TOTAL_STATS];

/*Used for checking if the minion will be blocked or not*/
int array_isMinionBlocked[ENEMY_MAX][MINION_MAX];

/*Types of Enemies*/
#define GUARD_ENEMY 0 //block minions
#define DAMAGE_ENEMY 1 // shorter range than tower
#define SLOW_ENEMY 2 //slow down minion around it
#define HEALING_TOWER 3 //heals enemies around it
#define RANGED_TOWER 4 //
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

/*render HP bar for minions*/
#define HP_BAR_HEIGHT 10
float max_hp;
float hp_percentage;
float default_hp = 50.f;
void renderminionhp_bar();

/*render HP bar for enemies*/
float enemy_max_hp;
float enemy_hp_percentage;
float default_hp_tower = 80.f;
float array_Enemy_HP_BAR[ENEMY_MAX][1];
void renderguardhp_bar(int i, int row, int col);

/*Recycling minions when minion dies*/
void minion_dies_array_recycle(int i);

/*Special attacks for minions*/
void minion_special_attack(int i, int current_row, int current_col);
void minion_attacking_towers(int i, int current_row, int current_col);

/*Restart level*/
float restartX, restartY, restart_length, restart_width;
void restart_level();
void display_restart_button(void); //found in gameplay_screen

/**/
#define FALSE 0
#define TRUE 1
int level_has_been_reset; //checks if level has reset so stats won't constantly be reassigned making the enemies immortal

/*Counter for no. of Minions who entered the base*/
int minions_in_base;
void minion_enter_base_counter(void);
/*
char base_counter[10];
//void display_minion_eneter_base_counter(void);
*/


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
int level_timer = 0;
float test;
float elapsed_timer;
float elapsed_timer2;
float gPauseButtonPositionX, gPauseButtonPositionY, gPauseButtonTextPositionX, gPauseButtonTextPositionY;
float gTimerPositionX, gTimerPositionY, gTimerButtonTextPositionX, gTimerButtonTextPositionY;
float currentElapsedTime;
static float totalElapsedTime;
float pauseButtonLimitX, pauseButtonLimitY;
float pauseButtonLimitX, pauseButtonLimitY;
int gIsPaused;
void start_timer(void);
void update_timer(void);
void initialise_pause_and_timer_button(void);
void draw_timer_and_pause_button(void);

/*Variables*/
int BlockPositionX;
int BlockPositionY;
int minion_count;
int spawn_row; 
int spawn_col;
void update_variables_and_make_screen_nice(); //since it's full screen, need to update the various variables so everything still looks nice

/*Money Code*/
void display_money_counter(void);
char money_buffer[400];
int money = 50;
int money_test = 0;

/*GamePlay Screen*/
int options_boxX, options_boxY, box_width, box_length; //this is the giant wide box atm which all the options etc. goes in
int minion_buttons_width, minion_buttons_height, minion_boxX, minion_boxY;
void gameplay_screen_clicked(float x, float y);

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
void assign_minion_color(int i);

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
        //display_minion_eneter_base_counter(); Commented out
        draw_timer_and_pause_button();
        display_money_counter();
        render_enemy();
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            if (((CP_Input_GetMouseX() >= gPauseButtonPositionX) && (CP_Input_GetMouseX() <= pauseButtonLimitX)) &&
                ((CP_Input_GetMouseY() >= gPauseButtonPositionY) && (CP_Input_GetMouseY() <= pauseButtonLimitY)))
            {
                gIsPaused = !gIsPaused;
            }
        }
        if (gIsPaused == TRUE) {
            for (int i = 0; i < minion_count; i++) {
                array_MinionStats[minion_count][MINION_TYPE] = array_MinionStats[i][MINION_TYPE];
                assign_minion_color(i);
                CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
                renderminionhp_bar();
            }
        }

        else if (gIsPaused == FALSE) {
            //test = CP_System_GetDt();
            start_timer();
            update_timer();
            if (elapsed_timer2 > 2)
            {
                money += 10;
                elapsed_timer2 -= 2;
            }
            snprintf(buffer, sizeof(buffer), "%d", (60 - (int)elapsed_timer));
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                gameplay_screen_clicked(CP_Input_GetMouseX(), CP_Input_GetMouseY());
            }
            if (minion_count > 0) {
                move_minion();
                renderminionhp_bar();
                minion_enter_base_counter(); //please do not comment this out
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
        //minions_in_base = 0; Part of minion counter which has been commented out

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
        for (int col = 0; col < MINION_TOTAL_STATS; ++col) {
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

    gTimerPositionX = 1330.f;
    gTimerPositionY = 845.f;
    gPauseButtonPositionX = gTimerPositionX;
    gPauseButtonPositionY = gTimerPositionY + 100.f;
    gTimerButtonTextPositionX = gTimerPositionX + 30.f;
    gTimerButtonTextPositionY = gTimerPositionY + 35.f;
    gPauseButtonTextPositionX = gPauseButtonPositionX + 15.f;
    gPauseButtonTextPositionY = gPauseButtonPositionY + 30.f;
    pauseButtonLimitX = gPauseButtonPositionX + 100.f;
    pauseButtonLimitY = gPauseButtonPositionY + 50.f;
}

void draw_timer_and_pause_button(void) {
    CP_Font_Set(CP_Font_GetDefault());
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(gPauseButtonPositionX, gPauseButtonPositionY, 100.f, 50.f);
    CP_Graphics_DrawRect(gTimerPositionX, gTimerPositionY, 100.f, 50.f);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(35);
    CP_Font_DrawText(buffer, gTimerButtonTextPositionX, gTimerButtonTextPositionY);
    if (gIsPaused == 1)
    {
        CP_Settings_TextSize(30);
        CP_Font_DrawText("Play", (gPauseButtonTextPositionX + 10.f), gPauseButtonTextPositionY);
    }
    else
    {
        CP_Settings_TextSize(30);
        CP_Font_DrawText("Pause", gPauseButtonTextPositionX, gPauseButtonTextPositionY);
    }

}

void start_timer(void) {
    test = CP_System_GetDt();
}

void update_timer(void)
{
    elapsed_timer += test;  //For Countdown
    elapsed_timer2 += test; //For Money
    /*for the minion charged attacks*/
    for (int i = 0; i < minion_count; i++) {
        array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] += test;
    }
}

void display_restart_button(void) {
    restartX = (float)origin_map_coordinateX + 15;
    restartY = 900; 
    restart_length = 80;
    restart_width = 50;
    /*Button*/
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(restartX, restartY, restart_length, restart_width);
    /*Text*/
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(25);
    CP_Font_DrawText("Restart", restartX + 4, restartY + 30);
}

void restart_level(void) {
    minion_count = 0;
    reset_map_and_minions();
    initialise_level();
    gIsPaused = FALSE;
    //minions_in_base = 0; Part of minion counter which has been commented out
    initialise_pause_and_timer_button();
    money = 50;
    elapsed_timer = 0;
    elapsed_timer2 = 0;
    draw_timer_and_pause_button();
    display_money_counter();
}

void gameplay_screen() {
    //initialise_level();
    render_background();
    box_length = MAP_GRID_COLS * BLOCK_SIZE;
    box_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    options_boxX = origin_map_coordinateX;
    options_boxY = origin_map_coordinateY + (MAP_GRID_ROWS * BLOCK_SIZE) ;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect((float)options_boxX, (float)options_boxY, (float)box_length, (float)box_width);
    minion_buttons_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    minion_buttons_height = BLOCK_SIZE;
    for (int i = 1; i < 6; i++) {
        minion_boxX = options_boxX + (i * minion_buttons_width) - minion_buttons_width/2;
        minion_boxY = options_boxY + (box_width / 4) - 20; //minion_boxY remains constant throughout
        CP_Graphics_DrawRect((float)minion_boxX, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
    }
    float minion_costboxY = (float)minion_boxY + 100.f;
    float minion_costbox_height =  50;
    /*Render minion pictures here BEFORE THE COST BOXES*/
    //
    //
    //
    for (int i = 0; i < 5; i++) {
        minion_boxX = options_boxX + minion_buttons_width + (i * minion_buttons_width) - minion_buttons_width / 2;
        CP_Settings_Fill(TRANSLUCENT_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX, minion_costboxY, (float)minion_buttons_width, minion_costbox_height);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Settings_TextSize(35);
        CP_Font_DrawText( (i == 0 
            ? "30"
            : i == 1
            ? "60"
            : i == 2
            ? "120"
            : i == 3
            ? "160"
            : i == 4
            ? "160"
            : "ERROR")
            , (float)minion_boxX + 90, minion_costboxY + 32);
    }
    CP_Settings_RectMode(CP_POSITION_CORNER);
    display_restart_button();

}

void gameplay_screen_clicked(float x, float y) {
    float origin_first_boxX = (float)options_boxX + (float)minion_buttons_width - (float)minion_buttons_width / 2.f;
    if (y >= minion_boxY && y <= minion_boxY + minion_buttons_height) {
        if (x >= origin_first_boxX && x < origin_first_boxX + minion_buttons_width) { //Create Spam Minion
            if (money >= 30 && minion_count < MINION_MAX)
            {
                money = money - 30;
                array_MinionStats[minion_count][MINION_TYPE] = SPAM_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }
        }
        else if (x >= (origin_first_boxX + minion_buttons_width) && x < (origin_first_boxX + 2 * minion_buttons_width)) { //Create Warrior Minion
            if (money >= 60 && minion_count < MINION_MAX)
            {
                money -= 60;
                array_MinionStats[minion_count][MINION_TYPE] = WARRIOR_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }
            
        }
        else if (x >= (origin_first_boxX + 2 * minion_buttons_width) && x < (origin_first_boxX + 3 * minion_buttons_width)) { //Create Tank Minion
            if (money >= 120 && minion_count < MINION_MAX)
            {
                money -= 120;
                array_MinionStats[minion_count][MINION_TYPE] = TANK_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }
        }
        else if (x >= (origin_first_boxX + 3 * minion_buttons_width) && x < (origin_first_boxX + 4 * minion_buttons_width)) { //Create Wizard Minion
            if (money >= 160 && minion_count < MINION_MAX)
            {
                money -= 160;
                array_MinionStats[minion_count][MINION_TYPE] = WIZARD_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }
        }
        else if (x >= (origin_first_boxX + 4 * minion_buttons_width) && x < (origin_first_boxX + 5 * minion_buttons_width)) { //Create Healer Minion
            if (money >= 160 && minion_count < MINION_MAX)
            {
                money -= 160;
                array_MinionStats[minion_count][MINION_TYPE] = HEALER_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }
        }
        render_minion();
    }
    if (y >= restartY && (y <= (restartY + restart_width)) && (x >= restartX && (x <= (restartX + restart_length)))) {
        restart_level();
    }
}

/*Initialises level depending on the current level - TBC*/
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
                : array_GameMap[row][col] == BLOCK_PRESENT || array_GameMap[row][col] == BLOCK_TOWER_ENEMY
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
            if (array_GameMap[row][col] == BLOCK_ENEMY || array_GameMap[row][col] == BLOCK_TOWER_ENEMY) {
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
                    assign_minion_color(minion_count);
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
        minion_special_attack(i, current_boxROW, current_boxCOL);
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
            //correct_enemy = 0;
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
                                renderguardhp_bar(correct_enemy, row_enemy, col_enemy);
                                // array_MinionStats[i][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];   for future use when aoe enemy is implemented
                            }

                            /*Single targeting system*/
                            int previous_minion = i - 1;
                            if (previous_minion >= 0 && array_MinionStats[previous_minion][MINION_HP] > 0) {
                                array_MinionStats[previous_minion][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];
                            }
                            else {
                                int new_minion = previous_minion + 1;
                                array_MinionStats[new_minion][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];
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
                            money += 20;
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
        if (array_MinionStats[i][MINION_HP] <= 0) {
            minion_dies_array_recycle(i);
        }
        if (array_MinionStats[i][MINION_HP] > 0) { //only live minion are drawn
            assign_minion_color(i);
            CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
        }
    }
}

/*Should probably update this code to make it shorter lol*/
void minion_dies_array_recycle(int dead_minion_number) {
    int array_Temp_MinionStats[MINION_MAX][MINION_TOTAL_STATS];
    float array_Temp_MinionCharge[MINION_MAX][2];
    for (int i = 0; i < dead_minion_number; i++) {
        for (int j = 0; j < MINION_TOTAL_STATS; j++) {
            array_Temp_MinionStats[i][j] = array_MinionStats[i][j];
        }
    }
    for (int i = 0; i < dead_minion_number; i++) {
        for (int j = 0; j < 2; j++) {
            array_Temp_MinionCharge[i][j] = array_MinionCurrentCharge[i][j];
        }
    }
    int dead_min_num = dead_minion_number;
    for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
        for (int m = 0; m < MINION_TOTAL_STATS; m++) {
            array_Temp_MinionStats[dead_min_num][m] = array_MinionStats[k][m];
        }
    }
    for (int k = (dead_minion_number + 1); k <= minion_count; k++, dead_minion_number++) {
        for (int m = 0; m < 2; m++) {
            array_Temp_MinionCharge[dead_minion_number][m] = array_MinionCurrentCharge[k][m];
        }
    }
    /*Now we update the original array with the stored values*/
    for (int h = 0; h < minion_count; h++) {
        for (int n = 0; n < MINION_TOTAL_STATS; n++) {
            array_MinionStats[h][n] = array_Temp_MinionStats[h][n];
        }
    }
    for (int h = 0; h < minion_count; h++) {
        for (int n = 0; n < 2; n++) {
            array_MinionCurrentCharge[h][n] = array_Temp_MinionCharge[h][n];
        }
    }
    minion_count--;
}

void minion_enter_base_counter() {
    for (int i = 0; i < MINION_MAX; i++) {
        int current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
        int current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
        if (array_GameMap[current_boxROW][current_boxCOL] == BLOCK_END) {
            minions_in_base++;
            array_MinionStats[i][MINION_HP] = 0; //so essentially the minion dies
            minion_dies_array_recycle(i);
        }
    }  
}

/*
void display_minion_enter_base_counter() {
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
*/

void renderminionhp_bar() {
    for (int i = 0; i < MINION_MAX; i++) {
        if (array_MinionStats[i][MINION_HP] > 0) {
            if (array_MinionStats[i][MINION_TYPE] == SPAM_MINION) {
                max_hp = 50;
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;//to find current hp
            }
            else if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
                max_hp = 130;
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
            }
            else if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) {
                max_hp = 240;
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
            }
            else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
                max_hp = 80;
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
            }
            else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
                max_hp = 120;
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
            }
            float new_hp_bar = hp_percentage * default_hp;
            CP_Settings_Fill(COLOR_RED);
            CP_Graphics_DrawRect((float)array_MinionStats[i][X]-20, (float)array_MinionStats[i][Y] - 80, (float)default_hp, (float)HP_BAR_HEIGHT); //max_hp
            CP_Settings_Fill(COLOR_GREEN);
            CP_Graphics_DrawRect((float)array_MinionStats[i][X]-20, (float)array_MinionStats[i][Y] - 80, (float)new_hp_bar, (float)HP_BAR_HEIGHT);
        }
    }
}

void renderguardhp_bar(int i, int row, int col) {
    if (array_EnemyStats[i][ENEMY_HP] > 0) {
        if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
            enemy_max_hp = 150;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;//to find current hp
        }
        else if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
            enemy_max_hp = 100;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
        }
        else if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
            enemy_max_hp = 120;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
        }
        else if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
            enemy_max_hp = 120;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
        }
        else if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
            enemy_max_hp = 150;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
        }
        else if (array_EnemyStats[i][ENEMY_TYPE] == BASE) {
            enemy_max_hp = 250;
            enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
        }
        float enemy_hp_bar = enemy_hp_percentage * default_hp_tower;
        array_EnemyStats[i][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[i][ENEMY_SIZE];
        array_EnemyStats[i][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[i][ENEMY_SIZE];
        CP_Settings_Fill(COLOR_RED);
        CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 70, (float)default_hp_tower, (float)HP_BAR_HEIGHT); //max_hp
        CP_Settings_Fill(COLOR_GREEN);
        CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 70, enemy_hp_bar, (float)HP_BAR_HEIGHT);
    }
}

void display_money_counter() {
    float counter_X, counter_Y, counter_width, counter_height;
    counter_height = 80;
    counter_width = (float)BLOCK_SIZE - 20;
    counter_X = (float)CP_System_GetWindowWidth() - (float)origin_map_coordinateX - (float)BLOCK_SIZE + 10;
    counter_Y = (float)origin_map_coordinateY + (float)MAP_GRID_ROWS * (float)BLOCK_SIZE + 10;
    snprintf(money_buffer, sizeof(money_buffer), "%d", money);
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(counter_X, counter_Y, counter_width, counter_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(50);
    CP_Font_DrawText("Money: " , (counter_X - 150.f), (counter_Y + 55));
    CP_Font_DrawText(money_buffer, (counter_X + 40.f), (counter_Y + 55));
}

void minion_special_attack(int i, int current_row, int current_col) {
    if (array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] >= array_MinionCurrentCharge[i][MINION_CHARGE_TIME]) {
        if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
            printf("meaningless gibberish");
            //insert code for whatever they can do here
        }
        else if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) { //restores HP and attack tower
            if (array_MinionStats[i][MINION_HP] < 140) {
                array_MinionStats[i][MINION_HP] += 100;
            }
            else {
                int hp_restored = 240 - array_MinionStats[i][MINION_HP];
                array_MinionStats[i][MINION_HP] += hp_restored;
            }
            
            
        }
        else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
            printf("ooo special attack time");
            minion_attacking_towers(i, current_row, current_col);
            //insert code for whatever they can do here
        }
        else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
            printf("heal");
            //insert code for whatever they can do here
        }
        array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
    }
}

void minion_attacking_towers(int i, int current_row, int current_col) { //can work on the minion's normal attack here, aka attacking towers while moving?
    if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) { //attacks all enemies?
        printf("ooo special attack time");
        //insert code for whatever they can do here
    }
}

/*IMPORTANT - BEFORE UPDATING ANY VALUE HERE, CTRL+F TO CHECK IF IT HAS BEEN USED ELSEWHERE AND UPDATE ACCORDINGLY*/
/*for example HP is used in rendering the hp bars. Thanks! :D*/
void assign_minion_stats() {
    if (array_MinionStats[minion_count][MINION_TYPE] == SPAM_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 50;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 6; //original speed was 8
        array_MinionStats[minion_count][MINION_ATTACK] = 4;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 25;
        array_MinionStats[minion_count][MINION_SIZE] = 50;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WARRIOR_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 130;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 6;
        array_MinionStats[minion_count][MINION_ATTACK] = 5;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 50;
        array_MinionStats[minion_count][MINION_SIZE] = 80;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 5; //one strong attack?
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == TANK_MINION) { //is tall so can attack tower
        array_MinionStats[minion_count][MINION_HP] = 240; 
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 4; //original speed was 3
        array_MinionStats[minion_count][MINION_ATTACK] = 0;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 2; //holds the line so other minions can pass
        array_MinionStats[minion_count][MINION_COST] = 100;
        array_MinionStats[minion_count][MINION_SIZE] = 120;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 2;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WIZARD_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 80;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 40;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 150;
        array_MinionStats[minion_count][MINION_SIZE] = 70;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 6;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == HEALER_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 120;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 0; //i mean... self defence?
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 150;
        array_MinionStats[minion_count][MINION_SIZE] = 60;
        array_MinionStats[minion_count][MINION_HEAL] = 20;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 2; //super healing???
    }
}

/*probably going to be removed in the final product*/
void assign_minion_color(int i) { 
        if (array_MinionStats[i][MINION_TYPE] == SPAM_MINION) {
            CP_Settings_Fill(COLOR_BLUE);
        }
        else if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
            CP_Settings_Fill(COLOR_SEAGREEN);
        }
        else if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) {
            CP_Settings_Fill(COLOR_BROWN);
        }
        else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
            CP_Settings_Fill(COLOR_CYAN);
        }
        else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
            CP_Settings_Fill(COLOR_PURPLE);
        }
}

void assign_enemy_stats() {
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 150;
            array_EnemyStats[i][ENEMY_ATTACK] = 2;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2; //idk how attack_speed works yet
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 100;
            array_EnemyStats[i][ENEMY_ATTACK] = 30;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
            array_EnemyStats[i][ENEMY_HP] = 150;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 4;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == BASE) {
            array_EnemyStats[i][ENEMY_HP] = 250;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
        }
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
    array_GameMap[3][8] = BLOCK_TOWER_ENEMY;
        array_EnemyStats[3][ENEMY_ROW] = 3;
        array_EnemyStats[3][ENEMY_COL] = 8;
        array_EnemyStats[3][ENEMY_TYPE] = HEALING_TOWER;
    array_GameMap[2][5] = BLOCK_TOWER_ENEMY;
        array_EnemyStats[4][ENEMY_ROW] = 2;
        array_EnemyStats[4][ENEMY_COL] = 5;
        array_EnemyStats[4][ENEMY_TYPE] = SLOW_ENEMY;
        /*Please be careful when adding a new enemy, change the number array_EnemyStats[3][ENEMY_TYPE] -> array_EnemyStats[4][ENEMY_TYPE]*/
    array_GameMap[4][6] = BLOCK_PRESENT;
}