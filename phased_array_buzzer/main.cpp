#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);



static void ThreadBody()
{
	PdmPeriodOutputArray<3> pdmPeriodOutputArray(
	{
		DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3)
	},
		50e3f
	);

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

	BuzzerArray<3> buzzers(pdmPeriodOutputArray);

	buzzers.setFrq(2);
	buzzers.setSins(1, 0);

	for (;;)
	{
		//g_LED1 = !g_LED1;
		pc.printf("%f\r\n", pdmPeriodOutputArray.getActualRate());
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
		pc.printf("%u, %u\r\n", thread.stack_size(), thread.used_stack());
		Thread::wait(300);
	}
}
