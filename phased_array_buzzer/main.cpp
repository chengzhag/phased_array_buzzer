#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

PdmPeriodOutArrayDma pdmPeriodOutArrayDma(PortB, 800e3);
SpeakerArray<16> speakerArray(pdmPeriodOutArrayDma);
Ticker fmcwTask;

static void fmcwBody()
{
	static float frq = 20e3, inc = 2;

	frq += inc;
	if (frq >= 22e3 /*|| frq <= 20e3*/)
	{
		frq = 20e3;
	}

	speakerArray.setFrq_byChangingSampleRate(frq);
}

static void ThreadBody()
{	
	Timer timer;
	
	speakerArray.setFrq_byChangingSamplePoints(20e3);
	speakerArray.setSins(1, 0);

	fmcwTask.attach_us(callback(fmcwBody), 5000);

	//array<float, 16> p2ps, phases;
	//sawPeriod(p2ps.begin(), p2ps.end(), 1, 1, 0);
	//sawPeriod(phases.begin(), phases.end(), 0, PI, 0);
	//buzzerArray.setSins(p2ps, phases);


	//float frq = 20e3, inc = 10;
	for (;;)
	{
		//frq += inc;
		//if (frq >= 24e3 || frq <= 20e3)
		//{
		//	inc = -inc;
		//}

		//buzzerArray.setFrq_byChangingSampleRate(frq);

		////g_LED1 = !g_LED1;
		//Thread::wait(1);
		////pc.printf("%d\r\n", dma->getState());
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

