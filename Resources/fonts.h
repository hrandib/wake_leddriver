#pragma once
#ifndef FONTS_H
#define FONTS_H

#include "stdint.h"
namespace Mcudrv {
namespace Resources
{
namespace Internal {
extern const uint8_t font_5x8[], font_10x16[];
}

	class Font
	{
	private:
		enum FontStruct
		{
			CharOffset,
			SizeX,
			SizeY,
			FontStart,
			EngEnd = 123,
			RusStart = 192,
			Gap = RusStart - EngEnd
		};
		const uint8_t* const ftable_;
		uint16_t GetIndex(uint8_t ch) const
		{
			return ((ch < EngEnd ? ch : ch - Gap) - ftable_[CharOffset]) * Width() * (Height() >> 3) + FontStart;
		}
	public:
		Font(const uint8_t* ftable) : ftable_(ftable) { }
		uint8_t Height() const { return ftable_[SizeY]; }
		uint8_t Width() const { return ftable_[SizeX]; }
		const uint8_t* operator[](uint8_t ch) const
		{
			return &ftable_[GetIndex(ch)];
		}
	};
	class Bitmap
	{
	private:
		const uint8_t* const data_;
		const uint8_t sizeX_, sizeY_;
	public:
		Bitmap(const uint8_t* data, uint8_t sizeX, uint8_t sizeY) :
			data_(data), sizeX_(sizeX), sizeY_(sizeY)
		{	}
		uint8_t Width() const
		{
			return sizeX_;
		}
		uint8_t Height() const
		{
			return sizeY_;
		}
		uint8_t operator[](uint16_t index) const
		{
			return data_[index];
		}
	};
	class BitmapArray
	{
	private:
		enum BitmapStruct
		{
			SizeX,
			SizeY,
			BitmapStart
		};
		const uint8_t* const data_;
		uint16_t GetPosition(uint8_t index) const
		{
			return index * Width() * (Height() >> 3) + BitmapStart;
		}
	public:
		BitmapArray(const uint8_t* data) :
			data_(data) {	}
		uint8_t Width() const
		{
			return data_[SizeX];
		}
		uint8_t Height() const
		{
			return data_[SizeY];
		}
		Bitmap operator[](uint8_t index) const
		{
			return Bitmap(&data_[GetPosition(index)], Width(), Height());
		}
	};

	const Font font5x8(Internal::font_5x8);
	const Font font10x16(Internal::font_10x16);
	enum Color
	{
		Clear,
		Solid = 0xFF
	};
}//Resources
}//Mcudrv

#endif






