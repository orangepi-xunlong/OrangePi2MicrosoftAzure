#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <OrangePi/v4l2.h>
#include <OrangePi/v4l2op.h>

#define DEBUG  0

#define DEVICE_NAME "/dev/video0"
#define CAPTURE_FORMAT  V4L2_PIX_FMT_YUV420
#define CAPTURE_TIMEOUT 5
#define EX_TIME         2
#define CAPTURE_BUFFERS 4

/* ioctl with a number of retries in the case of failure
* args:
* fd - device descriptor
* IOCTL_X - ioctl reference
* arg - pointer to ioctl data
* returns - ioctl result
*/
int xioctl(int fd, int IOCTL_X, void *arg)
{
    int ret = 0;
    int tries = IOCTL_RETRY;
    do {
        ret = IOCTL_VIDEO(fd, IOCTL_X, arg);
    } while(ret && tries-- &&
            ((errno == EINTR) || (errno == EAGAIN) || (errno == ETIMEDOUT)));

    if(ret && (tries <= 0)) fprintf(stderr, "ioctl (%i) retried %i times - giving up: %s)\n", IOCTL_X, IOCTL_RETRY, strerror(errno));

    return (ret);
}

/*
 * Open the device.
 */
static int OrangePi_open(struct OrangePi_v4l2_device *dev)
{
    int fd = open(dev->device_name,O_RDWR | O_NONBLOCK,0);
    
    if(fd < 0) {
	printf("ERROR: Fail to open the video0 device\n");
	return -1;
    }
//    printf("Open device %s\n",dev->device_name);
    
    dev->fd = fd;
    return 0;
}

/*
 * Close the device.
 */
static void OrangePi_close(struct OrangePi_v4l2_device *dev)
{
    int i = 0;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(dev->fd , VIDIOC_STREAMOFF , &type))
	printf("VIDIOC_STREAMOFF.\n");

unmap:
    for(; i < dev->buffers->n_buffers ; i++) {
	if(munmap(dev->buffers->Raw_buffers[i].start , dev->buffers->Raw_buffers[i].length)) {
	    printf("ERROR:Faild to free mmap area!\n");
	    goto unmap;
	}	
    }

    free(dev->buffers);
    close(dev->fd);
}

/*
 * Get camera information that support.
 */
static int OrangePi_Camera_Capabilities(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;

    if(ioctl(dev->fd,VIDIOC_QUERYCAP,&cap) < 0) {
	printf("ERROR:Can't get support information of camera\n");
	return -1;
    }

    if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
	printf("ERROR:[%s]Video capture not support\n",__func__);
    }

    if((cap.capabilities & V4L2_CAP_STREAMING) == 0) 
	printf("ERROR:[%s]Device don't support streaming i/o\n",__func__);

    /* Show support*/
    printf("======= Support Information ==========\n");
    printf("Driver name:%s\n",cap.driver);
    printf("device name:%s\n",cap.card);
    printf("Device Path:%s\n",cap.bus_info);
    printf("Version:    %d.%d\n",(cap.version >> 16) && 0xff,(cap.version >> 24) && 0xff);
    printf("Capabilitie:%u\n",cap.capabilities);

    fmtdesc.index = 0;
    fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    printf("======= Support Format ========\n");
    while(ioctl(dev->fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
	printf("[%d]%s\n",fmtdesc.index + 1,fmtdesc.description);
	fmtdesc.index++;
    }

    return 0;
}

