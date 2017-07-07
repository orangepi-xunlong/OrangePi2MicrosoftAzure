#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OrangePi_Internal.h"
/*
 * User API: To get a sound stream that format is wav.
 */
int OrangePi2Sound(int time)
{
    char buffer[200];

    sprintf(buffer,"OrangePi_Sound %d",time);

    system(buffer);
    return 0;
}
