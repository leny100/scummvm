/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Additional copyright for this file:
 * Copyright (C) 1995-1997 Presto Studios, Inc.
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

#include "common/events.h"
#include "common/system.h"

#include "gui/gui-manager.h"

#include "pegasus/cursor.h"
#include "pegasus/input.h"
#include "pegasus/pegasus.h"

namespace Common {
DECLARE_SINGLETON(Pegasus::InputDeviceManager);
}

namespace Pegasus {

InputDeviceManager::InputDeviceManager() {
	// Set all keys to "not down"
	for (uint i = 0; i < ARRAYSIZE(_keysDown); i++) {
		_keysDown[i] = false;
	}

	g_system->getEventManager()->getEventDispatcher()->registerObserver(this, 2, false);
	_lastRawBits = kAllUpBits;
	_AKeyWasDown = false;
}

InputDeviceManager::~InputDeviceManager() {
	g_system->getEventManager()->getEventDispatcher()->unregisterObserver(this);
}

void InputDeviceManager::getInput(Input &input, const InputBits filter) {
	// Poll for events, but ignore most of them!
	// We'll pick the rest up in notifyEvent()
	// We do that so that any pollEvent() call can update the variables
	// (ie. if one uses enter to access the restore menu, we never receive
	// the key up event, which leads to bad things)
	// This is to closely emulate what the GetKeys() function did on Mac OS
	InputBits currentBits = 0;

	Common::Event event;
	while (g_system->getEventManager()->pollEvent(event))
		;

	// Now fill in the rest of the bitfield
	if (_keysDown[kPegasusActionUp])
		currentBits |= (kRawButtonDown << kUpButtonShift);

	if (_keysDown[kPegasusActionDown])
		currentBits |= (kRawButtonDown << kDownButtonShift);

	if (_keysDown[kPegasusActionLeft])
		currentBits |= (kRawButtonDown << kLeftButtonShift);

	if (_keysDown[kPegasusActionRight])
		currentBits |= (kRawButtonDown << kRightButtonShift);

	if (_keysDown[kPegasusActionInteract])
		currentBits |= (kRawButtonDown << kTwoButtonShift);

	if (_keysDown[kPegasusActionToggleCenterDisplay])
		currentBits |= (kRawButtonDown << kThreeButtonShift);

	if (_keysDown[kPegasusActionShowInfoScreen])
		currentBits |= (kRawButtonDown << kFourButtonShift);

	if (_keysDown[kPegasusActionShowPauseMenu])
		currentBits |= (kRawButtonDown << kMod3ButtonShift);

	if (_keysDown[kPegasusActionShowInventory])
		currentBits |= (kRawButtonDown << kLeftFireButtonShift);

	if (_keysDown[kPegasusActionShowBiochip])
		currentBits |= (kRawButtonDown << kRightFireButtonShift);

	if (g_vm->isDVD()) {
		if (_keysDown[kPegasusActionToggleChattyAI] && !_AKeyWasDown) {
			g_vm->requestToggle();
			_AKeyWasDown = true;
		} else if (!_keysDown[kPegasusActionToggleChattyAI])
			_AKeyWasDown = false;
	}

	// Update mouse button state
	// Note that we don't use EVENT_LBUTTONUP/EVENT_LBUTTONDOWN because
	// they do not show if the button is being held down. We're treating
	// both mouse buttons as the same for ease of use.
	if (g_system->getEventManager()->getButtonState() != 0)
		currentBits |= (kRawButtonDown << kTwoButtonShift);

	// Update the mouse position too
	input.setInputLocation(g_system->getEventManager()->getMousePos());

	// Set the outgoing bits
	InputBits filteredBits = currentBits & filter;
	input.setInputBits((filteredBits & kAllButtonDownBits) | (filteredBits & _lastRawBits & kAllAutoBits));

	// Update the last bits
	_lastRawBits = currentBits;

	// WORKAROUND: The original had this in currentBits, but then
	// pressing alt would count as an event (and mess up someone
	// trying to do alt+enter or something). Since it's only used
	// as an easter egg, I'm just going to handle it as a separate
	// bool value.
	input.setAltDown(_keysDown[kPegasusActionEnableEasterEgg]);
}

// Wait until the input device stops returning input allowed by filter...
void InputDeviceManager::waitInput(const InputBits filter) {
	if (filter != 0) {
		for (;;) {
			Input input;
			getInput(input, filter);
			if (!input.anyInput())
				break;
		}
	}
}

bool InputDeviceManager::notifyEvent(const Common::Event &event) {
	if (GUI::GuiManager::instance().isActive()) {
		// For some reason, the engine hooks in the event system using an EventObserver.
		// So we need to explicitly ignore events that happen while ScummVM's GUI is open.
		return false;
	}

	// We're mapping from ScummVM events to pegasus events, which
	// are based on pippin events.

	switch (event.type) {
	case Common::EVENT_CUSTOM_ENGINE_ACTION_START:
		switch ((PegasusAction)event.customType) {
		case kPegasusActionSaveGameState:
			g_vm->requestSave();
			break;
		case kPegasusActionLoadGameState:
			g_vm->requestLoad();
			break;
		default:
			// Otherwise, set the action to down if we have it
			if (event.customType != kPegasusActionNone && event.customType < kPegasusActionCount)
				_keysDown[event.customType] = true;
			break;
		}
		break;
	case Common::EVENT_CUSTOM_ENGINE_ACTION_END:
		// Set the key to up if we have it
		if (event.customType != kPegasusActionNone && event.customType < kPegasusActionCount)
			_keysDown[event.customType] = false;
		break;
	default:
		break;
	}

	return false;
}

void InputDeviceManager::pumpEvents() {
	bool saveAllowed = g_vm->swapSaveAllowed(false);
	bool openAllowed = g_vm->swapLoadAllowed(false);

	// Just poll for events. notifyEvent() will pick up on them.
	Common::Event event;
	while (g_system->getEventManager()->pollEvent(event))
		;

	g_vm->swapSaveAllowed(saveAllowed);
	g_vm->swapLoadAllowed(openAllowed);
}

int operator==(const Input &arg1, const Input &arg2) {
	return arg1._inputState == arg2._inputState;
}

int operator!=(const Input &arg1, const Input &arg2) {
	return !operator==(arg1, arg2);
}

InputHandler *InputHandler::_inputHandler = nullptr;
bool InputHandler::_invalHotspots = false;
InputBits InputHandler::_lastFilter = kFilterNoInput;

InputHandler *InputHandler::setInputHandler(InputHandler *currentHandler) {
	InputHandler *result = nullptr;

	if (_inputHandler != currentHandler && (!_inputHandler || _inputHandler->releaseInputFocus())) {
		result = _inputHandler;
		_inputHandler = currentHandler;
		if (_inputHandler)
			_inputHandler->grabInputFocus();
	}

	return result;
}

void InputHandler::pollForInput() {
	if (_inputHandler) {
		Input input;
		Hotspot *cursorSpot = nullptr;

		InputHandler::getInput(input, cursorSpot);
		if (_inputHandler->isClickInput(input, cursorSpot))
			_inputHandler->clickInHotspot(input, cursorSpot);
		else
			_inputHandler->handleInput(input, cursorSpot);
	}
}

void InputHandler::getInput(Input &input, Hotspot *&cursorSpot) {
	Cursor *cursor = g_vm->_cursor;

	if (_inputHandler)
		_lastFilter = _inputHandler->getInputFilter();
	else
		_lastFilter = kFilterAllInput;

	InputDevice.getInput(input, _lastFilter);

	if (_inputHandler && _inputHandler->wantsCursor() && (_lastFilter & _inputHandler->getClickFilter()) != 0) {
		if (cursor->isVisible()) {
			g_allHotspots.deactivateAllHotspots();
			_inputHandler->activateHotspots();

			Common::Point cursorLocation;
			cursor->getCursorLocation(cursorLocation);
			cursorSpot = g_allHotspots.findHotspot(cursorLocation);

			if (_inputHandler)
				_inputHandler->updateCursor(cursorLocation, cursorSpot);
		} else {
			cursor->hideUntilMoved();
		}
	} else {
		cursor->hide();
	}
}

void InputHandler::readInputDevice(Input &input) {
	InputDevice.getInput(input, kFilterAllInput);
}

InputHandler::InputHandler(InputHandler *nextHandler) {
	_nextHandler = nextHandler;
	allowInput(true);
}

InputHandler::~InputHandler() {
	if (_inputHandler == this)
		setInputHandler(_nextHandler);
}

void InputHandler::handleInput(const Input &input, const Hotspot *cursorSpot) {
	if (_nextHandler)
		_nextHandler->handleInput(input, cursorSpot);
}

void InputHandler::clickInHotspot(const Input &input, const Hotspot *cursorSpot) {
	if (_nextHandler)
		_nextHandler->clickInHotspot(input, cursorSpot);
}

bool InputHandler::isClickInput(const Input &input, const Hotspot *cursorSpot) {
	if (_nextHandler)
		return _nextHandler->isClickInput(input, cursorSpot);

	return false;
}

void InputHandler::activateHotspots() {
	if (_nextHandler)
		_nextHandler->activateHotspots();
}

InputBits InputHandler::getInputFilter() {
	if (_allowInput) {
		if (_nextHandler)
			return _nextHandler->getInputFilter();
		else
			return kFilterAllInput;
	}

	return kFilterNoInput;
}

InputBits InputHandler::getClickFilter() {
	if (_allowInput && _nextHandler)
		return _nextHandler->getClickFilter();

	return kFilterNoInput;
}

void InputHandler::updateCursor(const Common::Point &cursorLocation, const Hotspot *cursorSpot) {
	if (_nextHandler)
		_nextHandler->updateCursor(cursorLocation, cursorSpot);
}

bool InputHandler::wantsCursor() {
	if (_allowInput) {
		if (_nextHandler)
			return _nextHandler->wantsCursor();
		else
			return true;
	}

	return false;
}

Tracker *Tracker::_currentTracker = nullptr;

void Tracker::handleInput(const Input &input, const Hotspot *) {
	if (stopTrackingInput(input))
		stopTracking(input);
	else if (isTracking())
		continueTracking(input);
}

void Tracker::startTracking(const Input &) {
	if (!isTracking()) {
		_savedHandler = InputHandler::setInputHandler(this);
		_currentTracker = this;
	}
}

void Tracker::stopTracking(const Input &) {
	if (isTracking()) {
		_currentTracker = nullptr;
		InputHandler::setInputHandler(_savedHandler);
	}
}

bool Tracker::isClickInput(const Input &input, const Hotspot *hotspot) {
	return !isTracking() && InputHandler::isClickInput(input, hotspot);
}

} // End of namespace Pegasus
