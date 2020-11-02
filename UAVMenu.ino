//diagnostic testing subsystem

#include <LiquidCrystal.h>

const int rs = 22, en = 23, d4 = 9, d5 = 10, d6 = 11, d7 = 12;  //the lcd pin initialisation
LiquidCrystal lcd (rs, en, d4, d5, d6, d7);


//Pin initialisation
const int ButtonPin1 = 3;
const int ButtonPin2 = 4;
const int B = 5;
const int G = 6;
const int R = 7;
const int Buzzer = 8;

//Global Variable Initialisation
int x_axis = A0;
int y_axis = A1;
int enter = 2;
int selector = 0;
int X = 512;
int Y = 512;
int OK = 1;
bool LCDSTATE = 0;
bool check = false;
bool check2 = false;

String Mode[5] = {"IMU test", "SV test", "Motor test", "WiFi test", "Exit test"}; //Setting the menu options


bool ButtonState[3] = {};     //Array tracking the state of the buttons
int counter [3] = {};         //Array counting the number of times each button was pressed


//Function initialisation
void loop ();
void LCD (bool setup);
void Buzzerfunction ();
void ButtonCounter ();
void Button (int index);
bool DebounceButton (bool state, int index);
void JoyStick ();
void GOUP ();
void GODOWN ();
void SELECT ();
void Display ();
void RGBFunction ();
void StopLEDS();

void setup ()
{
  Serial.begin (9600);
  pinMode (ButtonPin1, INPUT);
  pinMode (ButtonPin2, INPUT);
  pinMode (R, OUTPUT);
  pinMode (G, OUTPUT);
  pinMode (B, OUTPUT);
  pinMode (Buzzer, OUTPUT);
  pinMode (x_axis, INPUT);
  pinMode (y_axis, INPUT);
  pinMode (enter, INPUT);
  digitalWrite (enter, HIGH);
  lcd.begin (16, 2);
  lcd.noDisplay ();
}
//Starting 3 main functions
void loop ()
{
  LCD (1);
  JoyStick ();
  ButtonCounter ();
}

void LCD (bool setup)
{ 
  if (counter [1]%2 && !check2) {lcd.clear (); lcd.display (); lcd.print ("Test Mode ON"), delay (2000), lcd.clear (), Display (), check2 = true; check = false;}  //sets up the start-up button (depending on how many times the button has been pressed)
  if (counter [1]%2 == 0) {lcd.noDisplay (); StopLEDS (); delay (500); check = false; check2 = false;}              
  if (Y > 1000 && counter [1]%2 && !check) {                                                   //checks if there is a upwards joystick input and whether the display is on and whether the user is in the main menu
    Buzzerfunction ();                                                                         //Buzz sound
    delay (500);                                                                               //Small delay to prevent any component interference
    if (selector > 0) selector--;                                                              //Selector is the variable which is used to navigate the menu
    else selector = 4; 
    Display ();}                                                                               //Displays the current options
    
  else if (Y < 50 && counter [1]%2 && !check) {                                                //Same approach for the downward movement
    Buzzerfunction ();
    delay (500);
    if (selector < 4) selector++;
    else selector = 0;
    Display ();}
  
  else if (!OK && counter [1]%2 && !check) {delay (500), check = !check, SELECT ();}            //Checks if there has been a click and if so, marks that the user is no longer inside the main menu
  else if (!OK && counter [1]%2 && check) {Buzzerfunction (); Display (); StopLEDS (); check = !check;}   //Returns the user to the main menu if he clicks and is currently in a different menu
}
//Joystick "Click" function
void SELECT ()                                                                                  
{
  Serial.println (selector);
  int shadow = selector;        //this variable records the previous value of the menu selector so as not to lose by accidentally inputing a joystick command whilist in a different menu
  Buzzerfunction ();            //Buzz
  switch (selector){            //Depending on the current menu option, the tests will be initialised
    case 0: {lcd.clear (); lcd.print ("IMU Testing..."); delay (100); RGBFunction (); break;}
    case 1: {lcd.clear (); lcd.print ("SV Testing..."); delay (100); RGBFunction (); break;}
    case 2: {lcd.clear (); lcd.print ("Motor Testing..."); delay (100); RGBFunction (); break;}
    case 3: {lcd.clear (); lcd.print ("WiFi Testing..."); delay (100); RGBFunction (); break;}
    case 4: {lcd.clear (); lcd.clear (); lcd.print ("Goodbye!"); delay (1000); lcd.clear (); selector = 0; Display(); lcd.noDisplay(); counter [1]++; check2 = false; break;} //Closes the Tester and resets the variables
  }
  selector = shadow;            //returns the value recorded value to the menu index
}

void Display ()                 //Function to display the current options
{
  StopLEDS();                   //make sure to turn off any stray LED's
  //Serial.println (selector);
  lcd.clear ();
  lcd.setCursor (0, 0);
  lcd.print ("> " + Mode [selector]);
  lcd.setCursor (0, 1);
  if (selector < 4)
  lcd.print (Mode [selector+1]);
  else lcd.print (Mode [0]);
}

void RGBFunction ()            //RGB requested setup
{
  bool error = false;
  for (int i = 1; i<=10; i++) {
  ButtonCounter ();
  if (counter [2] % 2) error = true; 
  digitalWrite (B, LOW);
  delay (200);
  digitalWrite (B, HIGH);
  delay (200);
  }
  StopLEDS ();
  if (error) {digitalWrite (R, HIGH); lcd.clear (); lcd.print ("System Not"); lcd.setCursor (8, 1); lcd.print ("Working!"); lcd.setCursor (0,0);}
  else {digitalWrite (G, HIGH); lcd.clear (); lcd.print ("System Ready!");}
  counter [2] = 0;
}

void StopLEDS () //Stops the RGB
{
  digitalWrite (R, LOW);
  digitalWrite (G, LOW);
  digitalWrite (B, LOW);
}

void JoyStick ()      //Joystick function with additional commands to visualise the input values, in order to keep track of them
{
  X = analogRead (x_axis);
  Y = analogRead (y_axis);
  OK = digitalRead (enter);
  delay (100);
  Serial.println ("Switch: ");
  Serial.println (OK);
  Serial.println ("\n");
  Serial.println ("X Axis: ");
  Serial.println (X);
  Serial.println ("\n");
  Serial.println ("Y axis: ");
  Serial.println (Y);
  Serial.println ("\n");
  delay (500);
}

void ButtonCounter ()   //Starts the two individual button counters
{
  Button(1);
  Button(2);
}

void Button (int index)     //Single function to count the buttons for both buttons, using arrays and index
{
  //Serial.println (ButtonState [index]);
  if (DebounceButton (ButtonState [index], index) == true && ButtonState [index] == false)
    {
      ButtonState[index] = true;
      counter[index]++;
      //Serial.println (counter[index]);
    }
  else if (DebounceButton (ButtonState[index], index) == false && ButtonState [index] == true)
  {
    ButtonState [index] = false;
  }
}

bool DebounceButton (bool state, int index)             //Debounce function used previously
{
  bool stateNow;
  if (index == 1) stateNow = digitalRead(ButtonPin1);
  else stateNow = digitalRead(ButtonPin2);
  if (state != stateNow)                            //debounces the value with a 10 milisecond range
  {
    delay(10);
    if (index == 1)stateNow = digitalRead(ButtonPin1);
    else stateNow = digitalRead(ButtonPin2);
  }
  return stateNow;
}

void Buzzerfunction ()                                  //Buzz function
{
  tone (Buzzer, 1000);
  delay (100);
  noTone (Buzzer);
}
