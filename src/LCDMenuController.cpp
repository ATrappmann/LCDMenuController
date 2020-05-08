// NAME: LCDMenuController.cpp
//
// DESC: LCDMenuController is an Arduino library to control menu selection on a Liquid Crystal Display.
//	     Menu navigation is controlled by 4 push button switches.
//
// VERSION: This is Version 1.1 of the library.
//
// SOURCE: Code is available at https://github.com/ATrappmann/LCDMenuController
//
// DEPENDENCIES:
// * LiquidCrystal_MCP23017_I2C library from https://github.com/ATrappmann/LiquidCrystal_MCP23017_I2C
// * Bounce2 library from https://github.com/thomasfredericks/Bounce2
//
// MIT License
//
// Copyright (c) 2020 Andreas Trappmann
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#define DEBUG 1
#include "Debug.h"

#include "LCDMenuController.h"

uint8_t arrow[8] = {  // used for submenus
  B00000,
  B00100,
  B00010,
  B11111,
  B11111,
  B00010,
  B00100,
  B00000
};
#define SUBMENU_MARKER  1

LCDMenuController::LCDMenuController(const LiquidCrystal_MCP23017_I2C *lcd, const uint8_t lcdCols, const uint8_t lcdRows,
                               const int nextButtonPin, const int prevButtonPin,
                               const int selectButtonPin, const int backButtonPin) {
  display = lcd;
  displayWidth = lcdCols;
  displayHeight = lcdRows;

  nextButton = new Bounce();
  nextButton->attach(nextButtonPin, INPUT_PULLUP);
  nextButton->interval(10);  // ms

  prevButton = new Bounce();
  prevButton->attach(prevButtonPin, INPUT_PULLUP);
  prevButton->interval(10);  // ms

  selectButton = new Bounce();
  selectButton->attach(selectButtonPin, INPUT_PULLUP);
  selectButton->interval(10);  // ms

  backButton = new Bounce();
  backButton->attach(backButtonPin, INPUT_PULLUP);
  backButton->interval(10);  // ms

  menuStack = NULL;
  started = false;

  enterFunc = NULL;
  exitFunc = NULL;
}

LCDMenuController::~LCDMenuController() {
  delete nextButton; nextButton = NULL;
  delete prevButton; prevButton = NULL;
  delete selectButton; selectButton = NULL;
  delete backButton; backButton = NULL;

  if (NULL != menuStack) { delete menuStack; menuStack = NULL; }
}

#ifdef DEBUG
bool LCDMenuController::validate(const Menu menu[]) {
  if (NULL == menu) return false;
  int i = 0;
  while (NULL != menu[i].name) {
    if (NULL == menu[i].func) {
      if (NULL == menu[i].submenu) return false;
      if (!validate(menu[i].submenu)) return false;
    }
    i++;
  }
  return true;
}
#endif

int LCDMenuController::calcMaxMenuDepth(const Menu menu[]) {
  if (NULL == menu) return 0;
  int maxDepth = 0;
  int i=0;
  while (NULL != menu[i].name) {
    if (NULL != menu[i].submenu) {
      int depth = calcMaxMenuDepth(menu[i].submenu);
      if (depth > maxDepth) maxDepth = depth;
    }
    i++;
  }
  return maxDepth + 1;
}

void LCDMenuController::init() {
  SEROUT(F("InitMenu\n"));
  display->begin(displayWidth, displayHeight);
  display->createChar(SUBMENU_MARKER, arrow);
  display->backlight();
}

void LCDMenuController::begin(const Menu menu[]) {
  SEROUT(F("BeginMenu=") << (uint16_t)menu << LF);
  if (NULL == menu) return;

  int maxMenuDepth = calcMaxMenuDepth(menu);
  menuStack = new Stack<Menu>(maxMenuDepth);
  SEROUT("menuDepth: " << maxMenuDepth << ", menuStack=" << ((uint16_t)menuStack) << LF);

#ifdef DEBUG
  int i = 0;
  while (NULL != menu[i].name) {
    if (NULL != menu[i].name) Serial.print(menu[i].name); else Serial.print("NULL");
    if (NULL != menu[i].func) { Serial.print(" "); Serial.print((uint16_t)menu[i].func); } else Serial.print(" NULL");
    if (NULL != menu[i].submenu) Serial.print(" SUBMENU"); else Serial.print(" NULL");
    Serial.println();
    i++;
  }
#endif

  this->menu = NULL;
  showMenu(menu);
  started = true;
}

