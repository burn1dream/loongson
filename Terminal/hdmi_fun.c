#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "hdmi_fun.h"


/*
	width,height屏幕参数在主函数中定义和获取
	screen_base缓冲区也在主函数中获取
*/
/*填充函数*/
/*输入放方块区域的起始坐标和结束坐标，以及颜色*/
static void hdmi_fill(unsigned int start_x,unsigned int end_x,
					unsigned int start_y,unsigned int end_y,
					unsigned int argb8888_color)
{
	unsigned int x,y;
	
	if(end_x >= width)
		end_x = width - 1;
	if(end_y >= height)
		end_y = height - 1;
	
	for(y = start_y;y <= end_y;y+=2)			//y方向步进为2
	{
		for(x = start_x;x <= end_x;x++)
			*(screen_base + y*width + x) = argb8888_color;
	}	
}

/*画线函数*/
/*输入线段起始坐标，方向，长度，颜色*/
static void hdmi_draw_line(unsigned int x,unsigned int y,int dir,unsigned int length,unsigned argb8888_color)
{
	unsigned int end;
	unsigned long temp;
	
	if(x >= width)
		x = width - 1;
	if(y >= height)
		y = height - 1;
	
	temp = y*width + x;
	if(dir)				//水平线
	{	
		end = x+length - 1;
		if(end >= width)
			end = width - 1;
		
		for (;x < end;x++,temp++)
			screen_base[temp] = argb8888_color;
	}
	else 				//垂直线
	{
		end = y + (length - 1)*2;		//length = (end -y)*2 + 1
		if(end > height)
			end = height - 1;
		
		for(;y <= end;y+=2,temp+=2*width)
			screen_base[temp] = argb8888_color;
	}
}
	
/*画虚线函数*/
/*输入线段起始坐标，方向，长度，颜色*/	
static void hdmi_draw_dotted_line(unsigned int x,unsigned int y,int dir,unsigned int length,unsigned argb8888_color)
{
	unsigned int end;
	unsigned long temp;
	int i;
	if(x >= width)
		x = width - 1;
	if(y >= height)
		y = height - 1;
	
	temp = y*width + x;
	
	
	
	if(dir)				//水平虚线规格：10像素有色；10像素透明
	{	
		for(i=0;i<1200;i+=20)
			hdmi_draw_line(x+i,y,dir,10,argb8888_color);	
	}
	else 				//垂直虚线规格：10像素有色；10像素透明
	{
		for(i=0;i<960;i+=20)
			hdmi_draw_line(x,y+i,dir,5,argb8888_color);
	}
}

	
/*hdmi配置函数*/
static void hdmi_cfg(void)
{
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;
	
	int i,j;
	
	/*打开frambuffer设备*/
	if(0 > (fd_hdmi = open("/dev/fb0",O_RDWR)))
	{
		perror("open /dev/fb0 error!\n");
		exit(EXIT_FAILURE);
	}
	
	/*参数信息*/
	ioctl(fd_hdmi,FBIOGET_VSCREENINFO,&fb_var);
	ioctl(fd_hdmi,FBIOGET_FSCREENINFO,&fb_fix);
	
	screen_size = fb_fix.line_length*fb_var.yres;
	width = fb_var.xres;
	height = fb_var.yres;

	printf("当前的width与height为：%d*%d\n",width,height);
	
	
	/*将显示缓冲区映射到进程地址空间*/
	screen_base = mmap(NULL,screen_size,PROT_WRITE,MAP_SHARED,fd_hdmi,0);

	if(MAP_FAILED == (void*)screen_base)	
	{
		perror("mmap error\n");
		close(fd_hdmi);
		exit(EXIT_FAILURE);
	}
	


	
}	

