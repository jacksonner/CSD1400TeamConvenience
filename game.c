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
#define BLOCK_ENEMY_DEAD 6 
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
#define COLOR_BG CP_Color_Create(30, 30, 30, 255)
#define COLOR_GREY CP_Color_Create(100, 100, 100, 255)
#define COLOR_WHITE CP_Color_Create(255, 255, 255, 255)
#define COLOR_RED CP_Color_Create(255, 0, 0, 255)
#define COLOR_GREEN CP_Color_Create(0, 255, 0, 255)
#define COLOR_SEAGREEN CP_Color_Create(46, 139, 87, 255)
#define COLOR_BLUE CP_Color_Create(30, 144, 255, 255)
#define TEXT_COLOR CP_Color_Create(0, 0, 0, 255) //Text colour is just black though...?
#define COLOR_BROWN CP_Color_Create(165, 42, 42, 255)
#define COLOR_CYAN CP_Color_Create(0, 255, 255, 255)
#define COLOR_PURPLE CP_Color_Create(138, 43, 226, 255)
#define TRANSLUCENT_WHITE CP_Color_Create(255, 255, 255, 200)
#define COLOR_BRIGHT_BLUE CP_Color_Create(0, 204, 204, 255)
#define COLOR_DULLER_BLUE CP_Color_Create(0, 76, 153, 255)
#define TRANSLUCENT_BLUE CP_Color_Create(204, 255, 255, 100)
#define COLOR_DULL_GREEN CP_Color_Create(0, 153, 0, 255)
#define COLOR_DARK_BLUE CP_Color_Create(0, 102, 204, 255)
#define COLOR_YELLOW CP_Color_Create(255, 255, 0, 255)
#define COLOR_TURQUOISE CP_Color_Create(99, 212, 178, 255)
#define COLOR_WEIRD_PURPLE CP_Color_Create(153, 0, 153, 255)
#define TRANSLUCENT_PURPLE CP_Color_Create(204, 153, 255, 100)
#define TRANSLUCENT_GREEN CP_Color_Create(0, 230, 0, 80)
#define COLOR_MORE_BLUE CP_Color_Create(153, 204, 255, 255)
#define TRANSLUCENT_ORANGE CP_Color_Create(255, 153, 51, 100)
#define COLOR_HEALER_GREEN CP_Color_Create(0, 102, 0, 255)
#define COLOR_LIGHT_GREY CP_Color_Create(224, 224, 224, 255)
#define COLOR_GREYISH_BLUE CP_Color_Create(201, 223, 235, 255)
#define TRANSLUCENT_PINK CP_Color_Create(255, 204, 255, 100)

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
void render_special_current_charge(void);
float array_minion_attack_time[MINION_MAX][2];
float array_minion_effect[MINION_MAX]; //stores the size of the tank Minion's effect lol
void render_minion_special_attack(void);

/*Types of Minions*/
#define SPAM_MINION 0 //weak everything, but low cost
#define WARRIOR_MINION 1 //decent health, decent attack
#define TANK_MINION 2 //tanky but low attack //targets tower but very low damage rip
#define WIZARD_MINION 3 //low health, after x amount of time, lightning from the skies hits all enemies BUT not so high damage ||
                               //limited range BUT higher damage //targets towers
#define HEALER_MINION 4 //decent health, no attack, heal other minions, relatively ex

/*All Sprite Images + Block Images*/
static CP_Image spam_minion;
static CP_Image warrior_minion;
static CP_Image tank_minion;
static CP_Image wizard_minion;
static CP_Image healer_minion;
static CP_Image fspam_minion; //flipped spam minion so it doesn't look like they're moving backwards
static CP_Image fwarrior_minion;
static CP_Image ftank_minion;
static CP_Image fwizard_minion;
static CP_Image fhealer_minion;
static CP_Image guard_enemy;
static CP_Image slow_tower;
static CP_Image healing_tower;
static CP_Image ranged_tower;
static CP_Image empty_block;
static CP_Image spawn_block;
static CP_Image base_block0;
static CP_Image base_block1;
static CP_Image base_block2;
static CP_Image base_block3;
static CP_Image present_block;
static CP_Image tutorial_minion;
static CP_Image ftutorial_minion;
static CP_Image dummy_enemy;
static CP_Image dummy_enemy_tower;
void load_all_sprites(void);
void render_minion_sprite(int minion);
void free_all_sprites(void);

/*Music and BGM stuff*/
static CP_Sound minion_voice;
static CP_Sound knock;

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
#define ENEMY_BASIC_ATTACK_SPEED 0 //this is attack speed in the main array...
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
float array_enemy_attack_time[ENEMY_MAX][2];
void render_enemy_special_attack_bar(int i);
int find_enemy_full_hp(int j);
void render_special_effect_enemy(int i);
int find_minion_original_speed(int minion);

/*Enemy Single Targetting*/
int array_isMinionAttacked[ENEMY_MAX][MINION_MAX];
int is_minion_being_attacked(int enemy, int minion);

/*Types of Enemies*/
#define GUARD_ENEMY 0 //block minions
#define DAMAGE_ENEMY 1 // the projectile never came to be
#define SLOW_ENEMY 2 //slow down minion around it
#define HEALING_TOWER 3 //heals enemies around it
#define RANGED_TOWER 4 // AOE
#define DUMMY_ENEMY 5 //used in tutorial
#define DUMMY_TOWER 6

/*Gamestate*/
#define GAMEPLAY_SCREEN 0
#define MAIN_MENU_SCREEN 1
#define WIN_SCREEN 2
#define LOSE_SCREEN 3
#define PAUSE_SCREEN 4 //like a translucent array with a giant play button?
#define LEVEL_SELECTOR_SCREEN 5
#define SETTING_SCREEN 6
#define HELP_SCREEN 7
#define HELP_SCREENP2 8
#define HELP_SCREENP3 9
#define QUIT_SCREEN 10
#define CREDIT_SCREEN 11
#define CREDIT_SCREENP2 12
#define TUTORIAL_SCREEN 13
#define TUTORIAL_SCREEN2 14
#define TUTORIAL_SCREEN3 15
#define TUTORIAL_SCREEN4 16
#define TUTORIAL_SCREEN5 17
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
float healer_minion_basic_heals[MINION_MAX][2];
int array_minion_lowest_hp[MINION_MAX];
int healer_default_effect_size = 120;
#define ALPHA 0
#define R 1
#define G 2
#define B 3
int array_alpha[MINION_MAX][4];

/*checking surrounding squares for AOE attacks*/
int array_enemy_to_attack[MINION_MAX][ENEMY_MAX];
int check_which_enemy(int row, int col);

/*Restart level*/
float restartX, restartY, restart_length, restart_width;
void restart_level();
void display_restart_button(void); //found in gameplay_screen

/*Win Condition Related*/
/*Counter for no. of Minions who entered the base*/
int minions_in_base;
void minion_enter_base_counter(void);
void render_win_progress();

/*hover effect during gameplay*/
void enemy_info(void);
void minion_info(void);

/**/
#define FALSE 0
#define TRUE 1
int level_has_been_reset; //checks if level has reset so stats won't constantly be reassigned making the enemies immortal

/*Levels*/
void level_tutorial(void);
void level_1(void);
void level_2(void);
void level_3(void);
void level_4(void);
void level_5(void);
void level_6(void);
int current_level;

/*Tutorial*/
int tutorial_played;
static CP_Image tutorial_background;
int play_effect_once[10];
int minion_position[2]; //x and y
void tutorial(void);
void tutorial1(void);
void tutorial2(void);
void tutorial3(void);
void tutorial4(void);
int tutorial_part;

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

/*Projectile Variables*/
#define PROJ_MAX 5
#define PROJ_X 0
#define PROJ_Y 1
#define PROJ_SPEED 2
#define IS_ALIVE 3
#define PROJ_SIZE 20
#define PROJ_RELOAD 4
#define PROJ_CHARGE 5
#define PROJ_CHARGE_TIME 6
#define PROJ_TOTAL_CHARGE 7
#define PROJ_STATS 8
float projectile[ENEMY_MAX][PROJ_MAX][PROJ_STATS];
//float projectile_charge[PROJ_MAX][PROJ_TOTAL_CHARGE];
float l_time[ENEMY_MAX];
//int l_time = 0;
void projectile_move(int which_enemy, int i);
void projectile_recycle(int which_enemy, int dead_projectile);
void projectile_colliding(int which_enemy, int i);
//int check_projectile_basic_attack_charge(int which_enemy, int i);
int array_target[ENEMY_MAX][MINION_MAX][2];
int in_range[ENEMY_MAX];
int array_is_attacking[ENEMY_MAX];
//int in_range = 0;
int proj_count = 0;
int target_lock = 0;
//int is_Search = 0;
void projectile_logic();
void projectile_render();

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
int money = 60;
int money_test = 0;

/*Enemy Guard Comes back to life*/
void bring_back_enemy(void);
#define ENEMY_DEATH_TIMER 0
#define ENEMY_DEATH_COUNTER 1
#define ENEMY_DEATH_TIMER_STARTED 2
float array_enemy_death_timer[ENEMY_MAX][3];
int death_recharge_time = 20;
void render_enemy_death_comeback_bar(void);

/*GamePlay Screen*/
int options_boxX, options_boxY, box_width, box_length; //this is the giant wide box atm which all the options etc. goes in
int minion_buttons_width, minion_buttons_height, minion_boxX, minion_boxY;
void gameplay_screen_clicked(float x, float y);

/*Help Screen*/
static CP_Image guide_image1;
static CP_Image guide_image2;
static CP_Image guide_image3;
int current_guide;
void help_screen();
void help_screen2();
void help_screen3();

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

/*Credit Screen*/
void credit_screen(void);
void credit_screen_clicked(float x, float y);
CP_Image Credit_Screen = NULL;
CP_Image Credit_Screen2 = NULL;
float creditbackX, creditbackY, creditX, creditY;
float credit_textX, credit_textY;
void credit_screen2(void);

/*Level Selector Screen*/
void level_selector_screen(void);
float level1X, level1Y, level1_textX, level1_textY;
float level2X, level2Y, level2_textX, level2_textY;
float level3X, level3Y, level3_textY, level3_textY;
float level4X, level4Y, level4_textY, level4_textY;
float level5X, level5Y, level5_textY, level5_textY;
float level6X, level6Y, level6_textY, level6_textY;
CP_Image Level_Selector_Screen = NULL;

/*Setting Screen*/
void setting_screen(void);
void setting_screen_clicked(float x, float y);
static CP_Image setting_image;
float backX, backY, setting_width, setting_height;
float back_width, back_height;
int Previous_Gamestate;
int setting_popup;

/*Quit Screen*/
void quit_screen(void);
static CP_Image quit_image;

/*Move Minion*/
int initial_direction; //when setting up level, check for the initial direction to set this to
void move_minion(void);

/*HP*/
int find_full_hp(int i);

/*Functions*/
void reset_map_and_minions(void);
void render_background(void); //for the gameplay_screen
void gameplay_screen(void);

void initialise_level(void);
void setup_collaborative_diffusion_map(void);
void render_minion(void);
void assign_minion_stats(void);
void assign_enemy_stats(void);
void render_enemy(void);

