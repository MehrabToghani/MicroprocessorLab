# Microprocessor Laboratory

Exercises for the microprocessor lab course

## **Attributes**

- AVR GCC compiler
- ATmega16 Microcontroller
- C programming language
- VSCode IDE
- Proteus 8.9

## **Installation**

_`Simulator:`_

- Proteus 8.9

_`Code:`_

- install vscode
- install [AVR 8-bit Toolchain](https://www.microchip.com/en-us/development-tools-tools-and-software/gcc-compilers-avr-and-arm)
- install [GnuWin32.Make](https://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81.exe)

## **How to compile code**

- add _GnuWin32_ bin folder path to **Path** in **Environment Variable**.
- add _AVR 8-bit Toolchain_ bin folder path to **Path** in **Environment Variable**.
- change includePath and compilerPath in _c_cpp_properties.json_ file if necessary.
- open terminal and execute this command in Makefile path:

      make
- generates _.elf_, _.hex_ and _.o_ files for simulator.

## **License**

[MIT](LICENSE)
