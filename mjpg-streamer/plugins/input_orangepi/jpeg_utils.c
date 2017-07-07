/*******************************************************************************
# Linux-UVC streaming input-plugin for MJPG-streamer                           #
#                                                                              #
# This package work with the Logitech UVC based webcams with the mjpeg feature #
#                                                                              #
#   Orginally Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard       #
#   Modifications Copyright (C) 2006  Gabriel A. Devenyi                       #
#   Modifications Copyright (C) 2007  Tom Stöveken                             #
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

#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <jpeglib.h>

#include <OrangePi/v4l2op.h>
#include <OrangePi/jpeg_utils.h>

#define OUTPUT_BUF_SIZE  4096

typedef struct {
    struct jpeg_destination_mgr pub; /* public fields */

    JOCTET * buffer;    /* start of buffer */

    unsigned char *outbuffer;
    int outbuffer_size;
    unsigned char *outbuffer_cursor;
    int *written;

} mjpg_destination_mgr;

typedef mjpg_destination_mgr * mjpg_dest_ptr;

/******************************************************************************
Description.:
Input Value.:
Return Value:
******************************************************************************/
METHODDEF(void) init_destination(j_compress_ptr cinfo)
{
    mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;

    /* Allocate the output buffer --- it will be released when done with image */
    dest->buffer = (JOCTET *)(*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_IMAGE, OUTPUT_BUF_SIZE * sizeof(JOCTET));

    *(dest->written) = 0;

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

/******************************************************************************
Description.: called whenever local jpeg buffer fills up
Input Value.:
Return Value:
******************************************************************************/
METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo)
{
    mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;

    memcpy(dest->outbuffer_cursor, dest->buffer, OUTPUT_BUF_SIZE);
    dest->outbuffer_cursor += OUTPUT_BUF_SIZE;
    *(dest->written) += OUTPUT_BUF_SIZE;

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

    return TRUE;
}

/******************************************************************************
Description.: called by jpeg_finish_compress after all data has been written.
              Usually needs to flush buffer.
Input Value.:
Return Value:
******************************************************************************/
METHODDEF(void) term_destination(j_compress_ptr cinfo)
{
    mjpg_dest_ptr dest = (mjpg_dest_ptr) cinfo->dest;
    size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

    /* Write any data remaining in the buffer */
    memcpy(dest->outbuffer_cursor, dest->buffer, datacount);
    dest->outbuffer_cursor += datacount;
    *(dest->written) += datacount;
}

/******************************************************************************
Description.: Prepare for output to a stdio stream.
Input Value.: buffer is the already allocated buffer memory that will hold
              the compressed picture. "size" is the size in bytes.
Return Value: -
******************************************************************************/
GLOBAL(void) dest_buffer(j_compress_ptr cinfo, unsigned char *buffer, int size, int *written)
{
    mjpg_dest_ptr dest;

    if(cinfo->dest == NULL) {
        cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(mjpg_destination_mgr));
    }

    dest = (mjpg_dest_ptr) cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
    dest->outbuffer = buffer;
    dest->outbuffer_size = size;
    dest->outbuffer_cursor = buffer;
    dest->written = written;
}

/******************************************************************************
Description.: yuv2jpeg function is based on compress_yuyv_to_jpeg written by
              Gabriel A. Devenyi.
              It uses the destination manager implemented above to compress
              YUYV data to JPEG. Most other implementations use the
              "jpeg_stdio_dest" from libjpeg, which can not store compressed
              pictures to memory instead of a file.
Input Value.: video structure from v4l2uvc.c/h, destination buffer and buffersize
              the buffer must be large enough, no error/size checking is done!
Return Value: the buffer will contain the compressed data
******************************************************************************/
int compress_yuyv_to_jpeg(struct vdIn *vd, unsigned char *buffer, int size, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    unsigned char *line_buffer, *yuyv;
    int z;
    static int written;

    line_buffer = calloc(vd->width * 3, 1);
    yuyv = vd->framebuffer;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    /* jpeg_stdio_dest (&cinfo, file); */
    dest_buffer(&cinfo, buffer, size, &written);

    cinfo.image_width = vd->width;
    cinfo.image_height = vd->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    z = 0;
    while(cinfo.next_scanline < vd->height) {
        int x;
        unsigned char *ptr = line_buffer;

        for(x = 0; x < vd->width; x++) {
            int r, g, b;
            int y, u, v;

            if(!z)
                y = yuyv[0] << 8;
            else
                y = yuyv[2] << 8;
            u = yuyv[1] - 128;
            v = yuyv[3] - 128;

            r = (y + (359 * v)) >> 8;
            g = (y - (88 * u) - (183 * v)) >> 8;
            b = (y + (454 * u)) >> 8;

            *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
            *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
            *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);

            if(z++) {
                z = 0;
                yuyv += 4;
            }
        }

        row_pointer[0] = line_buffer;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    free(line_buffer);

    return (written);
}



