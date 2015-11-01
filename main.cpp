#include "stm8s.h"
#include "gpio.h"
#include "itc.h"

//typedef Mcudrv::InvertedPin<Mcudrv::Pd0> Green;
//typedef Mcudrv::Pd2 Led1;
//typedef Mcudrv::Pd3 Led2;
//typedef Mcudrv::Pd4 Led3;
typedef Mcudrv::Pb4 Yellow;
typedef Mcudrv::Pb5 Green;

//#define UARTECHO
#include "uart.h"
#include "delay.h"
#include "timers.h"
#include <intrinsics.h>
#include "pinlist.h"
//#include "ssd1306.h"
using namespace Mcudrv;
//typedef Twis::SoftTwi<Twis::Fast> Twi;
//typedef ssd1306<Twi> Disp;

//typedef Pinlist<Pd2, SequenceOf<3> > Leds;

typedef Pinlist<Yellow, Green> Leds;
#include "wakeLedDriver.h"
//#include "wake.h"

//typedef Uarts::UartIrq<128, 8> Uart;
//template void Uart::TxISR();	//explicit specialisation necessary to
//template void Uart::RxISR();	//the compliler to generate code for IRQ
//#include "onewire.h"

//typedef Wk::ModuleList<Wk::LedDriver<> > moduleList;
typedef Wk::LedDriver<> LedDriver;
//typedef Uarts::Uart Uart;
typedef Wk::Wake<LedDriver> Wake;
template void Wake::TxISR();
template void Wake::RxISR();

//typedef OneWire<Pc4> Ow;
//typedef Ds18b20<Ow, 6> Tsense;

//enum { maxDevices = 6 };
int main()
{
//	SysClock::Select(SysClock::HSE);
	GpioA::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioB::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioC::WriteConfig<0xFF, GpioBase::In_Pullup>();
 	GpioD::WriteConfig<0xFF, GpioBase::In_Pullup>();
	GpioB::SetConfig<P4 | P5, GpioBase::Out_OpenDrain>();
	Green::Clear();
	Yellow::Clear();
//	Green::SetConfig<GpioBase::Out_OpenDrain>();
//	Yellow::Set();
//	Pa3::SetConfig<GpioBase::Out_PushPull>();
//	Pa3::Clear();
//	Leds::SetConfig<GpioBase::Out_PushPull>();
//	Green::SetConfig<GpioBase::Out_PushPull>();
//	Green::Clear();
//	Uart::Init<Uarts::DefaultCfg, 9600UL>();
//	Uart::Puts("\rHello\r\n");
//	Twi::Init();
//	Disp::Init();
//	Disp::SetContrast(8);
//	Disp::Fill();
//	Disp::Puts("Hello\r\n");
	Wake::Init();
	enableInterrupts();
//	Ow::Init();


//	uint8_t devNumber = Tsense::Init();
//	Disp::Putch(devNumber + '0');
//	Disp::Puts(" Devices found\r\n");
//	Tsense::PrintID<Disp>();
	while(true)
	{
//		switch(Uart::Getch())
//		{
//		case 'a':
//			Led1::Toggle();
//			break;
//		case 's':
//			Led2::Toggle();
//			break;
//		default:
//			Led3::Toggle();

//		}

		Wake::Process();
/*		if(!Tsense::Convert())
		{
			Disp::Puts("Convert failed\n");
			delay_ms(3000);
			continue;
		}
		delay_ms(3000);
		Uart::Puts("   \r");
		for(uint8_t x = 0; x < devNumber; ++x)
		{
			uint16_t result = Tsense::Get(x);
			Uart::Puts(result >> 4);
			Uart::Putch('.');
			if((result & 0x0F) == 0x01) Uart::Putch('0');
			Uart::Puts((result & 0x0F) * 625);
			Uart::Putch(' ');
		}
*/
	}
}


