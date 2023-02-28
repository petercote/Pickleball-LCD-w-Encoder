//Encoder and Button Debounce Works
//All menu ranges work, each can be selected/unselected
//Menu Values save
//LCD page scroll works
//LCD cursor moves and can select/unselect menu items
//submenu values change with the encoder
//values recall except for the first time a submenu is selected.

//DECLARE INCLUDED LIBRARIES
#include "Arduino.h"
#include "NewEncoder.h"
//20x4 LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //SDA = B7[A4], SCL = B6[A5] STM32/[Arduino]
LiquidCrystal_I2C lcd(0x27, 20, 4);

//MENU & MACHINE VALUE VARIABLES
int16_t mainMenuValue;          //integer to store main menu value
int16_t speedValue;             //integer to store speed value
int16_t feedValue;              //integer to store feed value
int16_t spinValue;              //integer to store spin value
int16_t loftValue;              //integer to store loft value
int16_t oscillationValue;       //integer to store oscillation value
int16_t oscillationRangeValue;  //integer to store oscillation range value
int16_t oscillationSpeedValue;  //integer to store oscillation speed value
int16_t drillsValue;            //integer to store drills value

// Define a struct to store encoder settings (upper and lower limits and starting position)
struct EncoderSettings {
  int16_t low;    // lower limit of encoder values
  int16_t high;   // upper limit of encoder values
  int16_t start;  // starting position of encoder
};

// Define an index to keep track of current encoder setting
uint16_t settingIndex = 0;
// Define variable for encoder value
int16_t currentValue;

// Define an array of encoder settings
EncoderSettings settings[] = {
  { 1, 8, mainMenuValue },           // Setting 0: Main Menu lower limit 1, upper limit 8, starting position 1
  { 1, 50, speedValue },             // Setting 1: Speed Menu lower limit 1, upper limit 50, starting position 10
  { 0, 30, feedValue },              // Setting 2: Feed Menu lower limit 0, upper limit 30, starting position 3
  { -20, 20, spinValue },            // Setting 3: Spin Menu lower limit -20, upper limit 20, starting position 0
  { 0, 99, loftValue },              // Setting 4: Loft Menu lower limit 0, upper limit 99, starting position 50
  { 0, 1, oscillationValue },        // Setting 5: Oscillation Menu lower limit 0, upper limit 1, starting position 0
  { 1, 20, oscillationRangeValue },  // Setting 6: Oscillation Range Menu lower limit 1, upper limit 20, starting position 1
  { 1, 15, oscillationSpeedValue },  // Setting 7: Oscillation Speed Menu lower limit 1, upper limit 15, starting postion 3
  { 0, 2, drillsValue }              // Setting 8: Drills lower limit 0, upper limit 1, starting position 0
};

//Create a NewEncoder object with pin assignments and initial settings from the settings array
NewEncoder encoder(5, 18, settings[settingIndex].low, settings[settingIndex].high, settings[settingIndex].start, FULL_PULSE);

int16_t prevEncoderValue;        //Initialize a variable to store the previous encoder value
const uint8_t button = 19;       //Define the button pin and initialize time variables
uint8_t debounceThreshold = 20;  //Set the debounce threshold for the button

//MENU SELECTED TRUE OR FALSE VARIABLES
bool mainMenuSelected = true;           // initialize that main menu is selected
bool speedSelected = false;             // initialize that speed menu is not selected
bool feedSelected = false;              // initialize that feed menu is not selected
bool spinSelected = false;              // initialize that spin menu is not selected
bool loftSelected = false;              // initialize that loft menu is not selected
bool oscillationSelected = false;       // initialize that oscillation menu is not selected.
bool oscillationRangeSelected = false;  // initialize that oscillation range menu is not selected
bool oscillationSpeedSelected = false;  // initialize that oscillation speed menu is not selected
bool drillsSelected = false;            // initialize that drills menu is not selected
bool subMenu = false;

