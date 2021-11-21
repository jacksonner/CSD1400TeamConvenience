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
#define BLOCK_INVISIBLE 5 //ignore
#define BLOCK_ENEMY_DEAD 6 //ignore
#define BLOCK_TOWER_ENEMY 7 //used to mark location for tower enemies

#define BLOCK_TELEPORTER 8
#define BLOCK_TELEPORT_SPAWN 9

/*Teleportation*/
int level_has_teleporter;
void setup_teleport_diffusion_map();
float array_Teleport_DiffusionMap[MAP_GRID_ROWS][MAP_GRID_COLS][2];
int teleport_row, teleport_col;
int teleport_spawn_row, teleport_spawn_col;
int teleport_spawn_X, teleport_spawn_Y;

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
#define COLOR_BRIGHT_BLUE CP_Color_Create(0, 204, 204, 255)
#define COLOR_DULLER_BLUE CP_Color_Create(0, 76, 153, 255)
#define TRANSLUCENT_BLUE CP_Color_Create(204, 255, 255, 100)
#define COLOR_DULL_GREEN CP_Color_Create(0, 153, 0, 255)
#define COLOR_DARK_BLUE CP_Color_Create(0, 102, 204, 255)
#define COLOR_YELLOW CP_Color_Create(255, 255, 0, 255)


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
#define MINION_TRAVEL_DIST 12
#define MINION_PAST_DIRECTION 13
#define MINION_TELEPORTED 14

/*can add a MINION_COLOUR too maybe?*/
#define MINION_MAX 7 //Maximum number of minions in the game at any one time
#define MINION_TOTAL_STATS 15
int array_MinionStats[MINION_MAX][MINION_TOTAL_STATS]; //MUST edit reset_map_and_minions() when the array size is changed

/*Minion Charged Attacks*/
#define MINION_CHARGE_TIME 0
#define MINION_CURRENT_CHARGE 1
#define MINION_BASIC_ATTACK_SPEED 2 //this is attack speed in the main array lol...
#define MINION_BASIC_CURRENT_CHARGE 3

#define TOTAL_CHARGES 4
float array_MinionCurrentCharge[MINION_MAX][TOTAL_CHARGES];
int check_minion_basic_attack_charge(int i); //checks if it is time for minion to attack
void render_special_current_charge();

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
int enemy_count;

/*Used for checking if the minion will be blocked or not*/
int array_isMinionBlocked[ENEMY_MAX][MINION_MAX];

/*Enemy Charged Attacks*/
#define ENEMY_BASIC_ATTACK_SPEED 0 //this is attack speed in the main array lol...
#define ENEMY_BASIC_CURRENT_CHARGE 1
#define ENEMY_CURRENT_CHARGE 2
#define ENEMY_CHARGE_TIME 3

#define TOTAL_ENEMY_CHARGES 4
float array_EnemyCurrentCharge[ENEMY_MAX][TOTAL_ENEMY_CHARGES];
int check_enemy_basic_attack_charge(int i);
void enemy_special_attack();
#define EFFECT_TIMER 0
#define CHECKER 1
float array_Enemy_Slow_Effect_Time[ENEMY_MAX][2];
int array_isMinionSlowed[ENEMY_MAX][MINION_MAX][2];
void render_enemy_special_attack_bar(int i);
int find_enemy_full_hp(int j);

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
#define SETTING_SCREEN 6
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
float enemy_hp_percentage;
float default_hp_tower = 80.f;
float array_Enemy_HP_BAR[ENEMY_MAX][1];
void renderguardhp_bar(int i);

/*Recycling minions when minion dies*/
void minion_dies_array_recycle(int i);

/*Special attacks for minions*/
void minion_special_attack(int i, int current_row, int current_col);
void minion_attacking_towers(int i, int current_row, int current_col, int minion_range);
void healer_minion_basic_heal(int i);

/*checking surrounding squares for AOE attacks*/
int array_enemy_to_attack[MINION_MAX][ENEMY_MAX];
int check_which_enemy(int row, int col);

/*Restart level*/
float restartX, restartY, restart_length, restart_width;
void restart_level();
void display_restart_button(void); //found in gameplay_screen

/*Win Condition Related*/
void render_win_progress();

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
void level_2(void);
void level_3(void);
void level_4(void);
void level_5(void);
void level_6(void);
int current_level;

/*Timer and Pause button*/
int t_time;
char buffer[60];
int level_timer = 0;
float test;
float elapsed_timer;
float elapsed_timer2;
float gPauseButtonPositionX, gPauseButtonPositionY, gPauseButtonTextPositionX, gPauseButtonTextPositionY, setting_buttonX, setting_buttonY;
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
float level_selectorX, level_selectorY, start_game_buttonX, start_game_buttonY, settingX, settingY;
float start_textX, start_textY, levels_textX, levels_textY, setting_textX, setting_textY;
void main_menu_screen(void);
void main_menu_clicked(float x, float y);
static CP_Image main_menu_image;

/*Lose Screen*/
float button_heightL, button_widthL;
float restart_loseX, restart_loseY, main_loseX, main_loseY;
float restart_textX, restart_textY, main_textX, main_textY;
void lose_screen(void);
CP_Image Lose_Screen = NULL;

/*Win Screen*/
void win_screen(void);
float ButtonPositionX;
float ButtonPositionY;
float button_heightL, button_widthL;
float restart_loseX, restart_loseY, main_loseX, main_loseY;
float restart_textX, restart_textY, main_textX, main_textY;
CP_Image Win_Screen = NULL;

/*Level Selector Screen*/
void level_selector_screen(void);
float level1X, level1Y, level1_textX, level1_textY;
float level2X, level2Y, level2_textX, level2_textY;
float level3X, level3Y, level3_textY, level3_textY;
float level4X, level4Y, level4_textY, level4_textY;
float level5X, level5Y, level5_textY, level5_textY;
float level6X, level6Y, level6_textY, level6_textY;
CP_Image Level_Selector_Screen = NULL;

/*Setting Screen*/ //comment so it let me commit
void setting_screen(void);
void setting_screen_clicked(float x, float y);
static CP_Image setting_image;
float backX, backY, setting_width, setting_height;
float back_width, back_height;
int Previous_Gamestate;

/*Move Minion*/
int initial_direction; //when setting up level, check for the initial direction to set this to
void move_minion(void);
int check_for_enemy(int minion); //returns 1 if there is an enemy on the minions' current position

/*HP*/
int find_full_hp(int i);

/*Functions*/
void reset_map_and_minions(void);
void render_background(void); //for the gameplay_screen
void gameplay_screen(void);

void initialise_level(void); //TBC
void setup_collaborative_diffusion_map(void); //ensure no backtracking
void render_minion(void);
void assign_minion_stats(void);
void assign_enemy_stats(void);
void render_enemy(void);
void assign_minion_color(int i);
void projectile_logic(float x_coord, float y_coord);
void projectile_render(float x_coord, float y_coord);
void assign_enemy_color(int i);

void game_init(void) {
    /*Setting the FrameRate to 60fps*/
    CP_System_SetFrameRate(60.0f);

    /*Initialising variables for Fullscreen etc.*/
    CP_System_Fullscreen();
    update_variables_and_make_screen_nice();

    /*Initialise to Main_Menu*/
    Current_Gamestate = MAIN_MENU_SCREEN;

    current_level = 1;

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
        if (CP_Input_KeyTriggered(KEY_1))
        {
            money += 1000;
        }
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            setting_screen_clicked(CP_Input_GetMouseX(), CP_Input_GetMouseY());

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
                render_special_current_charge();
                enemy_special_attack();
            }
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                if (CP_Input_GetMouseY() >= restartY && (CP_Input_GetMouseY() <= (restartY + restart_width)) && (CP_Input_GetMouseX() >= restartX && (CP_Input_GetMouseX() <= (restartX + restart_length)))) {
                    restart_level();
                }
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
                render_special_current_charge();
                minion_enter_base_counter(); //please do not comment this out
            }
            enemy_special_attack();
            if ((int)elapsed_timer == 60)
            {
                Current_Gamestate = LOSE_SCREEN;
            }
            if (minions_in_base == 10) {
                Current_Gamestate = WIN_SCREEN;
            }
        }
    }
    else if (Current_Gamestate == LOSE_SCREEN)
    {
        lose_screen();

    }
    else if (Current_Gamestate == LEVEL_SELECTOR_SCREEN)
    {
        level_selector_screen();
    }
    else if (Current_Gamestate == WIN_SCREEN) {

        win_screen();
    }
    else if (Current_Gamestate == SETTING_SCREEN) {
        setting_screen();
    }
}

void game_exit(void) {

}

/*FUNCTIONS START HERE*/

void main_menu_screen(void) {
    main_menu_image = CP_Image_Load("./Assets/bg_mainmenu2.png");
    //CP_Graphics_ClearBackground(COLOR_WHITE);
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(main_menu_image);
    height = (float)CP_Image_GetWidth(main_menu_image) * 0.6f;
    CP_Image_Draw(main_menu_image, middleX, middleY, width, height, 100);
    /* Buttons */
        CP_Settings_Fill(COLOR_WHITE);
    float quarter_blockX = (float)CP_System_GetDisplayWidth() / 4;
    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 4;
    button_height = 120.f;
    button_width = 300.f;
    //level_selectorX = (quarter_blockX * 3) - button_width;
    level_selectorX = quarter_blockX;
    start_game_buttonX = quarter_blockX;
    start_game_buttonY = 2.f * quarter_blockY + 0.1f * quarter_blockY;
    level_selectorY = start_game_buttonY + quarter_blockY * 0.8f;
    settingX = 1600;
    settingY = 1000.f;
    CP_Graphics_DrawRect(level_selectorX, level_selectorY, button_width, button_height);
    CP_Graphics_DrawRect(start_game_buttonX, start_game_buttonY, button_width, button_height);
    setting_height = 60.f;
    setting_width = 240.f;
    CP_Graphics_DrawRect(settingX, settingY, setting_width, setting_height);
    /* Now Text */
        CP_Settings_TextSize(80);
    CP_Settings_Fill(COLOR_BLACK);
    start_textX = start_game_buttonX + 50;
    start_textY = start_game_buttonY + 80;
    levels_textX = level_selectorX + 40;
    levels_textY = level_selectorY + 80;
    setting_textX = settingX + 40;
    setting_textY = settingY + 40;
    CP_Font_DrawText("START", start_textX, start_textY);
    CP_Font_DrawText("LEVELS", levels_textX, levels_textY);
    CP_Settings_TextSize(40);
    CP_Font_DrawText("SETTING", setting_textX, setting_textY);
}

