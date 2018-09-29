# mLibs
**mLibs** ('m' for 'micro') is a library of reusable components for Arduino development.

These Arduino software libraries are provided as [Visual Studio](https://visualstudio.microsoft.com/free-developer-offers/) projects using the [Visual Micro](https://www.visualmicro.com/) extension. There are free versions of these tools available. They are highly recommended as significantly more powerful than the Arduino IDE. The source files can probably also be built using the Arduino IDE, but no support is provided for using other tools.

The libraries included:
* **Applet** - A uniform mechanism for organizing and communicating with hardware components
* **FMBlue** - An Applet for Bluetooth LE interfacing.
* **FMDebug** - An Applet for Serial interfacing and debugging enhancements.
* **FMIvalometer** - An Applet implementing the Intervalometer hardware functions.
* **FMStepper** - A generic Stepper Motor control Applet used to control the Slide and Pan functions.
* **FMTime** - A class to provide date/time functionality.
* **Metronome** - A simple class to provide polled Timer functions.

The Applets are designed to communicate with corresponding C# "Elements" found in the Libs\Elements project in a framework supported by other projects in the Libs folder.
See the Slider project for an example of using the Element framework for building cross-platform Android and Universal Windows Platform (UWP) applications
that communicate with Arduino implementations using the Applet framework.

Place the mLibs projects in sibling subdirectories with your main projects in your file system to insure proper compilation.

These projects have dependencies on several libraries, including the Arduino libraries of course:
* FMBlue
	* [**Adafruit nRF51 BLE Library**](https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/installing-ble-library)
* FMStepper
	* [**AccelStepper library**](http://www.airspayce.com/mikem/arduino/AccelStepper/).
