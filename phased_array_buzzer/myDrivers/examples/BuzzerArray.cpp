#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

//测试使用bitset、ParallelOut/vector<uint8_t>/PwmPeriodOutArraySoft的速度差别
//#define TEST_PdmPeriodOutArrayBitset
//#define TEST_PdmPeriodOutArrayUint8
#define TEST_PwmPeriodOutArray
#define SAMPLE_RATE 100e3f

static void ThreadBody()
{	
	//测试使用bitset、ParallelOut和使用vector<uint8_t>的速度差别
	Timer timer;
	timer.start();

#ifdef TEST_PdmPeriodOutArrayBitset
	//使用bitset、ParallelOut
	//PdmPeriodOutArrayBitsetSoft
	//	construct : 0.002310s
	//	setFrq : 0.013223s
	//	setSins : 0.013010s
	//	setPhases : 0.013513s
	//	setP2ps : 0.013523s
	//	setFrq : 0.027787s
	pc.printf("PdmPeriodOutArrayBitsetSoft\r\n");
	PdmPeriodOutArrayBitsetSoft<3> pdmPeriodOutArray(
	{
		DigitalOut(PE_9),DigitalOut(PE_11),DigitalOut(PE_13)
	},
		SAMPLE_RATE
	);
	BuzzerArray<3> buzzers(pdmPeriodOutArray);
#endif // TEST_PdmPeriodOutArrayBitset
#ifdef TEST_PdmPeriodOutArrayUint8
	//使用vector<uint8_t>
	//PdmPeriodOutArrayUint8Soft
	//	construct : 0.002248s
	//	setFrq : 0.011445s
	//	setSins : 0.011405s
	//	setPhases : 0.011950s
	//	setP2ps : 0.011960s
	//	setFrq : 0.023954s
	pc.printf("PdmPeriodOutArrayUint8Soft\r\n");
	PdmPeriodOutArrayUint8Soft<3> pdmPeriodOutArray(
	{
		PdmPeriodOutUint8Soft(PE_9),PdmPeriodOutUint8Soft(PE_11),PdmPeriodOutUint8Soft(PE_13)
	},
		SAMPLE_RATE
	);
	BuzzerArray<3> buzzers(pdmPeriodOutArray);
#endif // TEST_PdmPeriodOutArrayUint8
#ifdef TEST_PwmPeriodOutArray
	//使用PwmPeriodOutArraySoft
	//PwmPeriodOutArraySoft
	//	construct : 0.001887s
	//	setFrq : 0.001538s
	//	setSins : 0.001493s
	//	setPhases : 0.001576s
	//	setP2ps : 0.001556s
	//	setFrq : 0.003194s
	pc.printf("PwmPeriodOutArraySoft\r\n");
	PwmPeriodOutArraySoft<3> pwmPeriodOutArray(
	{
		PwmPeriodOutSoft(PE_9),
		PwmPeriodOutSoft(PE_11),
		PwmPeriodOutSoft(PE_13),
	},
	100e3f,
	25e3f
	);
	BuzzerArray<3> buzzers(pwmPeriodOutArray);
#endif // TEST_PwmPeriodOutArray
	
	pc.printf("construct: %fs\r\n", timer.read());

	timer.reset();
	buzzers.setFrq(2e3f);
	pc.printf("setFrq: %fs\r\n", timer.read());

	timer.reset();
	buzzers.setSins(1, 0);
	pc.printf("setSins: %fs\r\n", timer.read());

	//Thread::wait(2000);

	//timer.reset();
	//buzzers.setPhases({ 0,PI / 3,PI * 2 / 3 });
	//pc.printf("setPhases: %fs\r\n", timer.read());

	//Thread::wait(2000);

	//timer.reset();
	//buzzers.setP2ps({ 0.1,0.5,0.9 });
	//pc.printf("setP2ps: %fs\r\n", timer.read());

	//Thread::wait(2000);

	//timer.reset();
	//buzzers.setFrq(1);
	//pc.printf("setFrq: %fs\r\n", timer.read());


	float phase = 0;
	float delta = 0.001;
	for (;;)
	{
		if (phase > PI * 2 || phase < 0)
			delta = -delta;
		phase += delta;
		buzzers.setPhases({ 0,phase,phase * 2 });
		
		//g_LED1 = !g_LED1;
		//pc.printf("%f\r\n", pdmPeriodOutArray.getActualRate());
		//Thread::wait(10);
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