void main_menu_clicked(float x, float y) {
    //float button_height, button_width;
    //float level_selectorX, level_selectorY, start_game_buttonX, start_game_buttonY;
    // Play button clicked
        if (x >= start_game_buttonX && x <= (start_game_buttonX + button_width) &&
            y >= start_game_buttonY && y <= start_game_buttonY + button_height) {

            // Game will start at level 1
                current_level = 1;
            Current_Gamestate = GAMEPLAY_SCREEN;

            // Free image /
                CP_Image_Free(&main_menu_image);

            // initialise for gameplay screen /
                minion_count = 0;
            reset_map_and_minions();
            initialise_level();
            restart_level();
            gIsPaused = FALSE;
            minions_in_base = 0; //Part of minion counter which has been commented out

            initialise_pause_and_timer_button();
        }
    // Level selector button clicked /
        else if (x >= level_selectorX && x <= (level_selectorX + button_width) &&
            y >= level_selectorY && y <= level_selectorY + button_height) {
            Current_Gamestate = LEVEL_SELECTOR_SCREEN;
            // pending level_selector_screen completion /

                CP_Image_Free(&main_menu_image);
        }

    // Setting button clicked /
        else if (x >= settingX && x <= (settingX + setting_width) &&
            y >= settingY && settingY <= settingY + setting_height) {
            Current_Gamestate = SETTING_SCREEN;
        }


}

void lose_screen(void) {
    float width = (float)CP_System_GetWindowWidth();
    float height = (float)CP_System_GetWindowHeight();
    /*Lose Screen*/
    Lose_Screen = CP_Image_Load("./Assets/Lose_Screen.jpg");
    /*Draw Image*/
    CP_Image_Draw(Lose_Screen, width / 2, height / 2, width, height, 255);
    /*Buttons*/
    CP_Settings_Fill(COLOR_WHITE);

    float quarter_blockX = (float)CP_System_GetDisplayWidth() / 4;
    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 4;
    button_height = 120.f;
    button_width = 300.f;
    main_loseX = (quarter_blockX * 3) - button_width;
    restart_loseX = quarter_blockX;
    main_loseY = restart_loseY = quarter_blockY * 2.8f;
    CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
    CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(60);
    CP_Settings_Fill(COLOR_BLACK);
    restart_textX = restart_loseX + 40;
    restart_textY = restart_loseY + 80;
    main_textX = main_loseX + 35;
    main_textY = main_loseY + 75;
    CP_Font_DrawText("RESTART", restart_textX, restart_textY);
    CP_Settings_TextSize(50);
    CP_Font_DrawText("MAIN MENU", main_textX, main_textY);


    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= restart_loseX && mouseX <= (restart_loseX + button_width) &&
        mouseY >= restart_loseY && mouseY <= restart_loseY + button_height) {

        CP_Image_Draw(Lose_Screen, width / 2, height / 2, width, height, 255);

        /*Buttons*/
        //Hovering on Restart Button
        main_loseX = (quarter_blockX * 3) - button_width;
        restart_loseX = quarter_blockX;
        main_loseY = restart_loseY = quarter_blockY * 2.8f;
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);
        /*Text for button*/
        restart_textX = restart_loseX + 40;
        restart_textY = restart_loseY + 80;
        main_textX = main_loseX + 35;
        main_textY = main_loseY + 75;
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("RESTART", restart_textX, restart_textY);
        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("MAIN MENU", main_textX, main_textY);

        //When clicked, return back to gameplay screen
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            Current_Gamestate = GAMEPLAY_SCREEN;
            /*initialise for gameplay screen*/

            CP_Image_Free(&Lose_Screen);
            minion_count = 0;
            reset_map_and_minions();
            initialise_level();
            gIsPaused = FALSE;
            restart_level();


        }

    }
    else if (mouseX >= main_loseX && mouseX <= (main_loseX + button_width) && mouseY >= main_loseY && mouseY <= main_loseY + button_height)
    {
        //Hovering on Main menu Button
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);

        /*Text for Buttons*/
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("RESTART", restart_textX, restart_textY);
        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("MAIN MENU", main_textX, main_textY);

        /*When clicked*/
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Lose_Screen);
            Current_Gamestate = MAIN_MENU_SCREEN;


        }
    }
}
void win_screen(void) {

    float width = (float)CP_System_GetWindowWidth();
    float height = (float)CP_System_GetWindowHeight();
    /*Load Image*/
    Win_Screen = CP_Image_Load("./Assets/Win_Screen.jpg");
    CP_Image_Draw(Win_Screen, width / 2, height / 2, width, height, 255);

    /*Buttons*/
    CP_Settings_Fill(COLOR_WHITE);
    float quarter_blockX = (float)CP_System_GetDisplayWidth() / 4;
    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 4;
    button_height = 120.f;
    button_width = 300.f;
    main_loseX = (quarter_blockX * 3) - button_width;
    restart_loseX = quarter_blockX;
    main_loseY = restart_loseY = quarter_blockY * 2.8f;
    CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
    CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(50);
    CP_Settings_Fill(COLOR_BLACK);
    restart_textX = restart_loseX + 40;
    restart_textY = restart_loseY + 80;
    main_textX = main_loseX + 35;
    main_textY = main_loseY + 75;
    CP_Font_DrawText("MAIN MENU", restart_textX, restart_textY);
    CP_Settings_TextSize(50);
    CP_Font_DrawText("NEXT LEVEL", main_textX, main_textY);


    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= restart_loseX && mouseX <= (restart_loseX + button_width) &&
        mouseY >= restart_loseY && mouseY <= restart_loseY + button_height) {

        CP_Image_Draw(Win_Screen, width / 2, height / 2, width, height, 255);

        //Hovering on Main Menu Button

        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);

        /*Text*/

        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("MAIN MENU", restart_textX, restart_textY);
        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("NEXT LEVEL", main_textX, main_textY);

        //When clicked, return back to main menu screen
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Win_Screen);
            Current_Gamestate = MAIN_MENU_SCREEN;


        }

    }
    else if (mouseX >= main_loseX && mouseX <= (main_loseX + button_width) && mouseY >= main_loseY && mouseY <= main_loseY + button_height) {

        //Hovering on Next Level Button
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(main_loseX, main_loseY, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect(restart_loseX, restart_loseY, button_width, button_height);


        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("MAIN MENU", restart_textX, restart_textY);
        CP_Settings_TextSize(50);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("NEXT LEVEL", main_textX, main_textY);



        //When clicked, return back to gameplay screen
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Win_Screen);
            current_level++; //Go to the next level

            Current_Gamestate = GAMEPLAY_SCREEN;

            /*initialise for gameplay screen*/
            minion_count = 0;
            reset_map_and_minions();
            initialise_level();
            gIsPaused = FALSE;
            restart_level();
        }


    }


}

