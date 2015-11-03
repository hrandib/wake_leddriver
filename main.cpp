#include "stm8s.h"
#include "itc.h"
#include "gpio.h"
#include "delay.h"
#include "timers.h"
#include "pinlist.h"
#include "led_driver.h"
#include <intrinsics.h>

using namespace Mcudrv;

typedef Wk::LedDriver<> LedDriver;
typedef Wk::ModuleList<Wk::LedDriver<> > moduleList;
typedef Wk::Wake<moduleList> Wake;
template void Wake::TxISR();
template void Wake::RxISR();

int main()
{
//	SysClock::Select(SysClock::HSE);
	GpioA::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioB::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioC::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioD::WriteConfig<0xFF, GpioBase::In_Pullup>();
	Wake::Init();
	enableInterrupts();
	while(true)
	{
		Wake::Process();
	}
}


