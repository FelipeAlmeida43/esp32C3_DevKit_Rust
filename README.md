# ESP32C3 DevKit Rust
![License](https://img.shields.io/badge/license-MIT-blue.svg)
Welcome to the ESP32C3 DevKit-C project! This repository demonstrates how to use the

ESP32C3 DevKit-C Project with SHTC3, ICM-42670-P, and WS2812
Welcome to the ESP32C3 DevKit-C project! This repository demonstrates how to use the ESP32C3 development board with C to interface with the SHTC3 temperature and humidity sensor, the ICM-42670-P Inertial Measurement Unit (IMU), and the WS2812 LED.

##Overview
This project showcases the capabilities of the ESP32C3 development board by integrating it with the SHTC3 sensor for environmental data, the ICM-42670-P IMU for motion data, and the WS2812 LED for visual feedback. The code is written in C for efficient embedded systems programming.

##Features
C Programming: Utilizing the efficiency and control of C language for embedded development.
ESP32C3 Development Board: Leveraging the Wi-Fi and Bluetooth capabilities of the ESP32C3.
SHTC3 Sensor Integration: Accurate temperature and humidity readings.
ICM-42670-P IMU Integration: Precise motion and orientation data.
WS2812 LED Control: Visual feedback using addressable LEDs.
Button Input: User interaction through a push button.
Hardware Requirements
ESP32C3 Development Board
SHTC3 Temperature and Humidity Sensor
ICM-42670-P Inertial Measurement Unit (IMU)
WS2812 LED
Push Button
Connecting Wires
Software Requirements
ESP-IDF (Espressif IoT Development Framework)
Required libraries for sensor communication and LED control
Setup Instructions
1. Clone the Repository
bash
Copiar código
git clone https://github.com/yourusername/esp32c3-c-shtc3-imu-ws2812.git
cd esp32c3-c-shtc3-imu-ws2812
2. Install ESP-IDF
Follow the ESP-IDF setup guide to install the ESP-IDF.

3. Configure the Project
Set the correct serial port and other configurations for your ESP32C3 board in the sdkconfig file.

4. Build and Flash the Firmware
bash
Copiar código
idf.py build
idf.py flash
5. Connect the Sensors and LED
SHTC3:

VCC -> 3.3V
GND -> GND
SDA -> GPIO10
SCL -> GPIO8
ICM-42670-P:

VCC -> 3.3V
GND -> GND
SDA -> GPIO10
SCL -> GPIO8
WS2812 LED:

VCC -> 5V (or 3.3V depending on your LED strip specifications)
GND -> GND
Data In -> GPIO2
Button:

One terminal -> GND
Other terminal -> GPIO9
6. Run the Program
After flashing the firmware, reset the ESP32C3 board. The program will start running and you should see the sensor data being printed to the serial console and the LED responding to the data.

Usage
You can modify the code in main.c to add more functionality, such as sending data to a cloud service, logging data to an SD card, or triggering actions based on sensor readings.

Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue if you have any suggestions or improvements.

License
This project is licensed under the MIT License. See the LICENSE file for more details.

Acknowledgements
Special thanks to the developers of the ESP-IDF and the creators of the sensors and components for their excellent tools and documentation.