/*Level Selector Screen*/
void level_selector_screen(void) {

    float width = (float)CP_System_GetWindowWidth();
    float height = (float)CP_System_GetWindowHeight();

    Level_Selector_Screen = CP_Image_Load("./Assets/Level_Selector_Screen.jpg");
    CP_Image_Draw(Level_Selector_Screen, width / 2, height / 2, width, height, 255);

    /*Buttons*/
    CP_Settings_Fill(COLOR_WHITE);

    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 2;


    button_height = 80.f;
    button_width = 300.f;

    /*Level 1 Buttons*/
    level1X = 600;
    level1Y = quarter_blockY;
    CP_Graphics_DrawRect(level1X, quarter_blockY, button_width, button_height);

    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);
    level1_textX = 690;
    level1_textY = quarter_blockY + 40;
    CP_Font_DrawText("Level 1", level1_textX, level1_textY);

    /*Level 2 buttons*/
    level2X = 600;
    level2Y = quarter_blockY + 120;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(level2X, level2Y, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level2_textY = level2Y + 40;
    CP_Font_DrawText("Level 2", 690, level2_textY);

    /*Level 3 buttons*/
    level3Y = quarter_blockY + 240;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(600, level3Y, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level3_textY = level3Y + 40;
    CP_Font_DrawText("Level 3", 690, level3_textY);

    /*Level 4 Buttons*/
    level1Y = quarter_blockY + 120;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1000, quarter_blockY, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level1_textY = quarter_blockY + 40;
    CP_Font_DrawText("Level 4", 1100, level1_textY);

    /*Level 5 Buttons*/
    level2Y = quarter_blockY + 120;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1000, level2Y, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level2_textY = level2Y + 40;
    CP_Font_DrawText("Level 5", 1100, level2_textY);

    /*Level 6 Buttons*/
    level3Y = quarter_blockY + 240;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1000, level3Y, button_width, button_height);
    /*Now Text*/
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level3_textY = level3Y + 40;
    CP_Font_DrawText("Level 6", 1100, level3_textY);

    /*Screen Settings Button*/

    button_height = 60.f;
    button_width = 240.f;
    level3Y = quarter_blockY + 240;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1600, level3Y + 200, button_width, button_height);
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    level3_textY = level3Y + 240;
    CP_Font_DrawText("Settings", 1620, level3_textY);

    /*Back to Menu Button*/
    button_height = 60.f;
    button_width = 150.f;
    float back_buttonY = 10;
    float back_buttonX = 10;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(back_buttonX, back_buttonY, button_width, button_height);
    CP_Settings_TextSize(40);
    CP_Settings_Fill(COLOR_BLACK);

    back_buttonY = back_buttonY + 40;
    CP_Font_DrawText("Back", 30, back_buttonY);


    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    /*Hovering on Level 1*/
    if (mouseX >= 600 && mouseX <= 900 &&
        mouseY >= 540 && mouseY <= 615 ) {

        button_height = 80.f;
        button_width = 300.f;

        /*Drawing of buttons*/
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(600, 540, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);
        level1_textY = quarter_blockY + 40;
        CP_Font_DrawText("Level 1", 690, level1_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            /*Free image*/
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 1;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;
            reset_map_and_minions();
            gIsPaused = FALSE;

            //initialise_pause_and_timer_button();
            restart_level();

        }


    }
    /*Hovering on Level 2 Buttons*/
    else if (mouseX >= 600 && mouseX <= 900 && mouseY >= level2Y && mouseY <= 740)
    {
        button_height = 80.f;
        button_width = 300.f;
        /*Level 2 buttons*/
        level2X = 600;
        level2Y = quarter_blockY + 120;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(level2X, level2Y, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level2_textY = level2Y + 40;
        CP_Font_DrawText("Level 2", 690, level2_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 2;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;
            reset_map_and_minions();
            gIsPaused = FALSE;

            //initialise_pause_and_timer_button();
            restart_level();

        }
    }
    /*Hovering on Level 3 Buttons*/
    else if (mouseX >= 600 && mouseX <= 900 && mouseY >= level3Y && mouseY <= 855)
    {
        button_height = 80.f;
        button_width = 300.f;
        level3Y = quarter_blockY + 240;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(600, level3Y, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level3_textY = level3Y + 40;
        CP_Font_DrawText("Level 3", 690, level3_textY);


        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 3;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;

            reset_map_and_minions();
            gIsPaused = FALSE;
            //initialise_pause_and_timer_button();
            restart_level();

        }

    }
    /*Hovering on level 4 Button*/
    else if (mouseX >= 1000 && mouseX <= 1310 && mouseY >= quarter_blockY && mouseY <= 620)
    {


        button_height = 80.f;
        button_width = 300.f;
        /*Level 4 Buttons*/
        level1Y = quarter_blockY + 120;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1000, quarter_blockY, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level1_textY = quarter_blockY + 40;
        CP_Font_DrawText("Level 4", 1100, level1_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 4;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;

            reset_map_and_minions();
            gIsPaused = FALSE;

            //initialise_pause_and_timer_button();
            restart_level();

        }

    }
    /*Hovering on level 5 Button*/
    else if (mouseX >= 1000 && mouseX <= 1300 && mouseY >= level2Y && mouseY <= 735) {
        /*Level 5 Buttons*/

        button_height = 80.f;
        button_width = 300.f;
        level2Y = quarter_blockY + 120;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1000, level2Y, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level2_textY = level2Y + 40;
        CP_Font_DrawText("Level 5", 1100, level2_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 5;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;

            reset_map_and_minions();
            gIsPaused = FALSE;

            //initialise_pause_and_timer_button();
            restart_level();

        }
    }
    /*Hovering on level 6 Button*/
    else if (mouseX >= 1000 && mouseX <= 1310 && mouseY >= level3Y && mouseY <= 855) {

        button_height = 80.f;
        button_width = 300.f;
        /*Level 6 Buttons*/
        level3Y = quarter_blockY + 240;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1000, level3Y, button_width, button_height);
        /*Now Text*/
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level3_textY = level3Y + 40;
        CP_Font_DrawText("Level 6", 1100, level3_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Level_Selector_Screen);
            current_level = 6;
            Current_Gamestate = GAMEPLAY_SCREEN;

            initialise_level();
            minion_count = 0;

            reset_map_and_minions();
            gIsPaused = FALSE;

            //initialise_pause_and_timer_button();
            restart_level();

        }

    }

    //Hovering on Setting Screen
    else if ((mouseX >= 1600 && mouseX <= 1840 && mouseY >= level3Y + 200 && mouseY <= 1040 ))
    {
        button_height = 60.f;
        button_width = 240.f;
        level3Y = quarter_blockY + 240;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1600, level3Y + 200, button_width, button_height);
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        level3_textY = level3Y + 240;
        CP_Font_DrawText("Settings", 1620, level3_textY);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            Current_Gamestate = SETTING_SCREEN;
        }

    }
    //Hovering on Back Button
    else if ((mouseX >= 10 && mouseX <= (10 + button_width) && mouseY >= 10 && mouseY <= 10 + button_height))
    {


        button_height = 60.f;
        button_width = 150.f;

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(10, 10, button_width, button_height);
        CP_Settings_TextSize(40);
        CP_Settings_Fill(COLOR_WHITE);

        back_buttonY = back_buttonY + 40;
        CP_Font_DrawText("Back", 30, 50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {

            Current_Gamestate = MAIN_MENU_SCREEN;


        }



    }



}

void setting_screen(void) {

    setting_image = CP_Image_Load("./Assets/bg_mainmenu.png"); //temp image
    //CP_Graphics_ClearBackground(COLOR_WHITE);
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(setting_image);
    height = (float)CP_Image_GetWidth(setting_image) * 0.6f;
    CP_Image_Draw(setting_image, middleX, middleY, width, height, 100);

    float startX = (float)CP_System_GetDisplayWidth() / 3;
    float startY = (float)CP_System_GetDisplayHeight() / 5;
    float option_textX = startX + 20.f;
    float option_textY = startY + 20.f;

    button_height = 120.f;
    button_width = 300.f;

    /*options buttons*/
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(startX, startY, button_width, button_height); //main menu
    CP_Graphics_DrawRect(startX, startY * 2, button_width, button_height); //level selection
    CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height); //hordepedia?
    CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height); //music?

     /*options text*/
    CP_Settings_TextSize(60);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("MAIN MENU", option_textX, option_textY + 60);
    CP_Font_DrawText("LEVEL", option_textX, option_textY * 2 + 40);
    CP_Font_DrawText("HELP", option_textX, option_textY * 3 + 20);
    CP_Font_DrawText("BGM?", option_textX, option_textY * 4);

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    Previous_Gamestate = MAIN_MENU_SCREEN; //initialise game state as main menu

    if (Current_Gamestate == MAIN_MENU_SCREEN) {
        Previous_Gamestate = MAIN_MENU_SCREEN;
    }

    else if (Current_Gamestate == LEVEL_SELECTOR_SCREEN) {
        Previous_Gamestate = LEVEL_SELECTOR_SCREEN;
    }

    else if (Current_Gamestate == GAMEPLAY_SCREEN) {
        Previous_Gamestate = GAMEPLAY_SCREEN;
    }


    if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY + button_height) {

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = MAIN_MENU_SCREEN;
        }
    }

    else if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY * 2 + button_height) {
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = LEVEL_SELECTOR_SCREEN;
        }

    }
    /*
    if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY * 3 + button_height) {
    }

    if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY * 4 + button_height) {
    } */



    else if (mouseX >= backX && mouseX <= (backX + back_width) &&
        mouseY >= backY && mouseY <= backY + back_height) {   
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {


            if (Previous_Gamestate == MAIN_MENU_SCREEN) {
                Current_Gamestate = MAIN_MENU_SCREEN;
            }

            else if (Previous_Gamestate == LEVEL_SELECTOR_SCREEN) {
                Current_Gamestate = LEVEL_SELECTOR_SCREEN;
            } 

            else if (Previous_Gamestate == GAMEPLAY_SCREEN) {
                Current_Gamestate = GAMEPLAY_SCREEN;

            }
        }
    }

}