void LCDMenuController::showMenu(const Menu menu[]) {
  SEROUT(F("-----------------\n"));
  SEROUT(F("ShowMenu=") << (uint16_t)menu << LF);
  if (NULL == menu) return;

  if (menu != this->menu) {	// new (sub-)menu
	  this->menu = menu;
    this->contFunc = NULL;

	  numEntries = 0;
	  currentEntryNo = 0;
	  while (NULL != menu[numEntries].name) {
	    if ((0 == currentEntryNo) && ((NULL != menu[numEntries].func) || (NULL != menu[numEntries].submenu))) {
		    currentEntryNo = numEntries;
	    }
	    numEntries++;
	  }
	  topEntryNo = 0;
  }

  int startNo;
  if (currentEntryNo - topEntryNo < 0) {
    startNo = currentEntryNo;
    topEntryNo = currentEntryNo;
  }
  else if (currentEntryNo - topEntryNo < displayHeight) {
    startNo = topEntryNo;
  }
  else {
    startNo = currentEntryNo - displayHeight + 1;
    topEntryNo = startNo;
  }

  SEROUT(F("Current=") << currentEntryNo << F(", Top=") << topEntryNo << F(", Start=") << startNo << LF);

  display->clear();
  for (int i=startNo; i<numEntries && i<startNo+displayHeight; i++) {
    SEROUT(i-startNo << ": ");
    display->setCursor(0, i-startNo);
    if (currentEntryNo == i) {
      if (NULL != menu[i].submenu) {  // submenu
        SEROUT('>');
        display->write(SUBMENU_MARKER);
      }
      else { // function calling
        SEROUT('*');
        display->write('*');
      }
    } else if ((NULL != menu[i].func) || (NULL != menu[i].submenu)) {	// print space, if not an headline
      SEROUT(' ');
      display->write(' ');
    }
    SEROUT(menu[i].name << LF);
    display->print(menu[i].name);
  }
}

void LCDMenuController::navigate() {
  if (!started) return;

  backButton->update();
  if (backButton->fell()) {
    SEROUT("BackButton\n");
    contFunc = NULL;
    if (NULL != exitFunc) {
      exitFunc(this);
    }
    Menu *prevMenu = menuStack->pop();
    SEROUT("pop: " << ((uint16_t)prevMenu) << LF);
    if (NULL != prevMenu) {
      SEROUT("backPrevMenu\n");
      showMenu(prevMenu);
    }
    return;
  }

  if (NULL != contFunc) {
	  SEROUT("call func : " << (uint16_t)contFunc << LF);
    menuFuncPtr callback = contFunc(this);	// call given function
	  SEROUT("return val: " << (uint16_t)callback << LF);
    if (callback != contFunc) {
      contFunc = callback;
	    if (NULL == callback)	{
        Menu *prevMenu = menuStack->pop();
        SEROUT("pop: " << ((uint16_t)prevMenu) << LF);
        return showMenu(prevMenu);
      }
    }
	  else return;	// continue calling the given function
  }

  nextButton->update();
  if (nextButton->fell()) {
	currentEntryNo++;
	if ((NULL == menu[currentEntryNo].func) && (NULL == menu[currentEntryNo].submenu)) currentEntryNo++;
    if (currentEntryNo >= numEntries) {
      currentEntryNo = numEntries - 1;
    }
    SEROUT(F("NextButton: #E=") << numEntries << F(", current=") << currentEntryNo << LF);
    showMenu(menu);
    return;
  }

  prevButton->update();
  if (prevButton->fell()) {
	currentEntryNo--;
	if ((NULL == menu[currentEntryNo].func) && (NULL == menu[currentEntryNo].submenu)) {
	  topEntryNo--;
	  if (topEntryNo < 0) topEntryNo = 0;
	  currentEntryNo++;
	}

    if (currentEntryNo < 0) {
      currentEntryNo = 0;
    }
    SEROUT(F("PrevButton: #E=") << numEntries << F(", current=") << currentEntryNo << LF);
    showMenu(menu);
    return;
  }

  selectButton->update();
  if (selectButton->fell()) {
    SEROUT(F("SelectButton - push: ") << ((uint16_t)menu) << LF);
    if (NULL != enterFunc) {
      enterFunc(this);
    }
    menuStack->push(menu);
    if (NULL != menu[currentEntryNo].func) {
      contFunc = menu[currentEntryNo].func;
      SEROUT(F("contFunc: ") << ((uint16_t)contFunc) << LF);
    }
    else if (NULL != menu[currentEntryNo].submenu) {
      SEROUT(F("showSubMenu: ") << ((uint16_t)menu[currentEntryNo].submenu) << LF);
      showMenu(menu[currentEntryNo].submenu);
    }
    return;
  }
}

void LCDMenuController::callOnEnterMenu(menuFuncPtr callback) {
  enterFunc = callback;
}

void LCDMenuController::callOnExitMenu(menuFuncPtr callback) {
  exitFunc = callback;
}

bool LCDMenuController::isNextButtonPressed() {
	nextButton->update();
	return nextButton->fell();
}

bool LCDMenuController::isPrevButtonPressed() {
	prevButton->update();
	return prevButton->fell();
}

bool LCDMenuController::isSelectButtonPressed() {
	selectButton->update();
	return selectButton->fell();
}

bool LCDMenuController::isBackButtonPressed() {
	backButton->update();
	return backButton->fell();
}
