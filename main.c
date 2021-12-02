/*---------------------------------------------------------
 * file:	main.c
 * author:	[NAME]
 * email:	[DIGIPEN EMAIL ADDRESS]
* 
 * brief:	Main entry point for the sample project
			* of the CProcessing library
* 
 * documentation link:
 * https://inside.digipen.edu/main/GSDP:GAM100/CProcessing
* 
 * Copyright © 2020 DigiPen, All rights reserved.
* ---------------------------------------------------------*/
#include <stdio.h>
#include "cprocessing.h"


/* Forward declarations */
void game_init(void);
void game_update(void);
void game_exit(void);

/* Entry point */
int main(void)
{
    CP_Engine_SetNextGameState(game_init, game_update, game_exit);
    CP_System_ShowConsole();
    CP_Engine_Run();
    return 0;
}
