/*
 * DIY Smart Home Energy Monitor (ESP32 + SCT-013-000 + Blynk IoT)
 * 
 * Hardware Connections:
 * - SCT-013-000 Tip (Signal)    -> Burden Resistor (33 Ohm) -> ESP32 GPIO 34 (ADC1_CH6)
 * - SCT-013-000 Sleeve (Return) -> Other end of Burden Resistor -> 1.65V DC Bias Node
 * - 1.65V DC Bias Node          -> Voltage Divider (10k Ohm to 3.3V, 10k Ohm to GND) + 10uF Cap to GND
 */

// --- BLYNK CONFIGURATION ---
// Replace these with your actual credentials from Blynk.Cloud
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Energy Monitor"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

// Uncomment the line below to print detailed Blynk debug info to Serial Monitor
// #define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "EmonLib.h"

// --- WI-FI CREDENTIALS ---
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// --- HARDWARE & CALIBRATION SETTINGS ---
#define ADC_PIN       34      // Analog input pin on ESP32 (GPIO 34)
#define VOLT_CAL      230.0   // Nominal AC Voltage (230.0V for India/EU/UK, 120.0V for US)
#define CT_CAL        60.6    // Calibration factor: (100A / 0.05A) / 33 Ohm = 60.606
#define PRICE_PER_KWH 8.0     // Local electricity tariff per unit (kWh) in your currency
#define NOISE_CUTOFF  5.0     // Ignore phantom readings below 5 Watts
#define ALERT_LIMIT   3000.0  // High power threshold in Watts for push notifications

// --- GLOBAL OBJECTS & VARIABLES ---
EnergyMonitor emon1;
BlynkTimer timer;

double realPower = 0.0;
double monthlyBillEstimate = 0.0;
bool alertSent = false;

/**
 * Reads current from SCT-013, calculates power & cost, and pushes data to Blynk.
 */
void measureEnergy() {
  // Calculate Root Mean Square (RMS) current over 1480 samples
  double Irms = emon1.calcIrms(1480);
  
  // Calculate apparent/real power in Watts (P = V * I)
  realPower = Irms * VOLT_CAL;
  
  // Filter out sensor background noise when no load is connected
  if (realPower < NOISE_CUTOFF) {
    realPower = 0.0;
  }

  // Estimate monthly cost assuming this continuous load (30 days * 24 hours = 720 hours)
  double monthlyKWh = (realPower * 720.0) / 1000.0;
  monthlyBillEstimate = monthlyKWh * PRICE_PER_KWH;

  // Print readings to Serial Monitor for debugging/calibration
  Serial.print("Current (Irms): ");
  Serial.print(Irms, 3);
  Serial.print(" A | Power: ");
  Serial.print(realPower, 1);
  Serial.print(" W | Est. Monthly Cost: ");
  Serial.println(monthlyBillEstimate, 2);

  // Send telemetry data to Blynk Virtual Pins
  Blynk.virtualWrite(V0, realPower);
  Blynk.virtualWrite(V1, monthlyBillEstimate);

  // Trigger a Blynk Event notification if load exceeds threshold
  if (realPower > ALERT_LIMIT && !alertSent) {
    Blynk.logEvent("high_power_alert", "High consumption detected! Load: " + String(realPower, 0) + " W");
    alertSent = true;
  } else if (realPower <= ALERT_LIMIT) {
    alertSent = false; // Reset alert flag once load drops below limit
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Starting Smart Home Energy Monitor ---");

  // Set ADC resolution to 12-bit (0-4095) for ESP32
  analogReadResolution(12);

  // Initialize EmonLib current sensor input
  emon1.current(ADC_PIN, CT_CAL);

  // Connect to Wi-Fi and Blynk Cloud
  Serial.println("Connecting to Wi-Fi and Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Schedule energy measurement every 2 seconds (2000 milliseconds)
  timer.setInterval(2000L, measureEnergy);
}

void loop() {
  Blynk.run();
  timer.run();
}