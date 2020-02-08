#include "decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include "gifdec.h"
#include <string.h>

static float rgb2y(uint8_t R,uint8_t G,uint8_t B)
{
	return 0.257*R + 0.504*G + 0.098*B + 16;
}
static float rgb2u(uint8_t R,uint8_t G,uint8_t B)
{
	return   -0.148*R - 0.291*G + 0.439*B + 128;
}
static float rgb2v(uint8_t R,uint8_t G,uint8_t B)
{
	 return  0.439*R - 0.368*G - 0.071*B + 128;
}

static void convert2lines(uint8_t *line1,uint8_t *line2,int width,uint8_t *yline1,uint8_t *yline2,uint8_t *uvline)
{
	int x=0;
	for(x=0;x<width;x+=2)
	{
		float r00=(float)line1[x*3 + 0];
		float g00=(float)line1[x*3 + 1];
		float b00=(float)line1[x*3 + 2];
		
		float r01 = (float)line1[x*3 + 3];
        float g01 = (float)line1[x*3 + 4];
        float b01 = (float)line1[x*3 + 5];
        
        float r10 = (float)line2[x*3 + 0];
        float g10 = (float)line2[x*3 + 1];
        float b10 = (float)line2[x*3 + 2];
        
        float r11 = (float)line2[x*3 + 3];
        float g11 = (float)line2[x*3 + 4];
        float b11 = (float)line2[x*3 + 5];
        
        float y00 = rgb2y(r00, g00, b00);
        float y01 = rgb2y(r01, g01, b01);
        float y10 = rgb2y(r10, g10, b10);
        float y11 = rgb2y(r11, g11, b11);
        
        float u00 = rgb2u(r00, g00, b00);
        float u01 = rgb2u(r01, g01, b01);
        float u10 = rgb2u(r10, g10, b10);
        float u11 = rgb2u(r11, g11, b11);
        
        float v00 = rgb2v(r00, g00, b00);
        float v01 = rgb2v(r01, g01, b01);
        float v10 = rgb2v(r10, g10, b10);
        float v11 = rgb2v(r11, g11, b11);
        
        float u0 = (u00 + u01 + u10 + u11)*0.25f;
        
        float v0 = (v00 + v01 + v10 + v11)*0.25f;
        
        yline1[x + 0]	= (unsigned char)(y00 + 0.5f);
        yline1[x + 1]	= (unsigned char)(y01 + 0.5f);
        yline2[x + 0]	= (unsigned char)(y10 + 0.5f);
        yline2[x + 1]	= (unsigned char)(y11 + 0.5f);
        uvline[x + 0]	= (unsigned char)(u0 + 0.5f);
        uvline[x + 1]	= (unsigned char)(v0 + 0.5f);

	}
}

static void convert(uint8_t *data,uint8_t *yuv420sp,int width,int height)
{
	uint8_t *uvpos=&yuv420sp[width*height];
	uint8_t  *line1,*line2,*yline1,*yline2;
	uint8_t *uvline;
	
	int i=0;
	for(i=0;i<height;i+=2)
	{
		line1=&data[i*width*3];
		line2=&data[(i+1)*width*3];
		yline1=&yuv420sp[i*width];
		yline2=&yuv420sp[(i+1)*width];
		uvline=&uvpos[i*width/2];
		convert2lines(line1,line2,width,yline1,yline2,uvline);
	}
}




YUV_Buffer* Initialise(const char *fileName)
{
	YUV_Buffer *yuvfile = malloc(sizeof(YUV_Buffer));
	yuvfile->gif=gd_open_gif(fileName);
	if(!yuvfile->gif)
	{
		printf("Unable to open GIF file \n");
		return NULL;
	}
	uint8_t *frame = malloc(yuvfile->gif->width * yuvfile->gif->height * 3);
	if(!frame)
	{
		printf("Unable to allocate memory \n");
		return NULL;
	}
	yuvfile->yuvwidth=(yuvfile->gif->width%2==0)?yuvfile->gif->width:yuvfile->gif->width-1;
	yuvfile->yuvheight=(yuvfile->gif->height%2==0)?yuvfile->gif->height:yuvfile->gif->height-1;
	int yuvframesize=yuvfile->yuvwidth*yuvfile->yuvheight*3/2;
	if(yuvfile->gif->width%2!=0)
		yuvfile->gif->width=yuvfile->gif->width-1;
	if(yuvfile->gif->height%2!=0)
		yuvfile->gif->height=yuvfile->gif->height-1;
	int count=0;
	while(gd_get_frame(yuvfile->gif))
		++count;
	gd_rewind(yuvfile->gif);
	yuvfile->nFrames=count;
	yuvfile->currentFrame=0;
	yuvfile->buffer=malloc(yuvfile->nFrames*yuvframesize);
	count=0;
	while(gd_get_frame(yuvfile->gif))
	{
		++count;
		gd_render_frame(yuvfile->gif, frame);
		if(!frame)
		{
			printf("Error getting frame data \n");
			return NULL;
		}
		convert(frame,&(yuvfile->buffer[(count-1)*yuvframesize]),yuvfile->yuvwidth,yuvfile->yuvheight);
	}
	return yuvfile;
}

void Deinitialise(YUV_Buffer *yuvfile)
{
	gd_close_gif(yuvfile->gif);
	free(yuvfile->buffer);
	free(yuvfile);
}

uint8_t* FetchFrame(YUV_Buffer *yuvfile)
{
	if(yuvfile->currentFrame >= yuvfile->nFrames)
		return NULL;
	int yuvframesize=yuvfile->yuvwidth*yuvfile->yuvheight*3/2;
	uint8_t *frame =  malloc(yuvframesize);
	memmove(frame,&(yuvfile->buffer[(yuvfile->currentFrame)*yuvframesize]),yuvframesize);
	yuvfile->currentFrame+=1;
	return frame;
}
