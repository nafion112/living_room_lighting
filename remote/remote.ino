#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


const char* DEVICE = "Living Room Remote";
const char* VERSION = "0.1";
const char* PLATFORM = "3v3 arduino mini pro /w nRF24L01+";
const char* AUTHOR = "Dan Anderson";

const int BAUD_RATE = 9600;

// DEVICE SPECS
//// Blue LED = status
//// Red LED = low batt
//// single push button
////    click on/off
////    hold to dim
//// LED resistors - 330 ohm
//// switch pulldown - 10K ohm

///////// ADRUINO PINOUT
//////////  MISO = 12
//////////  MOSI = 11
//////////  SCK = 13
//////////  IRQ = NC
const short RADIO_CE_PIN = 9;
const short RADIO_CSN_PIN = 10;
const short BUTTON_PIN = 7;
const short RED_LED_PIN = 6;
const short BLUE_LED_PIN = 5;
const short VOLTAGE_PIN = A0;

//// ADDRESSING
const uint64_t LOCAL_MAC =  0x6E6166696F6E; /// living room remote, 12hex
const uint64_t REMOTE_MAC = 0x6E6166696F6F; /// wall outlet

// low batt
const int low_batt = 1234; /// this needs to be tweak tested

// button /w watchdog (for debounce)
const unsigned int button_watchdog_threshold = 50; /// milliseconds
unsigned int button_watchdog;
bool button_state = LOW; /// set to LOW and DO NOT hold the button down while rebooting!!!

// init radio
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void setup() {
   delay(20); /// energize the chip
   Serial.begin(BAUD_RATE);
   printf_begin();
   
   printf("INIT_BOOTING...\n\r");
   printf("\n\r%s - Version %s\n\r%s\n\rPlatform - %s\n\r\n\r", DEVICE, VERSION, AUTHOR, PLATFORM);
   
   pinMode(BUTTON_PIN, INPUT);
   printf("button initialized\n\r");

   pinMode(VOLTAGE_PIN, INPUT);
   printf("batt monitor initialized\n\r");
   
   pinMode(RED_LED_PIN, OUTPUT);
   digitalWrite(RED_LED_PIN, HIGH); /// inverse?
   pinMode(BLUE_LED_PIN, OUTPUT);
   digitalWrite(BLUE_LED_PIN, HIGH); ///inverse?
   printf("LEDs initialized\n\r");

   printf("\n\rENTERING_LOOP...\n\r");
}

void loop() {
   // debounce the button
   int loop_timestamp = millis();
   int button_reading = digitalRead(BUTTON_PIN);
   if (button_reading == button_state) {
      button_watchdog = loop_timestamp;
   } else {
      if (button_watchdog + button_watchdog_threshold < loop_timestamp) {
         button_state = button_reading;
         button_watchdog = loop_timestamp;
         if (button_state) {
            digitalWrite(BLUE_LED_PIN, LOW);
            printf("Button DEPRESSED\n\r");
         } else {
            digitalWrite(BLUE_LED_PIN, HIGH);
            printf("Button RELEASED\n\r");
         }
      }
   }

   // deal with low battery notification
   int batt_voltage = analogRead(VOLTAGE_PIN);
   printf("%d\n\r", batt_voltage);
}
