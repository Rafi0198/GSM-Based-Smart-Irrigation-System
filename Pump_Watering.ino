#include<Wire.h>
#include<DHT.h>
#define dhtPin 11
#define dhtType DHT11
#include<SoftwareSerial.h>

SoftwareSerial gsm (3, 2);
DHT dht (dhtPin, dhtType);


int trig = A2; int echo = A3;
int pump1 = 13; int pump2 = A4 ;
int L[] = {6, 7, 8, 9};
float s, t;
int moisture; float value;
int k;
int count1 = 0; int count2 = 0; int count3 = 0; int count4 = 0; int count5 = 0;

float temp, humidity;
String msg;

int rain = A5; int rainled = 10;

void setup() {
  // put your setup code here, to run once:

  dht.begin();
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(trig, OUTPUT); pinMode(echo, INPUT);
  pinMode(pump1, OUTPUT); pinMode(pump2, OUTPUT);
  pinMode(moisture, INPUT);
  pinMode(rain,INPUT);
  pinMode(rainled,OUTPUT);  

  for (k = 0; k <= 3; k++)
    pinMode(L[k], OUTPUT);


  Serial.println("Initializing Netwrok...");

  gsm.println("AT");
  checkSerial();

  gsm.println("AT+CSQ");
  checkSerial();

  gsm.println("AT+CCID");
  checkSerial();

  gsm.println("AT+CREG?");
  checkSerial();

  gsm.println("AT+CBC");
  checkSerial();

  gsm.println("AT+COPS?");
  checkSerial();

  gsm.println("AT+CMGF=1"); // Initializes the text mode
  checkSerial();

  gsm.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived messages will be handled
  checkSerial();

  gsm.println("AT+CMGD=1,4");
  checkSerial(); 

}

void loop() {
  // put your main code here, to run repeatedly:

  // Adjusting the GSM module

  if (gsm.available()) {
    msg = gsm.readString();
    Serial.println(msg);
  }

  if (msg.indexOf("ON1") >= 0) {
    digitalWrite(pump1, HIGH);
    msg1();
    msg = "";
  }

  if (msg.indexOf("DOWN1") >= 0) {
    digitalWrite(pump1, LOW);
    msg2();
    msg = "";
  }

  if (msg.indexOf("ON2") >= 0) {
    digitalWrite(pump2, HIGH);
    msg3();
    msg = "";
  }

  if (msg.indexOf("DOWN2") >= 0) {
    digitalWrite(pump2, LOW);
    msg4();
    msg = "";
  }


  //Measures the Temperature & Humidity using DHT11

  temp = dht.readTemperature();
  humidity = dht.readHumidity();

  // Measure the water level
  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  t = pulseIn(echo, HIGH);
  s = (0.035 * t) / 2 ;

  // Checks water level & turns on/off pump1


  if (s > 18) {
    digitalWrite(pump1, HIGH); // Lower Threshold of water level
//    count1++;
//
//    if (count1 == 2)
//      msg1();
//
//    if (count1 > 10)
//      count1 = 5;

  }

//  else count1 = 0;

  if (s < 3)  {
    digitalWrite(pump1, LOW); // Upper Threshold of water level
//    count2=count2+1;
//
//   if (count2 == (2))
//      msg2();
//
//    if (count2 > 10)
//      count2 = 5;
  }

  //else count2 = 0;

  // Shows the water level through LED

  if (s >= 14) {

    digitalWrite(L[0], HIGH);
    digitalWrite(L[1], LOW);
    digitalWrite(L[2], LOW);
    digitalWrite(L[3], LOW);

  }

  if (s >= 10 && s < 14) {

    digitalWrite(L[0], HIGH);
    digitalWrite(L[1], HIGH);
    digitalWrite(L[2], LOW);
    digitalWrite(L[3], LOW);
  }

  if (s >= 6 && s < 10) {

    digitalWrite(L[0], HIGH);
    digitalWrite(L[1], HIGH);
    digitalWrite(L[2], HIGH);
    digitalWrite(L[3], LOW);
  }

  if (s >= 2 && s < 6) {

    digitalWrite(L[0], HIGH);
    digitalWrite(L[1], HIGH);
    digitalWrite(L[2], HIGH);
    digitalWrite(L[3], HIGH);
  }


  // Reads soil Moisture & Turns on auto watering

  moisture = analogRead(A0);
  value = map(moisture, 1023, 200, 0, 100);

  if (value < 10)
  { // Lower Threshold
    digitalWrite(pump2, HIGH);
    count3++;

    if (count3 == 2)
      msg3();

    if (count3 > 10)
      count3 = 5;
  }

  else count3 = 0;


  if (value > 20) {  // Upper Threshold
    digitalWrite(pump2, LOW);
    count4++;


    if (count4 == 2)
      msg4();

    if (count4 > 10)
      count4 = 5;
  }

  else count4 = 0;

  if (digitalRead(rain)==0)
  {
    digitalWrite(rainled, HIGH);
    count5++;

    if (count5 > 10)
      count5 = 5;
  }

  else
  { digitalWrite(rainled, LOW);
    count5 = 0;
  }

  if (count5 == 2)
    msg5 ();

  // Read Data in Serial Monitor

  Serial.print("Distance = ");
  Serial.print(s);
  Serial.println("cm");

  Serial.print("Moisture % ");
  Serial.println(value);
  //
  Serial.print("Temperature is = ");
  Serial.println(temp);
  Serial.print("Humidity is is = ");
  Serial.println(humidity);
  //Serial.println(count2); 

  //Serial.print("Analog Value from sensor = ");
  //Serial.println(moisture);
  //delay(1500);

  if (s <= 3.4)  {
    count2=count2+1;

   if (count2 == 2) { 
      msg2();
      delay(4000); 
   } 

    if (count2 > 10)
      count2 = 5;
  }

  else count2 = 0; 

  if (s > 18.8) {
    count1++;

    if (count1 == 2) { 
      msg1();
      delay(4000); 
    }

    if (count1 > 10)
      count1 = 5;

  }

  else count1=0; 

     
}


void checkSerial() {

  delay(500); // Used to ensure enough lag time between the at commands

//  while (Serial.available())
//    gsm.write(Serial.read());

  while (gsm.available())
    Serial.write(gsm.read());

}

void msg1()  {

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.println("PUMP1 has been turned ON"); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void msg2()  {

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.println("PUMP1 has been turned OFF"); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void msg3()  {

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.println("Waterig Pump has been turned ON"); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void msg4()  {

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.println("Watering Pump has been turned OFF"); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void msg5()  {

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.println("Its Raining Now. Courtesy: Rafi's automation"); // Set Message Content
  checkSerial();

  gsm.write(26);

}
