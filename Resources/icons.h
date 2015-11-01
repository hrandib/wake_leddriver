/*
 * Icons.h
 *
 *  Created on: 22.01.2012
 *      Author: D
 */
#include "stdint.h"

#ifndef ICONS_H_
#define ICONS_H_
namespace Mcudrv {
namespace Resources {
enum
{
	EMPTY_CHECKBOX_SYM,
	CHECKED_CHECKBOX_SYM,
	POINTER_SYM_LEFT,
	POINTER_SYM_RIGHT,
	POINTER_SYM_UP = 0,
	POINTER_SYM_DOWN = 1
};

extern const uint8_t icons101x16[], icons8x8[], icons12x8[];

}//Resources
}//Mcudrv


#endif /* ICONS_H_ */

