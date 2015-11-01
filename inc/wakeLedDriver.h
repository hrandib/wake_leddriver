#pragma once
#include <cstring>

#include "wake.h"
#include "gpio.h"
#include "timers.h"

//		MBI6651
//	First Channel,	Fan Control
//  PC3(TIM1_CH3)	PD4(TIM2_CH1)
//		NCP3066
//	First Channel,				Fan Control
//	PA3(TIM2_CH3) - On/Off		PD4(TIM2_CH1)
//  PD3(TIM2_CH2) - driver NFB

namespace Mcudrv
{
	namespace Wk
  {

	struct LedDriverDefaultFeatures
	{
		enum
		{
			TwoChannels = false,
			FanControl = true
		};
	};

	static const uint8_t ledLinear[45] = {
		56, 58, 60, 62, 64, 66, 68, 70, 72, 74,
		77, 80, 83, 86, 89, 92, 95, 98, 101, 104, 107,
		111, 115, 119, 123, 127, 131,
		136, 141, 146, 151, 156, 161,
		167, 173, 179, 185, 191,
		198, 205, 212, 219,
		228, 237, 246
	};

	template<typename Features = LedDriverDefaultFeatures>
	class LedDriver : WakeData
	{
	private:
		union state_t
		{
			struct
			{
				uint8_t ch[2];
				uint8_t fanSpeed;
			};
			uint32_t Data;
		};
		enum Ch { Ch1, Ch2 = Features::TwoChannels };
		enum InstructionSet
		{
			C_GetState = 16,
			C_GetBright = 16,
			C_GetFan = 16,
			C_SetBright = 17,
			C_IncBright = 18,
			C_DecBright = 19,
			C_SetFan = 20,
			C_SetGfgFanAuto = 21
		};
		#pragma data_alignment=4
		#pragma location=".eeprom.noinit"
		static state_t state_nv;// @ ".eeprom.noinit";
		static state_t curState, onState;
		static uint8_t GetFanSpeed()
		{
			uint8_t tmp = curState.fanSpeed;
			return tmp > 20 ? (tmp - 20) / 2 : 0;
		}
		static void SetFanSpeed(uint8_t speed)
		{
			if(speed > 100) speed = 255;
			else if (speed > 0)
			{
				speed = speed * 2 + 20;
			}
			curState.fanSpeed = speed;
		}
		static void UpdIRQ()	//Soft Dimming
		{
			if (T2::Timer2::CheckIntStatus(T2::IRQ_Update))
			{
				T2::Timer2::ClearIntFlag(T2::IRQ_Update);
				if (T2::Timer2::ReadCompareByte<T2::Ch3>() < curState.ch[Ch1])
				{
					T2::Timer2::GetCompareByte<T2::Ch3>()++;
				}
				else if (T2::Timer2::ReadCompareByte<T2::Ch3>() > curState.ch[Ch1])
				{
					T2::Timer2::GetCompareByte<T2::Ch3>()--;
				}
				if (Features::TwoChannels)
				{
					if (T2::Timer2::ReadCompareByte<T2::Ch2>() < curState.ch[Ch2])
					{
						T2::Timer2::GetCompareByte<T2::Ch2>()++;
					}
					else if (T2::Timer2::ReadCompareByte<T2::Ch2>() > curState.ch[Ch2])
					{
						T2::Timer2::GetCompareByte<T2::Ch2>()--;
					}
				}
				if(Features::FanControl)
				{
					using namespace T2;
					if(Timer2::ReadCompareByte<T2::Ch1>() < curState.fanSpeed)
					{
						Timer2::GetCompareByte<T2::Ch1>()++;
					}
					else if(Timer2::ReadCompareByte<T2::Ch1>() > curState.fanSpeed)
					{
						Timer2::GetCompareByte<T2::Ch1>()--;
					}
				}
			}
		}
	public:
		enum
		{
			deviceMask = devLedDriver,
			features = Features::TwoChannels | Features::FanControl << 1UL
		};
		#pragma inline=forced
		static void Init()
		{
			using namespace T2;
			static const ChannelCfgOut channelConfig = ChannelCfgOut(Out_PWM_Mode1 | Out_PreloadEnable);
			Pa3::SetConfig<GpioBase::Out_PushPull_fast>();
			Timer2::Init(Div_16, Cfg(ARPE | CEN));
			Timer2::WriteAutoReload(255);			//Fcpu/16/256 ~= 490Hz for 2 MHz
			Timer2::SetChannelCfg<T2::Ch3, Output, channelConfig>();
			Timer2::ChannelEnable<T2::Ch3>();
			Pd3::SetConfig<GpioBase::Out_PushPull_fast>();
			if(Features::TwoChannels)
			{
				Timer2::SetChannelCfg<T2::Ch2, Output, channelConfig>();
				Timer2::ChannelEnable<T2::Ch2>();
			}
			if(Features::FanControl)
			{
				Pd4::SetConfig<GpioBase::Out_PushPull_fast>();
				Timer2::SetChannelCfg<T2::Ch1, Output, channelConfig>();
				Timer2::ChannelEnable<T2::Ch1>();
			}
			OpTime::SetTimerCallback(UpdIRQ);
		}
		#pragma inline=forced
		static void Process()
		{
			switch(cmd)
			{
				case C_GetState:
				{
					if(!pdata.n)	//Common query - all channels brightness
					{
						pdata.buf[0] = ERR_NO;
						pdata.buf[1] = GetBrightness(Ch1);
						if(Ch2)
						{
							pdata.buf[2] = GetBrightness(Ch2);
							pdata.n = 3;
						}
						else pdata.n = 2;
					}
					else if(pdata.n == 1)
					{
						if(!pdata.buf[0]) // Ch1 brightness
						{
							pdata.buf[0] = ERR_NO;
							pdata.buf[1] = GetBrightness(Ch1);
							pdata.n = 2;
						}
						else if(Ch2 && pdata.buf[0] == 0x80) //Ch2 brightness
						{
							pdata.buf[0] = ERR_NO;
							pdata.buf[1] = GetBrightness(Ch2);
							pdata.n = 2;
						}
						else if(Features::FanControl && pdata.buf[0] == 0x01) //fan speed
						{
							pdata.buf[0] = ERR_NO;
							pdata.buf[1] = GetFanSpeed();
							pdata.n = 2;
						}
						else
						{
							pdata.buf[0] = ERR_NI;
							pdata.n = 1;
						}
					}
					else // param error
					{
						pdata.buf[0] = ERR_PA;
						pdata.n = 1;
					}
				}
					break;
				case C_SetBright:
				{
					if(pdata.n == 1)
					{
						if(!(pdata.buf[0] & 0x80))
						{
							SetBrightness(pdata.buf[0], Ch1);
						}
						else if(Ch2)
						{
							SetBrightness(pdata.buf[0] & 0x7F, Ch2);
						}
						else
						{
							pdata.buf[0] = ERR_NI;
							break;
						}
						pdata.buf[0] = ERR_NO;
					}
					else
					{
						pdata.buf[0] = ERR_PA;
						pdata.n = 1;
					}
				}
					break;
				case C_IncBright:
				{
					if(pdata.n == 1)
					{
						if(!(pdata.buf[0] & 0x80))
						{
							pdata.buf[1] = IncBrightness(pdata.buf[0], Ch1);
						}
						else if(Ch2)
						{
							pdata.buf[1] = IncBrightness(pdata.buf[0] & 0x7F, Ch2);
						}
						else
						{
							pdata.buf[0] = ERR_NI;
							break;
						}
						pdata.buf[0] = ERR_NO;
						pdata.n = 2;
					}
					else
					{
						pdata.buf[0] = ERR_PA;
						pdata.n = 1;
					}
				}
					break;
				case C_DecBright:
				{
					if(pdata.n == 1)
					{
						if(!(pdata.buf[0] & 0x80))
						{
							DecBrightness(pdata.buf[0], Ch1);
						}
						else if(Ch2)
						{
							DecBrightness(pdata.buf[0] & 0x7F, Ch2);
						}
						else
						{
							pdata.buf[0] = ERR_NI;
							break;
						}
						pdata.buf[0] = ERR_NO;
						pdata.n = 2;
					}
					else
					{
						pdata.buf[0] = ERR_PA;
						pdata.n = 1;
					}
				}
					break;
				case C_SetFan:
				{
					if(pdata.n == 1)
					{
						if(Features::FanControl)
						{
							SetFanSpeed(pdata.buf[0]);
							pdata.buf[0] = ERR_NO;
							pdata.buf[1] = GetFanSpeed();
							pdata.n = 2;
						}
						else pdata.buf[0] = ERR_NI;
					}
					else
					{
						pdata.buf[0] = ERR_PA;
						pdata.n = 1;
					}
				}
					break;
				case C_SetGfgFanAuto:
				{
					if(pdata.n == 3) pdata.buf[0] = ERR_NI;
					else pdata.buf[0] = ERR_PA;
				}
					break;
			}
		}