void setting_screen_clicked(float x, float y) {

    /*Free image*/
    CP_Image_Free(&setting_image);

    if (Current_Gamestate == MAIN_MENU_SCREEN) {
        if (x >= settingX && x <= (settingX + setting_width) &&
            y >= settingY && y <= settingY + setting_height) {
            Current_Gamestate = SETTING_SCREEN;
        }
    }

    else if (Current_Gamestate == GAMEPLAY_SCREEN) {
        if (x >= setting_buttonX && x <= (setting_buttonX + button_width) &&
            y >= setting_buttonY && y <= setting_buttonY + button_height) {
            gIsPaused = TRUE;
            float startX = (float)CP_System_GetDisplayWidth() / 3;
            float startY = (float)CP_System_GetDisplayHeight() / 5;
            float option_textX = startX + 20.f;
            float option_textY = startY + 20.f;

            button_height = 120.f;
            button_width = 300.f;

            backX = 10.f;
            backY = 10.f;
            back_height = 80.f;
            back_width = 200.f;

            /*options buttons*/
            CP_Settings_Fill(COLOR_WHITE);
            CP_Graphics_DrawRect(startX, startY, button_width, button_height); //main menu
            CP_Graphics_DrawRect(startX, startY * 2, button_width, button_height); //level selection
            CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height); //hordepedia?
            CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height); //music?

            /*back button*/
            CP_Graphics_DrawRect(backX, backY, back_width, back_height);

            /*options text*/
            CP_Settings_TextSize(60);
            CP_Settings_Fill(COLOR_BLACK);
            CP_Font_DrawText("MAIN MENU", option_textX, option_textY + 60);
            CP_Font_DrawText("LEVEL", option_textX, option_textY * 2 + 40);
            CP_Font_DrawText("HELP", option_textX, option_textY * 3 + 20);
            CP_Font_DrawText("BGM?", option_textX, option_textY * 4);

            /*back text*/
            CP_Settings_TextSize(50);
            CP_Font_DrawText("BACK", 40.f, 60.f);

            float mouseX = (float)CP_Input_GetMouseX();
            float mouseY = (float)CP_Input_GetMouseY();


     if (Current_Gamestate == GAMEPLAY_SCREEN) {
        Previous_Gamestate = GAMEPLAY_SCREEN;
    }


    if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY + button_height) {

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = MAIN_MENU_SCREEN;
        }
    }

    else if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY && mouseY <= startY * 2 + button_height) {
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = LEVEL_SELECTOR_SCREEN;
        }

    }

            if (mouseX >= backX && mouseX <= (backX + back_width) &&
                mouseY >= backY && mouseY <= backY + back_height) {
                if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

                    CP_Image_Free(&setting_image);
                    gIsPaused = FALSE;
                }
            }
        }

        else if (Current_Gamestate == LEVEL_SELECTOR_SCREEN) {
            if (x >= 1600 && x <= (1600 + button_width) &&
                y >= level3Y && y <= level3Y + button_height) {
                Current_Gamestate = SETTING_SCREEN;
                // CP_Graphics_DrawRect(1600, level3Y + 200, button_width, button_height);
            }
        }
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
    for (int i = 0; i < ENEMY_MAX; i++) {
        array_EnemyStats[i][ENEMY_CURRENT_MINIONS_ON_BLOCK] = 0;
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        for (int j = 0; j < MINION_MAX; j++) {
            array_isMinionSlowed[i][j][0] = FALSE;
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
    setting_buttonX = gPauseButtonPositionX + 250.f;
    setting_buttonY = gPauseButtonPositionY - 30.f;
}

void draw_timer_and_pause_button(void) {
    CP_Font_Set(CP_Font_GetDefault());
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(gPauseButtonPositionX, gPauseButtonPositionY, 100.f, 50.f);
    CP_Graphics_DrawRect(gTimerPositionX, gTimerPositionY, 100.f, 50.f);
    CP_Graphics_DrawRect(setting_buttonX, setting_buttonY, 200.f, 80.f);
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
    CP_Settings_TextSize(50);
    CP_Font_DrawText("SETTING", (setting_buttonX + 10.f), (setting_buttonY + 50.f));
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
        array_MinionCurrentCharge[i][MINION_BASIC_CURRENT_CHARGE] += test;
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        array_EnemyCurrentCharge[i][ENEMY_BASIC_CURRENT_CHARGE] += test;
        array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] += test;
        array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] += test;
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
    minions_in_base = 0;
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
    options_boxY = origin_map_coordinateY + (MAP_GRID_ROWS * BLOCK_SIZE);
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect((float)options_boxX, (float)options_boxY, (float)box_length, (float)box_width);
    minion_buttons_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    minion_buttons_height = BLOCK_SIZE;
    for (int i = 1; i < 6; i++) {
        minion_boxX = options_boxX + (i * minion_buttons_width) - minion_buttons_width / 2;
        minion_boxY = options_boxY + (box_width / 4) - 20; //minion_boxY remains constant throughout
        CP_Graphics_DrawRect((float)minion_boxX, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
    }
    float minion_costboxY = (float)minion_boxY + 100.f;
    float minion_costbox_height = 50;
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
        CP_Font_DrawText((i == 0
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
    render_win_progress();
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

    if (current_level == 1) {
        level_1();
    }
    else if (current_level == 2) {
        level_2();
    }
    else if (current_level == 3) {
        level_3();
    }
    else if (current_level == 4) {
        level_4();
    }
    else if (current_level == 5) {
        level_5();
    }
    else if (current_level == 6) {
        level_6();
    }
    else if (current_level == 7) {

    }
    //and so on...

    setup_collaborative_diffusion_map();
    if (level_has_teleporter == TRUE) {
        setup_teleport_diffusion_map();
    }
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
                : array_GameMap[row][col] == BLOCK_TELEPORT_SPAWN
                ? COLOR_BRIGHT_BLUE
                : array_GameMap[row][col] == BLOCK_TELEPORTER
                ? COLOR_DULLER_BLUE
                : COLOR_GREY); //BLOCK_ENEMY
            CP_Graphics_DrawRect((float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
        }
    }
}

void setup_teleport_diffusion_map() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            array_Teleport_DiffusionMap[row][col][1] = FALSE;
            array_Teleport_DiffusionMap[row][col][0] = 0;
        }
    }
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_TELEPORTER) {
                array_Teleport_DiffusionMap[row][col][1] = TRUE; //when TRUE it means value has been filled and done
                array_Teleport_DiffusionMap[row][col][0] = 2048.f;
                teleport_row = row;
                teleport_col = col;
            }
            else if (array_GameMap[row][col] == BLOCK_TELEPORT_SPAWN) {
                teleport_spawn_row = row;
                teleport_spawn_col = col;
                teleport_spawn_X = origin_map_coordinateX + BLOCK_SIZE * col + BLOCK_SIZE / 2;
                teleport_spawn_Y = origin_map_coordinateY + BLOCK_SIZE * row + BLOCK_SIZE / 2;
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        for (int row = 0; row < MAP_GRID_ROWS; ++row) {
            for (int col = 0; col < MAP_GRID_COLS; ++col) {
                if (array_Teleport_DiffusionMap[row][col][1] == TRUE) { //value has been set in stone yay
                    if ((col - 1) >= 0 && array_Teleport_DiffusionMap[row][col - 1][1] == FALSE) {
                        if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY) {
                            array_Teleport_DiffusionMap[row][col - 1][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row][col - 1][1] = TRUE;
                        }
                    }
                    if ((col + 1) < MAP_GRID_COLS && array_Teleport_DiffusionMap[row][col + 1][1] == FALSE) {
                        if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY) {
                            array_Teleport_DiffusionMap[row][col + 1][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row][col + 1][1] = TRUE;
                        }
                    }
                    if ((row - 1) >= 0 && array_Teleport_DiffusionMap[row - 1][col][1] == FALSE) {
                        if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY) {
                            array_Teleport_DiffusionMap[row - 1][col][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row - 1][col][1] = TRUE;
                        }
                    }
                    if ((row + 1) < MAP_GRID_ROWS && array_Teleport_DiffusionMap[row + 1][col][1] == FALSE) {
                        if (array_GameMap[row + 1][col] == BLOCK_EMPTY || array_GameMap[row + 1][col] == BLOCK_ENEMY) {
                            array_Teleport_DiffusionMap[row + 1][col][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row + 1][col][1] = TRUE;
                        }
                    }
                }
            }
        }
    }
}

void setup_collaborative_diffusion_map() {
    //Reset everything to 0?
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            array_Collaborative_DiffusionMap[row][col][1] = FALSE;
            array_Collaborative_DiffusionMap[row][col][0] = 0;
        }
    }
    /*Find BLOCK_END*/
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_END) {
                array_Collaborative_DiffusionMap[row][col][1] = TRUE; //when TRUE it means value has been filled and done
                array_Collaborative_DiffusionMap[row][col][0] = 2048.f;
                spawn_row = row;
                spawn_col = col;
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        for (int row = 0; row < MAP_GRID_ROWS; ++row) {
            for (int col = 0; col < MAP_GRID_COLS; ++col) {
                if (array_Collaborative_DiffusionMap[row][col][1] == TRUE) { //value has been set in stone yay
                    if ((col - 1) >= 0 && array_Collaborative_DiffusionMap[row][col - 1][1] == FALSE) {
                        if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY) {
                            array_Collaborative_DiffusionMap[row][col - 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row][col - 1][1] = TRUE;
                        }
                    }
                    if ((col + 1) < MAP_GRID_COLS && array_Collaborative_DiffusionMap[row][col + 1][1] == FALSE) {
                        if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY) {
                            array_Collaborative_DiffusionMap[row][col + 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row][col + 1][1] = TRUE;
                        }
                    }
                    if ((row - 1) >= 0 && array_Collaborative_DiffusionMap[row - 1][col][1] == FALSE) {
                        if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY) {
                            array_Collaborative_DiffusionMap[row - 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row - 1][col][1] = TRUE;
                        }
                    }
                    if ((row + 1) < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[row + 1][col][1] == FALSE) {
                        if (array_GameMap[row + 1][col] == BLOCK_EMPTY || array_GameMap[row + 1][col] == BLOCK_ENEMY) {
                            array_Collaborative_DiffusionMap[row + 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row + 1][col][1] = TRUE;
                        }
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
                int which_enemy = check_which_enemy(row, col);
                array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[which_enemy][ENEMY_SIZE];
                array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[which_enemy][ENEMY_SIZE];
                if (level_has_been_reset == TRUE) {
                    assign_enemy_stats();
                    level_has_been_reset = FALSE;
                }
                if (array_EnemyStats[which_enemy][ENEMY_HP] > 0) {
                    CP_Settings_RectMode(CP_POSITION_CENTER);
                    assign_enemy_color(which_enemy);
                    CP_Graphics_DrawRect((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_SIZE], (float)array_EnemyStats[which_enemy][ENEMY_SIZE]);
                    CP_Settings_RectMode(CP_POSITION_CORNER);
                    renderguardhp_bar(which_enemy);
                    array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[which_enemy][ENEMY_SIZE];
                    array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[which_enemy][ENEMY_SIZE];

                    assign_enemy_color(which_enemy);
                    render_enemy_special_attack_bar(which_enemy);
                    if (array_EnemyStats[which_enemy][ENEMY_TYPE] == DAMAGE_ENEMY)
                    {
                        projectile_logic((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES]);
                    }
                }
                else if (array_EnemyStats[which_enemy][ENEMY_HP] <= 0) {
                    if (array_EnemyStats[which_enemy][ENEMY_TYPE] == GUARD_ENEMY) {
                        array_EnemyStats[which_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] = 0;
                        array_GameMap[row][col] = BLOCK_EMPTY;
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == SLOW_ENEMY || array_EnemyStats[which_enemy][ENEMY_TYPE] == HEALING_TOWER
                        || array_EnemyStats[which_enemy][ENEMY_TYPE] == RANGED_TOWER || array_EnemyStats[which_enemy][ENEMY_TYPE] == DAMAGE_ENEMY) {
                        array_EnemyStats[which_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] = 0;
                        array_GameMap[row][col] = BLOCK_PRESENT;
                    }
                }
            }
        }
    }
}

void projectile_logic(float x_coord, float y_coord)
{
    float right_limit = x_coord + (x_coord / 8);
    float left_limit = x_coord - (x_coord / 8);
    //float top_limit = y_coord + (y_coord / 4);
    float bot_limit = y_coord - (y_coord / 4);


    if ((float)array_MinionStats[0][X] <= right_limit && (float)array_MinionStats[0][X] >= left_limit)
    {
        if ((float)array_MinionStats[0][Y] >= bot_limit)
        {
            projectile_render(x_coord, y_coord);
        }
    }
}

