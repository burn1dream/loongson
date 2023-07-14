
#define UART "/dev/ttyS1"

typedef struct uart_hardware_cfg{
	unsigned int baudrate;
	unsigned char dbit;		/*数据位*/
	char parity;
	unsigned char sbit;
}	uart_cfg_t;

static struct termios old_cfg;
static int fd;
unsigned char buff[200];


static int uart_init(const char *device);
static int uart_cfg(const uart_cfg_t *cfg);
//static void io_handler(int sig, siginfo_t *info, void *context);
//static void async_io_init(void);
static void datard(void);
