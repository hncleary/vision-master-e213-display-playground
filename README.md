# Heltec ESP32 Series Firmware / Display Playground

## Firmware Examples

This repository contains several example firmware sketches for the Heltec E213 e-ink display:

### `firmwares/heltec_provided_sample/heltec_provided_sample.ino`
A comprehensive demonstration program that cycles through various display capabilities. It showcases multiple demo modes including font face rendering at different sizes, text flow and wrapping, text alignment (left, center, right), geometric shapes (rectangles, lines, circles), and image rendering. The program automatically switches between demo modes every 15 seconds, providing a complete overview of the display's graphical and text rendering features.

### `firmwares/rotating_cube/rotating_cube.ino`
Demo that renders a wireframe cube that continuously rotates on three axes.

### `firmwares/stick_figure/stick_figure.ino`
A simple graphics example that draws a static stick figure on the display. Draws the figure once during setup and maintains the static image.

### `firmwares/kanye_quote/kanye_quote.ino`
Fetches random Kanye West quotes from the kanye.rest API and displays them on the e-ink screen with automatic word wrapping. Refreshes the quote every 60 seconds.



## Heltec ESP32 Series Quick Start

**Source:** [Heltec ESP32 Series Quick Start Documentation](https://docs.heltec.org/en/node/esp32/esp32_general_docs/quick_start.html)

_This article describes the installation of the Heltec ESP32 series development framework and library._

## Summary

Heltec's Arduino-based ESP32 development environment consists of two parts:

* **Heltec ESP32 Development Framework**: Fully adapted from the official Espressif library, it includes board definitions, core functionalities, and examples for the ESP32 (such as Wi-Fi, Bluetooth, and external drivers etc.).
* **Heltec Extended Example Library**: This library depends on the aforementioned ESP32 Development Framework. It includes examples for LoRa/LoRaWAN, display demonstrations, GPS, sensors, and factory test programs for Heltec products etc..

> **Warning:** Framework v3.0.0 and Library v2.0.0 are updated together, Older frameworks and libraries are no longer applicable to the new ones.

> **Tip:** You can also use the official Espressif Arduino framework as a replacement for the Heltec ESP32 Development Framework. Just ensure that the corresponding version is compatible.

---

## Preparation

### Hardware

* Heltec ESP32 Series Node
* High quality USB_Type_C cable

> **Tip:** Some cables only charge and do not transfer data, so you need to avoid this when uploading code.

### Software

* **USB driver**: CP210x Driver.
* Arduino IDE.
* GIT

---

## Installing development framework and library

**There are three methods to install the development framework, choose one of they:**

* Via Arduino IDE
* Via Git
* Via Local File

---

### Via Arduino IDE

1. Open Arduino IDE, and click `File`→`Preferences`.

2. **Input the last ESP32 package URL:** https://resource.heltec.cn/download/package_heltec_esp32_index.json

3. Click on the `Boards Manager` icon on the left side, and enter "**heltec esp32**" in the search box that pops up, then select the latest version and click `INSTALL`.

4. Install Heltec Esp32 Extended library  
   Click on the `Library Manager` icon on the left side, search for "HELTEC ESP32", select the latest version and install

---

### Via Git

> **Note:** It is recommended to follow the path and file name as described below as much as possible to avoid unnecessary trouble.

#### Install Heltec ESP32 Development Framework

Check the following links for your operating system, the specific operation steps are detailed in the link:

* For Windows
* For MacOS
* For Linux
  * Debian/Ubuntu
  * Fedora
  * OpenSUSE

> **Warning:** After installation, please execute "get.exe" under the path of "Arduino\hardware\heltec\esp32\tools" to obtain the latest compilation tool.

#### Install Heltec Extended Example Library

The above is the framework installation. If you need to install the Heltec ESP32 extended library, you can refer to this link:

* Heltec ESP32 Series Library  
  Follow the instructions in the `Readme.MD` to install.

---

### Via Local File

> **Note:** It is recommended to follow the path and file name as described below as much as possible to avoid unnecessary trouble.

1. Download the development environment. Download Framework

2. Open Arduino IDE, and click `File`→`Preferences`.

3. Go to the folder in the red box.

4. Create a new "hardware" folder in the Arduino folder. If there is already a "hardware" folder, you don't need to create a new one.

5. Create a new "heltec" folder in "hardware" folder.

6. Go to the "heltec" folder and extract "esp32" into this folder.

7. Go to the "heltec" folder, refer to the figure below to confirm whether the path in the red box is correct.

8. Restart the Arduino IDE to confirm whether the development environment is installed successfully.

9. For special code library, search for "HELTEC ESP32" in `Library Manager`, select the latest version and install: ESP32 Series Library

---

## Programming

Once you have the framework and libraries installed, connect your computer to the board and start programming nodes.

---

### Example

1. Connect your node to the computer with a USB cable.

2. Open Arduino IDE, Correctly select a board and relevant options in the `Tools` menu:

3. Then select an example.

4. To execute the code in a library, you need to mouse over, scroll down, find the library, and find the code in it.

5. Compile & Upload

> **Tip:** If you cannot upload the code, please manually enter the BOOTLOADER mode: hold down the PRG (USER/BOOT) key and do not release it, press the RST key once, and then release the PRG (USER/BOOT).

---

### Programming

1. Connect your node to the computer with a USB cable.

2. Open Arduino IDE, Correctly select a board and relevant options in the `Tools` menu:

3. Programming
   * New sketch
   * To execute the demo in the heltec ESP32 library, you need to mouse over, scroll down, find the library, and find the code in it.

4. Compile & Upload

> **Tip:** If you cannot upload the code, please manually enter the BOOTLOADER mode: hold down the PRG (USER/BOOT) key and do not release it, press the RST key once, and then release the PRG (USER/BOOT).

---

**Reference:** [Heltec ESP32 Series Quick Start Documentation](https://docs.heltec.org/en/node/esp32/esp32_general_docs/quick_start.html)

