/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GRAPHICS_FONTS_BDF_H
#define GRAPHICS_FONTS_BDF_H

#include "common/system.h"
#include "common/types.h"

#include "graphics/font.h"

namespace Common {
class SeekableReadStream;
}

namespace Graphics {

struct BdfBoundingBox {
	uint8 width, height;
	int8 xOffset, yOffset;
};

struct BdfFontData {
	const char *familyName;
	const char *slant;

	int maxAdvance;
	int height;
	int size;
	BdfBoundingBox defaultBox;
	int ascent;

	uint firstCharacter;
	int defaultCharacter;
	uint numCharacters;

	const byte *const *bitmaps;
	const byte *advances;
	const BdfBoundingBox *boxes;
};

class BdfFont : public Font {
public:
	BdfFont(const BdfFontData &data, DisposeAfterUse::Flag dispose);
	~BdfFont();

	virtual int getFontHeight() const;
	virtual int getFontAscent() const;
	virtual int getMaxCharWidth() const;

	virtual int getCharWidth(uint32 chr) const;
	virtual void drawChar(Surface *dst, uint32 chr, int x, int y, uint32 color) const;

	const char *getFamilyName() const;
	const char *getFontSlant() const;
	int getFontSize() const;

	static BdfFont *loadFont(Common::SeekableReadStream &stream);
	static bool cacheFontData(const BdfFont &font, const Common::Path &filename);
	static BdfFont *loadFromCache(Common::SeekableReadStream &stream);
	static BdfFont *scaleFont(const BdfFont *src, int newSize);
private:
	int mapToIndex(uint32 ch) const;

	const BdfFontData _data;
	const DisposeAfterUse::Flag _dispose;
};

#define DEFINE_FONT(n) \
	const BdfFont *n = 0; \
	void create_##n() { \
		n = new BdfFont(desc, DisposeAfterUse::NO); \
	}

#define FORWARD_DECLARE_FONT(n) \
	extern const BdfFont *n; \
	extern void create_##n()

#define INIT_FONT(n) \
	create_##n()

} // End of namespace Graphics

#endif
