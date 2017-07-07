#ifndef _JPEG_UTILS_H_
#define _JPEG_UTILS_H_

#ifdef _cplusplus
extern "C" {
#endif

int compress_yuyv_to_jpeg(struct vdIn *vd, unsigned char *buffer, int size, int quality);

int OrangePi_420PTo422Pack(unsigned char * i_pBuff,unsigned char * o_pBuff,int m_iWidth,int m_iHeight);

void convert_yuv420p_to_yuyv(unsigned char *src,unsigned char *dst, int width,int height);

#ifdef _cplusplus
}
#endif
#endif
