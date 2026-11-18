DIY Smart Home Energy Monitor (ESP32 & SCT-013-000)

A non-invasive, IoT-enabled Smart Home Energy Monitor built using an ESP32, an SCT-013-000 (100A/50mA) split-core current transformer, and the Blynk IoT cloud platform. It measures real-time AC current and wattage, estimates monthly electricity costs, and sends high-load alerts directly to your smartphone.

⚠️ CRITICAL SAFETY WARNING

NEVER clamp the SCT-013 sensor around a complete power cord containing both Live and Neutral wires—their magnetic fields will cancel out, resulting in a 0W reading.

ONLY clamp the sensor around a single insulated conductor (either the Live/Phase wire OR the Neutral wire).

Ensure main power is turned off at the breaker before accessing any exposed wiring in your distribution box.

🛠️ Hardware Requirements

Microcontroller: ESP32 Development Board (30-pin or 38-pin)

Current Sensor: YHDC SCT-013-000 (Non-invasive AC Current Sensor, 100A max, 50mA output)

Burden Resistor: 1x 33Ω resistor (1/4W or 1/2W)

Voltage Divider Resistors: 2x 10kΩ resistors (for 1.65V DC bias)

Smoothing Capacitor: 1x 10µF electrolytic capacitor (rated 10V or higher)

Prototyping: Breadboard and Female-to-Male / Male-to-Male jumper wires

🔌 Breadboard Wiring Guide (No 3.5mm Jack Breakout Required)

Since the ESP32 ADC (GPIO 34) can only read positive voltages between 0V and 3.3V, we must shift the AC signal from the sensor up by 1.65V using a voltage divider.

1. Build the 1.65V DC Bias Circuit

Connect Resistor 1 (10kΩ) between the ESP32 3V3 pin and an empty breadboard row (let's call this the Bias Node).

Connect Resistor 2 (10kΩ) between the Bias Node and the ESP32 GND pin.

Connect the 10µF Capacitor across the Bias Node and GND (ensure the negative/striped leg goes to GND).

2. Connect the SCT-013-000 Plug (Jumper Method)

Using Female-to-Male jumper wires, slide the female ends directly onto the metal segments of the 3.5mm audio plug:

Tip (Signal): Connect to a free breadboard row (let's call this the Signal Node).

Sleeve (Base/Return): Connect directly to the Bias Node (1.65V).

(Leave the middle Ring contact disconnected).

3. Connect the Burden Resistor & ESP32 ADC

Place the 33Ω Burden Resistor between the Signal Node and the Bias Node.

Connect a jumper wire from the Signal Node directly to GPIO 34 on your ESP32.

💻 Software & Arduino IDE Setup

Use Classic Arduino IDE (v2.x): Ensure you are using the standard Arduino IDE, not Arduino App Lab.

Avoid OneDrive Sync Issues: Set your Sketchbook location (File > Preferences) to a local, English-only folder path (e.g., C:\ArduinoProjects).

Install ESP32 Board Support:

Add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json to Additional Boards Manager URLs in Preferences.

Open Boards Manager, search for esp32 by Espressif Systems, and install it.

Install Required Libraries:

Blynk (by Volodymyr Shymanskyy)

EmonLib (by OpenEnergyMonitor)
(Tip: If they don't appear in Library Manager, download their .zip files and install via Sketch > Include Library > Add .ZIP Library...)

📱 Blynk Cloud Dashboard Configuration

Log in to blynk.cloud and create a New Template:

Name: Energy Monitor

Hardware: ESP32 | Connection: Wi-Fi

Navigate to Datastreams and create two Virtual Pin datastreams:

V0: Name: Power | Data Type: Double | Units: Watts | Min: 0, Max: 10000

V1: Name: Monthly Cost | Data Type: Double | Units: Currency | Min: 0, Max: 50000

(Optional) Go to Events and create a new event with Event Code high_power_alert to receive push notifications when load exceeds 3000W.

Copy your Template ID, Template Name, and Auth Token from the Device Info tab and paste them into the top of SmartEnergyMonitor.ino.

🚀 Flashing & Calibration

Open SmartEnergyMonitor.ino in Arduino IDE.

Update YOUR_TEMPLATE_ID, YOUR_AUTH_TOKEN, YOUR_WIFI_NAME, and YOUR_WIFI_PASSWORD.

Adjust VOLT_CAL (230.0 for India/EU/UK, 120.0 for North America) and PRICE_PER_KWH to match your local utility rate.

Select Tools > Board > ESP32 Dev Module and your active COM Port.

Click Upload. Once finished, open the Serial Monitor at 115200 baud to verify live readings!