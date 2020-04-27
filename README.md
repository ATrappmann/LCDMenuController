# LCDMenuController

**LCDMenuController** is an Arduino library to control simple button controlled
menus on a LiquidCrystal Display.

## Documentation

Menus can be defined with makros like:
```
menuFuncPtr func1(const LCDMenuController *controller) {
  // do something useful

  // return NULL, when finished
  // return this, if function should be called again
  return NULL;  // done
}
menuFuncPtr func2(const LCDMenuController *controller) {
  // do something useful

  // return NULL, when finished
  // return this, if function should be called again
  return func2; // call again until BACK button is pressed
}
menuFuncPtr foo(const LCDMenuController *controller) {
  controller->getDisplay()->print("foo");
  delay(1000);
  return NULL;  // done
}

Menu subMenu[] = {
  FUNCCALL("Func 1", func1),
  FUNCCALL("Func 2", func2),
  ENDOFMENU()
};

Menu mainMenu[] = {
  HEADLINE("Main Menu"),
  FUNCCALL("Do Something", foo),
  SUBMENU ("Sub Menu", subMenu),
  ENDOFMENU()
};
```

Define a **LCDMenuController** with its constructor and pass the LCD used,
give the display size (16x2) and the pin numbers of the required push button
switches like here:
```
LCDMenuController controller = LCDMenuController(&lcd, 16, 2, DOWN_PIN, UP_PIN, SELECT_PIN, BACK_PIN);
```
The buttons are active LOW and can be connected directly to an Arduino pin. An
internal pullup resistor will be activated.

To use the **LCDMenuController**, add the following lines to your code:
```
void setup() {
  // ...
  controller.init();
  controller.begin(mainMenu);
}

void loop() {
  controller.navigate();
  // ...
}
```

## Copyright
**LCDMenuController** is written by Andreas Trappmann from
[Trappmann-Robotics.de](https://www.trappmann-robotics.de/). It is published
under the MIT license, check LICENSE for more information.
All text above must be included in any redistribution

## Release Notes

Version 1.2.1 - 27.04.2020

	* Simplyfied the example.
	
Version 1.2 - 24.04.2020

  * Added different Markers for sub-menus and function calls
  * Simplified the `SimpleMenu` example Sketch with use of makros

Version 1.1 - 22.04.2020

	* Added Headlines

Version 1.0 - 09.02.2020

	* Initial Version