static void OrangePi_Current_Framer(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_format fmt;
    struct v4l2_fmtdesc fmtdesc;

    memset(&fmt,0,sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(dev->fd,VIDIOC_G_FMT,&fmt);
    
    printf("Current Framer information:\n"
             		"Width:%d\nHeight:%d\n",
	    fmt.fmt.pix.width,fmt.fmt.pix.height);

    memset(&fmtdesc,0,sizeof(struct v4l2_fmtdesc));

    fmtdesc.index = 0;
    fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("Support:\n");
    while(ioctl(dev->fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
        if(fmtdesc.pixelformat & fmt.fmt.pix.pixelformat) {
            printf("Format:%s\n",fmtdesc.description);
            break;
        }
        fmtdesc.index++;
    }
}

/*
 * Check wether the camera support some format.
 */
static int OrangePi_Format_Support(struct OrangePi_v4l2_device *dev)
{
    return 0;
}

/*
 * OrangePi set video input.
 */
static int OrangePi_Set_input(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_input input;
    int count = 0;

    memset(&input,0,sizeof(struct v4l2_input));
    input.index = count;
    
    while(!ioctl(dev->fd, VIDIOC_ENUMINPUT , &input))
	input.index = ++count;
    count -= 1;

    if(ioctl(dev->fd , VIDIOC_S_INPUT , &count) == -1) {
	printf("ERROR:Selecting input %d\n",count);
	return -1;
    }
    return 0;
}

/*
 * Set the capture params.
 */
static int OrangePi_Set_Params(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_format fmt;

    memset(&fmt,0,sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width  = dev->width * EX_TIME;
    fmt.fmt.pix.height = dev->height * EX_TIME;
    fmt.fmt.pix.field  = V4L2_FIELD_ANY;
    fmt.fmt.pix.pixelformat = dev->format;
    fmt.fmt.pix.sizeimage   = dev->width * dev->height * 3 / 2;
    fmt.fmt.pix.colorspace = 8;
 
    if(OrangePi_Format_Support(dev) < 0) {
	printf("Pls use another mode.\n");
	return -1;
    }

    if(ioctl(dev->fd,VIDIOC_S_FMT,&fmt) == -1) {
	printf("ERROR:Faild to set format.\n");
	return -1;
    }

#if 0
    printf("Select Camera Mode:\n"
	    "Width:  %d\n"
	    "Height: %d\n"
	    "PixFmt: %s\n",
	    fmt.fmt.pix.width,
	    fmt.fmt.pix.height,
	    (char *)&fmt.fmt.pix.pixelformat);

#endif
    dev->width  = fmt.fmt.pix.width;
    dev->height = fmt.fmt.pix.height;

    if(dev->format != fmt.fmt.pix.pixelformat) {
	printf("ERROR:Format not accepted\n");
	return -1;
    }

    return 0;
}

/*
 * Set Camera frame rate.
 */
static int OrangePi_Set_Frame_Rate(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_streamparm setfps;

    memset(&setfps, 0 , sizeof(struct v4l2_streamparm));
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.numerator = 1;
    setfps.parm.capture.timeperframe.denominator = dev->fps;

    if(ioctl(dev->fd, VIDIOC_S_PARM , &setfps)) {
	printf("ERROR: Bad Setting fram rate.\n");
	return -1;
    }
    dev->fps = setfps.parm.capture.timeperframe.denominator;
    
    return 0;
}

/*
 * Alloc Buffer.
 */
static int OrangePi_Set_Buffer(struct OrangePi_v4l2_device *dev)
{
    struct v4l2_requestbuffers req;
    int i;

    memset(&req,0,sizeof(struct v4l2_requestbuffers));
    req.count  = dev->buffers->n_buffers;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(dev->fd, VIDIOC_REQBUFS,&req) < 0) {
	printf("ERROR:Can't get v4l2 buffer!\n");
	return -1;
    }
    if(req.count < 2) {
	printf("ERROR:Can't get full buffer!\n");
	return -1;
    }

    /* Alloc memory */
    dev->buffers->Raw_buffers = calloc(req.count , sizeof(struct buffer));
    for(i = 0; i < req.count ; i++) {
	struct v4l2_buffer buf;

	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory   = V4L2_MEMORY_MMAP;
	buf.index    = i;
	
	if(ioctl(dev->fd , VIDIOC_QUERYBUF , &buf) == -1) {
	    printf("ERROR:Can't get buffer from v4l2 driver.\n");
	    return -1;
	}
	dev->buffers->Raw_buffers[i].length = buf.length;
	dev->buffers->Raw_buffers[i].start  = 
				    mmap(NULL,buf.length, PROT_READ | PROT_WRITE,
					    MAP_SHARED , dev->fd , buf.m.offset);
	if(MAP_FAILED == dev->buffers->Raw_buffers[i].start) {
	    printf("ERROR: MAP_FAILED.\n");
	    return -1;
	}
    }

    return 0;
}

/*
 * Prepare to capture frams.
 */
static int OrangePi_Prepare_Capture(struct OrangePi_v4l2_device *dev)
{
    unsigned int i;
    enum v4l2_buf_type type;
    
    for(i = 0 ; i < dev->buffers->n_buffers ; i++) {
	struct v4l2_buffer buf;
    
	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index  = i;

	if(ioctl(dev->fd , VIDIOC_QBUF , &buf) < 0) {
	    printf("ERROR:BAD VIDIOC_QBUF\n");
	    return -1;
	}
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(dev->fd , VIDIOC_STREAMON , &type) < 0) {
	printf("ERROR:BAD VIDIOC_STREAM\n");
	return -1;
    }
    return 0;
}

/*
 * Capture one frame
 */
static int OrangePi_Capture(struct OrangePi_v4l2_device *dev)
{
    fd_set fds;
    struct timeval tv;
    struct v4l2_buffer buf;
    
    memset(&tv , 0 , sizeof(struct timeval));
    FD_ZERO(&fds);
    FD_SET(dev->fd,&fds);
    
    tv.tv_sec  = dev->timeout;
    tv.tv_usec = 0;
    
    if(select(dev->fd + 1 , &fds , NULL , NULL , &tv) == -1) {
	printf("ERROR:Waiting for Frame\n");
	return -1;
    }
    
    memset(&buf , 0 , sizeof(struct v4l2_buffer));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    /* Dequeue buffer */
    if(ioctl(dev->fd , VIDIOC_DQBUF , &buf) < 0) {
	printf("ERROR:VIDIOC_DQBUF\n");
	return -1;
    }

//    dev->buffers->YUV_buffer = (unsigned char *)(unsigned long)dev->buffers->Raw_buffers[buf.index].start;

#if DEBUG
    fwrite(dev->buffers->Raw_buffers[buf.index].start,buf.length,
		1,dev->buffers->YUV_fd);
#endif
    /* Put buffers */
    if(ioctl(dev->fd , VIDIOC_QBUF , &buf) < 0) {
	printf("ERROR:Bad Put Buffer\n");
	return -1;
    }
    
    return 0;  
}

/*
 * Initialize the OrangePi device.
 */
static int OrangePi_init(struct OrangePi_v4l2_device *dev,struct vdIn *vd)
{
    dev->width   = vd->width;
    dev->height  = vd->height;
    dev->format  = vd->formatIn;
    dev->fps     = vd->fps;
    dev->timeout = CAPTURE_TIMEOUT;

    /* Base setup for OrangePi,don't modify */

    dev->buffers = (struct OrangePi_buffer *)malloc(
		    sizeof(struct OrangePi_buffer));
    
    dev->buffers->n_buffers = CAPTURE_BUFFERS;

    OrangePi_open(dev);
//    OrangePi_Camera_Capabilities(dev);
    OrangePi_Set_input(dev);
    OrangePi_Set_Params(dev);
    OrangePi_Set_Frame_Rate(dev);
//    OrangePi_Current_Framer(dev);
    OrangePi_Set_Buffer(dev);
    OrangePi_Prepare_Capture(dev);

    /* End of the Base setup */

    vd->width  = dev->width;
    vd->height = dev->height;
    vd->formatIn = dev->format;
    vd->fps = dev->fps;
    vd->fd  = dev->fd;
    vd->streamingState = STREAMING_ON;

    return 0;
}

struct OrangePi_v4l2_device OrangePi = {
    .device_name = DEVICE_NAME,
    .open     = OrangePi_open,
    .close    = OrangePi_close,
    .check_format = OrangePi_Format_Support,
    .capable  = OrangePi_Camera_Capabilities,
    .capture  = OrangePi_Capture,
    .current_framer = OrangePi_Current_Framer,
};

int OrangePi_VideoIn(struct vdIn *vd,char *device,int width,int height,int fps, int format,
	    int grabmethod,globals *pglobal,int id)
{
    int currentWidth, currentHeight = 0;
    struct v4l2_format currentFormat;

    if(vd == NULL || device == NULL) 
	return -1;
    if(width == 0 || height == 0) 
	return -1;
    if(grabmethod < 0 || grabmethod > 1) 
	grabmethod = 1;
    vd->videodevice = NULL;
    vd->status = NULL;
    vd->pictName = NULL;
    vd->videodevice = (char *)calloc(1 , 16 * sizeof(char));
    vd->status = (char *)calloc(1 , 100 * sizeof(char));
    vd->pictName = (char *)calloc(1 , 80 * sizeof(char));
    snprintf(vd->videodevice , 12 , "%s" , device);
    vd->toggleAvi = 0;
    vd->getPict = 0;
    vd->signalquit = 1;
    vd->width = width;
    vd->height = height;
    vd->fps = fps;
    vd->formatIn = format;
    vd->grabmethod = grabmethod;
    if(OrangePi_init(&OrangePi,vd) < 0) {
	fprintf(stderr,"Init v4l2 failed !! exit fatal \n");
	printf("ERROR %s %d\n",__func__,__LINE__);
    }

    // enumerating formats
    currentFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(xioctl(vd->fd, VIDIOC_G_FMT, &currentFormat) == 0) {
        currentWidth = currentFormat.fmt.pix.width;
        currentHeight = currentFormat.fmt.pix.height;
        DBG("Current size: %dx%d\n", currentWidth, currentHeight);
    }

    pglobal->in[id].in_formats = NULL;
    for(pglobal->in[id].formatCount = 0; 1; pglobal->in[id].formatCount++) {
        struct v4l2_fmtdesc fmtdesc;
        fmtdesc.index = pglobal->in[id].formatCount;
        fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(xioctl(vd->fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
            break;
        }

        if (pglobal->in[id].in_formats == NULL) {
            pglobal->in[id].in_formats = (input_format*)calloc(1, sizeof(input_format));
        } else {
            pglobal->in[id].in_formats = (input_format*)realloc(pglobal->in[id].in_formats, (pglobal->in[id].formatCount + 1) * sizeof(input_format));
        }

        if (pglobal->in[id].in_formats == NULL) {
            DBG("Calloc/realloc failed: %s\n", strerror(errno));
            return -1;
        }


        memcpy(&pglobal->in[id].in_formats[pglobal->in[id].formatCount], &fmtdesc, sizeof(input_format));

        if(fmtdesc.pixelformat == format)
            pglobal->in[id].currentFormat = pglobal->in[id].formatCount;

        DBG("Supported format: %s\n", fmtdesc.description);
        struct v4l2_frmsizeenum fsenum;
        fsenum.index = pglobal->in[id].formatCount;
        fsenum.pixel_format = fmtdesc.pixelformat;
        int j = 0;
        pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions = NULL;
        pglobal->in[id].in_formats[pglobal->in[id].formatCount].resolutionCount = 0;
        pglobal->in[id].in_formats[pglobal->in[id].formatCount].currentResolution = -1;
        while(1) {
            fsenum.index = j;
            j++;
            if(xioctl(vd->fd, VIDIOC_ENUM_FRAMESIZES, &fsenum) == 0) {
                pglobal->in[id].in_formats[pglobal->in[id].formatCount].resolutionCount++;
                if (pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions == NULL) {
                    pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions =
                        (input_resolution*)calloc(1, sizeof(input_resolution));
                } else {
                    pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions =
                        (input_resolution*)realloc(pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions, j * sizeof(input_resolution));
                }

                if (pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions == NULL) {
                    DBG("Calloc/realloc failed\n");
                    return -1;
                }

                pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions[j-1].width = fsenum.discrete.width;
                pglobal->in[id].in_formats[pglobal->in[id].formatCount].supportedResolutions[j-1].height = fsenum.discrete.height;
                if(format == fmtdesc.pixelformat) {
                    pglobal->in[id].in_formats[pglobal->in[id].formatCount].currentResolution = (j - 1);
                    DBG("\tSupported size with the current format: %dx%d\n", fsenum.discrete.width, fsenum.discrete.height);
                } else {
                    DBG("\tSupported size: %dx%d\n", fsenum.discrete.width, fsenum.discrete.height);
                }
            } else {
                break;
            }
        }
    }

    vd->framesizeIn = (vd->width * vd->height * 3 / 2);
    vd->framebuffer =
	(unsigned char *)calloc(1 , (size_t)vd->width * (vd->height + 8) * 2);
    vd->Y2V = 
	(unsigned char *)calloc(1 , (size_t)vd->width * (vd->height + 8) * 2);
    return 0;
}

static int video_disable(struct vdIn *vd, streaming_state disabledState)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret;
    DBG("STopping capture\n");
    ret = xioctl(vd->fd, VIDIOC_STREAMOFF, &type);
    if(ret != 0) {
        perror("Unable to stop capture");
        return ret;
    }
    DBG("STopping capture done\n");
    vd->streamingState = disabledState;
    return 0;
}

static int video_enable(struct vdIn *vd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret;

    ret = ioctl(vd->fd, VIDIOC_STREAMON, &type);
    if(ret < 0) {
        perror("Unable to start capture");
        return ret;
    }
    vd->streamingState = STREAMING_ON;
    return 0;
}

int OrangePi_Grab(struct vdIn *vd)
{
#define HEADERFRAME1 0xaf
    fd_set fds;
    struct timeval tv;
    struct OrangePi_v4l2_device *dev = &OrangePi;
    struct v4l2_buffer buf;

    memset(&tv , 0 , sizeof(struct timeval));
    FD_ZERO(&fds);
    FD_SET(dev->fd,&fds);

    tv.tv_sec  = dev->timeout;
    tv.tv_usec = 0;

    if(select(dev->fd + 1 , &fds , NULL , NULL , &tv) == -1) {
	printf("ERROR:Waiting for Fram\n");
	return -1;
    }

    memset(&buf , 0 , sizeof(struct v4l2_buffer));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if(vd->streamingState == STREAMING_OFF) {
        if(video_enable(vd))
	    printf("%s %d\n",__func__,__LINE__);
    }

    /* Dequeue buffer */
    if(ioctl(dev->fd , VIDIOC_DQBUF , &buf) < 0) {
        printf("ERROR:VIDIOC_DQBUF\n");
        return -1;
    }

    memcpy(vd->Y2V, dev->buffers->Raw_buffers[buf.index].start , vd->framesizeIn);
//    convert_yuv420p_to_yuyv(vd->Y2V,vd->framebuffer,vd->width,vd->height);
    OrangePi_420PTo422Pack(vd->Y2V,vd->framebuffer,vd->width,vd->height);

    /* Put buffers */
    if(ioctl(dev->fd , VIDIOC_QBUF , &buf) < 0) {
        printf("ERROR:Bad Put Buffer\n");
        return -1;
    }

    return 0;
}

int close_v4l2(struct vdIn *vd)
{
    if(vd->streamingState == STREAMING_ON)
        video_disable(vd, STREAMING_OFF);
    if(vd->tmpbuffer)
        free(vd->tmpbuffer);
    vd->tmpbuffer = NULL;
    free(vd->framebuffer);
    vd->framebuffer = NULL;
    free(vd->Y2V);
    vd->Y2V = NULL;
    free(vd->videodevice);
    free(vd->status);
    free(vd->pictName);
    vd->videodevice = NULL;
    vd->status = NULL;
    vd->pictName = NULL;

    return 0;
}

void control_readed(struct vdIn *vd, struct v4l2_queryctrl *ctrl, globals *pglobal, int id)
{
    struct v4l2_control c;
    c.id = ctrl->id;
    if (pglobal->in[id].in_parameters == NULL) {
        pglobal->in[id].in_parameters = (control*)calloc(1, sizeof(control));
    } else {
        pglobal->in[id].in_parameters =
        (control*)realloc(pglobal->in[id].in_parameters,(pglobal->in[id].parametercount + 1) * sizeof(control));
    }

    if (pglobal->in[id].in_parameters == NULL) {
        DBG("Calloc failed\n");
        return;
    }

    memcpy(&pglobal->in[id].in_parameters[pglobal->in[id].parametercount].ctrl, ctrl, sizeof(struct v4l2_queryctrl));
    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].group = IN_CMD_V4L2;
    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = c.value;
    if(ctrl->type == V4L2_CTRL_TYPE_MENU) {
        pglobal->in[id].in_parameters[pglobal->in[id].parametercount].menuitems =
            (struct v4l2_querymenu*)malloc((ctrl->maximum + 1) * sizeof(struct v4l2_querymenu));
        int i;
        for(i = ctrl->minimum; i <= ctrl->maximum; i++) {
            struct v4l2_querymenu qm;
            qm.id = ctrl->id;
            qm.index = i;
            if(xioctl(vd->fd, VIDIOC_QUERYMENU, &qm) == 0) {
                memcpy(&pglobal->in[id].in_parameters[pglobal->in[id].parametercount].menuitems[i], &qm, sizeof(struct v4l2_querymenu));
                DBG("Menu item %d: %s\n", qm.index, qm.name);
            } else {
                DBG("Unable to get menu item for %s, index=%d\n", ctrl->name, qm.index);
            }
        }
    } else {
        pglobal->in[id].in_parameters[pglobal->in[id].parametercount].menuitems = NULL;
    }

    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = 0;
#ifdef V4L2_CTRL_FLAG_NEXT_CTRL
    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].class_id = (ctrl->id & 0xFFFF0000);
#else
    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].class_id = V4L2_CTRL_CLASS_USER;
