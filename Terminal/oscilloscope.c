#define _GNU_SOURCE

#include "uart.c"
#include "hdmi_fun.c"

/*显示屏参数*/
static int width;
static int height;
static unsigned short *screen_base = NULL;
unsigned int screen_size;
int fd_hdmi;

/*uart参数*/
static int fd;
unsigned char buff[200] = {0}; 

/**
** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
**/
static void io_handler(int sig, siginfo_t *info, void *context)
{
	int ret;
	int n;
	if(SIGRTMIN != sig)
	return;
	/* 判断串口是否有数据可读 */
	if (POLL_IN == info->si_code) {
		ret = read(fd, buff, 200); //一次最多读 8 个字节数据
		hdmi_ini();
		wave_disp(buff,200);
	}
}

/**
** 异步 I/O 初始化函数
**/
static void async_io_init(void)
{ 
	struct sigaction sigatn;
	int flag;
	/* 使能异步 I/O */
	flag = fcntl(fd, F_GETFL); //使能串口的异步 I/O 功能
	flag |= O_ASYNC;
	fcntl(fd, F_SETFL, flag);
	/* 设置异步 I/O 的所有者 */
	fcntl(fd, F_SETOWN, getpid());
	/* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
	fcntl(fd, F_SETSIG, SIGRTMIN);
	/* 为实时信号 SIGRTMIN 注册信号处理函数 */
	sigatn.sa_sigaction = io_handler; //当串口有数据可读时，会跳转到 io_handler 函数
	sigatn.sa_flags = SA_SIGINFO;
	sigemptyset(&sigatn.sa_mask);
	sigaction(SIGRTMIN, &sigatn, NULL);
}








int main()
{
	int i;
	int n=110;
	/*hdmi*初始化*/
	hdmi_cfg();
	hdmi_ini();
	
	datard();
		
	for(i=0;i<200;i++)
	{
		buff[i] = n;
		printf("buff %d : %d",i,buff[i]);
		
		if((i<50)||((i>=100)&&(i<150)))
		{n++;}
		else
		{n--;}
		
	}
	

	wave_disp(buff,200);

	
	/* 退出 */
	tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
	close(fd);
	exit(EXIT_SUCCESS);
	
	
	/*解除映射，退出显示*/
	munmap(screen_base,screen_size);
	close(fd_hdmi);
	exit(EXIT_SUCCESS);
	
}


