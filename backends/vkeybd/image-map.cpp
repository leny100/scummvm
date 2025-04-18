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

#include "common/scummsys.h"

#ifdef ENABLE_VKEYBD

#include "common/textconsole.h"

#include "backends/vkeybd/image-map.h"
#include "backends/vkeybd/polygon.h"

namespace Common {

ImageMap::~ImageMap() {
	removeAllAreas();
}

Polygon *ImageMap::createArea(const String &id) {
	if (_areas.contains(id)) {
		warning("Image map already contains an area with target of '%s'", id.c_str());
		return 0;
	}
	Polygon *p = new Polygon();
	_areas[id] = p;
	return p;
}

void ImageMap::removeArea(const String &id) {
	if (!_areas.contains(id))
		return;
	delete _areas[id];
	_areas.erase(id);
}

void ImageMap::removeAllAreas() {
	for (auto &area : _areas) {
		delete area._value;
	}
	_areas.clear();
}

String ImageMap::findMapArea(int16 x, int16 y) {
	for (auto &area : _areas) {
		if (area._value->contains(x, y))
			return area._key;
	}

	return String();
}


} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
