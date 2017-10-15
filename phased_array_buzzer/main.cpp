#include <mbed.h>
#include <rtos.h>

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);

static void ThreadBody(const void *) 
{
	for (;;)
	{
		g_LED1 = !g_LED1;
		Thread::wait(500);
	}
}

int main()
{
	Thread thread(ThreadBody);
	for (;;)
	{
		g_LED2 = !g_LED2;
		Thread::wait(300);
	}
}