void game_init(void) {
    /*Setting the FrameRate to 60fps*/
    CP_System_SetFrameRate(60.0f);

    /*Initialising variables for Fullscreen etc.*/
    //CP_System_Fullscreen();
    CP_System_SetWindowSize(1920, 1080);
    update_variables_and_make_screen_nice();

    current_guide = 1;
    tutorial_played = FALSE;

    /*Initialise to Main_Menu*/
    Current_Gamestate = MAIN_MENU_SCREEN;

    current_level = 1;

    /* get dt, time elapsed from last frame*/
    currentElapsedTime = CP_System_GetDt();

    /*updating total elapsed time*/
    totalElapsedTime = 0;
    totalElapsedTime += currentElapsedTime;

    /*things for my tutorial*/
    for (int i = 0; i < 10; i++) {
        play_effect_once[i] = 0;
    }
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
        draw_timer_and_pause_button();
        display_money_counter();
        render_enemy();
        enemy_info();
        minion_info();
        if (current_level == 0) {
            int endX, endY, spawnX, spawnY;
            endX = endY = spawnX = spawnY = 0;
            float window_width = (float)CP_System_GetWindowWidth();
            float window_height = (float)CP_System_GetWindowHeight();
            float minion_topY = (float)origin_map_coordinateY + 100; //used in tutorial part 8
            if (tutorial_part == 0 || tutorial_part == 1 || tutorial_part == 2) {
                for (int row = 0; row < MAP_GRID_ROWS; ++row) {
                    for (int col = 0; col < MAP_GRID_COLS; ++col) {
                        if (array_GameMap[row][col] == BLOCK_END) {
                            endX = origin_map_coordinateX + BLOCK_SIZE * col + (BLOCK_SIZE/2); //top left corner of the block's coordinates
                            endY = origin_map_coordinateY + BLOCK_SIZE * row + (BLOCK_SIZE / 2);
                        }
                        if (array_GameMap[row][col] == BLOCK_SPAWN) {
                            spawnX = origin_map_coordinateX + BLOCK_SIZE * col + (BLOCK_SIZE / 2); //top left corner of the block's coordinates
                            spawnY = origin_map_coordinateY + BLOCK_SIZE * row + (BLOCK_SIZE / 2);
                        }
                    }
                }
                CP_Settings_Fill(COLOR_WHITE);
                CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
                CP_Settings_Fill(COLOR_BLACK);
                CP_Settings_TextSize(60);
                if (tutorial_part == 0) {
                    money = 0;
                    CP_Image_Draw(tutorial_minion, window_width - 500, window_height - 300, 300, 300, 255);
                    CP_Font_DrawText("Welcome to the training ground!", 120, window_height - 130);
                    CP_Font_DrawText("To begin with, let's go through some things which will come in useful.", 120, window_height - 80);
                }
                if (tutorial_part == 1) {
                    CP_Graphics_ClearBackground(COLOR_BLACK);
                    money = 0;
                    CP_Font_DrawText("First of all, we have the cell. This is where the Squares keep ", 120, window_height - 130);
                    CP_Font_DrawText("our people trapped. We need to break in.", 120, window_height - 80);
                    CP_Image_Draw(tutorial_minion, (float)endX + 280, (float)endY + 200, 200, 200, 255);
                    CP_Settings_Fill(TRANSLUCENT_PINK);
                    CP_Graphics_DrawCircle((float)endX, (float)endY, 2 * (float)BLOCK_SIZE);
                    minion_position[0] = endX + 280; //X coordinates
                    minion_position[1] = endY + 200; //Y coordinates
                }
                if (tutorial_part == 2) {
                    money = 0;
                    if (minion_position[0] < spawnX - 200) {
                        minion_position[0] += 12;
                    }
                    CP_Image_Draw(ftutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    if (minion_position[0] >= spawnX - 200) {
                        CP_Font_DrawText("Now, our team will enter from the portal over here", 120, window_height - 130);
                        CP_Font_DrawText("before steadily making their way to the cell.", 120, window_height - 80);
                        CP_Settings_Fill(TRANSLUCENT_PINK);
                        CP_Graphics_DrawCircle((float)spawnX, (float)spawnY, 2 * (float)BLOCK_SIZE);
                    }
                }
                CP_Settings_Fill(COLOR_GREY);
                CP_Settings_TextSize(30);
                CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 60);
            }
            if (tutorial_part == 3 || tutorial_part == 4 || tutorial_part == 5 || tutorial_part == 6
                || tutorial_part == 7) {
                CP_Settings_Fill(COLOR_WHITE);
                CP_Graphics_DrawRect(100, window_height - 450, window_width - 200, 150);
                CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                CP_Settings_Fill(COLOR_BLACK);
                CP_Settings_TextSize(55);
                if (tutorial_part == 3) {
                    CP_Font_DrawText("Come, try sending out a scout by clicking on the button here.", 120, window_height - 365);
                    CP_Settings_Fill(TRANSLUCENT_PINK);
                    float x_coordinate = (float)minion_boxX - 3.f * (float)minion_buttons_width - (float)minion_buttons_width / 2.f;
                    CP_Graphics_DrawCircle(x_coordinate, (float)minion_boxY + (float)minion_buttons_height / 2.f, 2 * (float)BLOCK_SIZE);
                }
                if (tutorial_part == 4) {
                    CP_Font_DrawText("Oh, I forgot! To summon a minion you need to have money first.", 120, window_height - 365);
                    CP_Settings_Fill(TRANSLUCENT_PINK);
                    //location of the money box
                    float counter_X = (float)CP_System_GetWindowWidth() - (float)origin_map_coordinateX - (float)BLOCK_SIZE + 75;
                    float counter_Y = (float)origin_map_coordinateY + (float)MAP_GRID_ROWS * (float)BLOCK_SIZE + 50;
                    CP_Graphics_DrawCircle(counter_X, counter_Y, 160);
                }
                if (tutorial_part == 5) {
                    CP_Font_DrawText("Alright this is better now.", 120, window_height - 365);
                    money = 30;
                    gIsPaused = FALSE;
                }
                if (tutorial_part == 6) {
                    money = 0;
                    CP_Font_DrawText("Congrats on sending out your first minion! However, do remember", 120, window_height - 380);
                    CP_Font_DrawText("that you can only send out 7 minions at a time because reasons.", 120, window_height - 320);
                }
                if (tutorial_part == 7) {
                    money = 0;
                    CP_Font_DrawText("Now, in order to rescue our friends, we need 10 minions", 120, window_height - 380);
                    CP_Font_DrawText("to reach the cell and break the chains.", 120, window_height - 320);
                }
                CP_Settings_Fill(COLOR_GREY);
                CP_Settings_TextSize(30);
                CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 310);
            }
            if (tutorial_part == 8 || tutorial_part == 9 || tutorial_part == 10 || tutorial_part == 11
                || tutorial_part == 12 || tutorial_part == 13 || tutorial_part == 14 || tutorial_part == 15
                || tutorial_part == 16 || tutorial_part == 17 || tutorial_part == 18 || tutorial_part == 19) {
                CP_Settings_Fill(COLOR_WHITE);
                CP_Graphics_DrawRect(100, window_height - 450, window_width - 200, 150);
                CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                CP_Settings_Fill(COLOR_BLACK);
                CP_Settings_TextSize(55);
                if (tutorial_part == 8) {
                    if (minions_in_base == 1) {
                        gIsPaused = TRUE;
                        money = 0;
                    }
                    if (minion_position[1] > minion_topY) {
                        minion_position[1] -= 12;
                    }
                    CP_Image_Draw(ftutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    if (minion_position[1] <= minion_topY) {
                        CP_Font_DrawText("You can check you progress on this bar. Everytime one more bar turns", 120, window_height - 380);
                        CP_Font_DrawText("white, you become one step closer to freeing everyone!", 120, window_height - 320);
                        CP_Settings_Fill(TRANSLUCENT_PINK);
                        CP_Graphics_DrawCircle(window_width - 200, (float)origin_map_coordinateY - 20, (float)BLOCK_SIZE);
                    }
                }
                if (tutorial_part == 9) {
                    CP_Image_Draw(ftutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    CP_Font_DrawText("Unfortunately, those accursed squares won't make things so easy.", 120, window_height - 380);
                    CP_Font_DrawText("If only...", 120, window_height - 320);
                }
                if (tutorial_part == 10) {
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    CP_Font_DrawText("Dark thoughts aside, I'll bring out the training dummies now and move some", 120, window_height - 380);
                    CP_Font_DrawText("things around.", 120, window_height - 320);
                }
                if (tutorial_part == 11) {
                    restart_level();
                    CP_Graphics_ClearBackground(COLOR_BLACK);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Graphics_DrawRect(100, window_height - 450, window_width - 200, 150);
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Font_DrawText("Tada!", 120, window_height - 380);
                    CP_Font_DrawText("Now, there are 2 types of squares, the guards, and the towers.", 120, window_height - 320);
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                }
                if (tutorial_part == 11) {
                    restart_level();
                    money = 60;
                    CP_Graphics_ClearBackground(COLOR_BLACK);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Graphics_DrawRect(100, window_height - 450, window_width - 200, 150);
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Font_DrawText("These are just dummies so they can't attack. But of course, actual Squares will ", 120, window_height - 380);
                    CP_Font_DrawText("definitely attack. Try summoning a scout or warrior.", 120, window_height - 320);
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                }
                if (tutorial_part == 12) {
                    if (minion_count >= 2) {
                        money = 0;
                    }
                    else {
                        money = 60;
                    }
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    if (array_EnemyStats[1][ENEMY_HP] > 80) {
                        CP_Font_DrawText("Great job!", 120, window_height - 365);
                    }
                    else if (array_EnemyStats[1][ENEMY_HP] <= 80) {
                        CP_Font_DrawText("Notice how the enemy tower isn't being attacked? Only tank", 120, window_height - 380);
                        CP_Font_DrawText("and wizard minions will be capable of attacking these towers.", 120, window_height - 320);
                        CP_Settings_Fill(TRANSLUCENT_PINK);
                        CP_Graphics_DrawCircle((float)array_EnemyStats[0][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[0][ENEMY_COL_COORDINATES] - 40, 1.5f * (float)BLOCK_SIZE);
                    }
                }
                if (tutorial_part == 13) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Font_DrawText("Also, everytime a Square is defeated, they drop money! Isn't that just great?", 120, window_height - 380);
                    CP_Font_DrawText("However, do note enemy guards can come back, tho with lowered HP.", 120, window_height - 320);
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                }
                if (tutorial_part == 14) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Font_DrawText("See this bar? This let's you know how long you have before they come back", 120, window_height - 380);
                    CP_Font_DrawText("And everytime they come back and are defeated, you get more money!", 120, window_height - 320);
                    CP_Image_Draw(tutorial_minion, (float)minion_position[0], (float)minion_position[1], 200, 200, 255);
                    CP_Settings_Fill(TRANSLUCENT_PINK);
                    CP_Graphics_DrawCircle((float)array_EnemyStats[1][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[1][ENEMY_COL_COORDINATES] + 50, (float)BLOCK_SIZE);
                }
                if (tutorial_part == 15) {
                    CP_Font_DrawText("To speed things up, I have replaced the dummy.", 120, window_height - 365);
                    money = 0;
                    array_GameMap[2][6] = BLOCK_ENEMY;
                    array_EnemyStats[1][ENEMY_HP] = 100;
                }
                if (tutorial_part == 16) {
                    money = 110;
                    CP_Font_DrawText("Now try summoning a tank minion! For learning purposes, tank minion won't have", 120, window_height - 380);
                    CP_Font_DrawText("full HP for now.", 120, window_height - 320);
                }
                if (tutorial_part == 17) {
                    money = 110;
                    if (array_MinionStats[0][MINION_TYPE] == TANK_MINION) {
                        array_MinionStats[0][ENEMY_HP] = find_full_hp(0) / 2;
                        CP_Font_DrawText("Tank minion is quite tanky, and will focus all guard attacks onto himself! When", 120, window_height - 380);
                        CP_Font_DrawText("their special attack charges up, they regain health and deal AOE damage.", 120, window_height - 320);
                        money = 0;
                    }
                    else if (array_MinionStats[0][MINION_TYPE] != TANK_MINION) {
                        CP_Font_DrawText("Um, that isn't tank minion.", 120, window_height - 365);
                        array_MinionStats[0][MINION_HP] = 0;
                    }
                    if (array_EnemyStats[0][ENEMY_HP] < 200) {
                       tutorial_part += 1;
                    }
                }
                if (tutorial_part == 18) {
                    money = 130;
                    CP_Font_DrawText("Isn't tank minion just amazing?", 120, window_height - 380);
                    CP_Font_DrawText("Now let's move on to Wizard minion!", 120, window_height - 320);
                    array_MinionStats[0][MINION_HP] = 0;
                }
                if (tutorial_part == 19) {
                    money = 130;
                    if (array_MinionStats[0][MINION_TYPE] == WIZARD_MINION) {
                        CP_Font_DrawText("Wizard minion is rather weak BUT he has a huge 3x3 range with high damage", 120, window_height - 380);
                        CP_Font_DrawText("though his special attack does take quite some time to charge up.", 120, window_height - 320);
                    }
                    else if (array_MinionStats[0][MINION_TYPE] != WIZARD_MINION && array_MinionStats[0][MINION_HP] > 0) {
                        CP_Font_DrawText("Hey uh, wizard minion first.", 120, window_height - 365);
                        money = 0;
                        array_MinionStats[0][MINION_HP] = 0;
                    }
                }
                if (tutorial_part != 12 || (tutorial_part == 12 && array_EnemyStats[1][ENEMY_HP] <= 80)) {
                    CP_Settings_Fill(COLOR_GREY);
                    CP_Settings_TextSize(30);
                    CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 310);
                }
            }
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                float x = (float)CP_Input_GetMouseX();
                float y = (float)CP_Input_GetMouseY();
                float origin_first_boxX = (float)options_boxX + (float)minion_buttons_width - (float)minion_buttons_width / 2.f;
                if (tutorial_part == 2 && minion_position[0] < spawnX - 200) {
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 5 && !(y >= minion_boxY && y <= minion_boxY + minion_buttons_height
                    && x >= origin_first_boxX && x < origin_first_boxX + minion_buttons_width)) { //not spam minion being produced
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 8 && minion_position[1] > minion_topY) {
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 11 && !(y >= minion_boxY && y <= minion_boxY + minion_buttons_height
                    && x >= origin_first_boxX && x < origin_first_boxX + (2 * minion_buttons_width))) { //not spam minion being produced
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 12 && array_EnemyStats[1][ENEMY_HP] > 0) {
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 14 && array_EnemyStats[1][ENEMY_HP] > 0) {
                    tutorial_part += 1;
                }
                else if (tutorial_part == 17 && array_MinionStats[0][MINION_TYPE] != TANK_MINION) {
                    tutorial_part -= 1;
                }
                else if (tutorial_part == 19 && array_MinionStats[0][MINION_TYPE] != WIZARD_MINION) {
                    tutorial_part -= 1;
                }
                tutorial_part += 1;
            }
        }
        //Keeping in the game as an additional feature
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
                render_minion_sprite(i);
            }
            renderminionhp_bar();
            render_special_current_charge();
            render_minion_special_attack();
            enemy_special_attack();
            for (int j = 0; j < ENEMY_MAX; j++) {
                render_special_effect_enemy(j);
            }
            render_enemy_death_comeback_bar();
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                if (CP_Input_GetMouseY() >= restartY && (CP_Input_GetMouseY() <= (restartY + restart_width)) && (CP_Input_GetMouseX() >= restartX && (CP_Input_GetMouseX() <= (restartX + restart_length)))) {
                    if (current_level != 0) {
                        restart_level();
                    }
                }
            }
            if (setting_popup == TRUE) {
                setting_image = CP_Image_Load("./Assets/bg_mainmenu.png"); //temp image

                static float middleX, middleY, width, height;
                middleX = (float)(CP_System_GetWindowWidth() / 2);
                middleY = (float)(CP_System_GetWindowHeight() / 2);
                width = (float)CP_Image_GetWidth(setting_image);
                height = (float)CP_Image_GetWidth(setting_image) * 0.6f;
                CP_Image_Draw(setting_image, middleX, middleY, width, height, 300);
                float startX = (float)CP_System_GetDisplayWidth() / 4;
                float startY = (float)CP_System_GetDisplayHeight() / 5;
                float option_textX = startX + 20.f;
                float option_textY = startY + 20.f;

                button_height = 120.f;
                button_width = 300.f;

                backX = 10.f;
                backY = 10.f;
                back_height = 80.f;
                back_width = 200.f;

                float mouseX = (float)CP_Input_GetMouseX();
                float mouseY = (float)CP_Input_GetMouseY();

                /*options buttons*/
                CP_Settings_Fill(COLOR_WHITE);
                CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height); //main menu
                CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height); //level selection
                CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height); //hordepedia?
                CP_Graphics_DrawRect(startX * 2, startY * 4, button_width, button_height); //music?

                /*back button*/
                CP_Graphics_DrawRect(backX, backY, back_width, back_height);

                /*options text*/
                CP_Settings_TextSize(60);
                CP_Settings_Fill(COLOR_BLACK);
                CP_Font_DrawText("MAIN MENU", option_textX, option_textY * 3 + 20);
                CP_Font_DrawText("LEVEL", option_textX, option_textY * 4);
                CP_Font_DrawText("HELP", option_textX * 2, option_textY * 3 + 20);
                CP_Font_DrawText("QUIT", option_textX * 2, option_textY * 4);

                /*back text*/
                CP_Settings_TextSize(50);
                CP_Font_DrawText("BACK", 40.f, 60.f);

                if (mouseX >= startX && mouseX <= (startX + button_width) &&
                    mouseY >= startY * 3 && mouseY <= startY * 3 + button_height) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height);
                    CP_Settings_TextSize(60);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Font_DrawText("MAIN MENU", option_textX, option_textY * 3 + 20);
                    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                        Current_Gamestate = MAIN_MENU_SCREEN;
                        setting_popup = FALSE;
                    }
                }

                else if (mouseX >= startX && mouseX <= (startX + button_width) &&
                    mouseY >= startY * 4 && mouseY <= startY * 4 + button_height) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height);
                    CP_Settings_TextSize(60);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Font_DrawText("LEVEL", option_textX, option_textY * 4);
                    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                        Current_Gamestate = LEVEL_SELECTOR_SCREEN;
                        setting_popup = FALSE;
                    }
                }
                else if (mouseX >= startX * 2 && mouseX <= (startX * 2 + button_width) &&
                    mouseY >= startY * 3 && mouseY <= startY * 3 + button_height) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height);
                    CP_Settings_TextSize(60);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Font_DrawText("HELP", option_textX * 2, option_textY * 3 + 20);
                    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                        Current_Gamestate = HELP_SCREEN;
                        setting_popup = FALSE;
                    }
                }

                else if (mouseX >= startX * 2 && mouseX <= (startX * 2 + button_width) &&
                    mouseY >= startY * 4 && mouseY <= startY * 4 + button_height) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect(startX * 2, startY * 4, button_width, button_height);
                    CP_Settings_TextSize(60);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Font_DrawText("QUIT", option_textX * 2, option_textY * 4);
                    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                        Current_Gamestate = QUIT_SCREEN;
                        setting_popup = FALSE;
                    }
                }
                else   if (mouseX >= backX && mouseX <= (backX + back_width) &&
                    mouseY >= backY && mouseY <= backY + back_height) {
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect(backX, backY, back_width, back_height);
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Settings_TextSize(50);
                    CP_Font_DrawText("BACK", 40.f, 60.f);
                    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

                        CP_Image_Free(&setting_image);
                        gIsPaused = FALSE;
                        setting_popup = FALSE;
                    }
                }
            }
        }

        else if (gIsPaused == FALSE) {
            //test = CP_System_GetDt();
            start_timer();
            update_timer();
            if (elapsed_timer2 > 2) {
                money += 10;
                elapsed_timer2 -= 2;
            }
            snprintf(buffer, sizeof(buffer), "%d", (120 - (int)elapsed_timer));
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                gameplay_screen_clicked(CP_Input_GetMouseX(), CP_Input_GetMouseY());
            }
            if (minion_count > 0) {
                move_minion();
                renderminionhp_bar();
                render_special_current_charge();
                minion_enter_base_counter(); //please do not comment this out
            }
            render_minion_special_attack();
            enemy_special_attack();
            for (int j = 0; j < ENEMY_MAX; j++) {
                render_special_effect_enemy(j);
            }
            render_enemy_death_comeback_bar();
            bring_back_enemy();

            if ((int)elapsed_timer == 120)
            {
                Current_Gamestate = LOSE_SCREEN;
            }
            //projectile_render();
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
    else if (Current_Gamestate == HELP_SCREEN) {
        help_screen();
    }
    else if (Current_Gamestate == HELP_SCREENP2) {
        help_screen2();
    }
    else if (Current_Gamestate == HELP_SCREENP3) {
        help_screen3();
    }
    else if (Current_Gamestate == QUIT_SCREEN) {
        quit_screen();
    }
    else if (Current_Gamestate == CREDIT_SCREEN) {
        credit_screen();
    }
    else if (Current_Gamestate == CREDIT_SCREENP2) {
        credit_screen2();
    }
    else if (Current_Gamestate == TUTORIAL_SCREEN) {
        tutorial();
    }
    else if (Current_Gamestate == TUTORIAL_SCREEN2) {
        tutorial1();
    }
    else if (Current_Gamestate == TUTORIAL_SCREEN3) {
        tutorial2();
    }
    else if (Current_Gamestate == TUTORIAL_SCREEN4) {
        tutorial3();
    }
    else if (Current_Gamestate == TUTORIAL_SCREEN5) {
        tutorial4();
    }
}

void game_exit(void) {
    free_all_sprites();
}

/*FUNCTIONS START HERE*/

/*Display enemy info during gameplay*/
void enemy_info(void) {
    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if ((int)mouseX >= col * BLOCK_SIZE && (int)mouseX <= col * BLOCK_SIZE + BLOCK_SIZE &&
                (int)mouseY >= row * BLOCK_SIZE && (int)mouseY <= row * BLOCK_SIZE + BLOCK_SIZE) {
                if (array_GameMap[row][col] == BLOCK_ENEMY || array_GameMap[row][col] == BLOCK_TOWER_ENEMY) { //check if enemy is on the block
                    CP_Settings_Fill(COLOR_WHITE);
                    CP_Graphics_DrawRect(col * (float)BLOCK_SIZE + 85, row * (float)BLOCK_SIZE + 20, 100.f, 50.f); //info panel
                    CP_Settings_TextSize(35);
                    CP_Settings_Fill(COLOR_BLACK);
                    int i = check_which_enemy(row, col); //check which enemy belongs to which type
                    if (array_EnemyStats[i][ENEMY_TYPE] == GUARD_ENEMY) {
                        CP_Font_DrawText("GUARD", col * (float)BLOCK_SIZE + 90, row * (float)BLOCK_SIZE + 60);
                    }
                    else if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
                        CP_Font_DrawText("AOE", col * (float)BLOCK_SIZE + 90, row * (float)BLOCK_SIZE + 60);
                    }
                    else if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
                        CP_Font_DrawText("SLOW", col * (float)BLOCK_SIZE + 90, row * (float)BLOCK_SIZE + 60);
                    }
                    else if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
                        CP_Font_DrawText("HEAL", col * (float)BLOCK_SIZE + 90, row * (float)BLOCK_SIZE + 60);
                    }
                }
            }
        }
    }
}

