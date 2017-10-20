#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

const size_t bufSize = 16;

static void ThreadBody()
{
	PwmPeriodDirectArray<8, bufSize> pwmPeriodArray(
	{
		PwmPeriod<bufSize>(PE_9),
		PwmPeriod<bufSize>(PE_11),
		PwmPeriod<bufSize>(PE_13),
		PwmPeriod<bufSize>(PE_14),
		PwmPeriod<bufSize>(PA_5),
		PwmPeriod<bufSize>(PB_15),
		PwmPeriod<bufSize>(PB_10),
		PwmPeriod<bufSize>(PB_11),
	},
	20e3f,
	4
	);
	BuzzerArray<8, bufSize> buzzerArray(pwmPeriodArray);

	buzzerArray.setSins(1, 0);

	for (;;)
	{
		g_LED1 = !g_LED1;
		pc.printf("%f\r\n", pwmPeriodArray.getRate());
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
