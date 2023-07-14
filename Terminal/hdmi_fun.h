
static int width;
static int height;
static unsigned short *screen_base;
unsigned int screen_size;
int fd_hdmi;

/*画面填充函数*/
static void hdmi_fill(unsigned int start_x,unsigned int end_x,
					unsigned int start_y,unsigned int end_y,
					unsigned int argb8888_color);
					
/*画线函数*/
static void hdmi_draw_line(unsigned int x,unsigned int y,int dir,unsigned int length,unsigned argb8888_color);

/*画虚线函数*/
static void hdmi_draw_dotted_line(unsigned int x,unsigned int y,int dir,unsigned int length,unsigned argb8888_color);

/*界面初始化函数*/
static void hdmi_ini(void);

/*画点函数，输入坐标，颜色，画5*2的点*/
static void hdmi_draw_point(int x,int y,unsigned argb8888_color);

/*画波形函数，将波形和界面区域对应起来*/
/*输出入存储波形的数组，以及存储深度*/
static void wave_disp(unsigned char buff[],int len);