/*Display minion info during gameplay*/
void minion_info(void) {
    float mouseX = CP_Input_GetMouseX();
    float mouseY = CP_Input_GetMouseY();
    float minion_costboxY = (float)minion_boxY + 100.f;
    float minion_costbox_height = 50;
    if (mouseX >= (float)minion_boxX && mouseX <= (float)minion_boxX + minion_buttons_width &&
        mouseY >= minion_costboxY - 100 && mouseY <= minion_costboxY + minion_costbox_height) {
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX + 55, minion_costboxY - 150, 120.f, 50.f);
        CP_Settings_TextSize(35);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("HEALER", (float)minion_boxX + 60, minion_costboxY - 110);
    }
    else if (mouseX >= (float)minion_boxX - minion_buttons_width && mouseX <= (float)minion_boxX &&
        mouseY >= minion_costboxY - 100 && mouseY <= minion_costboxY + minion_costbox_height) {
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX + 55 - minion_buttons_width, minion_costboxY - 150, 120.f, 50.f);
        CP_Settings_TextSize(35);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("WIZARD", (float)minion_boxX + 60 - minion_buttons_width, minion_costboxY - 110);
    }
    else if (mouseX >= (float)minion_boxX - minion_buttons_width * 2 && mouseX <= (float)minion_boxX - minion_buttons_width &&
        mouseY >= minion_costboxY - 100 && mouseY <= minion_costboxY + minion_costbox_height) {
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX + 55 - minion_buttons_width * 2, minion_costboxY - 150, 120.f, 50.f);
        CP_Settings_TextSize(35);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("TANK", (float)minion_boxX + 70 - minion_buttons_width * 2, minion_costboxY - 110);
    }
    else if (mouseX >= (float)minion_boxX - minion_buttons_width * 3 && mouseX <= (float)minion_boxX - minion_buttons_width * 2 &&
        mouseY >= minion_costboxY - 100 && mouseY <= minion_costboxY + minion_costbox_height) {
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX + 55 - minion_buttons_width * 3, minion_costboxY - 150, 140.f, 50.f);
        CP_Settings_TextSize(35);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("WARRIOR", (float)minion_boxX + 60 - minion_buttons_width * 3, minion_costboxY - 110);
    }
    else if (mouseX >= (float)minion_boxX - minion_buttons_width * 4 && mouseX <= (float)minion_boxX - minion_buttons_width * 3 &&
        mouseY >= minion_costboxY - 100 && mouseY <= minion_costboxY + minion_costbox_height) {
        CP_Settings_Fill(COLOR_WHITE);
        CP_Graphics_DrawRect((float)minion_boxX + 55 - minion_buttons_width * 4, minion_costboxY - 150, 120.f, 50.f);
        CP_Settings_TextSize(35);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("SCOUT", (float)minion_boxX + 70 - minion_buttons_width * 4, minion_costboxY - 110);
    }
}

/*Display Main Menu*/
void main_menu_screen(void) {
    main_menu_image = CP_Image_Load("./Assets/bg_mainmenu2.png");
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(main_menu_image);
    height = (float)CP_Image_GetWidth(main_menu_image) * 0.6f;
    CP_Image_Draw(main_menu_image, middleX, middleY, width, height, 255);
    /* Buttons */
    CP_Settings_Fill(COLOR_WHITE);
    float quarter_blockX = (float)CP_System_GetDisplayWidth() / 4;
    float quarter_blockY = (float)CP_System_GetDisplayHeight() / 4;
    button_height = 120.f;
    button_width = 300.f;
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
    creditX = 130;
    creditY = 1000.f;
    CP_Graphics_DrawRect(creditX, creditY, setting_width, setting_height);
    /* Now Text */
    CP_Settings_TextSize(80);
    CP_Settings_Fill(COLOR_BLACK);
    start_textX = start_game_buttonX + 50;
    start_textY = start_game_buttonY + 80;
    levels_textX = level_selectorX + 40;
    levels_textY = level_selectorY + 80;
    setting_textX = settingX + 40;
    setting_textY = settingY + 40;
    credit_textX = creditX + 40;
    credit_textY = creditY + 40;
    CP_Font_DrawText("START", start_textX, start_textY);
    CP_Font_DrawText("LEVELS", levels_textX, levels_textY);
    CP_Settings_TextSize(40);
    CP_Font_DrawText("SETTING", setting_textX, setting_textY);
    CP_Font_DrawText("CREDITS", credit_textX, credit_textY);

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= start_game_buttonX && mouseX <= (start_game_buttonX + button_width) &&
        mouseY >= start_game_buttonY && mouseY <= start_game_buttonY + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(start_game_buttonX, start_game_buttonY, button_width, button_height);
        CP_Settings_TextSize(80);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("START", start_textX, start_textY);
    }

    if (mouseX >= level_selectorX && mouseX <= (level_selectorX + button_width) &&
        mouseY >= level_selectorY && mouseY <= level_selectorY + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(level_selectorX, level_selectorY, button_width, button_height);
        CP_Settings_TextSize(80);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("LEVELS", levels_textX, levels_textY);
    }

    if (mouseX >= settingX && mouseX <= (settingX + button_width) &&
        mouseY >= settingY && mouseY <= settingY + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(settingX, settingY, setting_width, setting_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Settings_TextSize(40);
        CP_Font_DrawText("SETTING", setting_textX, setting_textY);
    }

    if (mouseX >= creditX && mouseX <= (creditX + button_width) &&
        mouseY >= creditY && mouseY <= creditY + button_height) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(creditX, creditY, setting_width, setting_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Settings_TextSize(40);
        CP_Font_DrawText("CREDITS", credit_textX, credit_textY);
    }

}

void main_menu_clicked(float x, float y) {
    if (x >= start_game_buttonX && x <= (start_game_buttonX + button_width) &&
        y >= start_game_buttonY && y <= start_game_buttonY + button_height) {

        if (tutorial_played == FALSE) {
            tutorial_played = TRUE;
            Current_Gamestate = TUTORIAL_SCREEN;
            
        }
        else if (tutorial_played == TRUE) {
            Current_Gamestate = GAMEPLAY_SCREEN;
            restart_level();
        }
    }
    // Level selector button clicked /
    else if (x >= level_selectorX && x <= (level_selectorX + button_width) &&
        y >= level_selectorY && y <= level_selectorY + button_height) {
        Current_Gamestate = LEVEL_SELECTOR_SCREEN;
        Previous_Gamestate = MAIN_MENU_SCREEN;
        // pending level_selector_screen completion /

        CP_Image_Free(&main_menu_image);
    }

    // Setting button clicked /
    else if (x >= settingX && x <= (settingX + setting_width) &&
        y >= settingY && settingY <= settingY + setting_height) {
        Current_Gamestate = SETTING_SCREEN;
        Previous_Gamestate = MAIN_MENU_SCREEN;
    }

    // Credit button clicked /
    else if (x >= creditX && x <= (creditX + button_width) &&
        y >= creditY && y <= creditY + button_height) {
        Current_Gamestate = CREDIT_SCREEN;
        Previous_Gamestate = MAIN_MENU_SCREEN;
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

        /*Hovering on Main Menu Button*/
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

        /*When clicked, return back to main menu screen*/
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1))
        {
            CP_Image_Free(&Win_Screen);
            Current_Gamestate = MAIN_MENU_SCREEN;


        }

    }
    else if (mouseX >= main_loseX && mouseX <= (main_loseX + button_width) && mouseY >= main_loseY && mouseY <= main_loseY + button_height) {

        /*Hovering on Next Level Button*/
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
        mouseY >= 540 && mouseY <= 615) {

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
    else if ((mouseX >= 1600 && mouseX <= 1840 && mouseY >= level3Y + 200 && mouseY <= 1040))
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
            Previous_Gamestate = LEVEL_SELECTOR_SCREEN;
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

            if (Previous_Gamestate == MAIN_MENU_SCREEN) {
                Current_Gamestate = MAIN_MENU_SCREEN;
            }

            else if (Previous_Gamestate == SETTING_SCREEN) {
                Current_Gamestate = SETTING_SCREEN;
            }

            else {
                Current_Gamestate = SETTING_SCREEN;
            }


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
    CP_Image_Draw(setting_image, middleX, middleY, width, height, 300);
    float startX = (float)CP_System_GetDisplayWidth() / 4;
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
    CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height); //Main Menu
    CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height); //Level Selection
    CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height); //Guide Book
    CP_Graphics_DrawRect(startX * 2, startY * 4, button_width, button_height); //Quit

        /*back button*/
    CP_Graphics_DrawRect(backX, backY, back_width, back_height);

    /*options text*/
    CP_Settings_TextSize(60);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("MAIN MENU", option_textX, option_textY * 3 + 20);
    CP_Font_DrawText("LEVEL", option_textX, option_textY * 4);
    CP_Font_DrawText("HELP", option_textX * 2, option_textY * 3 + 20);
    CP_Font_DrawText("QUIT", option_textX * 2, option_textY * 4);

    /*back text*/
    CP_Settings_TextSize(50);
    CP_Font_DrawText("BACK", 40.f, 60.f);

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();


    if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY * 3 && mouseY <= startY * 3 + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height);
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("MAIN MENU", option_textX, option_textY * 3 + 20);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = MAIN_MENU_SCREEN;
            Previous_Gamestate = SETTING_SCREEN;
        }
    }

    else if (mouseX >= startX && mouseX <= (startX + button_width) &&
        mouseY >= startY * 4 && mouseY <= startY * 4 + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX, startY * 4, button_width, button_height);
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("LEVEL", option_textX, option_textY * 4);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = LEVEL_SELECTOR_SCREEN;
            Previous_Gamestate = SETTING_SCREEN;
        }
    }

    else if (mouseX >= startX * 2 && mouseX <= (startX * 2 + button_width) &&
        mouseY >= startY * 3 && mouseY <= startY * 3 + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height);
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("HELP", option_textX * 2, option_textY * 3 + 20);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = HELP_SCREEN;
            Previous_Gamestate = SETTING_SCREEN;
        }
    }

    else if (mouseX >= startX * 2 && mouseX <= (startX * 2 + button_width) &&
        mouseY >= startY * 4 && mouseY <= startY * 4 + button_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX * 2, startY * 4, button_width, button_height);
        CP_Settings_TextSize(60);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("QUIT", option_textX * 2, option_textY * 4);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            Current_Gamestate = QUIT_SCREEN;
            Previous_Gamestate = SETTING_SCREEN;
        }
    }

    else if (mouseX >= backX && mouseX <= (backX + back_width) &&
        mouseY >= backY && mouseY <= backY + back_height) {
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(backX, backY, back_width, back_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Settings_TextSize(50);
        CP_Font_DrawText("BACK", 40.f, 60.f);
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
            else {
                Current_Gamestate = LEVEL_SELECTOR_SCREEN;
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
            Previous_Gamestate = MAIN_MENU_SCREEN;
        }
    }

    else if (Current_Gamestate == LEVEL_SELECTOR_SCREEN) {
        if (x >= 1600 && x <= (1600 + button_width) &&
            y >= level3Y && y <= level3Y + button_height) {
            Current_Gamestate = SETTING_SCREEN;
            Previous_Gamestate = LEVEL_SELECTOR_SCREEN;
            // CP_Graphics_DrawRect(1600, level3Y + 200, button_width, button_height);
        }
    }

    else if (Current_Gamestate == GAMEPLAY_SCREEN) {
        if (x >= setting_buttonX && x <= (setting_buttonX + 180.f) &&
            y >= setting_buttonY && y <= setting_buttonY + 40.f) {
            gIsPaused = TRUE;
            setting_popup = TRUE;
        }
    }
}

