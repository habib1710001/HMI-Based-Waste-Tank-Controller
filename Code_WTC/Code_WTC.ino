#include "Nextion.h"
#include <ESP32Time.h>

//ESP32Time rtc;
ESP32Time rtc(21600);  // offset in seconds GMT+6

//Pin Definitions
#define PIN_VALVEA_OPEN_CH1 18
#define PIN_VALVEA_CLOSE_CH2 19

#define PIN_ALARM 21

#define PIN_VALVEB_OPEN_CH3 22
#define PIN_VALVEB_CLOSE_CH4 23


//Variables for Storing the Relay Channel States
uint8_t state_valveA_open_ch1  = 0;
uint8_t state_valveA_close_ch2 = 0;
uint8_t state_valveB_open_ch3  = 0;
uint8_t state_valveB_close_ch4 = 0;

//Stores the moment since the valves are open
int  last_valveA_open_min;
int  last_valveA_open_hr;
int  last_valveB_open_hr;
int  last_valveB_open_min;

//Stores the duration of the valves being openned
int  timeAElapsedHr;
int  timeAElapsedMin;
int  timeBElapsedHr;
int  timeBElapsedMin;

uint8_t flagValveA;
uint8_t flagValveB;

//Tracks the moments for turning on the RELAY channels for specific times (7s, 5s)
uint32_t lastTrack1;
uint32_t lastTrack2;
uint32_t lastTrack3;
uint32_t lastTrack4;
uint32_t lastTime;


//Object for the display
// (page id, component id, component name)
NexPage page0 = NexPage(0,0,"page0");
NexDSButton valveA = NexDSButton(0, 1, "bt0");
NexDSButton valveB = NexDSButton(0, 2, "bt1");

NexText p0_t0 = NexText(0,6,"t0");
NexText p0_t1 = NexText(0,7,"t1");
NexText p0_t2 = NexText(0,8,"t2");

NexNumber p0_n0 = NexNumber(0,9,"n0");
NexNumber p0_n1 = NexNumber(0,10,"n1");

NexSlider p0_h0 = NexSlider(0, 3, "h0");
NexPicture p0_p1 = NexPicture(0, 5, "p1");

NexTouch *nex_listen_list[] = {
  &page0,
  &valveA,
  &valveB,
  &p0_t0, 
  &p0_t1, 
  &p0_t2,
  &p0_h0, 
  &p0_p1,
  &p0_n0,
  &p0_n1,
  NULL
};


///////*******************Essential Callback Functions for the display*********************************///////////
void valveAPushCallback(void *ptr){
  uint32_t dual_state;
  valveA.getValue(&dual_state);
  Serial.println("VALVE A PRESSED");
  flagValveA = 1;
  if(dual_state){
    Serial.println("ON");
    state_valveA_open_ch1 = 1;
    state_valveA_close_ch2 = 0;
    last_valveA_open_hr  = rtc.getHour(true);
    last_valveA_open_min = rtc.getMinute();
    Serial.println(last_valveA_open_min); 
    Serial.println(last_valveA_open_hr);
    lastTrack1 = millis();
  }
  else{
    Serial.println("OFF");
    state_valveA_open_ch1  = 0;
    state_valveA_close_ch2 = 1;
    lastTrack2 = millis();
  }
}

void valveBPushCallback(void *ptr){
  uint32_t dual_state;
  valveB.getValue(&dual_state);
  Serial.println("VALVE B PRESSED");
  flagValveB = 1;
  if(dual_state) {
    Serial.println("ON");
    state_valveB_open_ch3 = 1;
    state_valveB_close_ch4 = 0;
    last_valveB_open_hr  = rtc.getHour(true);
    last_valveB_open_min = rtc.getMinute();
    Serial.println(last_valveB_open_min); 
    Serial.println(last_valveB_open_hr);
    lastTrack3 = millis();
  }
  else{
    Serial.println("OFF");
    state_valveB_open_ch3 = 0;
    state_valveB_close_ch4 = 1; 
    lastTrack4 = millis(); 
  }
}

void sliderPushCallback(void *ptr){
  
}

void picturePushCallback(void *ptr){
  
}

