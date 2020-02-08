#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decoder.h"
void main()
{
	FILE *fp=fopen("output.raw","wb+");
	YUV_Buffer *temp=Initialise("test.gif");
	int yuvframesize=temp->yuvwidth*temp->yuvheight*3/2;
	uint8_t *yuv=malloc(temp->nFrames*yuvframesize);
	uint8_t *frame;
	int i;
	for(i=0;i<temp->nFrames;++i)
	{
		frame=FetchFrame(temp);
		memmove(&yuv[i*yuvframesize],frame,yuvframesize);
	}
	fwrite(yuv,1,temp->nFrames*yuvframesize,fp);
	fclose(fp);
	Deinitialise(temp);
	
}
