// NAME: SimpleMenu.ino
//
// DESC:
//
// DEPENDENCIES:
// * MCP23017_LiquidCrystal_I2C library from https://github.com/ATrappmann/LiquidCrystal_MCP23017_I2C
// * Bounce2 library from https://github.com/thomasfredericks/Bounce2
//
#include "LCDMenuController.h"

#define LCD_I2C_ADDR  0x20

#define UP_PIN      5
#define DOWN_PIN    6
#define SELECT_PIN  7
#define BACK_PIN    8

menuFuncPtr sub31Func(const LCDMenuController *) { Serial.println("sub31Func"); return NULL; }
menuFuncPtr sub1Func(const LCDMenuController *)  { Serial.println("sub1Func"); return NULL; }

// continues callback function until BACK is pressed
menuFuncPtr contFunc(const LCDMenuController *)  { Serial.println("contFunc"); return contFunc; }
menuFuncPtr sub2Func(const LCDMenuController *)  { Serial.println("sub2Func"); return contFunc; }

menuFuncPtr mainFunc2(const LCDMenuController *) { Serial.println("mainFunc2"); return NULL; }

Menu sub31Menu[] = {
  FUNCCALL("Sub3.1", sub31Func),
  ENDOFMENU()
};

Menu sub1Menu[] = {
  FUNCCALL("Sub1", sub1Func),
  FUNCCALL("Sub2", sub2Func),
  ENDOFMENU()
};

Menu sub3Menu[] = {
  SUBMENU("Sub3", sub31Menu),
  ENDOFMENU()
};


Menu mainMenu[] = {
  HEADLINE("Headline"),
  SUBMENU ("Entry1", sub1Menu),
  FUNCCALL("Entry2", mainFunc2),
  SUBMENU ("Entry3", sub3Menu),
  ENDOFMENU()
};

LCD lcd = LCD(LCD_I2C_ADDR, 16, 2);
LCDMenuController controller = LCDMenuController(&lcd, 16, 2, DOWN_PIN, UP_PIN, SELECT_PIN, BACK_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("MenuLibrary uploaded " __DATE__ " " __TIME__);

#ifdef DEBUG
  if (!controller.validate(mainMenu)) {
    Serial.println("ERROR: Validating 'mainMenu' failed!");
    Serial.println("Exit.");
    Serial.flush();
    Serial.end();
    exit(-1);
  }
#endif

  controller.init();
  controller.begin(mainMenu);
}

void loop() {
  controller.navigate();
}
