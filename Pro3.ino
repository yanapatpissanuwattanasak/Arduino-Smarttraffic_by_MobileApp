#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FirebaseESP32.h>

#include "EEPROM.h"
#define EEPROM_SIZE 159
#define sw_changeled 36
#define red 13
#define yellow 15
#define green 17
#define D1 18
#define D2 12
#define A 27 
#define B 32
#define C 23
#define D 33
#define G 16
#define F 19
#define E 26
#define D3 3 
#define D4 25
const int pingPin = 2;
int inPin = 5;
#define Buzzer 14

#define period_t2 500

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
FirebaseData firebaseData;
const char* ssid = "KUWINEIEI";
const char* password = "09876543211";
#define FIREBASE_HOST "embedded-2d297.firebaseio.com"
#define FIREBASE_AUTH "kMVIS1AqDxKQ6xLS3hkvru4h3Nl8CcIk7iEvmXPO"
void IRAM_ATTR toState( void );
void IRAM_ATTR toCount_start( void );
void IRAM_ATTR toCount( void );
void IRAM_ATTR shownumber( void );
void IRAM_ATTR shownum(int r);
void IRAM_ATTR showDisplay(void);
void IRAM_ATTR TD(void);
void IRAM_ATTR timer1_callback( void );
void IRAM_ATTR showNumber(int num);

hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;
hw_timer_t *timer3 = NULL;
hw_timer_t *timer0 = NULL;
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 1500;
unsigned long last_time = 0;
int leds;
unsigned long time_1 = 0;
int status7Seg = 1;
int maxs = 2;
int num = 0;
char i = 0;
char color = 'R';
bool changes = true;
int ledstate,ledstate2;
bool off = false;
int states ; 
int reading ;

int lastButtonState = LOW;
int buttonState;  
int EEPaddress = 158;
int times = 20; 
void showred();
void showgreen();
void showyellow();
int restores ;

int modes  = 0;
bool re = false;
int oldmode = 0;
bool timeron = false;
int tmpnumber1,tmpnumber2;
bool ct = true; 
long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}
void setup()

