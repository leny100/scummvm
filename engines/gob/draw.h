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
 *
 * This file is dual-licensed.
 * In addition to the GPLv3 license mentioned above, this code is also
 * licensed under LGPL 2.1. See LICENSES/COPYING.LGPL file for the
 * full text of the license.
 *
 */

#ifndef GOB_DRAW_H
#define GOB_DRAW_H

#include "common/stack.h"
#include "gob/video.h"

namespace Common {
class WinResources;
}

namespace Gob {

#define RENDERFLAG_NOINVALIDATE      0x0001
#define RENDERFLAG_CAPTUREPUSH       0x0002
#define RENDERFLAG_COLLISIONS        0x0004
#define RENDERFLAG_CAPTUREPOP        0x0008
#define RENDERFLAG_USEDELTAS         0x0010
#define RENDERFLAG_BORDERHOTSPOTS    0x0040
#define RENDERFLAG_HASWINDOWS        0x0080
#define RENDERFLAG_NOBLITINVALIDATED 0x0200
#define RENDERFLAG_NOSUBTITLES       0x0400
#define RENDERFLAG_FROMSPLIT         0x0800
#define RENDERFLAG_DOUBLECOORDS      0x1000
#define RENDERFLAG_DOUBLEVIDEO       0x2000

class Draw {
public:
	static const int kSpriteCount    = 100;
	static const int kFontCount      =  16;
	static const int kFrontSurface   =  20;
	static const int kBackSurface    =  21;
	static const int kAnimSurface    =  22;
	static const int kCursorSurface  =  23;
	static const int kCaptureSurface =  30;

	struct FontToSprite {
		int8 sprite;
		int8 base;
		int8 width;
		int8 height;
		FontToSprite() : sprite(0), base(0), width(0), height(0) {}
	};

	struct fascinWin {
		int16 id;
		int16 left;
		int16 top;
		int16 width;
		int16 height;
		SurfacePtr savedSurface;
	};

	int16 _renderFlags;

	uint16 _fontIndex;
	int16 _spriteLeft;
	int16 _spriteTop;
	int16 _spriteRight;
	int16 _spriteBottom;
	int16 _destSpriteX;
	int16 _destSpriteY;
	int16 _backColor;
	int16 _frontColor;
	int16 _transparency;

	int16 _sourceSurface;
	int16 _destSurface;

	char _letterToPrint;
	const char *_textToPrint;
	char *_hotspotText;

	int16 _backDeltaX;
	int16 _backDeltaY;

	int16 _subtitleFont;
	int16 _subtitleColor;

	FontToSprite _fontToSprite[4];
	Font *_fonts[kFontCount];

	Common::Array<SurfacePtr> _spritesArray;

	int16 _invalidatedCount;
	int16 _invalidatedLefts[30];
	int16 _invalidatedTops[30];
	int16 _invalidatedRights[30];
	int16 _invalidatedBottoms[30];

	bool _noInvalidated;
	// Don't blit invalidated rects when in video mode 5 or 7
	bool _noInvalidated57;
	bool _paletteCleared;
	bool _applyPal;

	SurfacePtr _backSurface;
	SurfacePtr _frontSurface;

	int16 _unusedPalette1[18];
	int16 _unusedPalette2[16];
	Video::Color _vgaPalette[256];

	Common::Stack<Video::Color *> _paletteStack;

	// 0 (00b): No cursor
	// 1 (01b): Cursor would be on _backSurface
	// 2 (10b): Cursor would be on _frontSurface
	// 3 (11b): Cursor would be on _backSurface and _frontSurface
	uint8 _showCursor;
	int16 _cursorIndex;
	int16 _transparentCursor;
	uint32 _cursorTimeKey;

	int16 _cursorX;
	int16 _cursorY;
	int16 _cursorWidth;
	int16 _cursorHeight;

	int32 _cursorHotspotXVar;
	int32 _cursorHotspotYVar;

	int32 _cursorHotspotX;
	int32 _cursorHotspotY;

	SurfacePtr _cursorSprites;
	SurfacePtr _cursorSpritesBack;
	SurfacePtr _scummvmCursor;

	int16 _cursorAnim;
	int8 _cursorAnimLow[40];
	int8 _cursorAnimHigh[40];
	int8 _cursorAnimDelays[40];
	Common::String _cursorNames[40];
	Common::String _cursorName;
	bool _cursorDrawnFromScripts;

	int32 _cursorCount;

	int16 _palLoadData1[4];
	int16 _palLoadData2[4];

	// Coordinates adjustment mode
	// Some game were released for a higher resolution than the one they
	// were originally designed for. adjustCoords() is used to adjust
	//
	int16 _needAdjust;

	int16 _scrollOffsetY;
	int16 _scrollOffsetX;

	int16 _pattern;

	fascinWin _fascinWin[10];
	int16 _winMaxWidth;
	int16 _winMaxHeight;
	int16 _winCount;
	int16 _winVarArrayLeft;
	int16 _winVarArrayTop;
	int16 _winVarArrayWidth;
	int16 _winVarArrayHeight;
	int16 _winVarArrayStatus;
	int16 _winVarArrayLimitsX;
	int16 _winVarArrayLimitsY;