void projectile_render(float x_coord, float y_coord)
{
    CP_Graphics_DrawRect(x_coord, y_coord, 4, 4);

}

void render_minion() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_SPAWN) {
                int SpawnX = origin_map_coordinateX + (BLOCK_SIZE * col) + (BLOCK_SIZE / 2);
                int SpawnY = origin_map_coordinateY + (BLOCK_SIZE * row) + (BLOCK_SIZE / 2); //makes it spawn in the middle of the spawn block
                if (minion_count < 7) {
                    array_MinionStats[minion_count][X] = SpawnX;
                    array_MinionStats[minion_count][Y] = SpawnY;
                    assign_minion_color(minion_count);
                    CP_Graphics_DrawCircle((float)array_MinionStats[minion_count][X], (float)array_MinionStats[minion_count][Y], (float)array_MinionStats[minion_count][MINION_SIZE]);
                    array_MinionStats[minion_count][MINION_TRAVEL_DIST] = 0;
                    array_MinionStats[minion_count][MINION_DIRECTION] = initial_direction;
                    array_MinionStats[minion_count][MINION_TELEPORTED] = FALSE;
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
        current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX) / BLOCK_SIZE;
        current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY) / BLOCK_SIZE;
        minion_special_attack(i, current_boxROW, current_boxCOL);
        /*now we check, we want to move in the direction of the one with the highest value*/
        array_MinionStats[i][MINION_PAST_DIRECTION] = array_MinionStats[i][MINION_DIRECTION];
        if (array_MinionStats[i][MINION_TRAVEL_DIST] < BLOCK_SIZE) {
            array_MinionStats[i][MINION_DIRECTION] = array_MinionStats[i][MINION_PAST_DIRECTION];
        }
        else if (level_has_teleporter == TRUE && array_MinionStats[i][MINION_TELEPORTED] == FALSE
            && array_MinionStats[i][MINION_TRAVEL_DIST] >= BLOCK_SIZE) {
            array_MinionStats[i][MINION_TRAVEL_DIST] = 0;
            array_MinionStats[i][MINION_DIRECTION] = //i'm pretty sure these conditions aren't working tbh
                (current_boxCOL + 1 < MAP_GRID_COLS && array_Teleport_DiffusionMap[current_boxROW][current_boxCOL + 1][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_TELEPORTER))
                ? RIGHT
                : (current_boxCOL - 1 >= 0 && array_Teleport_DiffusionMap[current_boxROW][current_boxCOL - 1][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_TELEPORTER))
                ? LEFT
                : (current_boxROW + 1 < MAP_GRID_ROWS && array_Teleport_DiffusionMap[current_boxROW + 1][current_boxCOL][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_TELEPORTER))
                ? DOWN
                : (current_boxROW - 1 >= 0 && array_Teleport_DiffusionMap[current_boxROW - 1][current_boxCOL][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_TELEPORTER))
                ? UP
                : STOP;
        }
        else if (array_MinionStats[i][MINION_TRAVEL_DIST] >= BLOCK_SIZE) {
            array_MinionStats[i][MINION_TRAVEL_DIST] = 0;
            array_MinionStats[i][MINION_DIRECTION] =
                (current_boxCOL + 1 < MAP_GRID_COLS && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL + 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_END))
                ? RIGHT
                : (current_boxCOL - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL - 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_END ))
                ? LEFT
                : (current_boxROW + 1 < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[current_boxROW + 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_END))
                ? DOWN
                : (current_boxROW - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW - 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_END))
                ? UP
                : STOP;
        }
        if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION && array_MinionStats[i][MINION_HP] > 0) {
            healer_minion_basic_heal(i);
        }
        if (array_GameMap[current_boxROW][current_boxCOL] == BLOCK_ENEMY) {
            int row_enemy, col_enemy;
            row_enemy = current_boxROW;
            col_enemy = current_boxCOL;
            int correct_enemy;
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
                        //array_MinionStats[i][MINION_DIRECTION] = array_MinionStats[i][MINION_PAST_DIRECTION];
                        array_isMinionBlocked[correct_enemy][i] = 0;
                    }
                    /*Minion attacking ground guard if blocked and guard attacking minion in return*/
                    else {
                        if (array_EnemyStats[correct_enemy][ENEMY_HP] > 0) {
                            array_MinionStats[i][MINION_DIRECTION] = STOP;
                            int check_if_can_attack = check_minion_basic_attack_charge(i);
                            if (array_MinionStats[i][MINION_HP] > 0 && check_if_can_attack == 1) {
                                array_EnemyStats[correct_enemy][ENEMY_HP] -= array_MinionStats[i][MINION_ATTACK];
                                if (array_EnemyStats[correct_enemy][ENEMY_HP] <= 0) {
                                    money += 20;
                                }
                            }
                            /*Single targeting system*/
                            int enemy_charged_up = check_enemy_basic_attack_charge(correct_enemy);
                            int previous_minion = i - 1;
                            if (enemy_charged_up == 1) {
                                if (previous_minion >= 0 && array_MinionStats[previous_minion][MINION_HP] > 0) {
                                    array_MinionStats[previous_minion][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];
                                }
                                else {
                                    int new_minion = previous_minion + 1;
                                    array_MinionStats[new_minion][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];
                                }
                            }
                            if (array_MinionStats[i][MINION_HP] <= 0) {
                                array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] -= array_MinionStats[i][MINION_WEIGHT];

                                if (array_isMinionBlocked[correct_enemy][i + 1] == 1) {
                                    array_isMinionBlocked[correct_enemy][i] = 1;
                                }
                                else {
                                    array_isMinionBlocked[correct_enemy][i] = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (array_MinionStats[i][MINION_DIRECTION] == UP || array_MinionStats[i][MINION_DIRECTION] == DOWN) {
            array_MinionStats[i][X] = array_MinionStats[i][X];
            array_MinionStats[i][Y] = (array_MinionStats[i][MINION_DIRECTION] == UP
                ? move_up
                : move_down);
            array_MinionStats[i][MINION_TRAVEL_DIST] += array_MinionStats[i][MINION_MOVEMENT_SPEED];
        }
        else if (array_MinionStats[i][MINION_DIRECTION] == LEFT || array_MinionStats[i][MINION_DIRECTION] == RIGHT) {
            array_MinionStats[i][Y] = array_MinionStats[i][Y];
            array_MinionStats[i][X] = (array_MinionStats[i][MINION_DIRECTION] == LEFT
                ? move_left
                : move_right);
            array_MinionStats[i][MINION_TRAVEL_DIST] += array_MinionStats[i][MINION_MOVEMENT_SPEED];
        }
        else if (array_MinionStats[i][MINION_DIRECTION] == STOP) {
            array_MinionStats[i][X] = array_MinionStats[i][X];
            array_MinionStats[i][Y] = array_MinionStats[i][Y];
            array_MinionStats[i][MINION_DIRECTION] = array_MinionStats[i][MINION_PAST_DIRECTION];
        }
        //Minion is now teleporting... yay
        if (array_GameMap[current_boxROW][current_boxCOL] == BLOCK_TELEPORTER) {
            array_MinionStats[i][MINION_TELEPORTED] = TRUE;
            array_MinionStats[i][X] = teleport_spawn_X;
            array_MinionStats[i][Y] = teleport_spawn_Y;
            array_MinionStats[i][MINION_TRAVEL_DIST] = 0;
        }
        if (array_MinionStats[i][MINION_HP] <= 0) {
            minion_dies_array_recycle(i);
        }
        else if (array_MinionStats[i][MINION_HP] > 0) { //only live minion are drawn
            assign_minion_color(i);
            CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
        }
    }
}

void healer_minion_basic_heal(int i) {
    int check_if_can_attack = check_minion_basic_attack_charge(i);
    if (check_if_can_attack == 1) {
        int minion_lowest_hp = 0;
        float full_hp1, full_hp2;
        full_hp1 = full_hp2 = 0;
        for (int j = 0; j < minion_count; j++) {
            full_hp1 = (float)find_full_hp(j);
            full_hp2 = (float)find_full_hp(minion_lowest_hp);
            float percentage_hp1 = ((float)array_MinionStats[j][MINION_HP] / full_hp1) * 100;
            float percentage_minion_lowest_hp = ((float)array_MinionStats[minion_lowest_hp][MINION_HP] / full_hp2) * 100;
            if (percentage_hp1 < percentage_minion_lowest_hp) {
                minion_lowest_hp = j;
            }
        }
        int check = (int)full_hp2 - array_MinionStats[minion_lowest_hp][MINION_HP];
        //if the amount of hp missing is less than the amount to be healed, check is the total healable hp
        if (check < array_MinionStats[i][MINION_HEAL]) {
            array_MinionStats[minion_lowest_hp][MINION_HP] += check;
        }
        else {
            array_MinionStats[minion_lowest_hp][MINION_HP] += array_MinionStats[i][MINION_HEAL];
        }
    }
}

void render_enemy_special_attack_bar(int i) {
    float charge_percentage;
    if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER || array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
        charge_percentage = (array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] / array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME]) * (float)default_hp_tower;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 50, (float)default_hp_tower, (float)HP_BAR_HEIGHT - 1);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 50, charge_percentage, (float)HP_BAR_HEIGHT - 1);
    }
}

int find_enemy_full_hp(int n) {
    int full_hp = (array_EnemyStats[n][ENEMY_TYPE] == GUARD_ENEMY)
        ? 150
        : (array_EnemyStats[n][ENEMY_TYPE] == SLOW_ENEMY)
        ? 120
        : (array_EnemyStats[n][ENEMY_TYPE] == HEALING_TOWER)
        ? 120
        : (array_EnemyStats[n][ENEMY_TYPE] == DAMAGE_ENEMY)
        ? 100
        : (array_EnemyStats[n][ENEMY_TYPE] == RANGED_TOWER)
        ? 150
        : 0;
    return full_hp;
}

