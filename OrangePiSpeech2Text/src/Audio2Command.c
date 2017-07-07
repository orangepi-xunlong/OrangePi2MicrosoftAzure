#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,int argv)
{
    FILE *fd = fopen("../base/julius.out","r");
    char buffer[200];
    char *line;

    memset(buffer, 0, 200);

    fread(buffer,200,1,fd);
    
    /* Analysis the sentence */
    line = strstr(buffer,"CLOSE");
    if (line != NULL)
	system("OrangePi 0");
    else
	system("OrangePi 255");
    fclose(fd);;
    
    return 0;
}
