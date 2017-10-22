#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

//测试使用bitset、ParallelOutput和使用vector<uint8_t>的速度差别
//#define TEST_PdmPeriodOutputArrayP
#define SAMPLE_RATE 1e3f

static void ThreadBody()
{	
	//PwmPeriodOutputArray<8> pwmPeriodOutputArray(
	//{
	//	PwmPeriodOutput(PE_9),
	//	PwmPeriodOutput(PE_11),
	//	PwmPeriodOutput(PE_13),
	//	PwmPeriodOutput(PE_14),
	//	PwmPeriodOutput(PA_5),
	//	PwmPeriodOutput(PB_15),
	//	PwmPeriodOutput(PB_10),
	//	PwmPeriodOutput(PB_11),
	//},
	//100e3f,
	//16e3f
	//);

	//测试使用bitset、ParallelOutput和使用vector<uint8_t>的速度差别
	Timer timer;
	timer.start();

#ifdef TEST_PdmPeriodOutputArrayP
	//使用bitset、ParallelOutput
	//PdmPeriodOutputArrayP
	//	construct : 0.001874s
	//	setFrq : 0.013247s
	//	setSins : 0.013018s
	//	setPhases : 0.013526s
	//	setP2ps : 0.013537s
	//	setFrq : 0.027790s
	pc.printf("PdmPeriodOutputArrayP\r\n");
	PdmPeriodOutputArrayP<3> pdmPeriodOutputArray(
	{
		DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3)
	},
		SAMPLE_RATE
	);
#else
	//使用vector<uint8_t>
	//PdmPeriodOutputArray
	//	construct : 0.001815s
	//	setFrq : 0.011431s
	//	setSins : 0.011416s
	//	setPhases : 0.011956s
	//	setP2ps : 0.011952s
	//	setFrq : 0.023945s
	pc.printf("PdmPeriodOutputArray\r\n");
	PdmPeriodOutputArray<3> pdmPeriodOutputArray(
	{
		PdmPeriodOutput(LED1),PdmPeriodOutput(LED2),PdmPeriodOutput(LED3)
	},
		SAMPLE_RATE
	);
#endif // TEST_PdmPeriodOutputArrayP
	
	pc.printf("construct: %fs\r\n", timer.read());

	BuzzerArray<3> buzzers(pdmPeriodOutputArray);

	timer.reset();
	buzzers.setFrq(2);
	pc.printf("setFrq: %fs\r\n", timer.read());

	timer.reset();
	buzzers.setSins(1, 0);
	pc.printf("setSins: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setPhases({ 0,PI / 3,PI * 2 / 3 });
	pc.printf("setPhases: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setP2ps({ 0.1,0.5,0.9 });
	pc.printf("setP2ps: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setFrq(1);
	pc.printf("setFrq: %fs\r\n", timer.read());



	for (;;)
	{
		//g_LED1 = !g_LED1;
		//pc.printf("%f\r\n", pdmPeriodOutputArray.getActualRate());
		Thread::wait(500);
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
		Thread::wait(300);
	}
}