void enemy_special_attack() {
    int how_long_effect_is = 3;
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] >= array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME]) {
            if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY && array_EnemyStats[i][ENEMY_HP] > 0) {
                array_Enemy_Slow_Effect_Time[i][CHECKER] = TRUE; //attack can start
                array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] = 0;
            }
            else if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER && array_EnemyStats[i][ENEMY_HP] > 0) {
                int healing_amount = 40;
                for (int j = 0; j < ENEMY_MAX; j++) {
                    int full_hp = find_enemy_full_hp(j);
                    if ((full_hp - array_EnemyStats[j][ENEMY_HP]) < healing_amount) {
                        array_EnemyStats[j][ENEMY_HP] = full_hp;
                    }
                    else {
                        array_EnemyStats[j][ENEMY_HP] += healing_amount;
                    }
                }
            }
            array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] = 0;
        }
        //The Slow Minion's Special Attack is here
        if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY && array_Enemy_Slow_Effect_Time[i][CHECKER] == TRUE) {
            if (array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] >= how_long_effect_is || array_EnemyStats[i][ENEMY_HP] <= 0) {
                array_Enemy_Slow_Effect_Time[i][CHECKER] = FALSE;
                array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] = 0;
                for (int j = 0; j < minion_count; j++) {
                    if (array_isMinionSlowed[i][j][0] == TRUE) {
                        array_MinionStats[j][MINION_MOVEMENT_SPEED] = array_isMinionSlowed[i][j][1];
                        array_isMinionSlowed[i][j][0] = FALSE;
                    }
                }
            }
            else if (array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] < how_long_effect_is) {
                int minX, maxX, minY, maxY;
                minX = array_EnemyStats[i][ENEMY_ROW_COORDINATES] - BLOCK_SIZE - (BLOCK_SIZE / 2);
                maxX = array_EnemyStats[i][ENEMY_ROW_COORDINATES] + BLOCK_SIZE + (BLOCK_SIZE / 2);
                minY = array_EnemyStats[i][ENEMY_COL_COORDINATES] - BLOCK_SIZE - (BLOCK_SIZE / 2);
                maxY = array_EnemyStats[i][ENEMY_COL_COORDINATES] + BLOCK_SIZE + (BLOCK_SIZE / 2);
                for (int j = 0; j < minion_count; j++) {
                    if (array_MinionStats[j][X] > minX && array_MinionStats[j][X] < maxX
                        && array_MinionStats[j][Y] > minY && array_MinionStats[j][Y] < maxY) {
                        if (array_isMinionSlowed[i][j][0] != TRUE) {
                            array_isMinionSlowed[i][j][1] = array_MinionStats[j][MINION_MOVEMENT_SPEED];
                        }
                        array_MinionStats[j][MINION_MOVEMENT_SPEED] = 1;
                        array_isMinionSlowed[i][j][0] = TRUE;
                    }
                    //Minion no longer in range
                    if (!(array_MinionStats[j][X] > minX && array_MinionStats[j][X] < maxX
                        && array_MinionStats[j][Y] > minY && array_MinionStats[j][Y] < maxY)) {
                        if (array_isMinionSlowed[i][j][0] == TRUE) {
                            array_MinionStats[j][MINION_MOVEMENT_SPEED] = array_isMinionSlowed[i][j][1];
                            array_isMinionSlowed[i][j][0] = FALSE;
                        }
                    }
                }
                float length = (float)BLOCK_SIZE * 3;
                CP_Settings_Fill(TRANSLUCENT_BLUE);
                CP_Settings_RectMode(CP_POSITION_CENTER);
                CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[i][ENEMY_COL_COORDINATES], length, length);
                CP_Settings_RectMode(CP_POSITION_CORNER);
            }
        }
    }
}

/*Should probably update this code to make it shorter lol*/
void minion_dies_array_recycle(int dead_minion_number) {
    int array_Temp_MinionStats[MINION_MAX][MINION_TOTAL_STATS];
    float array_Temp_MinionCharge[MINION_MAX][TOTAL_CHARGES];
    int array_Temp_isMinionSlowed[ENEMY_MAX][MINION_MAX][2];

    for (int i = 0; i < dead_minion_number; i++) {
        for (int j = 0; j < MINION_TOTAL_STATS; j++) {
            array_Temp_MinionStats[i][j] = array_MinionStats[i][j];
        }
    }
    for (int i = 0; i < dead_minion_number; i++) {
        for (int j = 0; j < TOTAL_CHARGES; j++) {
            array_Temp_MinionCharge[i][j] = array_MinionCurrentCharge[i][j];
        }
    }
    for (int k = 0; k < ENEMY_MAX; k++) {
        for (int i = 0; i < dead_minion_number; i++) {
            for (int j = 0; j < 2; j++) {
                array_Temp_isMinionSlowed[k][i][j] = array_isMinionSlowed[k][i][j];
            }
        }
    }

    int dead_min_num = dead_minion_number;
    for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
        for (int m = 0; m < MINION_TOTAL_STATS; m++) {
            array_Temp_MinionStats[dead_min_num][m] = array_MinionStats[k][m];
        }
    }
    dead_min_num = dead_minion_number;
    for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
        for (int m = 0; m < TOTAL_CHARGES; m++) {
            array_Temp_MinionCharge[dead_min_num][m] = array_MinionCurrentCharge[k][m];
        }
    }
    dead_min_num = dead_minion_number;
    for (int i = 0; i < ENEMY_MAX; i++) {
        for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
            for (int m = 0; m < 2; m++) {
                array_Temp_isMinionSlowed[i][dead_min_num][m] = array_isMinionSlowed[i][k][m];
            }
        }
    }
    //Now we update the original array with the stored values
    for (int h = 0; h < minion_count; h++) {
        for (int n = 0; n < MINION_TOTAL_STATS; n++) {
            array_MinionStats[h][n] = array_Temp_MinionStats[h][n];
        }
    }
    for (int h = 0; h < minion_count; h++) {
        for (int n = 0; n < TOTAL_CHARGES; n++) {
            array_MinionCurrentCharge[h][n] = array_Temp_MinionCharge[h][n];
        }
    }
    for (int k = 0; k < ENEMY_MAX; k++) {
        for (int h = 0; h < minion_count; h++) {
            for (int n = 0; n < 2; n++) {
                array_isMinionSlowed[k][h][n] = array_Temp_isMinionSlowed[k][h][n];
            }
        }
    }
    minion_count--;
}

/*There's issues with basic attack*/
int check_minion_basic_attack_charge(int i) {
    if (array_MinionCurrentCharge[i][MINION_BASIC_CURRENT_CHARGE] >= array_MinionCurrentCharge[i][MINION_BASIC_ATTACK_SPEED]) {
        array_MinionCurrentCharge[i][MINION_BASIC_CURRENT_CHARGE] = 0;
        return 1;
    }
    return 0;
}

int check_enemy_basic_attack_charge(int i) {
    if (array_EnemyCurrentCharge[i][ENEMY_BASIC_CURRENT_CHARGE] >= array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED]) {
        array_EnemyCurrentCharge[i][ENEMY_BASIC_CURRENT_CHARGE] = 0;
        return 1;
    }
    return 0;
}

void minion_enter_base_counter() {
    for (int i = 0; i < MINION_MAX; i++) {
        int current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX) / BLOCK_SIZE;
        int current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY) / BLOCK_SIZE;
        //int current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
        //int current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY + BLOCK_SIZE / 2 - 1) / BLOCK_SIZE;
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
        int buffer_space = 0;
        max_hp = (float)find_full_hp(i);
        if (array_MinionStats[i][MINION_HP] > 0) {
            if (array_MinionStats[i][MINION_TYPE] == SPAM_MINION) {
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;//to find current hp
                buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 18;
            }
            else if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
                buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 18;
            }
            else if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) {
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
                buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 18;
            }
            else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
                buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 18;
            }
            else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
                hp_percentage = array_MinionStats[i][MINION_HP] / max_hp;
                buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 18;
            }
            float new_hp_bar = hp_percentage * default_hp;
            float positionY = (float)array_MinionStats[i][Y] - (float)buffer_space;
            CP_Settings_Fill(COLOR_RED);
            CP_Graphics_DrawRect((float)array_MinionStats[i][X] - 25, positionY, (float)default_hp, (float)HP_BAR_HEIGHT); //max_hp
            CP_Settings_Fill(COLOR_GREEN);
            CP_Graphics_DrawRect((float)array_MinionStats[i][X] - 25, positionY, (float)new_hp_bar, (float)HP_BAR_HEIGHT);
        }
    }
}

void renderguardhp_bar(int i) {
    if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
        enemy_max_hp = 150;
        enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;//to find current hp
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
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
    CP_Settings_Fill(COLOR_RED);
    CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 60, (float)default_hp_tower, (float)HP_BAR_HEIGHT); //max_hp
    CP_Settings_Fill(COLOR_GREEN);
    CP_Graphics_DrawRect((float)array_EnemyStats[i][ENEMY_ROW_COORDINATES] - 40, (float)array_EnemyStats[i][ENEMY_COL_COORDINATES] - 60, enemy_hp_bar, (float)HP_BAR_HEIGHT);
}

void render_special_current_charge() {
    for (int i = 0; i < MINION_MAX; i++) {
        if (array_MinionStats[i][MINION_TYPE] != SPAM_MINION) {
            int buffer_space = 0;
            float charge_percentage;
            if (array_MinionStats[i][MINION_HP] > 0) {
                /*
                if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
                    buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 7;
                }
                */
                if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) {
                    buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 7;
                }
                else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
                    buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 7;
                }
                else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
                    buffer_space = array_MinionStats[i][MINION_SIZE] / 2 + 7;
                }
                charge_percentage = array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] / array_MinionCurrentCharge[i][MINION_CHARGE_TIME];
                float charge_bar = charge_percentage * default_hp;
                float positionY = (float)array_MinionStats[i][Y] - (float)buffer_space;
                CP_Settings_Fill(COLOR_BLACK);
                CP_Graphics_DrawRect((float)array_MinionStats[i][X] - 25, positionY, (float)default_hp, (float)HP_BAR_HEIGHT); //max_hp
                CP_Settings_Fill(COLOR_WHITE);
                CP_Graphics_DrawRect((float)array_MinionStats[i][X] - 25, positionY, (float)charge_bar, (float)HP_BAR_HEIGHT);
            }
        }
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
    CP_Font_DrawText("Money: ", (counter_X - 150.f), (counter_Y + 55));
    CP_Font_DrawText(money_buffer, (counter_X + 40.f), (counter_Y + 55));
}

