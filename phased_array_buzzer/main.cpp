#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

const size_t bufSize = 8;

static void ThreadBody()
{
	PdmPeriodOutputArray<8> test;
	PwmPeriodOutputArray<8> pwmPeriodOutputArray(
	{
		PwmPeriodOutput(PE_9),
		PwmPeriodOutput(PE_11),
		PwmPeriodOutput(PE_13),
		PwmPeriodOutput(PE_14),
		PwmPeriodOutput(PA_5),
		PwmPeriodOutput(PB_15),
		PwmPeriodOutput(PB_10),
		PwmPeriodOutput(PB_11),
	},
	100e3f,
	16e3f
	);

	BuzzerArray<8> buzzers(pwmPeriodOutputArray);

	buzzers.setSamplePoints(8);
	buzzers.setSins(0.5, 0);

	for (;;)
	{
		g_LED1 = !g_LED1;
		pc.printf("%f\r\n", pwmPeriodOutputArray.getActualRate());
		Thread::wait(500);
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
