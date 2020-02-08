#ifndef DECODER_H
#define DECODER_H

/*YUV_Buffer is a struct that contains the yuvwidth, yuvheight, number
 * of frames in the file, number of the current frame being accessed
 * the buffer that stores the yuv data and a gif buffer that stores
 * the gif data
 */

#include <stdint.h>
#include "gifdec.h"

typedef struct YUV_Buffer
{
	int yuvwidth;
	int yuvheight;
	int nFrames;
	int currentFrame;
	uint8_t *buffer;
	gd_GIF *gif;
}YUV_Buffer;

/*Initialise takes the filename of the gif file as an input and
 * return a pointer to a YUV_Buffer structure.
 * 
 * FetchFrame takes a pointer to a YUV_Buffer structure and returns 
 * a pointer to a buffer which stores the data of the current frame.
 * After each FetchFrame() the frame count is incremented. FetchFrame()
 * return NULL if the current frame exceeds the total number of frames
 * 
 * Deinitialise frees up any allocated memory and makes sure that the
 * program exits safely
 */

YUV_Buffer* Initialise(const char *fileName);
uint8_t* FetchFrame(YUV_Buffer *buffer);
void Deinitialise(YUV_Buffer *buffer);

#endif