/*hdmi界面初始化函数*/
static void hdmi_ini(void)
{
		//清屏,黑色
	hdmi_fill(0,width-1,0,height - 1,0xFFFF);	
	
	//“田”字界面初始化
	hdmi_draw_line(20,20,1,1200,0x1f);
	hdmi_draw_line(20,978,1,1200,0x1f);
	hdmi_draw_line(20,20,0,480,0x1f);
	hdmi_draw_line(1219,20,0,480,0x1f);

	//hdmi_draw_line(20,498,1,1200,0x1f);
	//hdmi_draw_line(619,20,0,480,0x1f);
	
	//8*10网格（虚线）
	hdmi_draw_dotted_line(139,20,0,480,0x1f);
	hdmi_draw_dotted_line(259,20,0,480,0x1f);
	hdmi_draw_dotted_line(379,20,0,480,0x1f);
	hdmi_draw_dotted_line(499,20,0,480,0x1f);
	hdmi_draw_dotted_line(619,20,0,480,0x1f);
	hdmi_draw_dotted_line(739,20,0,480,0x1f);
	hdmi_draw_dotted_line(859,20,0,480,0x1f);
	hdmi_draw_dotted_line(979,20,0,480,0x1f);
	hdmi_draw_dotted_line(1099,20,0,480,0x1f);
	
	hdmi_draw_dotted_line(20,138,1,1200,0x1f);
	hdmi_draw_dotted_line(20,258,1,1200,0x1f);
	hdmi_draw_dotted_line(20,378,1,1200,0x1f);
	hdmi_draw_dotted_line(20,498,1,1200,0x1f);
	hdmi_draw_dotted_line(20,618,1,1200,0x1f);
	hdmi_draw_dotted_line(20,738,1,1200,0x1f);
	hdmi_draw_dotted_line(20,858,1,1200,0x1f);
}


/*画点函数，输入坐标，颜色，画5*2的点*/
static void hdmi_draw_point(int x,int y,unsigned argb8888_color)
{
	
	if(x >= width)
		x = width - 1;
	if(x < 3)
		x = 3;
	
	if(y >= height)
		y = height - 1;
	if(y < 26)
		y = 26;
	
	
	
	screen_base[y*width+x-2] = argb8888_color;
	screen_base[y*width+x-1] = argb8888_color;
	screen_base[y*width+x-0] = argb8888_color;
	screen_base[y*width+x+1] = argb8888_color;
	screen_base[y*width+x+2] = argb8888_color;
	screen_base[(y-2)*width+x-2] = argb8888_color;
	screen_base[(y-2)*width+x-1] = argb8888_color;
	screen_base[(y-2)*width+x-0] = argb8888_color;
	screen_base[(y-2)*width+x+1] = argb8888_color;
	screen_base[(y-2)*width+x+2] = argb8888_color;
	screen_base[(y-4)*width+x-2] = argb8888_color;
	screen_base[(y-4)*width+x-1] = argb8888_color;
	screen_base[(y-4)*width+x-0] = argb8888_color;
	screen_base[(y-4)*width+x+1] = argb8888_color;
	screen_base[(y-4)*width+x+2] = argb8888_color;
	screen_base[(y-6)*width+x-2] = argb8888_color;
	screen_base[(y-6)*width+x-1] = argb8888_color;
	screen_base[(y-6)*width+x-0] = argb8888_color;
	screen_base[(y-6)*width+x+1] = argb8888_color;
	screen_base[(y-6)*width+x+2] = argb8888_color;
	
}


/*画波形函数，将波形和界面区域对应起来*/
/*输出入存储波形的数组，以及存储深度*/
static void wave_disp(unsigned char buff[],int len)
{
	int i,j,k;
	unsigned int temp;
	unsigned int data[len];
	

	for(k=0;k<len;k++)
	{
		data[k]= buff[k];
	}

	printf("转化为偶数\n");
	for(j=0;j<len;j++)
	{
		temp = data[j];
		if(data[j]%2)
			data[j] = temp -1;
		else
			data[j] = temp;
	}
	
	for (i=1;i<len;i++)
	{
		hdmi_draw_point((3+i*6+20),(978-4*data[i]),0x9999);
	}
}
