#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OrangePi2CognitiveService.h>

const char *Emotion_key = "f8253e213f14441aa90d85ac866ab22f";
const char *Face_key = "ab5513cf348c49499dbdc82c545571c0";

int main()
{
    /* Update the key of Cognitive Service : Emotion */
//    OrangePi_Update_CognitiveServiceEmotion_Key(Emotion_key);
    /* Update the key of Cognitive Service : Face */
//    OrangePi_Update_CognitiveServiceFace_Key(Face_key);
    /* Capture and FaceEmotion */
    OrangePi2FaceEmotion();
    return 0;
}
