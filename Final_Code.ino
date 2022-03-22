// One Stop Station Project
// Measure the battery voltage, Take temperature, Count capacity
// Display the information to the screen
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Print battery voltage to 20x4 LCD via I2C
// using Voltage Divider of 2x 10k resisters to keep under 5V PIN limit
// written by Edward Baldwin 10/30/2021

// Temperature

// Count capacity
// count up when IN sensor detects motion
// count down when OUT sensor detects motion
// write to the screen the current count and message

// Fail cases and display?
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define led_pin 13
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();    //Define adafruit
LiquidCrystal_I2C lcd(0x27,20,4);  // Set the LCD I2C address
double userTemp;        // The user's temp
int inputPinIn = 28;             // choose the input pin going IN (for PIR sensor)
int inputPinOut = 32;           // choose the inpute pin going OUT (for PIR sensor)
int pirStateIn = LOW;             // when start, assuming no IN motion detected
int pirStateOut = LOW;            // when start, assuming no OUT motion detected
int valIn = 0;                    // variable for reading the In pin status
int valOut = 0;                   //variable for reading the Out pin status
int count = 0;                  // count the capacity
const int MAXCOUNT = 45;        //max capacity

void setup()
{
  lcd.init();               // initialize the lcd 
  lcd.backlight();
  mlx.begin();              // initialize the temp sensor
  pinMode(inputPinIn, INPUT);     // declare IN sensor as input
  pinMode(inputPinOut, INPUT);    // declare OUT sensor as input
}

//function prototype
void showVolts();     //show voltage
void showTemp();      //show temperature
void showCount();     //show count
void takeTemp();      //take temperature
void takeCount();     //take count

//main function
void loop()
{
  showVolts();
  showTemp();
  showCount();
} 

void showVolts()
{
  int batteryValue = analogRead(A0); //read the A0 pin value
  float voltage = batteryValue * (5.00 / 1023.00) * 2; // Formula to get true voltage
  lcd.setCursor(0,3);
  lcd.print("Voltage = ");
  lcd.print(voltage); //print the voltage to LCD
  lcd.print("V");
  if (voltage < 6.50) //set the voltage considered low battery here
  {
    digitalWrite(led_pin, HIGH);
    lcd.print(" LOW");
  }
  else
  {
    lcd.print(" OK ");
  }
}

void showTemp()
{ 
  double calibratedTemp = userTemp + 4;
  lcd.setCursor(0,0);
  lcd.print("Forehead within 6in.");
  takeTemp();

  //display temperature
  lcd.setCursor(0,1);
  lcd.print("Temp = ");

  //control displaying for >100
  if (calibratedTemp < 100) {
    lcd.print(calibratedTemp);
    lcd.print("F  ");
  }
  else {
    lcd.print(calibratedTemp);
    lcd.print("F ");
  }
  

  //check if the temperature is acceptable
  lcd.setCursor(15,1);
  if (userTemp >= 100.4)
   {
     digitalWrite(led_pin, HIGH);
     lcd.print("FEVER");
   }
  else
   {
     //digitalWrite(led_pin, LOW);
     lcd.print("OK   ");
   }
}

void takeTemp()
{
  userTemp = mlx.readObjectTempF();
}

void showCount()
{
  //display the count
  lcd.setCursor(0,2);
  lcd.print("Count = ");

  //control displaying for 1 digit
  if (count < 10) {
    lcd.print(count);  
    lcd.print("     ");
  }
  else {
    lcd.print(count);
  }
   
  takeCount();      //call takeCount function to take count

  //max capacity check
  lcd.setCursor(14,2);
  if (count >= MAXCOUNT) {        //max capacity reached
    lcd.print("WAIT! ");
  }
  else {                          //max capacity not reached
    lcd.print("HELLO!");
  }
}

void takeCount() {
  valIn = digitalRead(inputPinIn);  // read input IN value
  valOut = digitalRead(inputPinOut);  // read input OUT value

  ////PIR IN sensor
  if (valIn == HIGH) {            // check if IN sensor detects motion
    if (pirStateIn == LOW) {
      // IN sensor turned on
      count++;              //count up the capacity
      
      // We only want to print on the output change, not state
      pirStateIn = HIGH;
    }
  } else {
    if (pirStateIn == HIGH){
      // IN sensor turned off
      // We only want to print on the output change, not state
      pirStateIn = LOW;
    }
  }

  //PIR OUT sensor
  if (valOut == HIGH) {            // check if OUT sensor detects motion
    if (pirStateOut == LOW) {
      // OUT sensor turned on 
      if (count > 0) {            //count down the capacity only when count > 0
        count--;              
       
        // We only want to print on the output change, not state
        pirStateOut = HIGH;
      }
    }
  } else {
    if (pirStateOut == HIGH){
      // OUT sensor turned off
      // We only want to print on the output change, not state
      pirStateOut = LOW;
    }
  }
}
