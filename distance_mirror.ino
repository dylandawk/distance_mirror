#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>
#include <SimpleKalmanFilter.h>

#define NUMPIXELS 250
#define PIN 6
#define DELAY_TIME 3000

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);
SimpleKalmanFilter filter(2,2, 0.01);

// VL53L0X values
int sensorValue = 0;
int openDistance = 0;
int maxDistance = 999;

// Timer-delay variables
bool timerStarted = false;
unsigned long timer = 0;
unsigned long startTime = 0;

// SimpleKalmamnFilter variables



void setup() {
//  Serial.begin(115200);
//
//  // wait until serial port opens for native USB devices
//  while (! Serial) {
//    delay(1);
//  }

  pixels.begin();
  
//  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
//    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
//  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  //Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    sensorValue = measure.RangeMilliMeter;
    float smoothRange = filter.updateEstimate(sensorValue);
    if(smoothRange > 800) smoothRange = 800;
    timerFunction(smoothRange);
//    Serial.print(", Distance (mm): "); Serial.print(sensorValue); 
//    Serial.print(", Smooth Distance (mm): "); Serial.print(smoothRange);
  } else {
    sensorValue = 1000;
//    Serial.print(" out of range ");
  }
  Serial.println();
  delay(100);
}

void timerFunction(float data){
  // if object is within measurement range start timer
//  Serial.println(sensorValue);
  if(sensorValue > openDistance && sensorValue < maxDistance){
    if(!timerStarted){
      timerStarted = true;
      //Serial.println("timer started");
      startTime = millis();
    } else {
      timer = millis() - startTime;
      //Serial.print("timer: "); Serial.println(timer);
      if(timer > DELAY_TIME){
        runLED(data);
      }
    }
  } else {
    timerStarted = false;
    timer = 0;
    //Serial.println("timer stopped");
    stopLED();
  }
}

void runLED(int data){
  if(data > 800) data = 800;
  int brightness = map(data, 200, 800, 255, 5); // maps object distance to brightness level
  int hue = map(data, 200, 800, 65535, (65536 / 2));
//  Serial.print("Brightness: "); Serial.print(brightness);
  for(int i = 0; i < NUMPIXELS; i++) {
    if(i % 2 == 0){
          //pixels.setPixelColor(i, pixels.Color(0,0,0, brightness));
          pixels.setPixelColor(i, pixels.ColorHSV(hue)) ;
    } else {
      pixels.setPixelColor(i, pixels.Color(0,0,0,0));
    }
  }
  pixels.show();
}

void stopLED(){
  pixels.clear();
  pixels.show();
}
