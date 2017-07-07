#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "OrangePi2Azure.h"
#include "OrangePi_Internal.h"

/*
 * Finish JSON
 */
void JSON_END(char *json)
{
    json[strlen(json) - 1] = '\0';
}
/*
 * Data format to JSON
 */
void Data2JSON(char *name,char *data,char *json)
{
    strncat(json,name,strlen(name));
    strcat(json,"-");
    strncat(json,data,strlen(data));
    strcat(json,";");
}
/*
 * Update the Azure key.
 */
int OrangePi_Update_Azure_Key(const char *key)
{
    FILE *fp;
    
    fp = fopen("/etc/OrangePi/OrangePi2Azure.conf","w");
    if(fp == NULL) {
	printf("Can't open the OrangePi2Azure.conf\n");
	return -1;
    }
    if(fwrite(key,1,strlen(key),fp) != strlen(key)) {
	printf("Can't correct to write Azure Key!\n");
	return -2;
    }
    printf("Succeed to update the Azure Key\n");
    fclose(fp);
    return 0;
}

/*
 * Update data from OrangePi to Azure.
 */
int OrangePi2Azure(char *name,char *data)
{
    char JSON[200];
    char buffer[200];

    memset(JSON,0,200);
    memset(buffer,0,200);
    Data2JSON(name,data,JSON);
    JSON_END(JSON);
    sprintf(buffer,"OrangePi %s",JSON);
       
    /* Call OrangePi2Azure API */ 
    printf("Command %s\n",buffer);
    system(buffer);
    return 0;
}