		static void SetBrightness(uint8_t br, const Ch ch = Ch1)
		{
			//Led brightness linearisation
			if(br > 99) br = 255;
			else if(br > 54) br = ledLinear[br - 55];
			curState.ch[ch] = br;
		}
		static uint8_t GetBrightness(const Ch ch = Ch1)
		{
			uint8_t br = curState.ch[ch];
			if(br > 54)
			{
				for(uint8_t i = 0; i < sizeof(ledLinear); ++i)
				{
					if(ledLinear[i] == br) return i + 55;
				}
				return 100;
			}
			return br;
		}
		static uint8_t IncBrightness(const uint8_t step, const Ch ch = Ch1)
		{
			uint8_t cur = GetBrightness(ch);
			if(cur < 100)
			{
				cur += step;
				if(cur > 100) cur = 100;
				SetBrightness(cur, ch);
			}
			return cur;
		}
		static uint8_t DecBrightness(const uint8_t step, const Ch ch = Ch1)
		{
			uint8_t cur = GetBrightness(ch);
			if(cur <= step) cur = 0;
			else cur -= step;
			SetBrightness(cur, ch);
			return cur;
		}
		static void SaveState()
		{
			using namespace Mem;
			if (state_nv.Data != curState.Data) //Only if changed
			{
				Unlock<Eeprom>();
				if(IsUnlocked<Eeprom>())
				{
					SetWordProgramming();
					state_nv = curState;
				}
				Lock<Eeprom>();
			}
		}

		#pragma inline=forced
		static void On()
		{
			curState = onState;
		}
		static void Off()
		{
			onState.ch[Ch1] = curState.ch[Ch1] ? curState.ch[Ch1] : 30;
			if(Features::TwoChannels) onState.ch[Ch2] = curState.ch[Ch2] ? curState.ch[Ch2] : 30;
			if(Features::FanControl) onState.fanSpeed = curState.fanSpeed;
			curState.Data = 0;
		}
		#pragma inline=forced
		static void ToggleOnOff()
		{
			if(!curState.Data) On();
			else Off();
		}

		#pragma inline=forced
		static uint8_t GetDeviceFeatures(const uint8_t)
		{
			return features;
		}
	};

	template<typename Features>
	LedDriver<Features>::state_t LedDriver<Features>::state_nv;
	template<typename Features>
	LedDriver<Features>::state_t LedDriver<Features>::curState = state_nv;
	template<typename Features>
	LedDriver<Features>::state_t LedDriver<Features>::onState;

  } //Ldrv
} //Mcudrv