	void invalidateRect(int16 left, int16 top, int16 right, int16 bottom);
	void blitInvalidated();
	void setPalette();
	void clearPalette();

	void dirtiedRect(int16 surface, int16 left, int16 top, int16 right, int16 bottom);
	void dirtiedRect(SurfacePtr surface, int16 left, int16 top, int16 right, int16 bottom);

	void initSpriteSurf(int16 index, int16 width, int16 height, int16 flags, byte bpp = 0);
	void freeSprite(int16 index);
	void adjustCoords(char adjust, int16 *coord1, int16 *coord2);
	void adjustCoords(char adjust, uint16 *coord1, uint16 *coord2) {
		adjustCoords(adjust, (int16 *)coord1, (int16 *)coord2);
	}
	void resizeCursors(int16 width, int16 height, int16 count, bool transparency);
	int stringLength(const char *str, uint16 fontIndex);
	void printTextCentered(int16 id, int16 left, int16 top, int16 right,
			int16 bottom, const char *str, int16 fontIndex, int16 color);
	void oPlaytoons_sub_F_1B( uint16 id, int16 left, int16 top, int16 right, int16 bottom, char *paramStr, int16 var3, int16 var4, int16 shortId);

	int32 getSpriteRectSize(int16 index);
	void forceBlit(bool backwards = false);

	static const int16 _wobbleTable[360];
	void wobble(Surface &surfDesc);

	Font *loadFont(const char *path) const;
	bool loadFont(uint16 fontIndex, const char *path);

	virtual void initScreen() = 0;
	virtual void closeScreen() = 0;
	virtual void blitCursor() = 0;

	virtual void animateCursor(int16 cursor) = 0;
	virtual void printTotText(int16 id) = 0;
	virtual void spriteOperation(int16 operation) = 0;

	virtual int16 openWin(int16 id) { return 0; }
	virtual void closeWin(int16 id) {}
	virtual int16 handleCurWin() { return 0; }
	virtual int16 getWinFromCoord(int16 &dx, int16 &dy) { return -1; }
	virtual void moveWin(int16 id) {}
	virtual bool overlapWin(int16 idWin1, int16 idWin2) { return false; }
	virtual void closeAllWin() {}
	virtual void activeWin(int16 id) {}

	Draw(GobEngine *vm);
	virtual ~Draw();

protected:
	GobEngine *_vm;
};

class Draw_v1 : public Draw {
public:
	void initScreen() override;
	void closeScreen() override;
	void blitCursor() override;
	void animateCursor(int16 cursor) override;
	void printTotText(int16 id) override;
	void spriteOperation(int16 operation) override;

	Draw_v1(GobEngine *vm);
	~Draw_v1() override {}
};

class Draw_v2 : public Draw_v1 {
public:
	void initScreen() override;
	void closeScreen() override;
	void blitCursor() override;
	void animateCursor(int16 cursor) override;
	void printTotText(int16 id) override;
	void spriteOperation(int16 operation) override;

	Draw_v2(GobEngine *vm);
	~Draw_v2() override {}

private:
	uint8 _mayorWorkaroundStatus;

	void fixLittleRedStrings();
};

class Draw_Bargon: public Draw_v2 {
public:
	void initScreen() override;

	Draw_Bargon(GobEngine *vm);
	~Draw_Bargon() override {}
};

class Draw_Fascination: public Draw_v2 {
public:
	Draw_Fascination(GobEngine *vm);
	~Draw_Fascination() override {}
	void spriteOperation(int16 operation) override;

	void decompWin(int16 x, int16 y, SurfacePtr destPtr);
	void drawWin(int16 fct);
	void saveWin(int16 id);
	void restoreWin(int16 id);
	void handleWinBorder(int16 id);
	void drawWinTrace(int16 left, int16 top, int16 width, int16 height);

	int16 openWin(int16 id) override;
	void closeWin(int16 id) override;
	int16 getWinFromCoord(int16 &dx, int16 &dy) override;
	int16 handleCurWin() override;
	void activeWin(int16 id) override;
	void moveWin(int16 id) override;
	bool overlapWin(int16 idWin1, int16 idWin2) override;
	void closeAllWin() override;

};

class Draw_Playtoons: public Draw_v2 {
public:
	Draw_Playtoons(GobEngine *vm);
	~Draw_Playtoons() override {}
	void spriteOperation(int16 operation) override;
};


class Draw_v7 : public Draw_Playtoons {
public:
	Draw_v7(GobEngine *vm);
	~Draw_v7() override;

	void initScreen() override;
	void animateCursor(int16 cursor) override;


private:
	Common::WinResources *_cursors;

	bool loadCursorFile();
	bool loadCursorFromFile(Common::String filename);
};

// Draw operations

#define DRAW_BLITSURF	0
#define DRAW_PUTPIXEL	1
#define DRAW_FILLRECT	2
#define DRAW_DRAWLINE	3
#define DRAW_INVALIDATE	4
#define DRAW_LOADSPRITE	5
#define DRAW_PRINTTEXT	6
#define DRAW_DRAWBAR 7
#define DRAW_CLEARRECT	8
#define DRAW_FILLRECTABS 9
#define DRAW_DRAWLETTER	10

} // End of namespace Gob

#endif // GOB_DRAW_H