void convert_yuv420p_to_yuyv(unsigned char *src,unsigned char *dst, int width,int height) 
{ 
    int i, j; 
    unsigned char *pY420_0 = src;
    unsigned char *pY420_1 = src +width;
    unsigned char *pU420 = src + width*height; 
    unsigned char *pV420 = src + width*height*5/4; 
    unsigned char *pY422_0 = dst;
    unsigned char *pY422_1 = dst+width*2;

    for (i = 0; i < height/2; i++) {
	for (j = 0; j < width*2; j +=4) { 
	    *pY422_0++ = *pV420;
	    *pY422_1++ = *pV420++;
	    *pY422_0++ = *pY420_0++;
	    *pY422_1++ = *pY420_1++;
   
	    *pY422_0++ = *pU420;
	    *pY422_1++ = *pU420++;
	    *pY422_0++ = *pY420_0++;
	    *pY422_1++ = *pY420_1++;   
	}	

    pY420_0 +=width;
    pY420_1 +=width;
    pY422_0+=width*2;
    pY422_1+=width*2;  
    }  
} 

typedef enum E_YUVFormat
{
    //420p-planar
    emFM_I420 = 0,      //YYYYYYYY UU VV identical to IYUV
    emFM_YV12 = 1,      //YYYYYYYY VV UU //IYUV 

    //420sp: y-planar uv- packet 
    emFM_NV12 = 2,      //YYYYYYYY UVUV 
    emFM_NV21 = 3,      //YYYYYYYY VUVU 

    //422p-planar
    emFM_I422 = 4,      //YYYYYYYY UU VV 
    emFM_YV16 = 5,      //YYYYYYYY VV UU

    //422sp: y-planar uv- packet 
    emFM_NV16 = 6,      //YYYYYYYY UVUV 
    emFM_NV61 = 7,      //YYYYYYYY VUVU

    //422pack packet
    emFM_YUYV = 8,      //YUYVYUYVYUYV  //identical to YUY2
    emFM_UYVY = 9,      //UYVYUYVYUYVY
    emFM_YVYU = 10,     //YVYUYVYUYVYU
    emFM_VYUY = 11,     //VYUYVYUYVYUY
} m_eOutYUVFM;

const int tabYUV422P[4][4] ={
    {0,1,2,3},  // Y U Y V
    {1,0,3,2},  // U Y V Y
    {0,3,2,1},  // Y V Y U 
    {3,0,1,2}   // V Y U Y 
};

#define SWAP(a,b) (a = a^b, b = a^b, a = a^b)

int OrangePi_420PTo422Pack(unsigned char * i_pBuff,unsigned char* o_pBuff,int m_iWidth,int m_iHeight)
{
    int i = 0,j = 0;
    int m_eOutYUVFM = 8;
    int m_eInYUVFM  = 0;
    int iDstY = 0, iDstU = 0, iDstV = 0;
    int iPos = m_eOutYUVFM - emFM_YUYV;
    int iYSize = m_iWidth * m_iHeight;
    int iPixelHeight = m_iHeight >> 1;
    int iPixelWidth = m_iWidth << 1;

    int m_iUVSize = m_iWidth * m_iHeight >> 2;
    unsigned char *pSrcY0 = i_pBuff;
    unsigned char *pSrcY1 = i_pBuff + m_iWidth;
    unsigned char *pSrcU  = pSrcY0 + iYSize;
    unsigned char *pSrcV  = pSrcU + m_iUVSize;

    unsigned char *pDst = o_pBuff;

    for(i = 0; i < 4; i++) {
        if(tabYUV422P[iPos][i] == 0)
            iDstY = i;
        else if(tabYUV422P[iPos][i] == 1)
            iDstU = i;
        else if(tabYUV422P[iPos][i] == 3)
            iDstV = i;
    }

    if(m_eInYUVFM == emFM_YV12)
        SWAP(iDstU,iDstV);

    for (i = 0; i < iPixelHeight; i++) {
        for (j = 0; j < iPixelWidth; j+=4) {
            pDst[j+iDstY]   = *pSrcY0++;
            pDst[j+iDstU]   = *pSrcU;
            pDst[j+iDstY+2] = *pSrcY0++;
            pDst[j+iDstV]   = *pSrcV;

            pDst[j+iDstY+iPixelWidth]   = *pSrcY1++;
            pDst[j+iDstU+iPixelWidth]   = *pSrcU++;
            pDst[j+iDstY+iPixelWidth+2] = *pSrcY1++;
            pDst[j+iDstV+iPixelWidth]   = *pSrcV++;
        }
        pSrcY0 += m_iWidth;
        pSrcY1 += m_iWidth;
        pDst += m_iWidth<<2;
    }

    return 0;
}


