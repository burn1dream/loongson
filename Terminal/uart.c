#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>

#include "uart.h"

/*
	串口初始化
*/
static int uart_init(const char *device)
{
	fd = open(device,O_RDWR|O_NOCTTY);
	if(0>fd)
	{
		fprintf(stderr,"open error:%s:%s\n",device,strerror(errno));
		return -1;
	}
	
	/*获取串口当前的配置参数*/
	if(0>tcgetattr(fd,&old_cfg))
	{
		fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
		close(fd);
		return -1;
	}
	
	return 0;
}

/*
	串口配置
	cfg 指向一个uart_cfg_t结构体
*/

/*
	对cfg指向的uart_cfg_t结构体进行操作，即可将配置数据写入new_cfg中
*/
static int uart_cfg(const uart_cfg_t *cfg)
{
	struct termios new_cfg={0};
	speed_t speed;
	
	/*原始模式*/
	cfmakeraw(&new_cfg);
	
	/*使能接收*/
	new_cfg.c_cflag |= CREAD;
	
	/*baud*/
	switch (cfg->baudrate) {
	case 1200: speed = B1200;
		break;
	case 1800: speed = B1800;
		break;
	case 2400: speed = B2400;
		break;
	case 4800: speed = B4800;
		break;
	case 9600: speed = B9600;
		break;
	case 19200: speed = B19200;
		break;
	case 38400: speed = B38400;
		break;
	case 57600: speed = B57600;
		break;
	case 115200: speed = B115200;
		break;
	case 230400: speed = B230400;
		break;
	case 460800: speed = B460800;
		break;
	case 500000: speed = B500000;
		break;
	default:
		speed = B115200;
		printf("default baud rate: 115200\n");
		break;
	}
	
	if(0>cfsetspeed(&new_cfg,speed)){
		fprintf(stderr,"cfspeed error:%s\n",strerror(errno));
		return -1;
	}
	
	/*设置数据位大小*/
	new_cfg.c_cflag &= ~CSIZE;
	switch (cfg->dbit) {
		case 5:
			new_cfg.c_cflag |= CS5;
			break;
		case 6:
			new_cfg.c_cflag |= CS6;
			break;
		case 7:
			new_cfg.c_cflag |= CS7;
			break;
		case 8:
			new_cfg.c_cflag |= CS8;
			break;
		default: //默认数据位大小为 8
			new_cfg.c_cflag |= CS8;
			printf("default data bit size: 8\n");
			break;
	}

	/*设置奇偶校验*/
	switch (cfg->parity) {
		case 'N': //无校验
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK;
			break;
		case 'O': //奇校验
			new_cfg.c_cflag |= (PARODD | PARENB);
			new_cfg.c_iflag |= INPCK;
			break;
		case 'E': //偶校验
			new_cfg.c_cflag |= PARENB;
			new_cfg.c_cflag &= ~PARODD; /* 清除 PARODD 标志，配置为偶校验 */
			new_cfg.c_iflag |= INPCK;
			break;
		default: //默认配置为无校验
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK;
			printf("default parity: N\n");
			break;
	}	


	/*设置停止位*/
	switch (cfg->sbit) {
		case 1: //1 个停止位
			new_cfg.c_cflag &= ~CSTOPB;
			break;
		case 2: //2 个停止位
			new_cfg.c_cflag |= CSTOPB;
			break;
		default: //默认配置为 1 个停止位
			new_cfg.c_cflag &= ~CSTOPB;
			printf("default stop bit size: 1\n");
			break;
	}
	
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN] = 0;

	/*清空缓冲区*/
	if(0>tcflush(fd,TCIOFLUSH)){
		fprintf(stderr, "tcflush error: %s\n", strerror(errno));
		return -1;
	}
	
	/* 写入配置、使配置生效 */
	if (0 > tcsetattr(fd, TCSANOW, &new_cfg)) {
		fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
		return -1;
	}
	
	return 0;
}



/**
** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
**/
//static void io_handler(int sig, siginfo_t *info, void *context)
//{
//	int ret;
//	int n;
//	if(SIGRTMIN != sig)
//	return;
//	/* 判断串口是否有数据可读 */
//	if (POLL_IN == info->si_code) {
//		ret = read(fd, buff, 200); //一次最多读 8 个字节数据
//		
//	}
//}

/**
** 异步 I/O 初始化函数
**/

//static void async_io_init(void)
//{ 
//	struct sigaction sigatn;
//	int flag;
//	/*使能异步 I/O */
//	flag = fcntl(fd, F_GETFL); //使能串口的异步 I/O 功能
//	flag |= O_ASYNC;
//	fcntl(fd, F_SETFL, flag);
//	/* 设置异步 I/O 的所有者 */
//	fcntl(fd, F_SETOWN, getpid());
//	/* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
//	fcntl(fd, F_SETSIG, SIGRTMIN);
//	/* 为实时信号 SIGRTMIN 注册信号处理函数 */
//	sigatn.sa_sigaction = io_handler; //当串口有数据可读时，会跳转到 io_handler 函数
//	sigatn.sa_flags = SA_SIGINFO;
//	sigemptyset(&sigatn.sa_mask);
//	sigaction(SIGRTMIN, &sigatn, NULL);
//}



static void datard(void)
{
	uart_cfg_t cfg = {115200,8,'N',1};
	char *device = UART;
	int rw_flag = 0;
	int i,n;
	//unsigned char buff[100] = {0};
	/* 串口初始化 */
	if (uart_init(device))
		exit(EXIT_FAILURE);
	/* 串口配置 */
	if (uart_cfg(&cfg))/*配置失败，返回-1，恢复之前的配置，成功返回0，什么操作都不做*/
	{
		tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
		close(fd);
		exit(EXIT_FAILURE);
	}

	/* 读|写串口 */
	/*
	switch (rw_flag) {
	case 0: //读串口数据
		async_io_init(); //我们使用异步 I/O 方式读取串口的数据，调用该函数去初始化串口的异步 I/O
		for ( ; ; ){
			sleep(20); //进入休眠、等待有数据可读，有数据可读之后就会跳转到 io_handler()函数
			printf("一次休眠");
		}
		break;
	case 1: 
		break;
	}
	*/
	
	

}