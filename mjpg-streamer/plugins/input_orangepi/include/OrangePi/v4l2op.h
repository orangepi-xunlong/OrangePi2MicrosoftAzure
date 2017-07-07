/*******************************************************************************
# Linuc-UVC streaming input-plugin for MJPG-streamer                           #
#                                                                              #
# This package work with the Logitech UVC based webcams with the mjpeg feature #
#                                                                              #
# Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard                   #
#                    2007 Lucas van Staden                                     #
#                    2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; either version 2 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#ifndef V4L2_OP_H
#define V4L2_OP_H


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include "../../../../mjpg_streamer.h"

/* ioctl with a number of retries in the case of I/O failure
* args:
* fd - device descriptor
* IOCTL_X - ioctl reference
* arg - pointer to ioctl data
* returns - ioctl result
*/

#define NB_BUFFER 4

#define IOCTL_RETRY 4

#define IOCTL_VIDEO(fd, req, value) ioctl(fd, req, value)
#define OPEN_VIDEO(fd, flags) open(fd, flags)
#define CLOSE_VIDEO(fd) close(fd)

typedef enum _streaming_state streaming_state;
enum _streaming_state {
    STREAMING_OFF = 0,
    STREAMING_ON = 1,
    STREAMING_PAUSED = 2,
};

struct vdIn {
    int fd;
    char *videodevice;
    char *status;
    char *pictName;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_buffer buf;
    struct v4l2_requestbuffers rb;
    void *mem[NB_BUFFER];
    unsigned char *tmpbuffer;
    unsigned char *framebuffer;
    unsigned char *Y2V;
    streaming_state streamingState;
    int grabmethod;
    int width;
    int height;
    int fps;
    int formatIn;
    int formatOut;
    int framesizeIn;
    int signalquit;
    int toggleAvi;
    int getPict;
    int rawFrameCapture;
    /* raw frame capture */
    unsigned int fileCounter;
    /* raw frame stream capture */
    unsigned int rfsFramesWritten;
    unsigned int rfsBytesWritten;
    /* raw stream capture */
    FILE *captureFile;
    unsigned int framesWritten;
    unsigned int bytesWritten;
    int framecount;
    int recordstart;
    int recordtime;
};

/* context of each camera thread */
typedef struct {
    int id;
    globals *pglobal;
    pthread_t threadID;
    pthread_mutex_t controls_mutex;
    struct vdIn *videoIn;
} context;

context cams[MAX_INPUT_PLUGINS];

int OrangePi_VideoIn(struct vdIn *vd, char *device, int width, int height, int fps,int format , int grabmethod, globals *pglobal, int id);

void enumerateControls(struct vdIn *vd, globals *pglobal, int id);

int xioctl(int fd, int IOCTL_X, void *arg);
int close_v4l2(struct vdIn *vd);

int OrangePi_Grab(struct vdIn *vd);
#endif
