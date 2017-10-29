#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);



static void ThreadBody()
{	
	PdmPeriodOutArrayDma pdmPeriodOutArrayDma(PortB, 10e3f);
	BuzzerArray<16> buzzerArray(pdmPeriodOutArrayDma);
	buzzerArray.setFrq(3);
	//buzzerArray.setSins(1, 0);

	array<float, 16> p2ps, phases;
	sawPeriod(p2ps.begin(), p2ps.end(), 1, 1, 0);
	sawPeriod(phases.begin(), phases.end(), 0, PI * 2, 0);
	buzzerArray.setSins(p2ps, phases);



	for (;;)
	{
		//g_LED1 = !g_LED1;
		Thread::wait(500);
		//pc.printf("%d\r\n", dma->getState());
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