#endif

    int ret = -1;
    if (pglobal->in[id].in_parameters[pglobal->in[id].parametercount].class_id == V4L2_CTRL_CLASS_USER) {
        DBG("V4L2 parameter found: %s value %d Class: USER \n", ctrl->name, c.value);
        ret = xioctl(vd->fd, VIDIOC_G_CTRL, &c);
        if(ret == 0) {
            pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = c.value;
        } else {
            DBG("Unable to get the value of %s retcode: %d  %s\n", ctrl->name, ret, strerror(errno));
        }
    } else {
        DBG("V4L2 parameter found: %s value %d Class: EXTENDED \n", ctrl->name, c.value);
        struct v4l2_ext_controls ext_ctrls = {0};
        struct v4l2_ext_control ext_ctrl = {0};
        ext_ctrl.id = ctrl->id;
#ifdef V4L2_CTRL_TYPE_STRING
        ext_ctrl.size = 0;
        if(ctrl.type == V4L2_CTRL_TYPE_STRING) {
            ext_ctrl.size = ctrl->maximum + 1;
            // FIXMEEEEext_ctrl.string = control->string;
        }
#endif
        ext_ctrls.count = 1;
        ext_ctrls.controls = &ext_ctrl;
        ret = xioctl(vd->fd, VIDIOC_G_EXT_CTRLS, &ext_ctrls);
        if(ret) {
            DBG("control id: 0x%08x failed to get value (error %i)\n", ext_ctrl.id, ret);
        } else {
            switch(ctrl->type)
            {
#ifdef V4L2_CTRL_TYPE_STRING
                case V4L2_CTRL_TYPE_STRING:
                    //string gets set on VIDIOC_G_EXT_CTRLS
                    //add the maximum size to value
                    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = ext_ctrl.size;
                    break;
#endif
                case V4L2_CTRL_TYPE_INTEGER64:
                    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = ext_ctrl.value64;
                    break;
                default:
                    pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = ext_ctrl.value;
                    break;
            }
        }
    }

    pglobal->in[id].parametercount++;
}

