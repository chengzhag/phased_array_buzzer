#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

DigitalOut g_LED1(LED1);
DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

PwmPeriodDirectArray<8, 10> pwmPeriodArray(
{
	PwmPeriod<10>(PE_9),PwmPeriod<10>(PE_11),PwmPeriod<10>(PE_13),
	PwmPeriod<10>(PE_14),PwmPeriod<10>(PA_5),PwmPeriod<10>(PB_15),
	PwmPeriod<10>(PB_10),PwmPeriod<10>(PB_11),
},
500e3f,
20e3f
);

BuzzerArray<8, 10> buzzerArray(pwmPeriodArray);



static void ThreadBody(const void *) 
{
	for (;;)
	{
		g_LED1 = !g_LED1;
		pc.printf("%f\r\n", pwmPeriodArray.getRate());
		Thread::wait(500);
	}
}

void setup()
{
	buzzerArray.setSins({ 1 }, { 0 });
	for (auto& p: pwmPeriodArray)
	{
		triPeriod(p.begin(), p.end(), 0, 1, 0);
	}
}

int main()
{
	setup();

	Thread thread(ThreadBody);

	for (;;)
	{
		g_LED2 = !g_LED2;
		Thread::wait(300);
	}
}