void help_screen(void) {

    float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_System_GetWindowWidth();
    height = (float)CP_System_GetWindowHeight();

    guide_image1 = CP_Image_Load("./Assets/Help_guide.jpg");
    CP_Image_Draw(guide_image1, middleX, middleY, width, height, 100);

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    /*Button size*/
    button_height = 60.f;
    button_width = 150.f;

    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1750, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("NEXT", 1755, button_height + 5);
    CP_Settings_TextSize(50);

    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(10, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("BACK", 20, button_height + 5);
    CP_Settings_TextSize(50);

    if (mouseX >= 1750 && mouseX <= (1750 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        /*Hovering on Next button*/
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1750, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("NEXT", 1755, button_height + 5);
        CP_Settings_TextSize(50);
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&guide_image1);
            Current_Gamestate = HELP_SCREENP2;
        }
    }
    if (mouseX >= 10 && mouseX <= (10 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(10, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("BACK", 20, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&guide_image1);
            if (Previous_Gamestate == SETTING_SCREEN) {
                Current_Gamestate = SETTING_SCREEN;
            }
            else {
                Current_Gamestate = GAMEPLAY_SCREEN;
            }
        }
    }
}

void help_screen2(void) {
    float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_System_GetWindowWidth();
    height = (float)CP_System_GetWindowHeight();

    guide_image2 = CP_Image_Load("./Assets/Help_guide2.jpg");
    CP_Image_Draw(guide_image2, middleX, middleY, width, height, 100);

    button_height = 60.f;
    button_width = 150.f;

    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(10, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("BACK", 20, button_height + 5);
    CP_Settings_TextSize(50);

    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1750, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("NEXT", 1755, button_height + 5);
    CP_Settings_TextSize(50);
    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= 1750 && mouseX <= (1750 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        button_height = 60.f;
        button_width = 150.f;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1750, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("NEXT", 1755, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            CP_Image_Free(&guide_image2);
            Current_Gamestate = HELP_SCREENP3;
        }
    }
    if (mouseX >= 10 && mouseX <= (10 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(10, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("BACK", 20, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&guide_image2);
            Current_Gamestate = HELP_SCREEN;
        }
    }
}

void help_screen3(void) {

    float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_System_GetWindowWidth();
    height = (float)CP_System_GetWindowHeight();

    guide_image3 = CP_Image_Load("./Assets/Help_guide3.jpg");
    CP_Image_Draw(guide_image3, middleX, middleY, width, height, 100);
    button_height = 60.f;
    button_width = 150.f;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(10, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("BACK", 20, button_height + 5);
    CP_Settings_TextSize(50);

    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1750, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("EXIT", 1755, button_height + 5);
    CP_Settings_TextSize(50);
    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= 1750 && mouseX <= (1750 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        button_height = 60.f;
        button_width = 150.f;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1750, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("EXIT", 1755, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            CP_Image_Free(&guide_image2);
            if (Previous_Gamestate == SETTING_SCREEN) {
                Current_Gamestate = SETTING_SCREEN;
            }
            else {
                Current_Gamestate = GAMEPLAY_SCREEN;
            }
        }
    }

    if (mouseX >= 10 && mouseX <= (10 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(10, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("BACK", 20, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&guide_image2);
            Current_Gamestate = HELP_SCREENP2;
        }
    }
}

void quit_screen(void) {

    quit_image = CP_Image_Load("./Assets/bg_mainmenu.png");
    float middleX = (float)(CP_System_GetWindowWidth() / 2);
    float middleY = (float)(CP_System_GetWindowHeight() / 2);
    float width = (float)CP_Image_GetWidth(setting_image);
    float height = (float)CP_Image_GetWidth(setting_image) * 0.6f;
    CP_Image_Draw(setting_image, middleX, middleY, width, height, 300);
    float startX = (float)CP_System_GetDisplayWidth() / 4;
    float startY = (float)CP_System_GetDisplayHeight() / 5;
    CP_Image_Draw(quit_image, middleX, middleY, width, height, 100);
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
    CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height); //no
    CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height); //yes

    /*options text*/
    CP_Settings_TextSize(75);
    CP_Settings_Fill(TRANSLUCENT_WHITE);
    CP_Font_DrawText("Are you sure you want to quit?", option_textX - 40, option_textY * 3 - 120);
    CP_Settings_TextSize(75);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("Are you sure you want to quit?", option_textX - 41, option_textY * 3 - 120);
    CP_Settings_TextSize(60);
    CP_Font_DrawText("No", option_textX, option_textY * 3 + 20);
    CP_Font_DrawText("Yes", option_textX * 2, option_textY * 3 + 20);

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();

    if (mouseX >= startX && mouseX <= (startX + button_width) && mouseY >= startY * 3 && mouseY <= (startY * 3 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX, startY * 3, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText(":D", option_textX, option_textY * 3 + 20);
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&quit_image);
            Current_Gamestate = SETTING_SCREEN;
        }
    }

    else if (mouseX >= startX * 2 && mouseX <= (startX * 2 + button_width) && mouseY >= startY * 3 && mouseY <= (startY * 3 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(startX * 2, startY * 3, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("Yes", option_textX * 2, option_textY * 3 + 20);
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {

            CP_Image_Free(&quit_image);
            Current_Gamestate = SETTING_SCREEN;
            CP_Engine_Terminate();
        }
    }
}

void tutorial(void) {
    
    if (Current_Gamestate == TUTORIAL_SCREEN) {
        CP_Graphics_ClearBackground(COLOR_BLACK);
        if (play_effect_once[0] == 0) {
            knock = CP_Sound_Load("./Assets/music/frantic_knock.wav");
            minion_voice = CP_Sound_Load("./Assets/music/random_minion_voice.wav");
            CP_Sound_PlayAdvanced(knock, 1.2f, 1.f, FALSE, CP_SOUND_GROUP_0);
            CP_Sound_PlayAdvanced(minion_voice, 0.5f, 1.f, FALSE, CP_SOUND_GROUP_0);
            play_effect_once[0] = 1;
        }
        float window_width = (float)CP_System_GetWindowWidth();
        float window_height = (float)CP_System_GetWindowHeight();
        CP_Settings_Fill(TRANSLUCENT_WHITE);
        CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
        CP_Settings_TextSize(80);
        CP_Settings_Fill(COLOR_BLACK);
        CP_Font_DrawText("Commander please wake up!", 120, window_height - 100);
        CP_Settings_Fill(COLOR_GREY);
        CP_Settings_TextSize(30);
        CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 60);
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && Current_Gamestate == TUTORIAL_SCREEN) {
            Current_Gamestate = TUTORIAL_SCREEN2;
            CP_Sound_Free(&knock);
        }
    }
}
void tutorial1(void) {
    float window_width = (float)CP_System_GetWindowWidth();
    float window_height = (float)CP_System_GetWindowHeight();
    CP_Graphics_ClearBackground(COLOR_BLACK);
    tutorial_background = CP_Image_Load("./Assets/tutorial_background.png");
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(tutorial_background);
    height = (float)CP_Image_GetWidth(tutorial_background) * 0.6f;
    CP_Image_Draw(tutorial_background, middleX, middleY, width, height, 255);
    CP_Settings_Fill(TRANSLUCENT_WHITE);
    CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
    CP_Settings_TextSize(80);
    CP_Settings_Fill(COLOR_BLACK);
    
    if (play_effect_once[1] == 0) {
        minion_voice = CP_Sound_Load("./Assets/music/random_minion_voice.wav");
        CP_Sound_PlayAdvanced(minion_voice, 0.3f, 1.f, FALSE, CP_SOUND_GROUP_0);
        play_effect_once[1] = 1;
    }
    
    CP_Font_DrawText("The squares... They... They came...", 120, window_height - 100);
    CP_Settings_Fill(COLOR_GREY);
    CP_Settings_TextSize(30);
    CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 60);
    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && Current_Gamestate == TUTORIAL_SCREEN2) {
        Current_Gamestate = TUTORIAL_SCREEN3;
    }
}
void tutorial2(void) {
    float window_width = (float)CP_System_GetWindowWidth();
    float window_height = (float)CP_System_GetWindowHeight();
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(tutorial_background);
    height = (float)CP_Image_GetWidth(tutorial_background) * 0.6f;
    CP_Image_Draw(tutorial_background, middleX, middleY, width, height, 255);
    if (play_effect_once[2] == 0) {
        minion_voice = CP_Sound_Load("./Assets/music/random_minion_voice.wav");
        CP_Sound_PlayAdvanced(minion_voice, 0.8f, 1.f, FALSE, CP_SOUND_GROUP_0);
        play_effect_once[2] = 1;
    }
    CP_Settings_Fill(TRANSLUCENT_WHITE);
    CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(80);
    CP_Font_DrawText("THEY KIDNAPPED OUR PEOPLE!", 120, window_height - 100);
    CP_Settings_Fill(COLOR_GREY);
    CP_Settings_TextSize(30);
    CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 60);
    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && Current_Gamestate == TUTORIAL_SCREEN3) {
        Current_Gamestate = TUTORIAL_SCREEN4;
    }
}
void tutorial3(void) {
    float window_width = (float)CP_System_GetWindowWidth();
    float window_height = (float)CP_System_GetWindowHeight();
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(tutorial_background);
    height = (float)CP_Image_GetWidth(tutorial_background) * 0.6f;
    CP_Image_Draw(tutorial_background, middleX, middleY, width, height, 255);
    if (play_effect_once[3] == 0) {
        minion_voice = CP_Sound_Load("./Assets/music/random_minion_voice.wav");
        CP_Sound_PlayAdvanced(minion_voice, 0.2f, 1.f, FALSE, CP_SOUND_GROUP_0);
        play_effect_once[3] = 1;
    }
    CP_Settings_Fill(TRANSLUCENT_WHITE);
    CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(60);
    CP_Font_DrawText("Commander, you need to lead our forces and save them. Please,", 120, window_height - 130);
    CP_Font_DrawText("We need you.", 120, window_height - 80);
    CP_Settings_Fill(COLOR_GREY);
    CP_Settings_TextSize(30);
    CP_Font_DrawText("click to continue>>", window_width - 330, window_height - 60);
    if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && Current_Gamestate == TUTORIAL_SCREEN4) {
        Current_Gamestate = TUTORIAL_SCREEN5;
    }
}
void tutorial4(void) {
    float window_width = (float)CP_System_GetWindowWidth();
    float window_height = (float)CP_System_GetWindowHeight();
    static float middleX, middleY, width, height;
    middleX = (float)(CP_System_GetWindowWidth() / 2);
    middleY = (float)(CP_System_GetWindowHeight() / 2);
    width = (float)CP_Image_GetWidth(tutorial_background);
    height = (float)CP_Image_GetWidth(tutorial_background) * 0.6f;
    CP_Image_Draw(tutorial_background, middleX, middleY, width, height, 255);
    if (play_effect_once[4] == 0) {
        minion_voice = CP_Sound_Load("./Assets/music/random_minion_voice.wav");
        CP_Sound_PlayAdvanced(minion_voice, 0.3f, 1.f, FALSE, CP_SOUND_GROUP_0);
        play_effect_once[4] = 1;
    }
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(100, window_height - 200, window_width - 200, 150);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(60);
    CP_Font_DrawText("Do you remember how to lead our people into battle?", 120, window_height - 130);
    CP_Font_DrawText("If no, let's head to the training ground first.", 120, window_height - 80);
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(window_width - 580, window_height - 250, 480, 50);
    CP_Graphics_DrawRect(window_width - 580, window_height - 300, 480, 50);
    CP_Settings_TextSize(32);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("I remember.", window_width - 570, window_height - 270);
    CP_Font_DrawText("Let's head to the training ground first.", window_width - 570, window_height - 220);
    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();
    if (mouseX >= window_width - 580 && mouseX < window_width - 100) {
        if (mouseY >= window_height - 250 && mouseY < window_height - 200) {
            CP_Settings_Fill(COLOR_BLACK);
            CP_Graphics_DrawRect(window_width - 580, window_height - 250, 480, 50);
            CP_Settings_Fill(COLOR_WHITE);
            CP_Font_DrawText("Let's head to the training ground first.", window_width - 570, window_height - 220);
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                CP_Graphics_ClearBackground(COLOR_BLACK);
                CP_Image_Free(&tutorial_background);
                tutorial_part = 0;
                current_level = 0;
                restart_level(); 
                Current_Gamestate = GAMEPLAY_SCREEN;
                gIsPaused = TRUE;
            }
        }
        if (mouseY >= (window_height - 300) && mouseY < window_height - 250) {
            CP_Settings_Fill(COLOR_BLACK);
            CP_Graphics_DrawRect(window_width - 580, window_height - 300, 480, 50);
            CP_Settings_Fill(COLOR_WHITE);
            CP_Font_DrawText("I remember.", window_width - 570, window_height - 270);
            if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
                CP_Graphics_ClearBackground(COLOR_BLACK);
                CP_Image_Free(&tutorial_background);
                CP_Sound_Free(&minion_voice);
                current_level = 1;
                restart_level();
                Current_Gamestate = GAMEPLAY_SCREEN;
            }
        }
    }
}