void enumerateControls(struct vdIn *vd, globals *pglobal, int id)
{
    // enumerating v4l2 controls
    struct v4l2_queryctrl ctrl;
    pglobal->in[id].parametercount = 0;
    pglobal->in[id].in_parameters = NULL;
    /* Enumerate the v4l2 controls
     Try the extended control API first */
#ifdef V4L2_CTRL_FLAG_NEXT_CTRL
    DBG("V4L2 API's V4L2_CTRL_FLAG_NEXT_CTRL is supported\n");
    ctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    if(0 == IOCTL_VIDEO(vd->fd, VIDIOC_QUERYCTRL, &ctrl)) {
        do {
            control_readed(vd, &ctrl, pglobal, id);
            ctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
        } while(0 == IOCTL_VIDEO(vd->fd, VIDIOC_QUERYCTRL, &ctrl));
        // note: use simple ioctl or v4l2_ioctl instead of the xioctl
    } else
#endif
    {
        DBG("V4L2 API's V4L2_CTRL_FLAG_NEXT_CTRL is NOT supported\n");
        /* Fall back on the standard API */
        /* Check all the standard controls */
        int i;
        for(i = V4L2_CID_BASE; i < V4L2_CID_LASTP1; i++) {
            ctrl.id = i;
            if(IOCTL_VIDEO(vd->fd, VIDIOC_QUERYCTRL, &ctrl) == 0) {
                control_readed(vd, &ctrl, pglobal, id);
            }
        }

        /* Check any custom controls */
        for(i = V4L2_CID_PRIVATE_BASE; ; i++) {
            ctrl.id = i;
            if(IOCTL_VIDEO(vd->fd, VIDIOC_QUERYCTRL, &ctrl) == 0) {
                control_readed(vd, &ctrl, pglobal, id);
            } else {
                break;
            }
        }
    }

    memset(&pglobal->in[id].jpegcomp, 0, sizeof(struct v4l2_jpegcompression));
    if(xioctl(vd->fd, VIDIOC_G_JPEGCOMP, &pglobal->in[id].jpegcomp) != EINVAL) {
        DBG("JPEG compression details:\n");
        DBG("Quality: %d\n", pglobal->in[id].jpegcomp.quality);
        DBG("APPn: %d\n", pglobal->in[id].jpegcomp.APPn);
        DBG("APP length: %d\n", pglobal->in[id].jpegcomp.APP_len);
        DBG("APP data: %s\n", pglobal->in[id].jpegcomp.APP_data);
        DBG("COM length: %d\n", pglobal->in[id].jpegcomp.COM_len);
        DBG("COM data: %s\n", pglobal->in[id].jpegcomp.COM_data);
        struct v4l2_queryctrl ctrl_jpeg;
        ctrl_jpeg.id = 1;
        sprintf((char*)&ctrl_jpeg.name, "JPEG quality");
        ctrl_jpeg.minimum = 0;
        ctrl_jpeg.maximum = 100;
        ctrl_jpeg.step = 1;
        ctrl_jpeg.default_value = 50;
        ctrl_jpeg.flags = 0;
        ctrl_jpeg.type = V4L2_CTRL_TYPE_INTEGER;
        if (pglobal->in[id].in_parameters == NULL) {
            pglobal->in[id].in_parameters = (control*)calloc(1, sizeof(control));
        } else {
            pglobal->in[id].in_parameters = (control*)realloc(pglobal->in[id].in_parameters,(pglobal->in[id].parametercount + 1) * sizeof(control));
        }

        if (pglobal->in[id].in_parameters == NULL) {
            DBG("Calloc/realloc failed\n");
            return;
        }

        memcpy(&pglobal->in[id].in_parameters[pglobal->in[id].parametercount].ctrl, &ctrl_jpeg, sizeof(struct v4l2_queryctrl));
        pglobal->in[id].in_parameters[pglobal->in[id].parametercount].group = IN_CMD_JPEG_QUALITY;
        pglobal->in[id].in_parameters[pglobal->in[id].parametercount].value = pglobal->in[id].jpegcomp.quality;
        pglobal->in[id].parametercount++;
    } else {
        DBG("Modifying the setting of the JPEG compression is not supported\n");
        pglobal->in[id].jpegcomp.quality = -1;
    }

}