void printPg1TextLCD() {  // PRINTS MENU TEXT FROM PAGE 1 TO LCD
  Serial.println("Print Page 1 Executed");
  lcd.clear();
  lcd.setCursor(1, 0);  //2nd block, 1st line
  lcd.print("Speed:");  //text
  //----------------------
  lcd.setCursor(1, 1);  //3rd line, 2nd block
  lcd.print("Feed:");   //text
  //----------------------
  lcd.setCursor(1, 2);  //2nd line, 2nd block
  lcd.print("Spin:");   //text
  //----------------------
  lcd.setCursor(1, 3);  //4th line, 2nd block
  lcd.print("Loft:");   //text
}
void printPg2TextLCD() {  // PRINTS MENU TEXT FROM PAGE 2 TO LCD
  Serial.println("Print Page 2 Executed");
  lcd.clear();
  lcd.setCursor(1, 0);  //2nd block, 1st line
  lcd.print("Osc:");    //text
  //----------------------
  lcd.setCursor(1, 1);      //2nd line, 2nd block
  lcd.print("Osc Range:");  //text
  //----------------------
  lcd.setCursor(1, 2);      //3rd line, 2nd block
  lcd.print("Osc Speed:");  //text
  //----------------------
  lcd.setCursor(1, 3);   //4th line, 2nd block
  lcd.print("Drills:");  //text
}
void printCursorPositionLCD() {  //Updates and prints cursor position to LCD in the main menu
  if (mainMenuSelected == true) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print(" ");
    switch (currentValue) {
      case 1:
        lcd.setCursor(0, 0);
        lcd.print(">");
        break;
      case 2:
        lcd.setCursor(0, 1);
        lcd.print(">");
        break;
      case 3:
        lcd.setCursor(0, 2);
        lcd.print(">");
        break;
      case 4:
        lcd.setCursor(0, 3);
        lcd.print(">");
        break;
      case 5:
        lcd.setCursor(0, 0);
        lcd.print(">");
        break;
      case 6:
        lcd.setCursor(0, 1);
        lcd.print(">");
        break;
      case 7:
        lcd.setCursor(0, 2);
        lcd.print(">");
        break;
      case 8:
        lcd.setCursor(0, 3);
        lcd.print(">");
        break;
    }
  }
}
void printPg1MachineValuesLCD() {  //PRINTS SUBMENU VALUES FROM PAGE 1 TO LCD
  //SPEED
  lcd.setCursor(7, 0);
  lcd.print("  ");
  lcd.setCursor(7, 0);
  lcd.print(speedValue);
  //FEED
  lcd.setCursor(7, 1);
  lcd.print("  ");
  lcd.setCursor(7, 1);
  lcd.print(feedValue);
  //SPIN
  lcd.setCursor(7, 2);
  lcd.print("   ");
  lcd.setCursor(7, 2);
  lcd.print(spinValue);
  //LOFT
  lcd.setCursor(7, 3);
  lcd.print("  ");
  lcd.setCursor(7, 3);
  lcd.print(loftValue);
}
void printPg2MachineValuesLCD() {  //PRINTS SUBMENU VALUES FROM PAGE 2 TO LCD
  //OSCILLATION
  if (oscillationValue == 0) {
    lcd.setCursor(6, 0);
    lcd.print("   ");
    lcd.setCursor(6, 0);
    lcd.print("OFF");
  } else {
    lcd.setCursor(6, 0);
    lcd.print("   ");
    lcd.setCursor(6, 0);
    lcd.print("ON");
  }
  //OSCILLATION RANGE
  lcd.setCursor(12, 1);
  lcd.print("  ");
  lcd.setCursor(12, 1);
  lcd.print(oscillationRangeValue);
  //OSCILLATION SPEED
  lcd.setCursor(12, 2);
  lcd.print("   ");
  lcd.setCursor(12, 2);
  lcd.print(oscillationSpeedValue);
  //DRILLS
  switch (drillsValue) {
    case 0:
      lcd.setCursor(9, 3);
      lcd.print("          ");
      lcd.setCursor(9, 3);
      lcd.print("OFF");
      break;
    case 1:
      lcd.setCursor(9, 3);
      lcd.print("          ");
      lcd.setCursor(9, 3);
      lcd.print("CROSS DINK");
      break;
    case 2:
      lcd.setCursor(9, 3);
      lcd.print("          ");
      lcd.setCursor(9, 3);
      lcd.print("3RD DROPS");
      break;
  }
}
void subMenuSelected() {  //ASSIGNS TRUE TO SUBMENU THAT IS SELECTED BASED ON CURRENT VALUE WHEN BUTTON WAS PUSHED, MAKES ALL OTHER MENUS ARE ASSIGNED FALSE
  mainMenuSelected = false;
  subMenu = true;
  switch (currentValue) {
    case 1:  //SELECTS SPEED, DESELECTS EVERYTHING ELSE
      speedSelected = true;
      mainMenuSelected = false;
      feedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Speed is selected");
      break;
    case 2:  //SELECTS FEED, DESELECTS EVERYTHING ELSE
      feedSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Feed is selected");
      break;
    case 3:  //SELECTS SPIN, DESELECTS EVERYTHING ELSE
      spinSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Spin is selected");
      break;
    case 4:  //SELECTS LOFT, DESELECTS EVERYTHING ELSE
      loftSelected = true;
      spinSelected = false;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Spin is selected");
      break;
    case 5:  //SELECTS OSCILLATION, DESELECTS EVERYTHING ELSE
      oscillationSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Oscillation is selected");
      break;
    case 6:  //SELECTS OSCILLATION RANGE, DESELECTS EVERYTHING ELSE
      oscillationRangeSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationSpeedSelected = false;
      drillsSelected = false;
      Serial.println("Oscillation Range is selected");
      break;
    case 7:  //SELECTS OSCILLATION SPEED, DESELECTS EVERYTHING ELSE
      oscillationSpeedSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      drillsSelected = false;
      Serial.println("Oscillation Speed is selected");
      break;
    case 8:  //SELECTS DRILLS, DESELECTS EVERYTHING ELSE
      drillsSelected = true;
      mainMenuSelected = false;
      speedSelected = false;
      feedSelected = false;
      spinSelected = false;
      loftSelected = false;
      oscillationSelected = false;
      oscillationRangeSelected = false;
      oscillationSpeedSelected = false;
      Serial.println("Drills is selected");
      break;
  }
}
void deSelectSubMenus() {  // unSelects all sub menus, selects main menu
  subMenu = false;
  mainMenuSelected = true;
  speedSelected = false;
  feedSelected = false;
  spinSelected = false;
  loftSelected = false;
  oscillationSelected = false;
  oscillationRangeSelected = false;
  oscillationSpeedSelected = false;
  drillsSelected = false;
  Serial.println("All Sub Menus Unselected");
}
void cursorSelectionPrintLCD() {  //PRINTS AN "X" TO SUBMENU THAT WAS SELECTED. CLEARS PREVIOUS CURSOR ">"
  // lcd.setCursor(0, 0);
  // lcd.print(" ");
  // lcd.setCursor(0, 1);
  // lcd.print(" ");
  // lcd.setCursor(0, 2);
  // lcd.print(" ");
  // lcd.setCursor(0, 3);
  // lcd.print(" ");
  if (speedSelected == true) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("X");
  }
  if (feedSelected == true) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("X");
  }
  if (spinSelected == true) {
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print("X");
  }
  if (loftSelected == true) {
    lcd.setCursor(0, 3);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print("X");
  }
  if (oscillationSelected == true) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("X");
  }
  if (oscillationRangeSelected == true) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("X");
  }
  if (oscillationSpeedSelected == true) {
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print("X");
  }
  if (drillsSelected == true) {
    lcd.setCursor(0, 3);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print("X");
  }
}
void saveMachineValues() {  //SAVES CURRENT MENU VALUE TO VARIABLE FOR LATER RECALL
  if (mainMenuSelected == true) {
    mainMenuValue = currentValue;
    Serial.print("Main Menu Value is ");
    Serial.println(currentValue);
  }
  if (speedSelected == true) {
    speedValue = currentValue;
    Serial.print("Speed Value is ");
    Serial.println(speedValue);
  }
  if (feedSelected == true) {
    feedValue = currentValue;
    Serial.print("Feed Value is ");
    Serial.println(feedValue);
  }
  if (spinSelected == true) {
    spinValue = currentValue;
    Serial.print("Spin Value is ");
    Serial.println(spinValue);
  }
  if (loftSelected == true) {
    loftValue = currentValue;
    Serial.print("Loft Value is ");
    Serial.println(loftValue);
  }
  if (oscillationSelected == true) {
    oscillationValue = currentValue;
    Serial.print("Oscillation Value is ");
    Serial.println(oscillationValue);
  }
  if (oscillationRangeSelected == true) {
    oscillationRangeValue = currentValue;
    Serial.print("Oscillation Range is ");
    Serial.println(oscillationRangeValue);
  }
  if (oscillationSpeedSelected == true) {
    oscillationSpeedValue = currentValue;
    Serial.print("Oscillation Speed is ");
    Serial.println(oscillationSpeedValue);
  }
  if (drillsSelected == true) {
    drillsValue = currentValue;
    Serial.print("Drills value is ");
    Serial.println(drillsValue);
  }
}
void recallStoredMachineValues() {  //REASSIGNS NEW VALUE BASED ON SAVED VALUE
  if (mainMenuSelected == true) {
    currentValue = mainMenuValue;
  }
  if (speedSelected == true) {
    currentValue = speedValue;
  }
  if (feedSelected == true) {
    currentValue = feedValue;
  }
  if (spinSelected == true) {
    currentValue = spinValue;
  }
  if (loftValue == true) {
    currentValue = loftValue;
  }
  if (oscillationSelected == true) {
    currentValue = oscillationValue;
  }
  if (oscillationRangeSelected == true) {
    currentValue = oscillationRangeValue;
  }
  if (oscillationSpeedSelected == true) {
    currentValue = oscillationSpeedValue;
  }
  if (drillsSelected == true) {
    currentValue = drillsValue;
  }
}
void printValuesToSerialMonitor() {  //PRINTS ALL SAVED VALUES TO SERIAL MONITOR FOR DEBUGGING.  NON ESSENTIAL CODE
  Serial.print("Main Menu Value ");
  Serial.println(mainMenuValue);
  Serial.print("Speed Value ");
  Serial.println(speedValue);
  Serial.print("Feed Value ");
  Serial.println(feedValue);
  Serial.print("Spin Value ");
  Serial.println(spinValue);
  Serial.print("Loft Value ");
  Serial.println(loftValue);
  Serial.print("Oscillation Value ");
  Serial.println(oscillationValue);
  Serial.print("Oscillation Range Value ");
  Serial.println(oscillationRangeValue);
  Serial.print("Oscillation Speed Value ");
  Serial.println(oscillationSpeedValue);
  Serial.print("Drills Value ");
  Serial.println(drillsValue);
}
void setup() {
  NewEncoder::EncoderState state;  //Create a NewEncoder::EncoderState object to hold the state of the encoder

  Serial.begin(115200);  //Start serial communication

  delay(2000);  //Delay for 2 seconds

  Serial.println("Starting");

  //If the encoder fails to start, print an error message to the serial monitor and enter an infinite loop
  if (!encoder.begin()) {
    Serial.println("Encoder Failed to Start. Check pin assignments and available interrupts. Aborting.");
    while (1) {
      yield();
    }
  }
  //Otherwise, get the current state of the encoder and print the starting value to the serial monitor
  else {
    encoder.getState(state);
    Serial.print("Encoder Successfully Started at value = ");
    currentValue = state.currentValue;
    prevEncoderValue = state.currentValue;
    Serial.println(prevEncoderValue);
  }

  pinMode(button, INPUT_PULLUP);  //Set the button pin as an input with internal pull-up resistor

  speedValue = 10;            //initializes speedValue at 10;
  feedValue = 3;              //initializes feedValue at 3;
  spinValue = 0;              //initializes spinValue at 0;
  loftValue = 50;             //initializes loftValue at 50;
  oscillationValue = 0;       //initializes oscillationValue at 0;
  oscillationRangeValue = 1;  //initializes oscillationRangeValue at 1;
  oscillationSpeedValue = 3;  //initializes oscillationSpeedValue at 1;
  drillsValue = 0;            //initializes drillsValue at 0;
  mainMenuValue = 1;          //initializes mainMenuValue at 1;
  lcd.begin();                // initialize the lcd
  lcd.backlight();
  lcd.clear();  //clear the whole LCD
  printPg1TextLCD();
  printCursorPositionLCD();
  printPg1MachineValuesLCD();
}