void credit_screen(void) {

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();
    float width = (float)CP_System_GetWindowWidth();
    float height = (float)CP_System_GetWindowHeight();
    /*Load Image*/
    Credit_Screen = CP_Image_Load("./Assets/Credit_Screen.jpg");
    CP_Image_Draw(Credit_Screen, (width / 2), (height / 2), width, height, 255);
    button_height = 60.f;
    button_width = 150.f;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(10, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("BACK", 20, button_height + 5);
    CP_Settings_TextSize(50);
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1750, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("NEXT", 1780, button_height + 5);
    CP_Settings_TextSize(50);
    if (mouseX >= 10 && mouseX <= (10 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(10, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("BACK", 20, button_height + 5);
        CP_Settings_TextSize(50);
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            CP_Image_Free(&Credit_Screen);
            Current_Gamestate = MAIN_MENU_SCREEN;
        }
    }
    if (mouseX >= 1750 && mouseX <= (1750 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        button_height = 60.f;
        button_width = 150.f;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1750, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("NEXT", 1780, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            CP_Image_Free(&Credit_Screen);
            Current_Gamestate = CREDIT_SCREENP2;
        }
    }
}
void credit_screen2(void) {

    float mouseX = (float)CP_Input_GetMouseX();
    float mouseY = (float)CP_Input_GetMouseY();
    float width = (float)CP_System_GetWindowWidth();
    float height = (float)CP_System_GetWindowHeight();
    /*Load Image*/
    Credit_Screen2 = CP_Image_Load("./Assets/Credit_Screen2.jpg");
    CP_Image_Draw(Credit_Screen2, width / 2, height / 2, width, height, 255);
    button_height = 60.f;
    button_width = 150.f;
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(1750, 15, button_width, button_height);
    CP_Settings_Fill(COLOR_BLACK);
    CP_Font_DrawText("EXIT", 1780, button_height + 5);
    CP_Settings_TextSize(50);

    if (mouseX >= 1750 && mouseX <= (1750 + button_width) && mouseY >= 15 && mouseY <= (15 + button_height)) {

        button_height = 60.f;
        button_width = 150.f;
        CP_Settings_Fill(COLOR_BLACK);
        CP_Graphics_DrawRect(1750, 15, button_width, button_height);
        CP_Settings_Fill(COLOR_WHITE);
        CP_Font_DrawText("EXIT", 1780, button_height + 5);
        CP_Settings_TextSize(50);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
            CP_Image_Free(&Credit_Screen2);
            Current_Gamestate = MAIN_MENU_SCREEN;
        }
    }
}

void credit_screen_clicked(float x, float y) {
    if (Current_Gamestate == MAIN_MENU_SCREEN) {
        if (x >= creditX && x <= (creditX + button_width) &&
            y >= creditY && y <= creditY + button_height) {
            Current_Gamestate = CREDIT_SCREEN;
            Previous_Gamestate = MAIN_MENU_SCREEN;
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

    for (int i = 0; i < ENEMY_MAX; i++) {
        array_enemy_attack_time[i][EFFECT_TIMER] = 0;
        array_enemy_attack_time[i][CHECKER] = 0;
        array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] = 0;
        array_Enemy_Slow_Effect_Time[i][CHECKER] = 0;
    }
    for (int j = 0; j < ENEMY_MAX; j++) {
        array_EnemyCurrentCharge[j][ENEMY_CURRENT_CHARGE] = 0;
        array_enemy_death_timer[j][ENEMY_DEATH_COUNTER] = 0;
    }
    for (int i = 0; i < MINION_MAX; i++) {
        array_minion_attack_time[i][EFFECT_TIMER] = 0;
        array_minion_attack_time[i][CHECKER] = 0;
        healer_minion_basic_heals[i][EFFECT_TIMER] = 0;
        healer_minion_basic_heals[i][CHECKER] = 0;
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
    setting_buttonY = gPauseButtonPositionY + 30.f;
}

void draw_timer_and_pause_button(void) {
    CP_Font_Set(CP_Font_GetDefault());
    CP_Settings_Fill(COLOR_WHITE);
    CP_Graphics_DrawRect(gPauseButtonPositionX, gPauseButtonPositionY, 100.f, 50.f);
    CP_Graphics_DrawRect(gTimerPositionX, gTimerPositionY, 100.f, 50.f);
    CP_Graphics_DrawRect(setting_buttonX, setting_buttonY, 180.f, 40.f);
    CP_Graphics_DrawCircle(setting_buttonX + 230.f, setting_buttonY + 20.f, 40.f);
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
    CP_Settings_TextSize(40);
    CP_Font_DrawText("SETTING", (setting_buttonX + 10.f), (setting_buttonY + 29.f));
    CP_Font_DrawText("?", (setting_buttonX + 222.f), (setting_buttonY + 29.f));
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
        array_minion_attack_time[i][EFFECT_TIMER] += test;
        healer_minion_basic_heals[i][EFFECT_TIMER] += test;
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        array_EnemyCurrentCharge[i][ENEMY_BASIC_CURRENT_CHARGE] += test;
        array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] += test;
        array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] += test;
        array_enemy_attack_time[i][EFFECT_TIMER] += test;
        array_enemy_death_timer[i][ENEMY_DEATH_TIMER] += test;
    }
    for (int j = 0; j < ENEMY_MAX; j++) {
        for (int i = 0; i < proj_count; i++) {
            projectile[j][i][PROJ_CHARGE] += test;
        }
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        l_time[i] += test;
    }
}

void display_restart_button(void) {
    restartX = (float)origin_map_coordinateX + 15;
    restartY = 900;
    restart_length = 80;
    restart_width = 50;
    /*Button*/
    if (current_level == 0) {
        CP_Settings_Fill(COLOR_GREY);
    }
    else {
        CP_Settings_Fill(COLOR_WHITE);
    }
    CP_Graphics_DrawRect(restartX, restartY, restart_length, restart_width);
    /*Text*/
    CP_Settings_Fill(COLOR_BLACK);
    CP_Settings_TextSize(25);
    CP_Font_DrawText("Restart", restartX + 4, restartY + 20);
    CP_Font_DrawText("Game", restartX + 10, restartY + 40);
}

void restart_level(void) {
    minion_count = 0;
    reset_map_and_minions();
    initialise_level();
    gIsPaused = FALSE;
    minions_in_base = 0;
    initialise_pause_and_timer_button();
    money = 80;
    elapsed_timer = 0;
    elapsed_timer2 = 0;
    proj_count = 0;
    //l_time = 0;
    for (int i = 0; i < ENEMY_MAX; i++) {
        l_time[i] = 0;
    }
    gameplay_screen();
    render_background();
    draw_timer_and_pause_button();
    display_money_counter();
}

void gameplay_screen() {
    load_all_sprites();
    //initialise_level();
    render_background();
    box_length = MAP_GRID_COLS * BLOCK_SIZE;
    box_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    options_boxX = origin_map_coordinateX;
    options_boxY = origin_map_coordinateY + (MAP_GRID_ROWS * BLOCK_SIZE);

    CP_Settings_Fill(COLOR_LIGHT_GREY);
    CP_Graphics_DrawRect((float)options_boxX, (float)options_boxY, (float)box_length, (float)box_width);
    minion_buttons_width = BLOCK_SIZE + BLOCK_SIZE / 2;
    minion_buttons_height = BLOCK_SIZE;
    for (int i = 1; i < 6; i++) {
        minion_boxX = options_boxX + (i * minion_buttons_width) - minion_buttons_width / 2;
        minion_boxY = options_boxY + (box_width / 4) - 20; //minion_boxY remains constant throughout
        CP_Settings_Fill(COLOR_GREYISH_BLUE);
        CP_Graphics_DrawRect((float)minion_boxX, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
        float minion_size = 0;
        int minions_imageX = minion_boxX + minion_buttons_width / 2;
        int minions_imageY = minion_boxY + minion_buttons_height / 2;

        float origin_first_boxX = (float)options_boxX + (float)minion_buttons_width - (float)minion_buttons_width / 2.f;

        if (i == 1) {
            if (money < 30) {
                CP_Settings_Fill(COLOR_GREY);
                CP_Graphics_DrawRect(origin_first_boxX, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
            }
            minion_size = 65;
            minions_imageY -= 5;
            CP_Image_Draw(spam_minion, (float)minions_imageX, (float)minions_imageY, minion_size, minion_size, 255);
        }
        else if (i == 2) {
            if (money < 60) {
                CP_Settings_Fill(COLOR_GREY);
                CP_Graphics_DrawRect(origin_first_boxX + 1 * (float)minion_buttons_width, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
            }
            minion_size = 110;
            CP_Image_Draw(warrior_minion, (float)minions_imageX, (float)minions_imageY, minion_size, minion_size, 255);
        }
        else if (i == 3) {
            if (money < 110) {
                CP_Settings_Fill(COLOR_GREY);
                CP_Graphics_DrawRect(origin_first_boxX + 2 * (float)minion_buttons_width, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
            }
            minion_size = 140;
            CP_Image_Draw(tank_minion, (float)minions_imageX, (float)minions_imageY, minion_size, minion_size, 255);
        }
        else if (i == 4) {
            if (money < 130) {
                CP_Settings_Fill(COLOR_GREY);
                CP_Graphics_DrawRect(origin_first_boxX + 3 * (float)minion_buttons_width, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
            }
            minion_size = 100;
            minions_imageY -= 10;
            CP_Image_Draw(wizard_minion, (float)minions_imageX, (float)minions_imageY, minion_size, minion_size, 255);
        }
        else if (i == 5) {
            if (money < 150) {
                CP_Settings_Fill(COLOR_GREY);
                CP_Graphics_DrawRect(origin_first_boxX + 4 * (float)minion_buttons_width, (float)minion_boxY, (float)minion_buttons_width, (float)minion_buttons_height);
            }
            minion_size = 90;
            minions_imageY -= 10;
            CP_Image_Draw(healer_minion, (float)minions_imageX, (float)minions_imageY, minion_size, minion_size, 255);
        }      
    }
    float minion_costboxY = (float)minion_boxY + 100.f;
    float minion_costbox_height = 50;

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
            ? "110"
            : i == 3
            ? "130"
            : i == 4
            ? "150"
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
            if (money >= 110 && minion_count < MINION_MAX)
            {
                money -= 110;
                array_MinionStats[minion_count][MINION_TYPE] = TANK_MINION;
                assign_minion_stats(); //maybe can throw this function call in render_minion
            }

        }
        else if (x >= (origin_first_boxX + 3 * minion_buttons_width) && x < (origin_first_boxX + 4 * minion_buttons_width)) { //Create Wizard Minion
            if (money >= 130 && minion_count < MINION_MAX)
            {
                money -= 130;
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
    if (current_level != 0) {
        if (y >= restartY && (y <= (restartY + restart_width)) && (x >= restartX && (x <= (restartX + restart_length)))) {
            restart_level();
        }
    }

    if (x >= setting_buttonX + 210.f && x <= setting_buttonX + 250.f &&
        y >= setting_buttonY && y <= setting_buttonY + 40.f) {
        Current_Gamestate = HELP_SCREEN;
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
    else if (current_level == 0) {
        level_tutorial();
    }
    //and so on...

    setup_collaborative_diffusion_map();
    if (level_has_teleporter == TRUE) {
        setup_teleport_diffusion_map();
    }
}

void render_background() {
    CP_Graphics_ClearBackground(COLOR_BG);
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            BlockPositionX = origin_map_coordinateX + BLOCK_SIZE * col;
            BlockPositionY = origin_map_coordinateY + BLOCK_SIZE * row;
            CP_Settings_Fill(array_GameMap[row][col] == BLOCK_EMPTY //ternary operator
                ? COLOR_GREY
                : array_GameMap[row][col] == BLOCK_TELEPORT_SPAWN
                ? COLOR_BRIGHT_BLUE
                : array_GameMap[row][col] == BLOCK_TELEPORTER
                ? COLOR_DULLER_BLUE
                : COLOR_GREY); //BLOCK_ENEMY
            if (array_GameMap[row][col] == BLOCK_SPAWN) {
                CP_Settings_ImageMode(CP_POSITION_CORNER);
                CP_Image_Draw(spawn_block, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                CP_Settings_ImageMode(CP_POSITION_CENTER);
            }
            else if (array_GameMap[row][col] == BLOCK_END) {
                CP_Settings_ImageMode(CP_POSITION_CORNER);
                if (minions_in_base <= 2) {
                    CP_Image_Draw(base_block0, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                }
                else if (minions_in_base > 2 && minions_in_base <= 5) {
                    CP_Image_Draw(base_block1, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                }
                else if (minions_in_base > 5 && minions_in_base <= 8) {
                    CP_Image_Draw(base_block2, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                }
                else if (minions_in_base >= 9) {
                    CP_Image_Draw(base_block3, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                }
                CP_Settings_ImageMode(CP_POSITION_CENTER);
            }
            else if (array_GameMap[row][col] == BLOCK_PRESENT || array_GameMap[row][col] == BLOCK_TOWER_ENEMY) {
                CP_Settings_ImageMode(CP_POSITION_CORNER);
                CP_Image_Draw(present_block, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                CP_Settings_ImageMode(CP_POSITION_CENTER);
            }
            else if (array_GameMap[row][col] != BLOCK_EMPTY && array_GameMap[row][col] != BLOCK_ENEMY && array_GameMap[row][col] != BLOCK_ENEMY_DEAD) {
                CP_Graphics_DrawRect((float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
            }
            else {
                CP_Settings_ImageMode(CP_POSITION_CORNER);
                CP_Image_Draw(empty_block, (float)BlockPositionX, (float)BlockPositionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE, 255);
                CP_Settings_ImageMode(CP_POSITION_CENTER);
            }
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
                        if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Teleport_DiffusionMap[row][col - 1][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row][col - 1][1] = TRUE;
                        }
                    }
                    if ((col + 1) < MAP_GRID_COLS && array_Teleport_DiffusionMap[row][col + 1][1] == FALSE) {
                        if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Teleport_DiffusionMap[row][col + 1][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row][col + 1][1] = TRUE;
                        }
                    }
                    if ((row - 1) >= 0 && array_Teleport_DiffusionMap[row - 1][col][1] == FALSE) {
                        if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Teleport_DiffusionMap[row - 1][col][0] = array_Teleport_DiffusionMap[row][col][0] / 2;
                            array_Teleport_DiffusionMap[row - 1][col][1] = TRUE;
                        }
                    }
                    if ((row + 1) < MAP_GRID_ROWS && array_Teleport_DiffusionMap[row + 1][col][1] == FALSE) {
                        if (array_GameMap[row + 1][col] == BLOCK_EMPTY || array_GameMap[row + 1][col] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
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
                        if (array_GameMap[row][col - 1] == BLOCK_EMPTY || array_GameMap[row][col - 1] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Collaborative_DiffusionMap[row][col - 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row][col - 1][1] = TRUE;
                        }
                    }
                    if ((col + 1) < MAP_GRID_COLS && array_Collaborative_DiffusionMap[row][col + 1][1] == FALSE) {
                        if (array_GameMap[row][col + 1] == BLOCK_EMPTY || array_GameMap[row][col + 1] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Collaborative_DiffusionMap[row][col + 1][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row][col + 1][1] = TRUE;
                        }
                    }
                    if ((row - 1) >= 0 && array_Collaborative_DiffusionMap[row - 1][col][1] == FALSE) {
                        if (array_GameMap[row - 1][col] == BLOCK_EMPTY || array_GameMap[row - 1][col] == BLOCK_ENEMY
                            || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
                            array_Collaborative_DiffusionMap[row - 1][col][0] = array_Collaborative_DiffusionMap[row][col][0] / 2;
                            array_Collaborative_DiffusionMap[row - 1][col][1] = TRUE;
                        }
                    }
                    if ((row + 1) < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[row + 1][col][1] == FALSE
                        || array_GameMap[row][col - 1] == BLOCK_ENEMY_DEAD) {
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
                    //CP_Settings_RectMode(CP_POSITION_CENTER);
                    //assign_enemy_color(which_enemy);
                    //CP_Graphics_DrawRect((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_SIZE], (float)array_EnemyStats[which_enemy][ENEMY_SIZE]);
                    if (array_EnemyStats[which_enemy][ENEMY_TYPE] == GUARD_ENEMY) {
                        CP_Image_Draw(guard_enemy, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] -10, 80, 150, 255);
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == SLOW_ENEMY) {
                        CP_Image_Draw(slow_tower, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - 40, 110, 160, 255);
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == HEALING_TOWER) {
                        CP_Image_Draw(healing_tower, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - 40, 120, 160, 255);
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == RANGED_TOWER) {
                        CP_Image_Draw(ranged_tower, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - 40, 100, 180, 255);
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == DUMMY_ENEMY) {
                        CP_Image_Draw(dummy_enemy, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - 10, 100, 100, 255);
                    }
                    else if (array_EnemyStats[which_enemy][ENEMY_TYPE] == DUMMY_TOWER) {
                        CP_Image_Draw(dummy_enemy_tower, (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - 40, 100, 180, 255);
                    }
                    CP_Settings_RectMode(CP_POSITION_CORNER);
                    renderguardhp_bar(which_enemy);
                    array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[which_enemy][ENEMY_SIZE];
                    array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[which_enemy][ENEMY_SIZE];

                    //assign_enemy_color(which_enemy);
                    render_enemy_special_attack_bar(which_enemy);
                }
                else if (array_EnemyStats[which_enemy][ENEMY_HP] <= 0) {
                    for (int i = 0; i < MINION_MAX; i++) {
                        array_isMinionAttacked[which_enemy][i] = 0; //now attacking anyone anymore
                        array_isMinionBlocked[which_enemy][i] = 0;
                    }
                    if (array_EnemyStats[which_enemy][ENEMY_TYPE] == GUARD_ENEMY || array_EnemyStats[which_enemy][ENEMY_TYPE] == DUMMY_ENEMY) {
                        array_EnemyStats[which_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] = 0;
                        array_GameMap[row][col] = BLOCK_ENEMY_DEAD;
                        if (array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER_STARTED] == FALSE) {
                            array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER] = 0;
                            array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER_STARTED] = TRUE;
                        }
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

void projectile_logic() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_TOWER_ENEMY) { //only need to check for tower
                int which_enemy = check_which_enemy(row, col);
                array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] = origin_map_coordinateX + BLOCK_SIZE * col + array_EnemyStats[which_enemy][ENEMY_SIZE];
                array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] = origin_map_coordinateY + BLOCK_SIZE * row + array_EnemyStats[which_enemy][ENEMY_SIZE];
                float first_pos = ((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] / array_EnemyStats[which_enemy][ENEMY_COL]);
                float second_pos = ((float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] / array_EnemyStats[which_enemy][ENEMY_ROW]);
                float right_limit = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] + first_pos + first_pos / 2;
                float left_limit = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] - first_pos - first_pos / 2;
                float top_limit = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] - second_pos - second_pos / 2;
                float bot_limit = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] + second_pos + second_pos / 2;
                /*
                if (minion_count > 0) {
                    if (target_lock == 0) {
                        is_Search = 1;
                    }
                    else {
                        is_Search = 0;
                    }
                    if (is_Search == 1) {
                        for (int i = 0; i < minion_count; i++) {
                            if (((array_MinionStats[i][X]) <= right_limit && ((array_MinionStats[i][X]) >= left_limit)) &&
                                (array_MinionStats[i][Y] >= top_limit && array_MinionStats[i][Y] <= bot_limit)) {

                                projectile[i][X] = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES];
                                projectile[i][Y] = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES];
                                array_target[i][X] = array_MinionStats[i][X];
                                array_target[i][Y] = array_MinionStats[i][Y];
                                target_lock = 1;
                                in_range = 1;
                            }
                        }
                    } */
                if (minion_count > 0) {
                    int attacked_minion = 0;
                    /*
                    array_is_attacking[which_enemy] = FALSE;
                    for (int i = 0; i < MINION_MAX; i++) {
                        if (array_isMinionAttacked[which_enemy][i] == 1) { //is currently attacking an enemy
                            attacked_minion = i;
                            array_is_attacking[which_enemy] = TRUE;
                        }
                    }
                    */
                    if (array_is_attacking[which_enemy] == FALSE) {
                        for (int i = 0; i < minion_count; i++) {
                            if (((array_MinionStats[i][X]) <= right_limit && ((array_MinionStats[i][X]) >= left_limit)) &&
                                (array_MinionStats[i][Y] >= top_limit && array_MinionStats[i][Y] <= bot_limit)) {
                                projectile[which_enemy][i][X] = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES];
                                projectile[which_enemy][i][Y] = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES];
                                array_target[which_enemy][i][X] = array_MinionStats[i][X];
                                array_target[which_enemy][i][Y] = array_MinionStats[i][Y];
                                array_is_attacking[which_enemy] = TRUE;
                                array_isMinionAttacked[which_enemy][i] = 1;
                                attacked_minion = i;
                                in_range[which_enemy] = 1;
                                break; //now that minion is chosen, we break out of the loop
                            }
                        }
                    }
                    if (array_is_attacking[which_enemy] == FALSE) {
                        for (int i = 0; i < minion_count; i++) {
                            projectile[which_enemy][i][X] = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES];
                            projectile[which_enemy][i][Y] = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES];
                        }
                    }
                    //if minion is not searching for something to attack
                    else if (array_is_attacking[which_enemy] == TRUE) {
                        for (int i = 0; i < minion_count; i++) {
                            //generates new projectiles constantly
                            if (proj_count < PROJ_MAX) {
                                projectile[which_enemy][i][IS_ALIVE] = 1; //5 projectiles max, with 8 different stats (the array parameters)
                                proj_count++;
                            }
                            //update to the minion's coordnates
                            if (proj_count > 0) {
                                array_target[which_enemy][i][X] = array_MinionStats[attacked_minion][X];
                                array_target[which_enemy][i][Y] = array_MinionStats[attacked_minion][Y];
                                projectile_move(which_enemy, i);
                                projectile_colliding(which_enemy, i);
                            }
                            //projectile has hit the target, so now update the coordinates to be the tower's, like a prep stage
                            if (projectile[which_enemy][i][IS_ALIVE] == 0) {
                                projectile[which_enemy][i][X] = (float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES];
                                projectile[which_enemy][i][Y] = (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES];
                                array_is_attacking[which_enemy] = FALSE;
                                array_isMinionAttacked[which_enemy][attacked_minion] = 0;
                                in_range[which_enemy] = 0;
                                projectile_recycle(which_enemy, i);
                                //target_lock = 0;
                                //l_time = 0;
                                l_time[which_enemy] = 0;
                            }
                            if (array_MinionStats[i][MINION_HP] < 0) {
                                minion_dies_array_recycle(i);
                            }
                        }
                    }
                }
            }
        }
    }
}
/*
int check_projectile_basic_attack_charge(int i) {
    if (projectile[i][PROJ_CHARGE] >= projectile[i][PROJ_CHARGE_TIME]) {
        projectile[i][PROJ_CHARGE] = 0;
        return 1;
    }
    return 0;
}
*/

void projectile_recycle(int which_enemy, int dead_proj) {
    float temp_proj_array[ENEMY_MAX][PROJ_MAX][PROJ_STATS];

    for (int i = 0; i < dead_proj; i++) {
        for (int x = 0; x < PROJ_STATS; x++) {
            temp_proj_array[which_enemy][i][x] = projectile[which_enemy][i][x];
        }
    }
    int dead_proj_num = dead_proj;
    for (int k = (dead_proj_num + 1); k <= dead_proj; k++, dead_proj_num++) {
        for (int m = 0; m < PROJ_STATS; m++) {
            temp_proj_array[which_enemy][dead_proj_num][m] = projectile[which_enemy][k][m];
        }
    }
    for (int h = 0; h < proj_count; h++) {
        for (int n = 0; n < PROJ_STATS; n++) {
            projectile[which_enemy][h][n] = temp_proj_array[which_enemy][h][n];
        }
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        for (int j = 0; j < PROJ_MAX; j++) {
            projectile[i][j][IS_ALIVE] = 1;
        }
    }
    proj_count--;
}

void projectile_colliding(int which_enemy, int i) {
    float min_limit_right = (float)array_MinionStats[i][X] + (((float)array_MinionStats[i][MINION_SIZE] / 16) * 5);
    float min_limit_left = (float)array_MinionStats[i][X] - (((float)array_MinionStats[i][MINION_SIZE] / 16) * 5);
    float min_limit_bot = (float)array_MinionStats[i][Y] + (((float)array_MinionStats[i][MINION_SIZE] / 16) * 5);
    float min_limit_top = (float)array_MinionStats[i][Y] - (((float)array_MinionStats[i][MINION_SIZE] / 16) * 5);
    float proj_limit_right = (float)projectile[which_enemy][i][X] + (float)PROJ_SIZE / 2;
    float proj_limit_left = (float)projectile[which_enemy][i][X] - (float)PROJ_SIZE / 2;
    float proj_limit_bot = (float)projectile[which_enemy][i][Y] + (float)PROJ_SIZE / 2;
    float proj_limit_top = (float)projectile[which_enemy][i][Y] - (float)PROJ_SIZE / 2;
    if (proj_limit_left <= min_limit_right && proj_limit_right >= min_limit_left)
    {
        if (proj_limit_top <= min_limit_bot && proj_limit_bot >= min_limit_top)
        {
            projectile[which_enemy][i][IS_ALIVE] = 0;
            array_is_attacking[which_enemy] = FALSE;
            //int attacked = 0;
            array_MinionStats[i][MINION_HP] -= array_EnemyStats[i][ENEMY_ATTACK];
            //array_MinionStats[i][MINION_HP] -= 1;
        }
    }
}

void projectile_render() {
    CP_Settings_Fill(COLOR_GREEN);
    //CP_Graphics_DrawRect(20.f, 20.f, 20, 20);
    /*
    if (in_range[which_enemy] == 1)
    {
        //if ((projectile[which_enemy][i][IS_ALIVE] == 1) && (minion_count > 0))
        if (minion_count > 0 && array_EnemyStats[which_enemy][ENEMY_HP] > 0)
        {
            CP_Graphics_DrawRect(projectile[which_enemy][i][X], projectile[which_enemy][i][Y], PROJ_SIZE, PROJ_SIZE);
        }
    }
    */
    for (int i = 0; i < PROJ_MAX; i++) {
        for (int j = 0; j < ENEMY_MAX; j++) {
            if (array_EnemyStats[j][ENEMY_TYPE] == DAMAGE_ENEMY) {
                for (int t = 0; t < MINION_MAX; t++) {
                    if (array_isMinionAttacked[j][t] == 1 && minion_count > 0
                        && array_EnemyStats[j][ENEMY_HP] > 0 && in_range[j] == 1
                        && array_is_attacking[j] == TRUE) {
                        CP_Graphics_DrawRect(projectile[j][i][X], projectile[j][i][Y], PROJ_SIZE, PROJ_SIZE);
                    }
                }
            }
        }
    }
    /*
    for (int i = 0; i < PROJ_MAX; i++) {
        for (int j = 0; j < ENEMY_MAX; j++) {
            int chosen_enemy;
            if (array_EnemyStats[j][ENEMY_TYPE] == DAMAGE_ENEMY) {
                for (int t = 0; t < MINION_MAX; t++) {
                    if (array_isMinionAttacked[j][t] == 1) {
                        chosen_enemy = j;
                        projectile_render(chosen_enemy, i);
                    }
                }
            }
        }
    }
    */
}

