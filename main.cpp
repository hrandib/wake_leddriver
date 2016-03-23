#include "stm8s.h"
#include "itc.h"
#include "gpio.h"
#include "delay.h"
#include "timers.h"
#include "pinlist.h"
#include "bootloader.h"
#include <intrinsics.h>

using namespace Mcudrv;
using namespace Wk;
typedef Bootloader<ID_STM8S003F3> Ldr;

int main()
{
	Ldr::Init();
//Never return from here
	Ldr::Process();
}


