#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc , char **argv)
{
    char buffer[200];

    printf("Content-Type:text/html\n\n");
    memset(buffer,0,200);
    sprintf(buffer,"./update_new.sh");
    
    system(buffer);
    return 0;
}
