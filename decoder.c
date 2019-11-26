#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gifdec.h"
#include <math.h>

#define FILENAME "output_yuv420sp.raw"


float rgb2y(uint8_t R,uint8_t G,uint8_t B)
{
	return 0.257*R + 0.504*G + 0.098*B + 16;
}
float rgb2u(uint8_t R,uint8_t G,uint8_t B)
{
	return   -0.148*R - 0.291*G + 0.439*B + 128;
}
float rgb2v(uint8_t R,uint8_t G,uint8_t B)
{
	 return  0.439*R - 0.368*G - 0.071*B + 128;
}


//@TODO RESIZE FOR ODD SIZES

void convert(uint8_t *frameData,uint8_t *convertedData,int width,int height);

void convert2lines(uint8_t *line1,uint8_t *line2,int width,uint8_t *yline1,uint8_t *yline2,uint8_t *uvline);

void resize(uint8_t *original,uint8_t **new,int width,int height);

int main(int argc, char *argv[])
{
	FILE *yuv420sp_file=fopen(FILENAME,"wb+");
	gd_GIF *gif;
	uint8_t *frame,*data,*yuv420sp;
	float *yuvframe,*yuvdata;
	
	int i,j,yuvheight,yuvwidth;
	long yuvframesize=0;
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
	if(!yuv420sp_file)
	{
		printf("There was an error when creating the yuv420sp file \n");
		return 4;
	}
	
	yuvwidth = (gif->width%2==0)?gif->width:gif->width-1;
	yuvheight = (gif->height%2==0)?gif->height:gif->height-1;
	if(gif->width%2!=0)
		gif->width=gif->width-1;
	if(gif->height%2!=0)
		gif->height=gif->height-1;
	yuvframesize=yuvwidth*yuvheight*3/2;
	frame = malloc(gif->width * gif->height * 3);
	yuvframe = malloc(yuvwidth*yuvheight*3*sizeof(float));
	yuv420sp = malloc(yuvframesize);
	
	if(!frame || !yuvframe || !yuv420sp)
	{
		printf("Could not allocate memory to frame \n");
		return 5;
	}
	
	while(gd_get_frame(gif))
	{
		++count;
		gd_render_frame(gif, frame);
		if(!frame)
		{
			printf("Error getting frame data \n");
			return 6;
		}
		yuv420sp=realloc(yuv420sp,count*yuvframesize);
		convert(frame,&yuv420sp[(count-1)*yuvframesize],yuvwidth,yuvheight);
		free(data);
	}
	
	fwrite(yuv420sp,1,(count)*yuvframesize,yuv420sp_file);
	fclose(yuv420sp_file);
	free(frame);
	free(yuvframe);
	printf("Conversion to YUV420sp is done, please find the file output_yuv420p to view your data.\n");
	printf("The dimensions of the converted image is %d %d \n",yuvwidth,yuvheight);
}
void convert(uint8_t *data,uint8_t *yuv420sp,int width,int height)
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

void convert2lines(uint8_t *line1,uint8_t *line2,int width,uint8_t *yline1,uint8_t *yline2,uint8_t *uvline)
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

void resize(uint8_t *original,uint8_t **new,int width,int height)
{
	int newWidth,newHeight;
	if(width%2!=0)
		newWidth=width+1;
	if(height%2!=0)
		newHeight=height+1;
	(*new)=malloc(newWidth*newHeight*3);
	uint8_t matrix[newHeight][newWidth*3];
	int i,j,pos;
	for(i=0;i<height;++i)
	{
		pos=i*width;
		for(j=0;j<width;++j)
		{
			matrix[i][j*3]=original[pos];
			matrix[i][(j*3)+1]=original[pos+1];
			matrix[i][(j*3)+2]=original[pos+2];
			pos+=3;
		}
		if(width!=newWidth)
		{
			matrix[i][width*3]=original[pos];
			matrix[i][(width*3)+1]=original[pos+1];
			matrix[i][(width*3)+2]=original[pos+2];
		}
	}
	if(height!=newHeight)
	{
		for(j=0;j<newWidth;++j)
		{
			matrix[newHeight-1][j]=matrix[height-1][j];
			matrix[newHeight-1][j+1]=matrix[height-1][j+1];
			matrix[newHeight-1][j+2]=matrix[height-1][j+2];

		}
	}
	pos=0;
	for(i=0;i<newHeight;++i)
	{
		for(j=0;j<newWidth;++j)
		{
			(*new)[pos]=matrix[i][j];
			(*new)[pos+1]=matrix[i][j+1];
			(*new)[pos+2]=matrix[i][j+2];
			pos+=3;
		}
	}
}
