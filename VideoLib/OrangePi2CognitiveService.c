#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./OrangePi_V4L2/include/OrangePi/OrangePiV4L2.h"

/*
 * Update the Cognitive Service Face key
 */
int OrangePi_Update_CognitiveServiceFace_Key(const char *key)
{
    FILE *fp;

    fp = fopen("/etc/OrangePi/OrangePi_2_CognitiveService_Face.conf","w");
    if(fp == NULL) {
	printf("Can't open the OrangePi_2_CognitiveService_Face.conf!\n");
	return -1;
    }

    if(fwrite(key,1,strlen(key),fp) != strlen(key)) {
	printf("Can't correct to write Cognitive Service key!\n");
	return -2;
    }
    printf("Succeed to update the Cognitive Service key!\n");
    fclose(fp);
    return 0;
}

/*
 * Update the Cognitive Service Emotion key
 */
int OrangePi_Update_CognitiveServiceEmotion_Key(const char *key)
{
    FILE *fp;

    fp = fopen("/etc/OrangePi/OrangePi_2_CognitiveService_Emotion.conf","w");
    if(fp == NULL) {
	printf("Can't open the OrangePi_2_CognitiveService_Emotion.conf!\n");
	return -1;
    }

    if(fwrite(key,1,strlen(key),fp) != strlen(key)) {
	printf("Can't correct to write Cognitive Service key!\n");
	return -2;
    }
    printf("Succeed to update the Cognitive Service key!\n");
    fclose(fp);
    return 0;
}

/* build-in capture */
void OrangePi_Capturein(void)
{
    char buffer[200];
    
    sprintf(buffer,"OrangePi_Capture");

    system(buffer);
}

/*
 * OrangePi 2 FaceEmotion.
 */
int OrangePi2FaceEmotion(void)
{
    char buffer[200];
    
    /* Capture */
//    OrangePi_Update_Picture();
//    OrangePi_Capturein();
  
    sprintf(buffer,"python /var/www/html/OrangePi_CognitiveService.py");
    /* Call FaceEmotion */
    system(buffer);
    return 0;
}
