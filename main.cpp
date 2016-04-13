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

enum { BOOTSTART = 0x5A };

int main()
{
  using namespace Uarts;
  Ldr::Init();
  uint8_t count = 0;
//  wait for request ~256 ms and then exit if no request;
  while(--count) {
    if(Uart::IsEvent(EvRxne) && Uart::Getch() == BOOTSTART) {
      break;
    }
    delay_us<1000>();
  }
  if(!count) {
    Ldr::Go();
  }
//Never return from here
  Ldr::Process();
}


