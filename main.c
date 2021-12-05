/*!
@file main.c
@authors:   Jan Tiong Shu Ling         tiong.j@digipen.edu.sg
            Raiyan Faliq Bin Zahid     raiyanfaliq.b@digipen.edu
            Tan Yuan Cheng             yuancheng.tan@digipen.edu
            Amirah Binte Isa           amirah.b@digipen.edu
@course CSD 1400
@section D
@brief This file is the main entry point for our project Horde
of the CProcessing library.

Copyright © 2020 DigiPen, All rights reserved.
*//*_____________________________________________________________*/

#include <stdio.h>
#include "cprocessing.h"


/* Forward declarations */
void splash_init(void);
void splash_update(void);
void splash_exit(void);



/* Entry point */
int main(void)
{
    CP_Engine_SetNextGameState(splash_init, splash_update, splash_exit);
    CP_Engine_Run();
    return 0;
}
