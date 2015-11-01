#pragma once
#ifndef MENU_H
#define MENU_H
#include "stdint.h"

namespace Mcudrv
{
namespace Menu {
//	template
	class Item
	{
		Item* Next;
		Item* Previous;
		Item* Parent;
		Item* Child;
		const uint8_t* Name;
	};

}

}

#endif // MENU_H