void setup() {
  nexInit();
  rtc.setTime(00, 39, 9, 8, 5, 2023);  // not a must to set

  //Defining the output pins
  
  pinMode(PIN_VALVEA_OPEN_CH1, OUTPUT);
  pinMode(PIN_VALVEA_CLOSE_CH2, OUTPUT);
  pinMode(PIN_VALVEB_OPEN_CH3, OUTPUT);
  pinMode(PIN_VALVEB_CLOSE_CH4, OUTPUT);
  pinMode(PIN_ALARM , OUTPUT);
    
  digitalWrite(PIN_VALVEA_OPEN_CH1, LOW);
  digitalWrite(PIN_VALVEA_CLOSE_CH2, LOW);
  digitalWrite(PIN_VALVEB_OPEN_CH3, LOW);
  digitalWrite(PIN_VALVEB_CLOSE_CH4, LOW);

  valveA.attachPush(valveAPushCallback, &valveA);
  valveB.attachPush(valveBPushCallback, &valveB);

  p0_t0.setText("TIME ELAPSED SINCE LAST EMPTIED");
  p0_n0.setValue(0);
  p0_n1.setValue(0);
  p0_t1.setText("hr");
  p0_t2.setText("hr");
  p0_h0.attachPush(sliderPushCallback, &p0_h0);
  p0_p1.attachPush(picturePushCallback, &p0_p1);



  lastTime = millis();
}

void loop() {
  nexLoop(nex_listen_list);

  if ((state_valveA_open_ch1 == 1) && (millis() <= lastTrack1 + 7000UL))
  {
    digitalWrite(PIN_VALVEA_OPEN_CH1, HIGH);
    digitalWrite(PIN_VALVEA_CLOSE_CH2, LOW);
  }
  else
  {
    state_valveA_open_ch1 = 0;
    digitalWrite(PIN_VALVEA_OPEN_CH1, LOW);
  }

  if ((state_valveA_close_ch2 == 1) && (millis() <= lastTrack2 + 7000))
  {
    digitalWrite(PIN_VALVEA_CLOSE_CH2, HIGH);
    digitalWrite(PIN_VALVEA_OPEN_CH1, LOW); 
  }
  else{
    state_valveA_close_ch2 = 0;
    digitalWrite(PIN_VALVEA_CLOSE_CH2, LOW);
  }

  if ((state_valveB_open_ch3 == 1) && (millis() <= lastTrack3 + 5000))
  {
    digitalWrite(PIN_VALVEB_OPEN_CH3, HIGH);
    digitalWrite(PIN_VALVEB_CLOSE_CH4, LOW); 
  }
  else{
    state_valveB_open_ch3 = 0;
    digitalWrite(PIN_VALVEB_OPEN_CH3, LOW);  
  }
  
  if ((state_valveB_close_ch4 == 1) && (millis() <= lastTrack4 + 5000))
  {
    digitalWrite(PIN_VALVEB_CLOSE_CH4, HIGH);
    digitalWrite(PIN_VALVEB_OPEN_CH3, LOW); 
  }
  else{
    state_valveB_close_ch4 = 0;
    digitalWrite(PIN_VALVEB_CLOSE_CH4, LOW);
  }
  
  if (millis() > lastTime + 2000){
    if(flagValveA == 1)
    {
      timeAElapsedHr = (rtc.getHour(true) - last_valveA_open_hr );
      Serial.println("Hour Elapsed ValveA: "+ String(timeAElapsedHr));
      if (timeAElapsedHr <= 0){
          timeAElapsedMin = (rtc.getMinute() - last_valveA_open_min);
          p0_n0.setValue(timeAElapsedMin);
          p0_t1.setText("min");
      }
      else{ 
          p0_n0.setValue(timeAElapsedHr);
          p0_t1.setText("hr");
      }
    }
    if(flagValveB == 1){
      timeBElapsedHr = (rtc.getHour(true) - last_valveB_open_hr);
      Serial.println("Hour Elapsed ValveB: "+ String(timeBElapsedHr));
      if (timeBElapsedHr <= 0){
          timeBElapsedMin = (rtc.getMinute() - last_valveB_open_min);
          p0_n1.setValue(timeBElapsedMin);
          p0_t2.setText("min");
      }
      else{
          p0_n1.setValue(timeBElapsedHr);
          p0_t2.setText("hr");
      }
    }
    lastTime = millis();
  }
  
  if( timeBElapsedHr > 12){
    digitalWrite(PIN_ALARM , HIGH);
  }
  else{
    digitalWrite(PIN_ALARM , LOW);
  }
}
