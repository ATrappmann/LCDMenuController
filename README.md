# LCDMenuController

This is an Arduino library to control simple button controlled menus on an LCD display.

Written by Andreas Trappmann.
MIT license, check LICENSE for more information
All text above must be included in any redistribution

## Documentation

Menus can be defined with makros like
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

## Release Notes

Version 1.1 - 22.04.2020

	* Added Headlines

Version 1.0 - 09.02.2020

	* Initial Version
