#include "stm8s.h"
#include "delay.h"
#include "bootloader.h"

using namespace Mcudrv;
using namespace Wk;
typedef Bootloader<ID_STM8S003F3> Ldr;

int main()
{
  using namespace Uarts;
  Ldr::Init();
  uint8_t count = 0;
  while(true) {
//wait for request ~256 ms and then (if no request);
    while(--count) {
      if(Ldr::ProcessHandshake()) break;
      delay_us<1000>();
    }
// try to start user firmware if exist
    if(!count) Ldr::Go();
// go to process commands if got the request
    else break;
// firmware not exist, new cycle
  }
//Never return from here
  Ldr::Process();
}