void loop() {
  //Declare variables for current encoder value and encoder state
  NewEncoder::EncoderState currentEncoderState;
  uint32_t TimeSinceLastButtonReading;
  int buttonReading;
  uint32_t currentMillis = millis();
  static uint32_t TimeOfLastButtonReading = currentMillis;
  static int buttonState = digitalRead(button);

  //Calculate the time since the last button reading
  TimeSinceLastButtonReading = currentMillis - TimeOfLastButtonReading;  //If enough time has passed for debounce, read the button state
  if (TimeSinceLastButtonReading > debounceThreshold) {
    buttonReading = digitalRead(button);
    if (buttonReading != buttonState) {  //If the button state has changed, update the button state variable
      buttonState = buttonReading;
      if (buttonState == LOW) {                 //If the button is pressed, toggle through the different encoder settings
        encoder.getState(currentEncoderState);  //Get the current state of the encoder and set the setting index based on the current value
        currentValue = currentEncoderState.currentValue;
        if ((currentValue >= 0) && (currentValue < 9) && (subMenu == false)) {
          subMenuSelected();  //selects a sub menu based on the current value from the main menu
          cursorSelectionPrintLCD();
          settingIndex = currentValue;  //selects encoder range settings to match the subMenu
        } else {
          settingIndex = 0;    //sets encoder settings to range for main menu
          deSelectSubMenus();  //deselects all sub menus and selects main menu.
        }
        encoder.newSettings(settings[settingIndex].low, settings[settingIndex].high, settings[settingIndex].start, currentEncoderState);
        currentValue = currentEncoderState.currentValue;
        recallStoredMachineValues();  //stores machine values so they can be recalled when revisiting a submenu
        Serial.print("Encoder Settings Toggled to Index ");
        Serial.println(settingIndex);
        Serial.print("Updated Encoder Value: ");
        Serial.println(currentValue);
        prevEncoderValue = currentValue;  //saves encoder value for comparison to determine if the encoder has moved.
        printCursorPositionLCD();
      }
      TimeOfLastButtonReading = currentMillis;  //sets time of last button reading to ensure time has passed before a subsequent button reading occurs
    }
  }

  if (encoder.getState(currentEncoderState)) {
    currentValue = currentEncoderState.currentValue;
    if (currentValue != prevEncoderValue) {  //IF ENCODER HAS CHANGED POSITION FROM PREVIOUS SAVED POSIITION, ENTER LOOP
      saveMachineValues();
      Serial.println(currentValue);
      settings[settingIndex].start = currentValue;
      prevEncoderValue = currentValue;
      if ((mainMenuSelected == true) && (currentValue < 5)) {
        printPg1TextLCD();
        printPg1MachineValuesLCD();
      }
      if ((mainMenuSelected == true) && (currentValue > 4)) {
        printPg2TextLCD();
        printPg2MachineValuesLCD();
      }
      printCursorPositionLCD();
      cursorSelectionPrintLCD();
      if ((speedSelected == true) || (spinSelected == true) || (feedSelected == true) || (loftSelected == true)) {
        printPg1MachineValuesLCD();
      }
      if ((oscillationSelected == true) || (oscillationRangeSelected == true) || (oscillationSpeedSelected == true) || (drillsSelected == true)) {
        printPg2MachineValuesLCD();
      }
    } else {
      switch (currentEncoderState.currentClick) {
        case NewEncoder::UpClick:
          Serial.println("at upper limit.");
          break;

        case NewEncoder::DownClick:
          Serial.println("at lower limit.");
          printValuesToSerialMonitor();
          break;

        default:
          break;
      }
    }
  }
}
