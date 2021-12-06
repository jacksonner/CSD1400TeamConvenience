/*!
@file game.h
@primary author: Jan Tiong Shu Ling         tiong.j@digipen.edu.sg
@co-authors:     Raiyan Faliq Bin Zahid     raiyanfaliq.b@digipen.edu
				 Tan Yuan Cheng             yuancheng.tan@digipen.edu
				 Amirah Binte Isa           amirah.b@digipen.edu
@course CSD 1400
@section D
@brief This file contains all the functions required to run Horde.

Copyright � 2020 DigiPen, All rights reserved.
*//*_____________________________________________________________*/

#pragma once

void game_init(void);
void game_update(void);
void game_exit(void);

/*Window Dimension of Game*/
int origin_map_coordinateX; //cause I have a border around everything
int origin_map_coordinateY;
int BLOCK_SIZE; //in caps, jic people have tiny screens and block_size needs to change

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
#define TRANSLUCENT_GOLD CP_Color_Create(255, 195, 54, 100)

/*Minion Stats*/
#define X 0 //x-coordinates
#define Y 1 //y-coordinates
#define MINION_TYPE 2 //is the minion a spam-type, warrior-type etc.
#define MINION_HP 3
#define MINION_MOVEMENT_SPEED 4
#define MINION_ATTACK 5
#define MINION_ATTACK_SPEED 6
#define MINION_IS_STOPPED 7 //if minion is stopped, value stored is the value to enemy stopping the minion
#define MINION_COST 8
#define MINION_SIZE 9 //like the radius of the minion, might be removed when actualy sprites are included?
#define MINION_DIRECTION 10
#define MINION_HEAL 11
#define MINION_TRAVEL_DIST 12
#define MINION_PAST_DIRECTION 13
#define MINION_TELEPORTED 14
#define MINION_EFFECTS 15
int not_stopped = 20; //used to initialise minions to well, not be blocked by any enemy since the value stored would be the enemy blocking the minion.

#define MINION_MAX 7 //Maximum number of minions in the game at any one time
#define MINION_TOTAL_STATS 16
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
void render_minion_special_attack(void);

/*Types of Minions*/
#define SPAM_MINION 0 //weak everything, but low cost
#define WARRIOR_MINION 1 //decent health, decent attack
#define TANK_MINION 2 //tanky but low attack //targets tower but very low damage rip
#define WIZARD_MINION 3 //low health, but very long range
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
static CP_Image teleport_block;
static CP_Image teleport_spawn_block;
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
static CP_Sound gameplay_bgm;
static CP_Sound main_menu_bgm;
static CP_Sound mouse_click_sfx;

/*Music arrays*/
int play_bgm[5];
void gameplay_sound(void);
void main_menu_sound(void);

/*SFX*/
void mouse_click(void);

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
float fire_timer[ENEMY_MAX];

/*Variables*/
int BlockPositionX;
int BlockPositionY;
int minion_count;
int spawn_row;
int spawn_col;
void update_variables_and_make_screen_nice(); //since it's full screen, need to update the various variables so everything still looks nice

/*Money Code*/
void display_money_and_minions_counter(void); //shows current amount of money and number of minions on the map
char money_buffer[100];
char minion_number_buffer[2];
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
CP_Sound lose_sound = NULL;
void lose_screen_sound(void);
int lose_sound_played;

/*Win Screen*/
void win_screen(void);
float ButtonPositionX;
float ButtonPositionY;
float button_heightL, button_widthL;
float restart_loseX, restart_loseY, main_loseX, main_loseY;
float restart_textX, restart_textY, main_textX, main_textY;
CP_Image Win_Screen = NULL;
CP_Sound win_sound = NULL;
void win_screen_sound(void);
int win_sound_played;
CP_Image End_Screen;

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

/*Tank tanking damage if nearby*/
int tank_minion_tanking(int minion);

/*Gameplay related functions*/
/*Resets the map and minions so there will be no carry over values from a previous level*/
void reset_map_and_minions(void);
/*Renders the background of the game, the tiles to be exact*/
void render_background(void);

/*renders the buttons and UI of the gameplay screen*/
void gameplay_screen(void);

void initialise_level(void);
void setup_collaborative_diffusion_map(void);
void render_minion(void);
void assign_minion_stats(void);
void assign_enemy_stats(void);
void render_enemy(void);

/*Digipen Splash Screen */
CP_Image Digipen_logo = NULL;
float current_seconds;
int splash_opacity;