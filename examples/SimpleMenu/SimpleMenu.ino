// NAME: SimpleMenu.ino
//
// DESC:
//
// DEPENDENCIES:
// * MCP23017_LiquidCrystal_I2C library from https://github.com/ATrappmann/LiquidCrystal_MCP23017_I2C
// * Bounce2 library from https://github.com/thomasfredericks/Bounce2
//
#include "LCDMenuController.h"

#define LCD_RS_PIN  MCP23017_PA7
#define LCD_RW_PIN  MCP23017_PA6
#define LCD_EN_PIN  MCP23017_PA5
#define LCD_D0_PIN  MCP23017_PB0
#define LCD_D1_PIN  MCP23017_PB1
#define LCD_D2_PIN  MCP23017_PB2
#define LCD_D3_PIN  MCP23017_PB3
#define LCD_D4_PIN  MCP23017_PB4
#define LCD_D5_PIN  MCP23017_PB5
#define LCD_D6_PIN  MCP23017_PB6
#define LCD_D7_PIN  MCP23017_PB7
#define LCD_BACKLIGHT_PIN MCP23017_PA1
#define LCD_I2C_ADDR  0x20

#define UP_PIN      5
#define DOWN_PIN    6
#define SELECT_PIN  7
#define BACK_PIN    8

menuFuncPtr sub31Func() { Serial.println("sub31Func"); return NULL; }
menuFuncPtr sub1Func()  { Serial.println("sub1Func"); return NULL; }
menuFuncPtr contFunc()  { Serial.println("contFunc"); return contFunc; }
menuFuncPtr sub2Func()  { Serial.println("sub2Func"); return contFunc; }
menuFuncPtr mainFunc2() { Serial.println("mainFunc2"); return NULL; }

Menu sub31Menu[] = {
  { "Sub31", sub31Func, NULL },
  NULL
};

Menu sub1Menu[] = {
  { "Sub1", sub1Func, NULL },
  { "Sub2", sub2Func, NULL },
  NULL
};

Menu sub3Menu[] = {
  { "Sub3", NULL, sub31Menu },
  NULL
};

Menu mainMenu[] = {
  { "Entry1", NULL, sub1Menu },
  { "Entry2", mainFunc2, NULL },
  { "Entry3", NULL, sub3Menu },
  NULL
};

LiquidCrystal_MCP23017_I2C lcd = LiquidCrystal_MCP23017_I2C(LCD_I2C_ADDR, LCD_RS_PIN, LCD_RW_PIN, LCD_EN_PIN, LCD_BACKLIGHT_PIN,
                               LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN,
                               LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

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
