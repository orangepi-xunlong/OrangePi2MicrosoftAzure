#ifndef _V4L2_H_
#define _V4L2_H_

#include <fcntl.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#include <OrangePi/OrangePi_Internal.h>


struct buffer 
{
    unsigned long int *start;
    size_t length;
};

struct OrangePi_buffer {
    int n_buffers;
    FILE *Out_fd;
    FILE *YUV_fd;
    FILE *JPG_fd;
    struct buffer *Raw_buffers;
    char *YUV_buffer;
};

struct OrangePi_v4l2_device {
    int  fd;
    FILE *buffer_fd;
    char *device_name;
    int  width;
    int  height;
    int  format;
    unsigned int timeout;
    unsigned int fps;
    struct OrangePi_buffer *buffers;
    void *private_data;
    int  (*open)(struct OrangePi_v4l2_device *dev);
    void (*close)(struct OrangePi_v4l2_device *dev);
    int  (*init)(struct OrangePi_v4l2_device *dev);
    int  (*capable)(struct OrangePi_v4l2_device *dev);
    int  (*check_format)(struct OrangePi_v4l2_device *dev);
    int  (*capture)(struct OrangePi_v4l2_device *dev);
    void (*current_framer)(struct OrangePi_v4l2_device *dev);
};


#endif
