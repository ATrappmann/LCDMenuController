// NAME: LCDMenuController.h
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
#ifndef LCDMENUCONTROLLER_H
#define LCDMENUCONTROLLER_H

#include <LiquidCrystal_MCP23017_I2C.h>
#include <Bounce2.h>
#include "Stack.h"

class LCDMenuController;
typedef void * (*menuFuncPtr)();

/*
 * Menu - structure to hold information about a single menu line to display on an LCD.
 *        An array of Menu[] holds the entire menu. The array is terminated by NULL.
 * 		  Either func or submenu has to be NULL in each entry, depending on its function.
 */
struct Menu {
  char       	*name;			// name of menu entry, text to show on display
  menuFuncPtr  (*func)();		// function to call, when menu entry is selected
  Menu       	*submenu;		// submenu to show, when menu entry is selected
};

class LCDMenuController {
private:
  LiquidCrystal_MCP23017_I2C *display;
  uint8_t displayWidth;
  uint8_t displayHeight;
  
  Bounce *nextButton;
  Bounce *prevButton;
  Bounce *selectButton;
  Bounce *backButton;

  Menu  *menu;
  int   numEntries;
  int   topEntryNo;
  int   currentEntryNo;

  Stack<Menu> *menuStack;
  menuFuncPtr (*contFunc)(const LCDMenuController *);

  bool  started;
  
public:
  LCDMenuController(const LiquidCrystal_MCP23017_I2C *lcd, const uint8_t lcdCols, const uint8_t lcdRows,
                 const int nextButtonPin, const int prevButtonPin, 
                 const int selectButtonPin, const int backButtonPin);

  ~LCDMenuController();
  
#ifdef DEBUG
  static bool validate(const Menu menu[]);
#endif

  void init();
  void begin(const Menu menu[]);
  void navigate();

  bool isNextButtonPressed();
  bool isPrevButtonPressed();
  bool isSelectButtonPressed();
  bool isBackButtonPressed();
  
  inline LiquidCrystal_MCP23017_I2C *getDisplay() { return display; }
  
private:
  void showMenu(const Menu menu[]);
  int  calcMaxMenuDepth(const Menu menu[]);
  
};

#endif /* LCDMENUCONTROLLER_H */
