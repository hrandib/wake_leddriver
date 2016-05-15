#include <intrinsics.h>
#include "stm8s.h"
#include "itc.h"
#include "gpio.h"
#include "delay.h"
#include "timers.h"
#include "pinlist.h"

#define INSTRUCTION_SET_VER_MAJOR 2
#define INSTRUCTION_SET_VER_MINOR 1

#define NCP3066

#if defined NCP3066
#include "led_driver_ncp3066.h"
#elif defined LED_STRIP
#include "led_driver_strip.h"
#elif defined MBI6651
#include "led_driver_mbi6651.h"
#endif

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
	while(true) {
		Wake::Process();
	}
}


