#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"
//#include <arm_math.h>
//#include <algorithm>

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

PwmPeriodArray<8, 10> pwmPeriodArray(
{
PwmPeriodTicker<10>(PE_9,{ 0 }),
PwmPeriodTicker<10>(PE_11,{ 0 }),
PwmPeriodTicker<10>(PE_13,{ 0 }),
PwmPeriodTicker<10>(PE_14,{ 0 }),
PwmPeriodTicker<10>(PA_5,{ 0 }),
PwmPeriodTicker<10>(PB_15,{ 0 }),
PwmPeriodTicker<10>(PB_10,{ 0 }),
PwmPeriodTicker<10>(PB_11,{ 0 }),
},
20000
);


static void ThreadBody(const void *) 
{
	for (;;)
	{
		g_LED1 = !g_LED1;
		pc.printf("%f\r\n", pwmPeriodArray.rate());
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
