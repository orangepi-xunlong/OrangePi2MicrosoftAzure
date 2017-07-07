/*
 * This program reads from the default PCM device
 * and writes to standard output/stream file for 10 seconds of data.
 * Compile:
 *      gcc record.c -lasound -o record
 */

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "OrangePi_Internal.h"

#define RATE      44100 //CD 
#define SIZE      16   
#define CHANNELS  1   
#define RSIZE     8    

/* 
 * Output settings
 * STDOUT: stdout,use write,open and read.
 * STREAMOUT: use stream on fopen,fwrite and fread.
 */
#define STDOUT     0
#define STREAMOUT  1

/******** The WAV format information ******/
/*------------------------------------------------
|             RIFF WAVE Chunk                  |
|             ID = 'RIFF'                     |
|             RiffType = 'WAVE'                |
------------------------------------------------
|             Format Chunk                     |
|             ID = 'fmt '                      |
------------------------------------------------
|             Fact Chunk(optional)             |
|             ID = 'fact'                      |
------------------------------------------------
|             Data Chunk                       |
|             ID = 'data'                      |
------------------------------------------------*/
/*
 * WAV header structure
 */
struct fhead
{
    /****RIFF WAVE CHUNK*/
    unsigned char a[4];
    long int b;        
    unsigned char c[4];
    /****RIFF WAVE CHUNK*/
    /****Format CHUNK*/
    unsigned char d[4];
    long int e;       
    short int f;       
    short int g;       
    long int h;        
    long int i;        
    short int j;       
    short int k;      
    /****Format CHUNK*/
    /***Data Chunk**/
    unsigned char p[4];
    long int q;       
} wavehead;

/*
 * Start recored.
 */
static int OrangePi_Sound(int times)
{
    long loops;
    int rc;
    int size;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    char *buffer;
    int fd_f;
    int status;
    FILE *fp;
    int LENGTH = times;

    /* fill value to wav header */
    wavehead.a[0] = 'R';
    wavehead.a[1] = 'I';
    wavehead.a[2] = 'F';
    wavehead.a[3] = 'F';
    wavehead.b    = LENGTH * RATE * CHANNELS * SIZE / 8 - 8;
    wavehead.c[0] = 'W';
    wavehead.c[1] = 'A';
    wavehead.c[2] = 'V';
    wavehead.c[3] = 'E';
    wavehead.d[0] = 'f';
    wavehead.d[1] = 'm';
    wavehead.d[2] = 't';
    wavehead.d[3] = ' ';
    wavehead.e    = 16;
    wavehead.f    = 1;
    wavehead.g    = CHANNELS;
    wavehead.h    = RATE;
    wavehead.i    = RATE*CHANNELS*SIZE/8;
    wavehead.j    = CHANNELS*SIZE/8;
    wavehead.k    = SIZE;
    wavehead.p[0] = 'd';
    wavehead.p[1] = 'a';
    wavehead.p[2] = 't';
    wavehead.p[3] = 'a';
    wavehead.q = LENGTH * RATE * CHANNELS * SIZE / 8;

    /* Open PCM device for recording (capture). */
    rc = snd_pcm_open(&handle, "default",SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr,"unable to open pcm device: %s\n",
	            snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params,
	                      SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params,
	                              SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);

    /* 44100 bits/second sampling rate (CD quality) */
    val = RATE;
    snd_pcm_hw_params_set_rate_near(handle, params,
	                                  &val, &dir);

    /* Set period size to 32 frames. */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);  

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);   
    if (rc < 0) {
	fprintf(stderr,"unable to set hw parameters: %s\n",
	            snd_strerror(rc));
        exit(1);
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params,
	                                      &frames, &dir);
    size = frames * 2; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    /* We want to loop for 5 seconds */
    snd_pcm_hw_params_get_period_time(params,
	                                         &val, &dir);
    loops = LENGTH * 1000000 / val; 

#if STDOUT
    if(( fd_f = open("./sound.wav", O_CREAT|O_RDWR,0777))==-1) {
        perror("cannot creat the sound file");
    }
    /* Write the wav header */
    if((status = write(fd_f, &wavehead, sizeof(wavehead)))==-1) {
	   perror("write to sound'head wrong!!");
    }
#endif
#if STREAMOUT
    if((fp = fopen("./sound.wav","w")) < 0)
	printf("Open sound.wav faild\n");
    /* Write the wav header */
    if(fwrite(&wavehead,1,sizeof(wavehead),fp) != sizeof(wavehead))
	printf("Short warite: wrote bytes\n");
#endif

    while (loops > 0) { 
        loops--;
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "error from read: %s\n",
                    snd_strerror(rc));
        } else if (rc != (int)frames) {  
            fprintf(stderr, "short read, read %d frames\n", rc);
        }
#if STDOUT
        if(write(fd_f, buffer, size)==-1) {
            perror("write to sound wrong!!");
        }
#endif
#if STREAMOUT
	rc = fwrite(buffer,1,size,fp);
#endif
        if (rc != size)
	      fprintf(stderr,
	              "short write: wrote %d bytes\n", rc);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
#if STDOUT
    close(fd_f);
#endif
#if STREAMOUT
    fclose(fp);
#endif

    return 0;
}

int main(int argc,char *argv[])
{
    int time;
    string2int(argv[1],&time);

    OrangePi_Sound(time);
    return 0;
}
