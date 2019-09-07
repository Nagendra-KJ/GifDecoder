#include "gifdec.h"
#include <stdio.h>
int main(int argc,char *argv[])
{
	const char *filename="test.gif";
	if(argc<2)
	{
		printf("The format is ./obj <filename.gif> \n");
		return -2;
	}
	gd_GIF *gif=gd_open_gif(argv[1]);
	if(!gif)
	{
		printf("File not found \n");
		return -1;
	}
	printf("The resolution of the GIF is %d x %d \n",gif->width,gif->height);
	printf("The size of the global colour palette is %d \n",gif->palette->size);
	int frame_count=0;
	while(gd_get_frame(gif))
	{
		++frame_count;
		printf("The %d frame has a delay of %.2f seconds \n",frame_count,gif->gce.delay/100.0);
	}
	printf("The number of frames in the gif is %d \n",frame_count);
	gd_close_gif(gif);
	return 0;
}
