
#include "hdmi_fun.c"

static int width;
static int height;
static unsigned short *screen_base = NULL;
unsigned int screen_size;
int fd_hdmi;

unsigned char buff[200] = {0};
int main(void)
{
	
	/*初始化*/
	hdmi_cfg();
	hdmi_ini();

	int i;
	for (i=0;i<200;i++)
	{
		buff[i] = i+50;
	}

	printf("初始化完成，准备显示\n");
	wave_disp(buff,200);
	
	munmap(screen_base,screen_size);
	close(fd_hdmi);
	exit(EXIT_SUCCESS);
}
