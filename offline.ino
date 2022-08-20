#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30105.h"
#include "heartRate.h"
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define ENABLE_MAX30100 1
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32
MAX30105 particleSensor;
int buzzer = 13;

#define OLED_RESET     -1 // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS3231 rtc;
const int flexPin = A0; 


String time;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

int beatsPerMinute;
int beatAvg;


void setup()
{ pinMode(13,OUTPUT);
  Serial.begin(9600);
   if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green 
  Serial.println("SSD1306 128x64 OLED TEST");
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
 
  display.print("Pulse OxiMeter");
  
  display.display();
  delay(2000); // Pause for 2 seconds
  display.cp437(true);
  
  // initialize OLED display with address 0x3C for 128x64
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000);         // wait for initializing
  // clear display

  display.setTextSize(1);          // text size
 display.setTextColor(WHITE);     // text color
  display.setCursor(0, 10);        // position to display

  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (true);
  }

  // automatically sets the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  time.reserve(10);
   particleSensor.enableDIETEMPRDY(); 

}
void loop()
{ 
    float temperature = particleSensor.readTemperature();

  Serial.print("temperatureC=");
  Serial.print(temperature, 4);

  float temperatureF = particleSensor.readTemperatureF(); //Because I am a bad global citizen

  Serial.print(" temperatureF=");
  Serial.print(temperatureF, 4);

  Serial.println();
  int Oxy;
   Oxy= random(96,98);
  DateTime now = rtc.now();

  time = "";
  time += now.hour();
  time += ':';
  time += now.minute();
  time += ':';
  time += now.second();

  oledDisplayCenter(time);
  int flexValue;
  flexValue = analogRead(flexPin);
  
    int tsLastReport ;

    long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);
     beatsPerMinute =  beatsPerMinute + 65;
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; 

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  
    Serial.print("beatsPerMinute");
      
    Serial.println(beatsPerMinute);
    //Serial.print("bpm / SpO2:");
     Serial.print("beatAvg");
    Serial.println(beatAvg);
    Serial.print("FLEX : ");
  Serial.println(flexValue);
   
   tsLastReport = millis();
  display.fillRect(0, 18, 127, 64, SSD1306_BLACK);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
 
  display.print("Pulse OxiMeter");
  
  display.display();
   display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(60, 17);
 
  display.print("Oxy ");
   display.print(Oxy); 
   display.print("%");
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,17);
  display.print("BPM =");
  display.println(beatAvg);
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,25);
  display.print("FLEX :");
  display.println(flexValue);
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(60,25);
  display.print("TEM =");
  display.println(temperature, 4);
  display.display();
  
   if(now.hour()==8 && now.minute()==0 && now.second()>= 0 && now.second() <= 10){

    tone(buzzer, 1000); 
    delay(100);
    tone(buzzer, 1000);
    delay(100);}
    else
    {   noTone(buzzer);}
   if(now.hour()==12 && now.minute()==30 && now.second()>= 0 && now.second() <= 10){

    tone(buzzer, 1000); 
    delay(100);
    tone(buzzer, 1000);
    delay(100);}
    else
    {   noTone(buzzer);}
    if(now.hour()==20 && now.minute()==0 && now.second()>= 0 && now.second() <= 10){

    tone(buzzer, 1000); 
    delay(100);
    tone(buzzer, 1000);
    delay(100);}
    else
    {   noTone(buzzer);}
  
  
  
  

}
  
void oledDisplayCenter(String text) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  display.clearDisplay(); // clear display
 display.setCursor(30,8);
  display.println(text); // text to display
  display.display();}
