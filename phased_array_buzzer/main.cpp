#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);
PdmPeriodOutArrayDma test(PortB);

uint16_t data[] = { 0x0000,0xffff,0x0000 };


static void ThreadBody()
{	

	Dma2Timer1 *dma2Timer1 = Dma2Timer1::instance(3);
	dma2Timer1->start((uint32_t *)data, (uint32_t *)&GPIOB->ODR, 3);

	for (;;)
	{
		//g_LED1 = !g_LED1;
		Thread::wait(500);
		pc.printf("%d\r\n", dma2Timer1->getState());
	}
}

Thread thread(osPriorityNormal, uint32_t(4 * 1024));

int main()
{
	thread.start(callback(ThreadBody));
	
	for (;;)
	{
		//g_LED2 = !g_LED2;
		//pc.printf("%u, %u\r\n", thread.stack_size(), thread.used_stack());
		//Thread::wait(300);
	}
}