void projectile_move(int which_enemy, int i) {
    float vectorX = (float)array_target[which_enemy][i][X];
    float vectorY = (float)array_target[which_enemy][i][Y];
    projectile[which_enemy][i][X] = projectile[which_enemy][i][X] + (l_time[which_enemy] * ((vectorX - projectile[which_enemy][i][X]) / 80));
    projectile[which_enemy][i][Y] = projectile[which_enemy][i][Y] + (l_time[which_enemy] * ((vectorY - projectile[which_enemy][i][Y]) / 80));
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
                    //assign_minion_color(minion_count);
                    //CP_Graphics_DrawCircle((float)array_MinionStats[minion_count][X], (float)array_MinionStats[minion_count][Y], (float)array_MinionStats[minion_count][MINION_SIZE]);
                    render_minion_sprite(minion_count);
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

void load_all_sprites(void) {
    spam_minion = CP_Image_Load("./Assets/sprites/spam_minion_image.png");
    warrior_minion = CP_Image_Load("./Assets/sprites/warrior_minion_image.png");
    tank_minion = CP_Image_Load("./Assets/sprites/tank_minion_image.png");
    wizard_minion = CP_Image_Load("./Assets/sprites/wizard_minion_image.png");
    healer_minion = CP_Image_Load("./Assets/sprites/healer_minion_image.png");

    fspam_minion = CP_Image_Load("./Assets/sprites/spam_minion_flip_image.png");
    fwarrior_minion = CP_Image_Load("./Assets/sprites/warrior_minion_flip_image.png");
    ftank_minion = CP_Image_Load("./Assets/sprites/tank_minion_flip_image.png");
    fwizard_minion = CP_Image_Load("./Assets/sprites/wizard_minion_flip_image.png");
    fhealer_minion = CP_Image_Load("./Assets/sprites/healer_minion_flip_image.png");

    ftutorial_minion = CP_Image_Load("./Assets/sprites/tutorial_minion_flip_image.png");

    guard_enemy = CP_Image_Load("./Assets/sprites/guard_enemy_image.png");
    slow_tower = CP_Image_Load("./Assets/sprites/slow_tower_image.png");
    healing_tower = CP_Image_Load("./Assets/sprites/healing_tower_image.png");
    ranged_tower = CP_Image_Load("./Assets/sprites/ranged_tower_image.png");

    empty_block = CP_Image_Load("./Assets/sprites/empty_block_image.png");
    spawn_block = CP_Image_Load("./Assets/sprites/spawn_block_image.png");
    base_block0 = CP_Image_Load("./Assets/sprites/base/base_image.png");
    base_block1 = CP_Image_Load("./Assets/sprites/base/base_image1.png");
    base_block2 = CP_Image_Load("./Assets/sprites/base/base_image2.png");
    base_block3 = CP_Image_Load("./Assets/sprites/base/base_image3.png");
    present_block = CP_Image_Load("./Assets/sprites/present_block_image.png");

    tutorial_minion = CP_Image_Load("./Assets/sprites/tutorial_minion_image.png");
    dummy_enemy = CP_Image_Load("./Assets/sprites/dummy_enemy_image.png");
    dummy_enemy_tower = CP_Image_Load("./Assets/sprites/dummy_enemy_tower_image.png");
}

//and sounds too
void free_all_sprites(void) {
    CP_Image_Free(&spam_minion);
    CP_Image_Free(&warrior_minion);
    CP_Image_Free(&tank_minion);
    CP_Image_Free(&wizard_minion);
    CP_Image_Free(&healer_minion);

    CP_Image_Free(&fspam_minion);
    CP_Image_Free(&fwarrior_minion);
    CP_Image_Free(&ftank_minion);
    CP_Image_Free(&fwizard_minion);
    CP_Image_Free(&fhealer_minion);

    CP_Image_Free(&guard_enemy);
    CP_Image_Free(&slow_tower);
    CP_Image_Free(&healing_tower);
    CP_Image_Free(&ranged_tower);

    CP_Image_Free(&empty_block);
    CP_Image_Free(&spawn_block);
    CP_Image_Free(&base_block0);
    CP_Image_Free(&base_block1);
    CP_Image_Free(&base_block2);
    CP_Image_Free(&base_block3);
    CP_Image_Free(&present_block);

    CP_Image_Free(&main_menu_image);

    CP_Image_Free(&tutorial_minion);
    CP_Image_Free(&ftutorial_minion);
    CP_Image_Free(&dummy_enemy);
    CP_Image_Free(&dummy_enemy_tower);

    CP_Sound_Free(&minion_voice);
}

void render_minion_sprite(int minion) {
    if (array_MinionStats[minion][MINION_TYPE] == SPAM_MINION) {
        if (array_MinionStats[minion][MINION_DIRECTION] == RIGHT) {
            CP_Image_Draw(fspam_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
        else {
            CP_Image_Draw(spam_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
    }
    else if (array_MinionStats[minion][MINION_TYPE] == WARRIOR_MINION) {
        if (array_MinionStats[minion][MINION_DIRECTION] == RIGHT) {
            CP_Image_Draw(fwarrior_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
        else {
            CP_Image_Draw(warrior_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
    }
    else if (array_MinionStats[minion][MINION_TYPE] == TANK_MINION) {
        if (array_MinionStats[minion][MINION_DIRECTION] == RIGHT) {
            CP_Image_Draw(ftank_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
        else {
            CP_Image_Draw(tank_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
    }
    else if (array_MinionStats[minion][MINION_TYPE] == WIZARD_MINION) {
        if (array_MinionStats[minion][MINION_DIRECTION] == RIGHT) {
            CP_Image_Draw(fwizard_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
        else {
            CP_Image_Draw(wizard_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
        }
    }
    else if (array_MinionStats[minion][MINION_TYPE] == HEALER_MINION) {
        if (array_MinionStats[minion][MINION_DIRECTION] == RIGHT) {
            CP_Image_Draw(fhealer_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);

        }
        else {
            CP_Image_Draw(healer_minion, (float)array_MinionStats[minion][X], (float)array_MinionStats[minion][Y], (float)array_MinionStats[minion][MINION_SIZE], (float)array_MinionStats[minion][MINION_SIZE], 255);
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
        /*
        if (array_MinionStats[i][MINION_TRAVEL_DIST] < BLOCK_SIZE) {
            array_MinionStats[i][MINION_DIRECTION] = array_MinionStats[i][MINION_PAST_DIRECTION];
        }
        */
        //else 
        if (level_has_teleporter == TRUE && array_MinionStats[i][MINION_TELEPORTED] == FALSE
            && array_MinionStats[i][MINION_TRAVEL_DIST] >= BLOCK_SIZE) {
            array_MinionStats[i][MINION_TRAVEL_DIST] = 0;
            array_MinionStats[i][MINION_DIRECTION] = //i'm pretty sure these conditions aren't working tbh
                (current_boxCOL + 1 < MAP_GRID_COLS && array_Teleport_DiffusionMap[current_boxROW][current_boxCOL + 1][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_TELEPORTER
                        || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY_DEAD))
                ? RIGHT
                : (current_boxCOL - 1 >= 0 && array_Teleport_DiffusionMap[current_boxROW][current_boxCOL - 1][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_TELEPORTER
                        || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY_DEAD))
                ? LEFT
                : (current_boxROW + 1 < MAP_GRID_ROWS && array_Teleport_DiffusionMap[current_boxROW + 1][current_boxCOL][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_TELEPORTER
                        || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY_DEAD))
                ? DOWN
                : (current_boxROW - 1 >= 0 && array_Teleport_DiffusionMap[current_boxROW - 1][current_boxCOL][0] > array_Teleport_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_TELEPORTER
                        || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY_DEAD))
                ? UP
                : STOP;
        }
        else if (array_MinionStats[i][MINION_TRAVEL_DIST] >= BLOCK_SIZE) {
            array_MinionStats[i][MINION_TRAVEL_DIST] = 0;
            array_MinionStats[i][MINION_DIRECTION] =
                (current_boxCOL + 1 < MAP_GRID_COLS && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL + 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL + 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL + 1] == BLOCK_ENEMY_DEAD))
                ? RIGHT
                : (current_boxCOL - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL - 1][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW][current_boxCOL - 1] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_END || array_GameMap[current_boxROW][current_boxCOL - 1] == BLOCK_ENEMY_DEAD))
                ? LEFT
                : (current_boxROW + 1 < MAP_GRID_ROWS && array_Collaborative_DiffusionMap[current_boxROW + 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW + 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW + 1][current_boxCOL] == BLOCK_ENEMY_DEAD))
                ? DOWN
                : (current_boxROW - 1 >= 0 && array_Collaborative_DiffusionMap[current_boxROW - 1][current_boxCOL][0] > array_Collaborative_DiffusionMap[current_boxROW][current_boxCOL][0]
                    && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_TOWER_ENEMY && array_GameMap[current_boxROW - 1][current_boxCOL] != BLOCK_PRESENT
                    && (array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_EMPTY
                        || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_END || array_GameMap[current_boxROW - 1][current_boxCOL] == BLOCK_ENEMY_DEAD))
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
                                    money += 25;
                                    array_enemy_death_timer[correct_enemy][ENEMY_DEATH_TIMER] = 0;
                                }
                            }
                            /*Single targeting system*/
                            int enemy_charged_up = check_enemy_basic_attack_charge(correct_enemy);
                            int can_attack = is_minion_being_attacked(correct_enemy, i);
                            if (enemy_charged_up == 1 && can_attack == 1) {
                                if (array_MinionStats[i][MINION_HP] > 0) {
                                    array_MinionStats[i][MINION_HP] -= array_EnemyStats[correct_enemy][ENEMY_ATTACK];
                                }
                                else if (array_MinionStats[i][MINION_HP] <= 0) {
                                    array_isMinionAttacked[correct_enemy][i] = 0;
                                }
                            }
                            /*
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
                            */

                        }
                    }
                    /*
                    if (array_MinionStats[i][MINION_HP] <= 0 && array_isMinionBlocked[correct_enemy][i] == 1) { //if minion dies
                        array_EnemyStats[correct_enemy][ENEMY_CURRENT_MINIONS_ON_BLOCK] -= array_MinionStats[i][MINION_WEIGHT];
                        if (array_isMinionBlocked[correct_enemy][i + 1] == 1) {
                            array_isMinionBlocked[correct_enemy][i] = 1;
                        }
                        else {
                            array_isMinionBlocked[correct_enemy][i] = 0;
                        }
                    }
                    */
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
            // assign_minion_color(i);
            //CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], (float)array_MinionStats[i][MINION_SIZE]);
            render_minion_sprite(i);
        }
    }
}

/*Check if the current minion is the one to be attacked by the enemy*/
int is_minion_being_attacked(int enemy, int minion) {
    if (array_isMinionAttacked[enemy][minion] == 1) {
        return 1;
    }
    //checks if the enemy is currently attacking a minion
    for (int i = 0; i < MINION_MAX; i++) {
        if (array_isMinionAttacked[enemy][i] == 1) {
            return 0;
        }
    }
    //if nothing == 1, that means that enemy is currently not attacking any minion
    array_isMinionAttacked[enemy][minion] = 1;
    return 1;
}

void healer_minion_basic_heal(int i) {
    float basic_heal_time = 0.4f;
    int check_if_can_attack = check_minion_basic_attack_charge(i);
    if (check_if_can_attack == 1) {
        if (healer_minion_basic_heals[i][CHECKER] == FALSE) {
            healer_minion_basic_heals[i][EFFECT_TIMER] = 0;
            healer_minion_basic_heals[i][CHECKER] = TRUE;
            array_minion_effect[i] = (float)healer_default_effect_size;
        }
        int minion_lowest_hp = 0;
        float full_hp1, full_hp2;
        full_hp1 = full_hp2 = 0;
        for (int j = 0; j < minion_count; j++) {
            full_hp1 = (float)find_full_hp(j);
            full_hp2 = (float)find_full_hp(minion_lowest_hp);
            float percentage_hp1 = ((float)array_MinionStats[j][MINION_HP] / full_hp1) * 100;
            float percentage_minion_lowest_hp = ((float)array_MinionStats[minion_lowest_hp][MINION_HP] / full_hp2) * 100;
            if (percentage_hp1 <= percentage_minion_lowest_hp) {
                minion_lowest_hp = j;
                array_minion_lowest_hp[i] = j;
            }
        }
        int check = (int)full_hp2 - array_MinionStats[minion_lowest_hp][MINION_HP]; //how much hp to full hp
        //if the amount of hp missing is less than the amount to be healed, check is the total healable hp
        if (check >= array_MinionStats[i][MINION_HEAL]) {
            array_MinionStats[minion_lowest_hp][MINION_HP] += array_MinionStats[i][MINION_HEAL];
        }
        else {
            array_MinionStats[minion_lowest_hp][MINION_HP] = find_full_hp(minion_lowest_hp);
        }
    }
    if (healer_minion_basic_heals[i][CHECKER] == TRUE && healer_minion_basic_heals[i][EFFECT_TIMER] < basic_heal_time) {
        int minion_lowest_hp = array_minion_lowest_hp[i];
        if (gIsPaused == FALSE) {
            array_minion_effect[i] += 1;
        }
        CP_Settings_Stroke(TRANSLUCENT_GREEN);
        CP_Settings_Fill(TRANSLUCENT_GREEN);
        CP_Graphics_DrawCircle((float)array_MinionStats[minion_lowest_hp][X], (float)array_MinionStats[minion_lowest_hp][Y], array_minion_effect[i]);
        CP_Settings_Stroke(COLOR_BLACK);
    }
    else if (healer_minion_basic_heals[i][EFFECT_TIMER] >= basic_heal_time) {
        healer_minion_basic_heals[i][CHECKER] = FALSE;
        array_minion_attack_time[i][EFFECT_TIMER] = 0; //pretty sure this is redundant lol
        array_minion_effect[i] = (float)healer_default_effect_size;
    }
}

void render_enemy_special_attack_bar(int i) {
    float charge_percentage;
    if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER || array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY
        || array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER
        /* || array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY*/) {
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
        ? 110
        : 0;
    return full_hp;
}

/*Should be tgt with render_special_effect_enemy() tbh*/
void enemy_special_attack() {
    int i = 0;
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_ENEMY || array_GameMap[row][col] == BLOCK_TOWER_ENEMY) {
                i = check_which_enemy(row, col);
            }
            if (array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] >= array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME]) {
                if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY && array_EnemyStats[i][ENEMY_HP] > 0) {
                    array_Enemy_Slow_Effect_Time[i][CHECKER] = TRUE; //attack can start
                    array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] = 0;
                }
                else if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER && array_EnemyStats[i][ENEMY_HP] > 0) {
                    int healing_amount = 25;
                    for (int j = 0; j < ENEMY_MAX; j++) {
                        if (array_EnemyStats[j][ENEMY_HP] > 0) {
                            int full_hp = find_enemy_full_hp(j);
                            if ((full_hp - array_EnemyStats[j][ENEMY_HP]) < healing_amount) {
                                array_EnemyStats[j][ENEMY_HP] = full_hp;
                            }
                            else {
                                array_EnemyStats[j][ENEMY_HP] += healing_amount;
                            }
                            array_enemy_attack_time[i][CHECKER] = TRUE;
                            array_enemy_attack_time[i][EFFECT_TIMER] = 0;
                        }
                    }
                }
                else if (array_EnemyStats[i][ENEMY_TYPE] == DAMAGE_ENEMY && array_EnemyStats[i][ENEMY_HP] > 0) {
                    //projectile_logic();
                }
                else if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
                    int minX, maxX, minY, maxY;
                    minX = array_EnemyStats[i][ENEMY_ROW_COORDINATES] - BLOCK_SIZE - (BLOCK_SIZE / 2);
                    maxX = array_EnemyStats[i][ENEMY_ROW_COORDINATES] + BLOCK_SIZE + (BLOCK_SIZE / 2);
                    minY = array_EnemyStats[i][ENEMY_COL_COORDINATES] - BLOCK_SIZE - (BLOCK_SIZE / 2);
                    maxY = array_EnemyStats[i][ENEMY_COL_COORDINATES] + BLOCK_SIZE + (BLOCK_SIZE / 2);
                    array_enemy_attack_time[i][CHECKER] = TRUE;
                    array_enemy_attack_time[i][EFFECT_TIMER] = 0;
                    for (int j = 0; j < minion_count; j++) {
                        if (array_MinionStats[j][X] > minX && array_MinionStats[j][X] < maxX
                            && array_MinionStats[j][Y] > minY && array_MinionStats[j][Y] < maxY) {
                            array_MinionStats[j][MINION_HP] -= array_EnemyStats[i][ENEMY_ATTACK];
                        }
                    }
                }
                //render_special_effect_enemy(i);
                array_EnemyCurrentCharge[i][ENEMY_CURRENT_CHARGE] = 0;
            }
        }
    }
}