void render_win_progress() {
    float width = 30;
    float height = 40;
    int x_coordinate = CP_System_GetWindowWidth() - origin_map_coordinateX - ((int)width * 10);
    int y_coordinate = origin_map_coordinateY - 50;
    //float percentage = ((float)minions_in_base / 10) * width;
    int x = 0;
    for (int i = 0; i < 10; i++) {
        x = x_coordinate + ((int)width * i);
        CP_Settings_Fill(COLOR_DARK_BLUE);
        CP_Graphics_DrawRect((float)x, (float)y_coordinate, width, height);
    }
    for (int i = 0; i < minions_in_base; i++) {
        x = x_coordinate + ((int)width * i);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)x, (float)y_coordinate, width, height);
    }
}

int find_full_hp(int n) {
    int full_hp = (array_MinionStats[n][MINION_TYPE] == SPAM_MINION)
        ? 50
        : (array_MinionStats[n][MINION_TYPE] == WARRIOR_MINION)
        ? 130
        : (array_MinionStats[n][MINION_TYPE] == TANK_MINION)
        ? 240
        : (array_MinionStats[n][MINION_TYPE] == WIZARD_MINION)
        ? 80
        : (array_MinionStats[n][MINION_TYPE] == HEALER_MINION)
        ? 120
        : 0;
    return full_hp;
}

void minion_special_attack(int i, int current_row, int current_col) {
    int tank_range = 2, wizard_range = 3;
    if (array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] >= array_MinionCurrentCharge[i][MINION_CHARGE_TIME]) {
        if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
            //insert code for whatever they can do here
        }
        else if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) { //restores HP and attack tower

            //array_MinionStats[i][MINION_SIZE] += 20;
            if (array_MinionStats[i][MINION_HP] < 140) {
                array_MinionStats[i][MINION_HP] += 100;
            }
            else {
                int hp_restored = 240 - array_MinionStats[i][MINION_HP];
                array_MinionStats[i][MINION_HP] += hp_restored;
            }

            minion_attacking_towers(i, current_row, current_col, tank_range);
            for (int t = 0; t < ENEMY_MAX; t++) {
                if (array_enemy_to_attack[i][t] == 1) {
                    array_EnemyStats[t][ENEMY_HP] -= 20;
                    if (array_EnemyStats[t][ENEMY_HP] <= 0) {
                        money += 20;
                    }
                }
            }
        }
        else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
            minion_attacking_towers(i, current_row, current_col, wizard_range);
            for (int t = 0; t < ENEMY_MAX; t++) {
                if (array_enemy_to_attack[i][t] == 1) {
                    array_EnemyStats[t][ENEMY_HP] -= 30;
                    if (array_EnemyStats[t][ENEMY_HP] <= 0) {
                        money += 20;
                    }
                }
            }
        }
        else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
            int full_health[MINION_MAX];
            for (int n = 0; n < MINION_MAX; ++n) {
                full_health[n] = find_full_hp(n);
            }
            for (int t = 0; t < minion_count; ++t) {
                if (array_MinionStats[t][MINION_HP] < full_health[i]) { //minion hp wont go higer than 100%
                    int check = full_health[t] - array_MinionStats[t][MINION_HP];
                    if (check < 50) {
                        array_MinionStats[t][MINION_HP] += check; //so it doesnt overheal
                    }
                    else {
                        array_MinionStats[t][MINION_HP] += 50;
                    }
                }
            }
        }
        array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
    }
}

void minion_attacking_towers(int i, int current_row, int current_col, int minion_range) { //can work on the minion's normal attack here, aka attacking towers while moving?
    int which_enemy, check_row, check_col;
    for (int t = 0; t < ENEMY_MAX; t++) {
        array_enemy_to_attack[i][t] = 0;
    }
    /*Case 1*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row - n;
        check_col = current_col - n;
        if (check_row >= 0 && check_col >= 0) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 2*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row;
        check_col = current_col - n;
        if (check_row >= 0 && check_col >= 0) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 3*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row + n;
        check_col = current_col - n;
        if (check_row < MAP_GRID_ROWS && check_col >= 0) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 4*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row - n;
        check_col = current_col;
        if (check_row >= 0 && check_col >= 0) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 5*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row + n;
        check_col = current_col;
        if (check_row < MAP_GRID_ROWS && check_col >= 0) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 6*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row - n;
        check_col = current_col + n;
        if (check_row >= 0 && check_col < MAP_GRID_COLS) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 7*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row;
        check_col = current_col + n;
        if (check_row >= 0 && check_col < MAP_GRID_COLS) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 8*/
    for (int n = 0; n < minion_range; ++n) {
        check_row = current_row + n;
        check_col = current_col + n;
        if (check_row < MAP_GRID_ROWS && check_col < MAP_GRID_COLS) {
            if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
                which_enemy = check_which_enemy(check_row, check_col);
                array_enemy_to_attack[i][which_enemy] = 1;
            }
        }
    }
    /*Case 9 - Current Row*/
    if ((check_row = current_row) >= 0 && (check_col = current_col) >= 0) {
        if (array_GameMap[check_row][check_col] == BLOCK_TOWER_ENEMY || array_GameMap[check_row][check_col] == BLOCK_ENEMY) {
            which_enemy = check_which_enemy(check_row, check_col);
            array_enemy_to_attack[i][which_enemy] = 1;
        }
    }
}

int check_which_enemy(int row, int col) { //input is the row and col to be checked for enemy presence
    int correct_enemy = 0;
    for (int r = 0; r < ENEMY_MAX; r++) {
        /*Finds out which is the right enemy, since there can be 10 enemies at a time*/
        if ((array_EnemyStats[r][ENEMY_ROW] == row) && (array_EnemyStats[r][ENEMY_COL] == col)) {
            correct_enemy = r;
        }
    }
    return correct_enemy;
}

/*IMPORTANT - BEFORE UPDATING ANY VALUE HERE, CTRL+F TO CHECK IF IT HAS BEEN USED ELSEWHERE AND UPDATE ACCORDINGLY*/
/*for example HP is used in rendering the hp bars. Thanks! :D*/
void assign_minion_stats() {
    if (array_MinionStats[minion_count][MINION_TYPE] == SPAM_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 50;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 6; //original speed was 8
        array_MinionStats[minion_count][MINION_ATTACK] = 10;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 30;
        array_MinionStats[minion_count][MINION_SIZE] = 50;
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 0.5f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WARRIOR_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 130;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 20;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 60;
        array_MinionStats[minion_count][MINION_SIZE] = 80;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 5; //one strong attack?
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 0.7f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == TANK_MINION) { //is tall so can attack tower
        array_MinionStats[minion_count][MINION_HP] = 240;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 3; //original speed was 3
        array_MinionStats[minion_count][MINION_ATTACK] = 1;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 2; //holds the line so other minions can pass
        array_MinionStats[minion_count][MINION_COST] = 120;
        array_MinionStats[minion_count][MINION_SIZE] = 120;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 2;
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 1.f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WIZARD_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 80;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 4;
        array_MinionStats[minion_count][MINION_ATTACK] = 4;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 160;
        array_MinionStats[minion_count][MINION_SIZE] = 70;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 4;
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 0.8f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == HEALER_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 120;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 3;
        array_MinionStats[minion_count][MINION_ATTACK] = 0; //i mean... self defence?
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 160;
        array_MinionStats[minion_count][MINION_SIZE] = 55;
        array_MinionStats[minion_count][MINION_HEAL] = 20;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 3; //super healing???
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 0.5f;
    }
}

void assign_enemy_color(int i) {
    if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
        CP_Settings_Fill(COLOR_RED);
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
        CP_Settings_Fill(COLOR_BLUE);
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
        CP_Settings_Fill(COLOR_CYAN);
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
        CP_Settings_Fill(COLOR_DULL_GREEN);
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
        CP_Settings_Fill(COLOR_PURPLE);
    }
}

/*probably going to be removed in the final product*/
void assign_minion_color(int i) { 
        if (array_MinionStats[i][MINION_TYPE] == SPAM_MINION) {
            CP_Settings_Fill(COLOR_BLUE);
        }
        else if (array_MinionStats[i][MINION_TYPE] == WARRIOR_MINION) {
            CP_Settings_Fill(COLOR_YELLOW);
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
            array_EnemyStats[i][ENEMY_ATTACK] = 5;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2; //idk how attack_speed works yet
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.4f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 100;
            array_EnemyStats[i][ENEMY_ATTACK] = 30;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
            array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME] = 4.f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
            array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME] = 4.5f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
            array_EnemyStats[i][ENEMY_HP] = 150;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 4;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == BASE) {
            array_EnemyStats[i][ENEMY_HP] = 250;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
        }
    }
}