{
 Serial.begin(115200);
 delay(1000);
 Serial.printf("Connecting to %s ", ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
  if (!EEPROM.begin(EEPROM_SIZE)) {
 Serial.println("Failed init EEPROM");
 delay(1000);
 ESP.restart();
 }
 Serial.println(" CONNECTED");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  EEPROM.writeUChar(EEPaddress, 0);
  EEPROM.commit();
  pinMode(sw_changeled, INPUT_PULLUP);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(D,OUTPUT);
  pinMode(E,OUTPUT);
  pinMode(F,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  showred();
  long duration, cm;
  Serial.println(color);
  if (Firebase.getInt(firebaseData, "/mode")) {
    if (firebaseData.dataType() == "int") {  
      modes = firebaseData.intData();   
    }
   }
  oldmode = modes;

   
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(("SSD1306 allocation failed"));
   }
   display.display();
   display.clearDisplay();
   display.setTextSize(2);
   display.setCursor(0 , 10);
   display.setTextColor(WHITE);
 
 /*  timer4 = timerBegin( 4, 80, true );
    timerAlarmWrite( timer4, 5000000, true );
    timerAttachInterrupt( timer4, &TD, true );
    timerAlarmEnable( timer4 );*/
  timer0 = timerBegin( 0, 80, true );
  timerAlarmWrite( timer0, 1000, true );
  timerAttachInterrupt( timer0, &timer1_callback, true );
  timerAlarmEnable( timer0 );
}

  

void loop()

{
  long duration,cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  delay(10);
   
  if((cm < 14)  && (color != 'G')){
    digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
  }
  if(off){
    Firebase.setInt(firebaseData,"/ledstate",leds);
    off = false;
    color = 'Y';
    times = 20;
  }
  if (Firebase.getInt(firebaseData, "/ledstate")) {
    if (firebaseData.dataType() == "int") {  
      ledstate = firebaseData.intData();   
    }
   }

  if (Firebase.getInt(firebaseData, "/mode")) {
    if (firebaseData.dataType() == "int") {  
      modes = firebaseData.intData();   
    }
   }
  

  
  
  if(ledstate == num){
    states = 1;
  }
  else{
    states = 0;
  }
  if((modes > oldmode) && (states == 1)){
     timer1 = timerBegin( 1, 80, true );
    timerAlarmWrite( timer1, 4000000, true );
    timerAttachInterrupt( timer1, &toCount_start, true );
    timerAlarmEnable( timer1 );
    oldmode = modes;
    ct = false;
    
  }
  if(modes < oldmode && (states == 1) ){
    oldmode = modes;
  }
  if(states > EEPROM.readUChar(EEPaddress)){
    
    restores = EEPROM.readUChar(EEPaddress);
    Serial.println(color);
    EEPROM.writeUChar(EEPaddress, 1);
    EEPROM.commit();
    
    timer1 = timerBegin( 1, 80, true );
    timerAlarmWrite( timer1, 4000000, true );
    timerAttachInterrupt( timer1, &toCount_start, true );
    timerAlarmEnable( timer1 );
    
    
    
  }if(states < EEPROM.readUChar(EEPaddress)){
    restores = EEPROM.readUChar(EEPaddress);
    
    color = 'Y';
    
    showyellow();
    Serial.println(color);
    EEPROM.writeUChar(EEPaddress, 0);
    EEPROM.commit();
    timer2 = timerBegin( 2, 80, true );
    timerAlarmWrite( timer2, 4000000, true );
    timerAttachInterrupt( timer2, &toState, true );
    timerAlarmEnable( timer2 );
    
    
  }

}
void IRAM_ATTR toState( void ){
     

     color = 'R';
     showred();
     Serial.println(color);
    
     timerAlarmDisable(timer2);
     
     
     
     
     
     
}
void IRAM_ATTR toCount_start(){
    color = 'G';
    showgreen();
    ct = true;
    timer3 = timerBegin( 3, 80, true );
    timerAlarmWrite( timer3, 1000000, true );
    timerAttachInterrupt( timer3, &toCount, true );
    timerAlarmEnable( timer3 );
    timerAlarmDisable(timer1);
}
void IRAM_ATTR toCount( void ){
  
  
  Serial.println(color);
  showgreen();
  Serial.println(times);
  
 
  times--;
  if(modes == 0){
    times = 20;
    timerAlarmDisable(timer3);
    
    
  }
  
    
  
  if(times == 0){
    
    if(ledstate + 1 > maxs){
      leds = 0;
    }
    else{
      leds = ledstate+1;
    }
    off = true;
    timerAlarmDisable(timer3);
  }
}

void showred(){
    
      digitalWrite(2, LOW );
      digitalWrite(red, HIGH );
      digitalWrite(green, LOW );
      digitalWrite(yellow, LOW );
}

void showgreen(){
      
      digitalWrite(2, LOW );
      digitalWrite(red, LOW );
      digitalWrite(green, HIGH );
      digitalWrite(yellow, LOW );
}

void showyellow(){
     
      digitalWrite(2, LOW );
      digitalWrite(red, LOW);
      digitalWrite(green, LOW );
      digitalWrite(yellow, HIGH );
}




void IRAM_ATTR timer1_callback( void )
{
  tmpnumber1 = times/10;
  tmpnumber2 = times%10;
  

  if (status7Seg == 1)
  {
    if(color == 'G' && times > 0 && (modes != 0) && ct){
      showNumber(tmpnumber1);
    }
    else
      showNumber(99);
    digitalWrite( D1, HIGH);
    digitalWrite( D2, LOW);
    digitalWrite( D3, HIGH);
    digitalWrite( D4, HIGH);
   
    status7Seg = 2;
  }
  else if (status7Seg == 2)
  {
    if(color == 'G' && times > 0 && (modes != 0) && ct){
      showNumber(tmpnumber2);
    }
    else
      showNumber(99);
    digitalWrite( D1, HIGH );
    digitalWrite( D2, HIGH );
    digitalWrite( D3, LOW);
    digitalWrite( D4, HIGH);
    
    status7Seg = 3;
  }
  else
  {
    status7Seg = 1;
  }

}
void IRAM_ATTR showNumber(int num)
{
  if (num == 0)
  {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, HIGH );  digitalWrite( F, HIGH ); digitalWrite( G, LOW );
  }
  else if (num == 1) {
    digitalWrite( A, LOW ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, LOW ); digitalWrite( E, LOW );  digitalWrite( F, LOW ); digitalWrite( G, LOW );
  }
  else if (num == 2) {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, LOW );
    digitalWrite( D, HIGH ); digitalWrite( E, HIGH );  digitalWrite( F, LOW ); digitalWrite( G, HIGH );
  }
  else if (num == 3) {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, LOW );  digitalWrite( F, LOW ); digitalWrite( G, HIGH );
  }
  else if (num == 4) {
    digitalWrite( A, LOW ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, LOW ); digitalWrite( E, LOW );  digitalWrite( F, HIGH ); digitalWrite( G, HIGH );
  }
  else if (num == 5) {
    digitalWrite( A, HIGH ); digitalWrite( B, LOW ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, LOW );  digitalWrite( F, HIGH ); digitalWrite( G, HIGH );
  }
  else if (num == 6) {
    digitalWrite( A, HIGH ); digitalWrite( B, LOW ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, HIGH );  digitalWrite( F, HIGH ); digitalWrite( G, HIGH );
  }
  else if (num == 7) {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, LOW ); digitalWrite( E, LOW );  digitalWrite( F, LOW ); digitalWrite( G, LOW );
  }
  else if (num == 8) {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, HIGH );  digitalWrite( F, HIGH ); digitalWrite( G, HIGH );
  }
  else if (num == 9) {
    digitalWrite( A, HIGH ); digitalWrite( B, HIGH ); digitalWrite( C, HIGH );
    digitalWrite( D, HIGH ); digitalWrite( E, LOW );  digitalWrite( F, HIGH ); digitalWrite( G, HIGH );
  }else {
    digitalWrite( A, LOW ); digitalWrite( B, LOW ); digitalWrite( C, LOW );
    digitalWrite( D, LOW ); digitalWrite( E, LOW );  digitalWrite( F, LOW ); digitalWrite( G, HIGH );
  }
}
