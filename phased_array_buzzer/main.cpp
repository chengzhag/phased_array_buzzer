#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);


static void ThreadBody()
{	
	
	for (;;)
	{
		g_LED1 = !g_LED1;
		Thread::wait(10);
	}
}

Thread thread(osPriorityNormal, uint32_t(4 * 1024));

int main()
{
	thread.start(callback(ThreadBody));
	
	for (;;)
	{
		g_LED2 = !g_LED2;
		pc.printf("%u, %u\r\n", thread.stack_size(), thread.used_stack());
		Thread::wait(300);
	}
}
