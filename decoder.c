#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gifdec.h"
#include <math.h>

#define FILENAME "output_rgb.raw"
#define FILENAMEYUV "output_yuv.raw"

uint8_t rgb2y(uint8_t R,uint8_t G,uint8_t B)
{
	return round((66*R + 129*G + 25*B)>>8) +  16;
}
uint8_t rgb2u(uint8_t R,uint8_t G,uint8_t B)
{
	return round((-38 * R - 74 * G + 112 * B)>>8) + 128;
}
uint8_t rgb2v(uint8_t R,uint8_t G,uint8_t B)
{
	 return round(( 112 * R - 94 * G - 18 * B)>>8) + 128;
}

void convert(uint8_t *frameData,int width,int height);

int main(int argc, char *argv[])
{
	FILE *fp=fopen(FILENAME,"wb+");
	gd_GIF *gif;
	uint8_t *frame,*data,*yuvframe,*yuvdata;
	int i, j,pixelCount=0,yuvheight,yuvwidth;
	int count=0;
	if(argc!=2)
	{
		printf("Incorrect Useage \n");
		return 2;
	}
	gif = gd_open_gif(argv[1]);
	if(!gif)
	{
		printf("Invalid file \n");
		return 3;
	}
	yuvwidth = (gif->width%2==0)?gif->width:gif->width-1;
	yuvheight = (gif->height%2==0)?gif->height:gif->height-1;
	frame = malloc(gif->width * gif->height * 3);
	yuvframe = malloc(yuvwidth*yuvheight*3);
	
	if(!frame || !yuvframe)
	{
		printf("Could not allocate memory to frame \n");
		return 4;
	}
	while(gd_get_frame(gif))
	{
	 	++count;
		gd_render_frame(gif, frame);
		yuvdata=yuvframe;
		data=frame;
		fwrite(data,1,gif->width*gif->height*3,fp);
		for(i=0;i<yuvheight;++i)
		{
			for(j=0;j<yuvwidth;++j)
			{
				yuvdata[0]=rgb2y(data[0],data[1],data[2]);
				yuvdata[1]=rgb2u(data[0],data[1],data[2]);
				yuvdata[2]=rgb2v(data[0],data[1],data[2]);
				yuvdata+=3;
				data+=3;

			}
		}
		
	}
	convert(yuvframe,yuvwidth,yuvheight);
	fclose(fp);
	free(frame);
	free(yuvframe);
	printf("Conversion to YUV420sp is done, please find the file output_yuv420p to view your data. Raw RGB data is present in output_rgb.raw file. \n");
}
void convert(uint8_t *data,int width,int height)
{
	uint8_t *yuv420sp = malloc(width*height*3/2);
	FILE *yuv420sp_file=fopen("output_yuv420sp.raw","wb+");
	uint8_t *copy=data;
	if(!yuv420sp_file)
	{
		printf("There was an error when creating the yuv420sp file \n");
		return;
	}
	if(!yuv420sp)
	{
		printf("Memory could not be allocated to yuv420sp buffer \n");
		return;
	}
	int pixelCount=0,i=0,j=0;
	for(i=0;i<width*height;++i)
	{
		yuv420sp[pixelCount]=data[0];
		data+=3;
		++pixelCount;  
	}
	data=copy;
	uint8_t u1,u2,u3,u4,v1,v2,v3,v4;
	float uavg,vavg;
	for(i=0;i<height;i+=2)
	{
		for(j=0;j<width;j+=2)
		{
			int pos1=(i*3)+(j*3);
			int pos2=((i+width)*3)+(j*3);
			int pos3=(i*3)+((j+1)*3);
			int pos4=((i+width)*3)+((j+1)*3);
			u1=data[pos1+1];
			v1=data[pos1+2];
			u2=data[pos2+1];
			v2=data[pos2+2];
			u3=data[pos3+1];
			v3=data[pos3+2];
			u4=data[pos4+1];
			v4=data[pos4+2];
			uavg=(u1+u2+u3+u4)/4.0;
			vavg=(v1+v2+v3+v4)/4.0;
			yuv420sp[pixelCount]=(uint8_t)(uavg+0.5);
			yuv420sp[pixelCount+1]=(uint8_t)(vavg+0.5);
			pixelCount+=2;
		}
	}
	fwrite(yuv420sp,1,pixelCount,yuv420sp_file);
	fclose(yuv420sp_file);
}
