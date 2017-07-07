#include <stdio.h>
#include <stdlib.h>

#include <OrangePi/OrangePi_Internal.h>
#include <OrangePi/v4l2.h>

extern struct OrangePi_v4l2_device OrangePi;
#define JPG_PATH     "/var/www/html/Before.jpeg"

void OrangePi_Process_Image(struct OrangePi_v4l2_device *dev)
{
    write_JPEG_file(JPG_PATH,dev->buffers->YUV_buffer,100,dev->width,dev->height);
}

/*
 * Initialize all
 */
struct OrangePi_v4l2_device *OrangePi_init(void)
{
    /* Prepare the v4l2 device */
    OrangePi.init(&OrangePi);
    return &OrangePi;   
}

/*
 * Close All
 */
void OrangePi_close(struct OrangePi_v4l2_device *dev)
{
    dev->close(dev);
}

/*
 * Basic API to User.
 */
int OrangePi_Update_Picture(void)
{
    struct OrangePi_v4l2_device *dev;
   
    dev = OrangePi_init();
    dev->capture(dev);
    OrangePi_Process_Image(dev);
    OrangePi_close(dev);
    return 0;
}

int main(void)
{
    struct OrangePi_v4l2_device *dev;
  
    printf("Start Capture.....\n") ;
    dev = OrangePi_init();
    dev->capture(dev);
    OrangePi_Process_Image(dev);
    OrangePi_close(dev);
    printf("Finish Capture....\n");
    return 0;
}