void render_special_effect_enemy(int i) {
    int how_long_effect_is_slow = 3;
    float how_long_effect_is_AOE = 0.4f;
    float how_long_effect_is_heal = 0.4f;
    if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY && array_Enemy_Slow_Effect_Time[i][CHECKER] == TRUE) {
        if (array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] >= how_long_effect_is_slow || array_EnemyStats[i][ENEMY_HP] <= 0) {
            array_Enemy_Slow_Effect_Time[i][CHECKER] = FALSE;
            array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] = 0;
            for (int j = 0; j < minion_count; j++) {
                if (array_isMinionSlowed[i][j][0] == TRUE) {
                    array_MinionStats[j][MINION_MOVEMENT_SPEED] = array_isMinionSlowed[i][j][1];
                    array_isMinionSlowed[i][j][0] = FALSE;
                }
            }
        }
        else if (array_Enemy_Slow_Effect_Time[i][EFFECT_TIMER] < how_long_effect_is_slow && array_EnemyStats[i][ENEMY_HP] >= 0) {
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
                    array_MinionStats[j][MINION_MOVEMENT_SPEED] = find_minion_original_speed(j) / 3;
                    array_isMinionSlowed[i][j][0] = TRUE;
                }
                //Minion no longer in range
                if (!(array_MinionStats[j][X] > minX && array_MinionStats[j][X] < maxX
                    && array_MinionStats[j][Y] > minY && array_MinionStats[j][Y] < maxY)) {
                    if (array_isMinionSlowed[i][j][0] == TRUE) {
                        //array_MinionStats[j][MINION_MOVEMENT_SPEED] = array_isMinionSlowed[i][j][1];
                        array_MinionStats[j][MINION_MOVEMENT_SPEED] = find_minion_original_speed(j);
                        array_isMinionSlowed[i][j][0] = FALSE;
                    }
                }
            }
            int default_start_row = array_EnemyStats[i][ENEMY_ROW] - 1; //default to top left
            int default_start_col = array_EnemyStats[i][ENEMY_COL] - 1; //default to top left
            int default_iterations = 3; //3x3
            int default_iterations2 = 3;
            if (array_EnemyStats[i][ENEMY_ROW] == 0) { //don't draw the top boxes
                default_start_row += 1;
                default_iterations = 2;
            }
            else if (array_EnemyStats[i][ENEMY_ROW] == 4) { //don't draw the bottom row
                default_iterations = 2;
            }
            if (array_EnemyStats[i][ENEMY_COL] == 0) { //don't draw to the left row
                default_start_col += 1;
                default_iterations2 = 2;
            }
            else if (array_EnemyStats[i][ENEMY_COL] == 11) { //don't draw to the right row
                default_iterations2 = 2;
            }
            int starting_point1 = origin_map_coordinateY + (default_start_row * BLOCK_SIZE);
            int starting_point2 = origin_map_coordinateX + (default_start_col * BLOCK_SIZE);
            for (int s = 0; s < default_iterations; s++) {
                for (int d = 0; d < default_iterations2; d++) {
                    CP_Settings_NoStroke();
                    CP_Settings_RectMode(CP_POSITION_CORNER);
                    int special_X = starting_point2 + (s * BLOCK_SIZE);
                    int special_Y = starting_point1 + (d * BLOCK_SIZE);
                    CP_Settings_Fill(TRANSLUCENT_BLUE);
                    CP_Graphics_DrawRect((float)special_X, (float)special_Y, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
                }
            }
            CP_Settings_Stroke(COLOR_BLACK);
        }
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == HEALING_TOWER) {
        if (array_enemy_attack_time[i][EFFECT_TIMER] >= how_long_effect_is_heal || array_EnemyStats[i][ENEMY_HP] <= 0) {
            array_enemy_attack_time[i][EFFECT_TIMER] = 0;
            array_enemy_attack_time[i][CHECKER] = FALSE;
        }
        else if (array_enemy_attack_time[i][CHECKER] == TRUE && array_enemy_attack_time[i][EFFECT_TIMER] < how_long_effect_is_heal && array_EnemyStats[i][ENEMY_HP] > 0) {
            for (int j = 0; j < ENEMY_MAX; j++) {
                if (array_EnemyStats[j][ENEMY_HP] > 0) {
                    CP_Settings_NoStroke();
                    CP_Settings_Fill(TRANSLUCENT_GREEN);
                    CP_Settings_RectMode(CP_POSITION_CENTER);
                    CP_Graphics_DrawRect((float)array_EnemyStats[j][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[j][ENEMY_COL_COORDINATES], (float)BLOCK_SIZE - 15, (float)BLOCK_SIZE - 15);
                    CP_Settings_RectMode(CP_POSITION_CORNER);
                }
            }
            CP_Settings_Stroke(COLOR_BLACK);
        }
    }
    if (array_EnemyStats[i][ENEMY_TYPE] == RANGED_TOWER) {
        if (array_enemy_attack_time[i][EFFECT_TIMER] >= how_long_effect_is_AOE || array_EnemyStats[i][ENEMY_HP] <= 0) {
            array_enemy_attack_time[i][EFFECT_TIMER] = 0;
            array_enemy_attack_time[i][CHECKER] = FALSE;
        }
        else if (array_enemy_attack_time[i][CHECKER] == TRUE && array_enemy_attack_time[i][EFFECT_TIMER] < how_long_effect_is_AOE
            && array_EnemyStats[i][ENEMY_HP] > 0) {
            int default_start_row = array_EnemyStats[i][ENEMY_ROW] - 1; //default to top left
            int default_start_col = array_EnemyStats[i][ENEMY_COL] - 1; //default to top left
            int default_iterations = 3; //3x3
            int default_iterations2 = 3;
            if (array_EnemyStats[i][ENEMY_ROW] == 0) { //don't draw the top boxes
                default_start_row += 1;
                default_iterations = 2;
            }
            else if (array_EnemyStats[i][ENEMY_ROW] == 4) { //don't draw the bottom row
                default_iterations = 2;
            }
            if (array_EnemyStats[i][ENEMY_COL] == 0) { //don't draw to the left row
                default_start_col += 1;
                default_iterations2 = 2;
            }
            else if (array_EnemyStats[i][ENEMY_COL] == 11) { //don't draw to the right row
                default_iterations2 = 2;
            }
            int starting_point1 = origin_map_coordinateY + (default_start_row * BLOCK_SIZE);
            int starting_point2 = origin_map_coordinateX + (default_start_col * BLOCK_SIZE);
            for (int s = 0; s < default_iterations; s++) {
                for (int d = 0; d < default_iterations2; d++) {
                    CP_Settings_NoStroke();
                    CP_Settings_RectMode(CP_POSITION_CORNER);
                    int special_X = starting_point2 + (d * BLOCK_SIZE);
                    int special_Y = starting_point1 + (s * BLOCK_SIZE);
                    CP_Settings_Fill(TRANSLUCENT_PURPLE);
                    CP_Graphics_DrawRect((float)special_X, (float)special_Y, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
                }
            }
            CP_Settings_Stroke(COLOR_BLACK);
        }
    }
}

int find_minion_original_speed(int minion) {
    int minion_speed = (array_MinionStats[minion][MINION_TYPE] == SPAM_MINION)
        ? 6
        : (array_MinionStats[minion][MINION_TYPE] == WARRIOR_MINION)
        ? 5
        : (array_MinionStats[minion][MINION_TYPE] == TANK_MINION)
        ? 3
        : (array_MinionStats[minion][MINION_TYPE] == WIZARD_MINION)
        ? 4
        : (array_MinionStats[minion][MINION_TYPE] == HEALER_MINION)
        ? 3
        : 0;
    return minion_speed;
}

/*Should probably update this code to make it shorter lol*/
void minion_dies_array_recycle(int dead_minion_number) {
    int array_Temp_MinionStats[MINION_MAX][MINION_TOTAL_STATS] = { 0 };
    float array_Temp_MinionCharge[MINION_MAX][TOTAL_CHARGES] = { 0 };
    int array_Temp_isMinionSlowed[ENEMY_MAX][MINION_MAX][2] = { 0 };
    float array_Temp_minion_attack_time[MINION_MAX][2] = { 0 };
    float array_Temp_healer_minion_basic_heals[MINION_MAX][2] = { 0 };
    float array_Temp_minion_effect[MINION_MAX] = { 0 };
    //int array_Temp_isMinionBlocked[ENEMY_MAX][MINION_MAX] = { 0 };

    //make it so no enemy is targetting this number first
    for (int i = 0; i < ENEMY_MAX; i++) {
        array_isMinionAttacked[i][dead_minion_number] = 0;
    }
    //

    for (int i = 0; i < dead_minion_number; i++) {
        for (int j = 0; j < 2; j++) {
            array_Temp_minion_attack_time[i][j] = array_minion_attack_time[i][j];
            array_Temp_healer_minion_basic_heals[i][j] = healer_minion_basic_heals[i][j];
        }
    }
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
    for (int i = 0; i < dead_minion_number; i++) {
        array_Temp_minion_effect[i] = array_minion_effect[i];
    }
    //
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
    dead_min_num = dead_minion_number;
    for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
        for (int j = 0; j < 2; j++) {
            array_Temp_minion_attack_time[dead_min_num][j] = array_minion_attack_time[k][j];
            array_Temp_healer_minion_basic_heals[dead_min_num][j] = healer_minion_basic_heals[dead_min_num][j];
        }
    }
    dead_min_num = dead_minion_number;
    for (int k = (dead_min_num + 1); k <= minion_count; k++, dead_min_num++) {
        array_Temp_minion_effect[dead_min_num] = array_minion_effect[k];
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
    for (int i = 0; i < minion_count; i++) {
        for (int j = 0; j < 2; j++) {
            array_minion_attack_time[i][j] = array_Temp_minion_attack_time[i][j];
            healer_minion_basic_heals[i][j] = array_Temp_healer_minion_basic_heals[i][j];

        }
    }
    for (int i = 0; i < dead_minion_number; i++) {
        array_minion_effect[i] = array_Temp_minion_effect[i];
    }
    //very inefficient but i hope this works
    for (int i = 0; i < minion_count; i++) {
        array_MinionStats[i][MINION_MOVEMENT_SPEED] = find_minion_original_speed(i);
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
    for (int i = 0; i < minion_count; i++) {
        int current_boxCOL = (array_MinionStats[i][X] - origin_map_coordinateX) / BLOCK_SIZE;
        int current_boxROW = (array_MinionStats[i][Y] - origin_map_coordinateY) / BLOCK_SIZE;
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
        enemy_max_hp = 110;
        enemy_hp_percentage = array_EnemyStats[i][ENEMY_HP] / enemy_max_hp;
    }
    else if (array_EnemyStats[i][ENEMY_TYPE] == DUMMY_ENEMY || array_EnemyStats[i][ENEMY_TYPE] == DUMMY_TOWER) {
        enemy_max_hp = 200;
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
        if (array_MinionStats[i][MINION_TYPE] != SPAM_MINION && array_MinionStats[i][MINION_TYPE] != WARRIOR_MINION) {
            //ADDED THE != WARRIOR_MINION
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
        ? 60
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

void render_minion_special_attack() {
    float tank_minion_effect_lasts = 0.4f;
    float healer_minion_effect_lasts = 0.4f;
    float wizard_minion_effect_lasts = 0.4f;
    for (int i = 0; i < minion_count; i++) {
        if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) {
            if (array_minion_attack_time[i][CHECKER] == TRUE && array_minion_attack_time[i][EFFECT_TIMER] < tank_minion_effect_lasts) {
                if (gIsPaused == FALSE) {
                    array_minion_effect[i] += 2.5;
                }
                CP_Settings_Stroke(CP_Color_Create(153, 76, 0, 200));
                CP_Settings_Fill(TRANSLUCENT_ORANGE);
                CP_Graphics_DrawCircle((float)array_MinionStats[i][X], (float)array_MinionStats[i][Y], array_minion_effect[i]);
                CP_Settings_Stroke(COLOR_BLACK);
            }
            else if (array_minion_attack_time[i][EFFECT_TIMER] >= tank_minion_effect_lasts) {
                array_minion_attack_time[i][CHECKER] = FALSE;
                array_minion_attack_time[i][EFFECT_TIMER] = 0; //pretty sure this is redundant lol
                array_minion_effect[i] = 0;
            }
        }
        else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
            if (array_minion_attack_time[i][CHECKER] == TRUE && array_minion_attack_time[i][EFFECT_TIMER] < healer_minion_effect_lasts) {
                if (gIsPaused == FALSE) {
                    array_minion_effect[i] += 1;
                }
                for (int j = 0; j < minion_count; j++) {
                    CP_Settings_Stroke(TRANSLUCENT_GREEN);
                    CP_Settings_Fill(TRANSLUCENT_GREEN);
                    CP_Graphics_DrawCircle((float)array_MinionStats[j][X], (float)array_MinionStats[j][Y], array_minion_effect[i]);
                    CP_Settings_Stroke(COLOR_BLACK);
                }
            }
            else if (array_minion_attack_time[i][EFFECT_TIMER] >= healer_minion_effect_lasts) {
                array_minion_attack_time[i][CHECKER] = FALSE;
                array_minion_attack_time[i][EFFECT_TIMER] = 0; //pretty sure this is redundant lol
                array_minion_effect[i] = (float)healer_default_effect_size;
            }
        }
        else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
            if (array_minion_attack_time[i][CHECKER] == TRUE && array_minion_attack_time[i][EFFECT_TIMER] < wizard_minion_effect_lasts) {
                if (gIsPaused == FALSE) {
                    array_minion_effect[i] -= 2;
                    array_alpha[i][ALPHA] += 5;
                    array_alpha[i][G] += 10;
                    array_alpha[i][B] += 10;
                    if (array_alpha[i][ALPHA] > 255) {
                        array_alpha[i][ALPHA] = 255;
                    }
                    if (array_alpha[i][G] > 255) {
                        array_alpha[i][G] = 255;
                    }
                    if (array_alpha[i][B] > 255) {
                        array_alpha[i][B] = 255;
                    }
                }
                for (int t = 0; t < ENEMY_MAX; t++) {
                    if (array_enemy_to_attack[i][t] == 1 && array_EnemyStats[t][ENEMY_HP] > 0) {
                        CP_Settings_Stroke(CP_Color_Create(array_alpha[i][R], array_alpha[i][G], array_alpha[i][B], array_alpha[i][ALPHA]));
                        CP_Settings_Fill(CP_Color_Create(array_alpha[i][R], array_alpha[i][G], array_alpha[i][B], array_alpha[i][ALPHA]));
                        CP_Graphics_DrawCircle((float)array_EnemyStats[t][ENEMY_ROW_COORDINATES], (float)array_EnemyStats[t][ENEMY_COL_COORDINATES], array_minion_effect[i]);
                        CP_Settings_Stroke(COLOR_BLACK);
                    }
                }
                CP_Settings_Stroke(COLOR_BLACK);
                
            }
            else if (array_minion_attack_time[i][EFFECT_TIMER] >= wizard_minion_effect_lasts) {
                array_minion_attack_time[i][CHECKER] = FALSE;
                array_minion_attack_time[i][EFFECT_TIMER] = 0; //pretty sure this is redundant lol
            }
        }
    }
}

void minion_special_attack(int i, int current_row, int current_col) {
    int tank_range = 2, wizard_range = 3;
    if (array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] >= array_MinionCurrentCharge[i][MINION_CHARGE_TIME]) {
        if (array_MinionStats[i][MINION_TYPE] == TANK_MINION) { //restores HP and attack tower
            int full_hp = find_full_hp(i);
            if (array_MinionStats[i][MINION_HP] != full_hp) {
                array_minion_effect[i] = (float)array_MinionStats[i][MINION_SIZE];
                if (array_MinionStats[i][MINION_HP] < 205) {
                    array_MinionStats[i][MINION_HP] += 35;
                }
                else {
                    array_MinionStats[i][MINION_HP] = full_hp;
                }
            }
            minion_attacking_towers(i, current_row, current_col, tank_range);
            for (int t = 0; t < ENEMY_MAX; t++) {
                if (array_enemy_to_attack[i][t] == 1) {
                    array_EnemyStats[t][ENEMY_HP] -= 8;
                    if (array_EnemyStats[t][ENEMY_HP] <= 0) {
                        money += 25;
                        array_enemy_death_timer[t][ENEMY_DEATH_TIMER] = 0;

                    }
                }
            }
            array_minion_attack_time[i][CHECKER] = TRUE;
            array_minion_attack_time[i][EFFECT_TIMER] = 0;
            array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
        }
        else if (array_MinionStats[i][MINION_TYPE] == WIZARD_MINION) {
            int should_wizard_attack = 0;
            minion_attacking_towers(i, current_row, current_col, wizard_range);
            for (int t = 0; t < ENEMY_MAX; t++) {
                if (array_enemy_to_attack[i][t] == 1) {
                    should_wizard_attack++;
                }
            }
            if (should_wizard_attack > 0) { //there is something to attack
                array_alpha[i][ALPHA] = 140;
                array_alpha[i][R] = 0;
                array_alpha[i][G] = 0;
                array_alpha[i][B] = 153;
                array_minion_effect[i] = 65; //PAY ATTENTION

                array_minion_attack_time[i][CHECKER] = TRUE;
                array_minion_attack_time[i][EFFECT_TIMER] = 0;
                for (int t = 0; t < ENEMY_MAX; t++) {
                    if (array_enemy_to_attack[i][t] == 1) {
                        array_EnemyStats[t][ENEMY_HP] -= 35;
                        if (array_EnemyStats[t][ENEMY_HP] <= 0) {
                            money += 25;
                            array_enemy_death_timer[t][ENEMY_DEATH_TIMER] = 0;
                        }
                    }
                }
                array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
            }
            else if (should_wizard_attack == 0) {
                array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = array_MinionCurrentCharge[i][MINION_CHARGE_TIME];
            }
        }
        else if (array_MinionStats[i][MINION_TYPE] == HEALER_MINION) {
            array_minion_attack_time[i][CHECKER] = TRUE;
            array_minion_attack_time[i][EFFECT_TIMER] = 0;
            array_minion_effect[i] = (float)healer_default_effect_size;
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
            array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
        }
        //array_MinionCurrentCharge[i][MINION_CURRENT_CHARGE] = 0;
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

void bring_back_enemy() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_ENEMY_DEAD) {
                int which_enemy = check_which_enemy(row, col);
                if (array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER] >= death_recharge_time && array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER_STARTED] == TRUE) {
                    int new_hp = find_enemy_full_hp(which_enemy);
                    array_EnemyStats[which_enemy][ENEMY_HP] = new_hp / (int)(array_enemy_death_timer[which_enemy][ENEMY_DEATH_COUNTER] + 2);
                    array_GameMap[row][col] = BLOCK_ENEMY;
                    array_enemy_death_timer[which_enemy][ENEMY_DEATH_COUNTER] += 1;
                    array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER] = 0;
                    array_EnemyStats[which_enemy][ENEMY_DEATH_TIMER_STARTED] = FALSE;
                }
            }
        }
    }
}