void level_1() {
    array_GameMap[0][11] = BLOCK_SPAWN;
    array_GameMap[0][1] = BLOCK_END;
    
    array_GameMap[2][0] = BLOCK_PRESENT;
    array_GameMap[4][0] = BLOCK_PRESENT;
    array_GameMap[3][0] = BLOCK_PRESENT;
    array_GameMap[0][2] = BLOCK_PRESENT;
    array_GameMap[1][2] = BLOCK_PRESENT;
    array_GameMap[2][2] = BLOCK_PRESENT;
    array_GameMap[3][2] = BLOCK_PRESENT;
    array_GameMap[3][3] = BLOCK_PRESENT;
    array_GameMap[1][5] = BLOCK_PRESENT;
    array_GameMap[2][5] = BLOCK_PRESENT;
    array_GameMap[3][5] = BLOCK_PRESENT;
    array_GameMap[1][7] = BLOCK_PRESENT;
    array_GameMap[3][7] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[0][10] = BLOCK_PRESENT;
    array_GameMap[1][10] = BLOCK_PRESENT;
    array_GameMap[3][10] = BLOCK_PRESENT;
    
    array_GameMap[4][2] = BLOCK_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 4;
    array_EnemyStats[1][ENEMY_COL] = 2;
    array_EnemyStats[1][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[1][4] = BLOCK_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 1;
    array_EnemyStats[2][ENEMY_COL] = 4;
    array_EnemyStats[2][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[4][5] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 4;
    array_EnemyStats[3][ENEMY_COL] = 5;
    array_EnemyStats[3][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[0][6] = BLOCK_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 0;
    array_EnemyStats[4][ENEMY_COL] = 6;
    array_EnemyStats[4][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[1][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 1;
    array_EnemyStats[5][ENEMY_COL] = 6;
    array_EnemyStats[5][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[3][8] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 8;
    array_EnemyStats[6][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[4][8] = BLOCK_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 4;
    array_EnemyStats[7][ENEMY_COL] = 8;
    array_EnemyStats[7][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[2][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 2;
    array_EnemyStats[8][ENEMY_COL] = 10;
    array_EnemyStats[8][ENEMY_TYPE] = DAMAGE_ENEMY;
    
    initial_direction = DOWN;
    level_has_teleporter = FALSE;
}

void level_2() {
    array_GameMap[0][9] = BLOCK_SPAWN;
    array_GameMap[4][1] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[0][3] = BLOCK_PRESENT;
    array_GameMap[0][5] = BLOCK_PRESENT;
    array_GameMap[0][8] = BLOCK_PRESENT;
    array_GameMap[1][5] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[3][3] = BLOCK_PRESENT;
    array_GameMap[2][1] = BLOCK_PRESENT;
    array_GameMap[2][5] = BLOCK_PRESENT;
    array_GameMap[3][0] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[4][2] = BLOCK_PRESENT;

    initial_direction = DOWN;
    /*Enemies*/
    array_GameMap[2][7] = BLOCK_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 2;
    array_EnemyStats[0][ENEMY_COL] = 7;
    array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][4] = BLOCK_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 3;
    array_EnemyStats[1][ENEMY_COL] = 4;
    array_EnemyStats[1][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[2][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 2;
    array_EnemyStats[2][ENEMY_COL] = 3;
    array_EnemyStats[2][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[4][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 4;
    array_EnemyStats[3][ENEMY_COL] = 6;
    array_EnemyStats[3][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[1][1] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 1;
    array_EnemyStats[4][ENEMY_COL] = 1;
    array_EnemyStats[4][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[1][3] = BLOCK_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 1;
    array_EnemyStats[5][ENEMY_COL] = 3;
    array_EnemyStats[5][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][1] = BLOCK_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 1;
    array_EnemyStats[6][ENEMY_TYPE] = GUARD_ENEMY;

    level_has_teleporter = FALSE;
}

void level_3() {
    array_GameMap[0][10] = BLOCK_SPAWN;
    array_GameMap[0][1] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[0][3] = BLOCK_PRESENT;
    array_GameMap[0][11] = BLOCK_PRESENT;
    array_GameMap[0][6] = BLOCK_PRESENT;
    array_GameMap[1][2] = BLOCK_PRESENT;
    array_GameMap[1][6] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[1][9] = BLOCK_PRESENT;
    array_GameMap[1][10] = BLOCK_PRESENT;
    array_GameMap[2][2] = BLOCK_PRESENT;
    array_GameMap[2][6] = BLOCK_PRESENT;
    array_GameMap[3][0] = BLOCK_PRESENT;
    array_GameMap[3][4] = BLOCK_PRESENT;
    array_GameMap[3][7] = BLOCK_PRESENT;
    array_GameMap[3][8] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[4][4] = BLOCK_PRESENT;

    initial_direction = LEFT;

    /*Enemies*/
    array_GameMap[1][1] = BLOCK_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 1;
    array_EnemyStats[0][ENEMY_COL] = 1;
    array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[1][3] = BLOCK_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 1;
    array_EnemyStats[1][ENEMY_COL] = 3;
    array_EnemyStats[1][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[2][7] = BLOCK_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 2;
    array_EnemyStats[2][ENEMY_COL] = 7;
    array_EnemyStats[2][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[4][11] = BLOCK_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 4;
    array_EnemyStats[3][ENEMY_COL] = 11;
    array_EnemyStats[3][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[0][4] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 0;
    array_EnemyStats[4][ENEMY_COL] = 4;
    array_EnemyStats[4][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[3][2] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 3;
    array_EnemyStats[5][ENEMY_COL] = 2;
    array_EnemyStats[5][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[3][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 10;
    array_EnemyStats[6][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[2][4] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 2;
    array_EnemyStats[7][ENEMY_COL] = 4;
    array_EnemyStats[7][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[0][2] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 0;
    array_EnemyStats[8][ENEMY_COL] = 2;
    array_EnemyStats[8][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[9][ENEMY_ROW] = 3;
    array_EnemyStats[9][ENEMY_COL] = 6;
    array_EnemyStats[9][ENEMY_TYPE] = RANGED_TOWER;

    level_has_teleporter = FALSE;
}

/*Example on how to use Teleporter*/
void level_4() {
    array_GameMap[2][5] = BLOCK_SPAWN;
    array_GameMap[4][2] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[1][0] = BLOCK_PRESENT;
    array_GameMap[1][1] = BLOCK_PRESENT;
    array_GameMap[3][1] = BLOCK_PRESENT;
    array_GameMap[4][1] = BLOCK_PRESENT;
    array_GameMap[3][3] = BLOCK_PRESENT;
    array_GameMap[3][5] = BLOCK_PRESENT;
    array_GameMap[3][6] = BLOCK_PRESENT;
    array_GameMap[3][7] = BLOCK_PRESENT;
    array_GameMap[1][4] = BLOCK_PRESENT;
    array_GameMap[2][4] = BLOCK_PRESENT;
    array_GameMap[2][8] = BLOCK_PRESENT;
    array_GameMap[2][9] = BLOCK_PRESENT;
    array_GameMap[1][9] = BLOCK_PRESENT;
    array_GameMap[0][9] = BLOCK_PRESENT;
    array_GameMap[4][11] = BLOCK_PRESENT;

    /*Enemies*/
    array_GameMap[0][5] = BLOCK_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 0;
    array_EnemyStats[0][ENEMY_COL] = 5;
    array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[4][6] = BLOCK_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 4;
    array_EnemyStats[1][ENEMY_COL] = 6;
    array_EnemyStats[1][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][2] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 3;
    array_EnemyStats[2][ENEMY_COL] = 2;
    array_EnemyStats[2][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[3][4] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 3;
    array_EnemyStats[3][ENEMY_COL] = 4;
    array_EnemyStats[3][ENEMY_TYPE] = HEALING_TOWER;

    array_GameMap[4][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 4;
    array_EnemyStats[4][ENEMY_COL] = 10;
    array_EnemyStats[4][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[2][2] = BLOCK_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 2;
    array_EnemyStats[5][ENEMY_COL] = 2;
    array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;

    
    initial_direction = UP;

    /*Using Teleporter*/
    level_has_teleporter = TRUE;
    array_GameMap[4][0] = BLOCK_TELEPORTER;
    array_GameMap[0][10] = BLOCK_TELEPORT_SPAWN;
}

void level_5() {
    array_GameMap[4][11] = BLOCK_SPAWN;
    array_GameMap[4][4] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[3][11] = BLOCK_PRESENT;
    array_GameMap[3][10] = BLOCK_PRESENT;
    array_GameMap[2][1] = BLOCK_PRESENT;
    array_GameMap[2][10] = BLOCK_PRESENT;
    array_GameMap[4][6] = BLOCK_PRESENT;
    array_GameMap[4][5] = BLOCK_PRESENT;
    array_GameMap[3][6] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[1][7] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[0][9] = BLOCK_PRESENT;
    array_GameMap[0][0] = BLOCK_PRESENT;
    array_GameMap[1][1] = BLOCK_PRESENT;
    array_GameMap[3][3] = BLOCK_PRESENT;
    array_GameMap[2][4] = BLOCK_PRESENT;
    array_GameMap[2][3] = BLOCK_PRESENT;
    array_GameMap[4][3] = BLOCK_PRESENT;

    /*Enemies*/
    array_GameMap[4][8] = BLOCK_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 4;
    array_EnemyStats[0][ENEMY_COL] = 8;
    array_EnemyStats[0][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][8] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 3;
    array_EnemyStats[1][ENEMY_COL] = 8;
    array_EnemyStats[1][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[3][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 3;
    array_EnemyStats[2][ENEMY_COL] = 3;
    array_EnemyStats[2][ENEMY_TYPE] = DAMAGE_ENEMY;

    array_GameMap[1][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 1;
    array_EnemyStats[3][ENEMY_COL] = 3;
    array_EnemyStats[3][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[0][5] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 0;
    array_EnemyStats[4][ENEMY_COL] = 5;
    array_EnemyStats[4][ENEMY_TYPE] = HEALING_TOWER;

    array_GameMap[2][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 2;
    array_EnemyStats[5][ENEMY_COL] = 6;
    array_EnemyStats[5][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[2][7] = BLOCK_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 2;
    array_EnemyStats[6][ENEMY_COL] = 7;
    array_EnemyStats[6][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][5] = BLOCK_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 3;
    array_EnemyStats[7][ENEMY_COL] = 5;
    array_EnemyStats[7][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[2][2] = BLOCK_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 2;
    array_EnemyStats[8][ENEMY_COL] = 2;
    array_EnemyStats[8][ENEMY_TYPE] = GUARD_ENEMY;

    /*Using Teleporter*/
    level_has_teleporter = TRUE;
    array_GameMap[2][11] = BLOCK_TELEPORTER;
    array_GameMap[1][0] = BLOCK_TELEPORT_SPAWN;


    initial_direction = LEFT;
    /*placeholders, please change everything*/
}

void level_6() {
    
}