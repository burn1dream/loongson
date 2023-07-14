CC = loongarch64-linux-gnu-gcc
TARGET = oscilloscope
OBJS = oscilloscope.o hdmi_fun.o uart.o


$(TARGET):$(OBJS)
	$(CC) $(OBJS)  -static -o $(TARGET)

oscilloscope.o: oscilloscope.c uart.c hdmi_fun.c uart.h hdmi_fun.h
	$(CC) -static  -c hdmi_fun.c uart.c oscilloscope.c

uart.o:uart.c uart.h
	$(CC) -static -c uart.c
	
hdmi_fun.o:hdmi_fun.c hdmi_fun.h
	&(CC) -static -c hdmi_fun.c
	
.PHONY:clean 
clean:
	rm -f *.o 
	