void render_enemy_death_comeback_bar() {
    for (int row = 0; row < MAP_GRID_ROWS; ++row) {
        for (int col = 0; col < MAP_GRID_COLS; ++col) {
            if (array_GameMap[row][col] == BLOCK_ENEMY_DEAD) {
                float long_length = 120;
                int which_enemy = check_which_enemy(row, col);
                if (array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER_STARTED] == TRUE) {
                    float charge_percentage = (array_enemy_death_timer[which_enemy][ENEMY_DEATH_TIMER] / (float)death_recharge_time) * long_length;
                    CP_Settings_Fill(COLOR_BLACK);
                    CP_Graphics_DrawRect((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] - 60, (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] + 50, long_length, (float)HP_BAR_HEIGHT);
                    CP_Settings_Fill(COLOR_MORE_BLUE);
                    CP_Graphics_DrawRect((float)array_EnemyStats[which_enemy][ENEMY_ROW_COORDINATES] - 60, (float)array_EnemyStats[which_enemy][ENEMY_COL_COORDINATES] + 50, charge_percentage, (float)HP_BAR_HEIGHT);
                }
            }
        }
    }
}

/*IMPORTANT - BEFORE UPDATING ANY VALUE HERE, CTRL+F TO CHECK IF IT HAS BEEN USED ELSEWHERE AND UPDATE ACCORDINGLY*/
/*for example HP is used in rendering the hp bars. Thanks! :D*/
void assign_minion_stats() {
    if (array_MinionStats[minion_count][MINION_TYPE] == SPAM_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 60;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 6; //original speed was 8
        array_MinionStats[minion_count][MINION_ATTACK] = 10;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 30;
        array_MinionStats[minion_count][MINION_SIZE] = 65;
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 0.5f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WARRIOR_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 130;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 5;
        array_MinionStats[minion_count][MINION_ATTACK] = 20;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 60;
        array_MinionStats[minion_count][MINION_SIZE] = 110;
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
        array_MinionStats[minion_count][MINION_SIZE] = 140;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 4;
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 1.f;
    }
    if (array_MinionStats[minion_count][MINION_TYPE] == WIZARD_MINION) {
        array_MinionStats[minion_count][MINION_HP] = 80;
        array_MinionStats[minion_count][MINION_MOVEMENT_SPEED] = 4;
        array_MinionStats[minion_count][MINION_ATTACK] = 4;
        array_MinionStats[minion_count][MINION_ATTACK_SPEED] = 2;
        array_MinionStats[minion_count][MINION_WEIGHT] = 1;
        array_MinionStats[minion_count][MINION_COST] = 150;
        array_MinionStats[minion_count][MINION_SIZE] = 100;
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
        array_MinionStats[minion_count][MINION_SIZE] = 90;
        array_MinionStats[minion_count][MINION_HEAL] = 20;
        array_MinionCurrentCharge[minion_count][MINION_CHARGE_TIME] = 7; //super healing???
        array_MinionCurrentCharge[minion_count][MINION_BASIC_ATTACK_SPEED] = 2.f;
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
            array_EnemyStats[i][ENEMY_ATTACK] = 1;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 5;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 2;
            array_EnemyStats[i][PROJ_CHARGE] = 0;
            array_EnemyStats[i][PROJ_CHARGE_TIME] = 3;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
            array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME] = 0.1f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == SLOW_ENEMY) {
            array_EnemyStats[i][ENEMY_HP] = 120;
            array_EnemyStats[i][ENEMY_ATTACK] = 20;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 2;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
            array_EnemyStats[i][PROJ_CHARGE] = 0;
            array_EnemyStats[i][PROJ_CHARGE_TIME] = 3;
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
            array_EnemyStats[i][ENEMY_HP] = 110;
            array_EnemyStats[i][ENEMY_ATTACK] = 7;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 4;
            array_EnemyStats[i][PROJ_CHARGE] = 0;
            array_EnemyStats[i][PROJ_CHARGE_TIME] = 3;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
            array_EnemyCurrentCharge[i][ENEMY_CHARGE_TIME] = 2.f;
        }
        if (array_EnemyStats[i][ENEMY_TYPE] == DUMMY_ENEMY || array_EnemyStats[i][ENEMY_TYPE] == DUMMY_TOWER) {
            array_EnemyStats[i][ENEMY_HP] = 200;
            array_EnemyStats[i][ENEMY_ATTACK] = 0;
            array_EnemyStats[i][ENEMY_ATTACK_SPEED] = 3;
            array_EnemyStats[i][ENEMY_BLOCK] = 2;
            array_EnemyStats[i][ENEMY_SIZE] = BLOCK_SIZE / 2;
            array_EnemyStats[i][ENEMY_RANGE] = 1;
            array_EnemyCurrentCharge[i][ENEMY_BASIC_ATTACK_SPEED] = 0.5f;
        }
    }
}

/*Levels*/
void level_tutorial() {
    if (tutorial_part < 11) {
        array_GameMap[2][11] = BLOCK_SPAWN;
        array_GameMap[2][1] = BLOCK_END;
    }

    if (tutorial_part >= 11) {
        array_GameMap[2][9] = BLOCK_SPAWN;
        array_GameMap[2][3] = BLOCK_END;

        array_GameMap[1][7] = BLOCK_TOWER_ENEMY;
        array_EnemyStats[0][ENEMY_ROW] = 1;
        array_EnemyStats[0][ENEMY_COL] = 7;
        array_EnemyStats[0][ENEMY_TYPE] = DUMMY_TOWER;

        array_GameMap[2][6] = BLOCK_ENEMY;
        array_EnemyStats[1][ENEMY_ROW] = 2;
        array_EnemyStats[1][ENEMY_COL] = 6;
        array_EnemyStats[1][ENEMY_TYPE] = DUMMY_ENEMY;

        array_GameMap[1][11] = BLOCK_PRESENT;
        array_GameMap[1][10] = BLOCK_PRESENT;
        array_GameMap[1][9] = BLOCK_PRESENT;
        array_GameMap[1][8] = BLOCK_PRESENT;
        array_GameMap[1][6] = BLOCK_PRESENT;
        array_GameMap[1][5] = BLOCK_PRESENT;
        array_GameMap[1][4] = BLOCK_PRESENT;
        array_GameMap[1][3] = BLOCK_PRESENT;
        array_GameMap[1][2] = BLOCK_PRESENT;
        array_GameMap[1][1] = BLOCK_PRESENT;
        array_GameMap[1][0] = BLOCK_PRESENT;
        array_GameMap[3][11] = BLOCK_PRESENT;
        array_GameMap[3][10] = BLOCK_PRESENT;
        array_GameMap[3][9] = BLOCK_PRESENT;
        array_GameMap[3][8] = BLOCK_PRESENT;
        array_GameMap[3][7] = BLOCK_PRESENT;
        array_GameMap[3][6] = BLOCK_PRESENT;
        array_GameMap[3][5] = BLOCK_PRESENT;
        array_GameMap[3][4] = BLOCK_PRESENT;
        array_GameMap[3][3] = BLOCK_PRESENT;
        array_GameMap[3][2] = BLOCK_PRESENT;
        array_GameMap[3][1] = BLOCK_PRESENT;
        array_GameMap[3][0] = BLOCK_PRESENT;
    }

    initial_direction = LEFT;
    level_has_teleporter = FALSE;
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
    array_GameMap[4][5] = BLOCK_PRESENT;
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

    array_GameMap[3][5] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 3;
    array_EnemyStats[3][ENEMY_COL] = 5;
    array_EnemyStats[3][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[0][6] = BLOCK_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 0;
    array_EnemyStats[4][ENEMY_COL] = 6;
    array_EnemyStats[4][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[1][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 1;
    array_EnemyStats[5][ENEMY_COL] = 6;
    array_EnemyStats[5][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][8] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 8;
    array_EnemyStats[6][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[4][8] = BLOCK_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 4;
    array_EnemyStats[7][ENEMY_COL] = 8;
    array_EnemyStats[7][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[2][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 2;
    array_EnemyStats[8][ENEMY_COL] = 10;
    array_EnemyStats[8][ENEMY_TYPE] = RANGED_TOWER;

    initial_direction = DOWN;
    level_has_teleporter = FALSE;
}

void level_2() {
    array_GameMap[0][11] = BLOCK_SPAWN;
    array_GameMap[4][1] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[0][3] = BLOCK_PRESENT;
    array_GameMap[0][5] = BLOCK_PRESENT;
    array_GameMap[0][8] = BLOCK_PRESENT;
    array_GameMap[1][5] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[1][10] = BLOCK_PRESENT;
    array_GameMap[1][11] = BLOCK_PRESENT;
    array_GameMap[3][3] = BLOCK_PRESENT;
    array_GameMap[2][1] = BLOCK_PRESENT;
    array_GameMap[2][8] = BLOCK_PRESENT;
    array_GameMap[3][8] = BLOCK_PRESENT;
    array_GameMap[2][5] = BLOCK_PRESENT;
    array_GameMap[3][0] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[4][2] = BLOCK_PRESENT;

    initial_direction = LEFT;
    /*Enemies*/
    array_GameMap[3][7] = BLOCK_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 3;
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
    array_EnemyStats[3][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[1][1] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 1;
    array_EnemyStats[4][ENEMY_COL] = 1;
    array_EnemyStats[4][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[1][3] = BLOCK_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 1;
    array_EnemyStats[5][ENEMY_COL] = 3;
    array_EnemyStats[5][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][1] = BLOCK_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 1;
    array_EnemyStats[6][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 3;
    array_EnemyStats[7][ENEMY_COL] = 10;
    array_EnemyStats[7][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[4][10] = BLOCK_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 4;
    array_EnemyStats[8][ENEMY_COL] = 10;
    array_EnemyStats[8][ENEMY_TYPE] = GUARD_ENEMY;

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
    array_EnemyStats[4][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][2] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 3;
    array_EnemyStats[5][ENEMY_COL] = 2;
    array_EnemyStats[5][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][10] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 10;
    array_EnemyStats[6][ENEMY_TYPE] = RANGED_TOWER;

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

void level_4() {
    array_GameMap[2][5] = BLOCK_SPAWN;
    array_GameMap[4][2] = BLOCK_END;

    /*Filler Blocks*/
    array_GameMap[1][0] = BLOCK_PRESENT;
    array_GameMap[1][1] = BLOCK_PRESENT;
    array_GameMap[3][1] = BLOCK_PRESENT;
    array_GameMap[4][1] = BLOCK_PRESENT;
    array_GameMap[3][5] = BLOCK_PRESENT;
    array_GameMap[3][6] = BLOCK_PRESENT;
    array_GameMap[2][4] = BLOCK_PRESENT;
    array_GameMap[2][8] = BLOCK_PRESENT;
    array_GameMap[4][10] = BLOCK_PRESENT;
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

    array_GameMap[4][9] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 4;
    array_EnemyStats[4][ENEMY_COL] = 9;
    array_EnemyStats[4][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[2][2] = BLOCK_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 2;
    array_EnemyStats[5][ENEMY_COL] = 2;
    array_EnemyStats[5][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][7] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 7;
    array_EnemyStats[6][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 3;
    array_EnemyStats[7][ENEMY_COL] = 3;
    array_EnemyStats[7][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][8] = BLOCK_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 3;
    array_EnemyStats[8][ENEMY_COL] = 8;
    array_EnemyStats[8][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[1][4] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[9][ENEMY_ROW] = 1;
    array_EnemyStats[9][ENEMY_COL] = 4;
    array_EnemyStats[9][ENEMY_TYPE] = RANGED_TOWER;

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
    array_EnemyStats[1][ENEMY_TYPE] = RANGED_TOWER;


    array_GameMap[3][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 3;
    array_EnemyStats[2][ENEMY_COL] = 3;
    array_EnemyStats[2][ENEMY_TYPE] = RANGED_TOWER;

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

    array_GameMap[3][2] = BLOCK_ENEMY;
    array_EnemyStats[9][ENEMY_ROW] = 3;
    array_EnemyStats[9][ENEMY_COL] = 2;
    array_EnemyStats[9][ENEMY_TYPE] = GUARD_ENEMY;

    /*Using Teleporter*/
    level_has_teleporter = TRUE;
    array_GameMap[2][11] = BLOCK_TELEPORTER;
    array_GameMap[1][0] = BLOCK_TELEPORT_SPAWN;


    initial_direction = LEFT;
    /*placeholders, please change everything*/
}

void level_6() {
    array_GameMap[0][0] = BLOCK_SPAWN;
    array_GameMap[4][6] = BLOCK_END;

    array_GameMap[1][0] = BLOCK_PRESENT;
    array_GameMap[1][1] = BLOCK_PRESENT;
    array_GameMap[2][4] = BLOCK_PRESENT;
    array_GameMap[0][4] = BLOCK_PRESENT;
    array_GameMap[3][0] = BLOCK_PRESENT;
    array_GameMap[3][2] = BLOCK_PRESENT;
    array_GameMap[4][2] = BLOCK_PRESENT;
    array_GameMap[4][5] = BLOCK_PRESENT;
    array_GameMap[2][0] = BLOCK_PRESENT;
    array_GameMap[1][7] = BLOCK_PRESENT;
    array_GameMap[1][8] = BLOCK_PRESENT;
    array_GameMap[1][9] = BLOCK_PRESENT;
    array_GameMap[1][10] = BLOCK_PRESENT;
    array_GameMap[1][11] = BLOCK_PRESENT;
    array_GameMap[3][6] = BLOCK_PRESENT;
    array_GameMap[3][9] = BLOCK_PRESENT;
    array_GameMap[3][10] = BLOCK_PRESENT;

    array_GameMap[1][2] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[0][ENEMY_ROW] = 1;
    array_EnemyStats[0][ENEMY_COL] = 2;
    array_EnemyStats[0][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[1][4] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[1][ENEMY_ROW] = 1;
    array_EnemyStats[1][ENEMY_COL] = 4;
    array_EnemyStats[1][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[1][6] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[2][ENEMY_ROW] = 1;
    array_EnemyStats[2][ENEMY_COL] = 6;
    array_EnemyStats[2][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[2][3] = BLOCK_ENEMY;
    array_EnemyStats[3][ENEMY_ROW] = 2;
    array_EnemyStats[3][ENEMY_COL] = 3;
    array_EnemyStats[3][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[0][5] = BLOCK_ENEMY;
    array_EnemyStats[4][ENEMY_ROW] = 0;
    array_EnemyStats[4][ENEMY_COL] = 5;
    array_EnemyStats[4][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[3][3] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[5][ENEMY_ROW] = 3;
    array_EnemyStats[5][ENEMY_COL] = 3;
    array_EnemyStats[5][ENEMY_TYPE] = HEALING_TOWER;

    array_GameMap[3][8] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[6][ENEMY_ROW] = 3;
    array_EnemyStats[6][ENEMY_COL] = 8;
    array_EnemyStats[6][ENEMY_TYPE] = RANGED_TOWER;

    array_GameMap[3][7] = BLOCK_TOWER_ENEMY;
    array_EnemyStats[7][ENEMY_ROW] = 3;
    array_EnemyStats[7][ENEMY_COL] = 7;
    array_EnemyStats[7][ENEMY_TYPE] = SLOW_ENEMY;

    array_GameMap[2][9] = BLOCK_ENEMY;
    array_EnemyStats[8][ENEMY_ROW] = 2;
    array_EnemyStats[8][ENEMY_COL] = 9;
    array_EnemyStats[8][ENEMY_TYPE] = GUARD_ENEMY;

    array_GameMap[4][7] = BLOCK_ENEMY;
    array_EnemyStats[9][ENEMY_ROW] = 4;
    array_EnemyStats[9][ENEMY_COL] = 7;
    array_EnemyStats[9][ENEMY_TYPE] = GUARD_ENEMY;

    //Using Teleporter
    level_has_teleporter = TRUE;
    array_GameMap[4][0] = BLOCK_TELEPORTER;
    array_GameMap[0][11] = BLOCK_TELEPORT_SPAWN;

    initial_direction = RIGHT;
    /*placeholders, please change everything*/
}