#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <String.h>

#define STATE_WELCOME_MESSAGE 0
#define STATE_MENU 1
#define STATE_MENU_SETTINGS 2
#define STATE_HIGHSCORE 3
#define STATE_ABOUT 4
#define STATE_HOW_TO_PLAY 5
#define STATE_GAME 6
#define STATE_SET_DIFFICULTY 7
#define STATE_SET_LCD_CONTRAST 8
#define STATE_SET_LCD_LIGHT 9
#define STATE_SET_MATRIX_LIGHT 10
#define STATE_SET_SOUND_VOLUME 11
#define STATE_RESET_HIGHSCORES 12
#define STATE_INITIALIZE_GAME 13 
#define STATE_END_GAME 14

#define MATRIX_SIZE 8
#define MATRIX_BRIGHTNESS 2
#define MATRIX_LIGHT_ITEMS_COUNT 6

#define STATE_WELCOME_MESSAGE_DURATION 5000

#define MENU_ITEMS_COUNT 6
#define SETTINGS_MENU_ITEMS_COUNT 8
#define LCD_LIGHT_ITEMS_COUNT 7
#define DIFFICULTY_ITEMS_COUNT 5
#define SOUND_VOLUME_ITEMS_COUNT 4
#define HIGHSCORES_SAVED_COUNT 5
#define HIGHSCORES_MENU_ITEMS_COUNT 7

#define SW_PRESSED_STATE 1
#define SW_RELEASED_STATE 0

#define LEFT 1
#define RIGHT 2
#define DOWN 3
#define UP 4

#define JOYSTICK_MIN_TRESHOLD 350
#define JOYSTICK_MAX_TRESHOLD 750

#define LCD_LIGHT_MIN_VALUE 250
#define LCD_LIGHT_MAX_VALUE 50

#define MATRIX_LIGHT_MIN_VALUE 2
#define MATRIX_LIGHT_MAX_VALUE 15

#define SNAKE_SPEED_EASY_LEVEL 500 
#define SNAKE_SPEED_MEDIUM_LEVEL 350
#define SNAKE_SPEED_HARD_LEVEL 200 

#define SOUND_MENU 200
#define SOUND_MENU_DURATION 100
#define SOUND_GAME_OVER 400
#define SOUND_GAME_OVER_DURATION 500
#define SOUND_EATING 600
#define SOUND_EATING_DURATION 100

#define GAME_OVER_SCREEN_DURATION 1500

const byte pinSW = 2;
byte swState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;

unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;

const byte pinX = A0;
const byte pinY = A1;

short xValue = 0;
short yValue = 0;

bool joystickMoved = false;

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 13;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte arrowUp[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

const byte arrowDown[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};

const byte currentSelectionPointer[8] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};
 
unsigned long welcomeMessageTimer = millis();

const char* menuItems[] = {"Menu", "1.Play", "2.Highscore", "3.Settings", "4.About", "5.How to play"};

byte menuIndex = 0;
byte selectedMenuIndex = 0;

const char* settingsItems[] = {"Settings", "1.Difficulty", "2.LCD contrast", "3.LCD light", "4.Matrix light", "5.Sound", "6.Reset HS", "7.Back to menu"};

byte settingsMenuIndex = 0;
byte selectedSettingsMenuIndex = 0;

const char* aboutItems[] = {"Lolot Bogdan", "Github:LSBogdan"};

const char* lcdLightItems[] = {"LCD Light", "1.Verry low", "2.Low", "3.Medium", "4.High", "5.Verry high", "6.Back"};

const byte lcdLightPin = 5;

byte lcdLight = 50;

byte lightLCDIndex = 0;
byte selectedLightLCDIndex = 0;

const char* matrixLightItems[] = {"Matrix Light", "1.Low", "2.Medium", "3.High", "4.Verry high", "5.Back"};

byte matrixLight = 2;

byte lightMatrixIndex = 0;
byte selectedLightMatrixIndex = 0;

const char* difficultySetItems[] = {"Difficulty", "1.Easy :)", "2.Medium :|", "3.Hard :(", "4.Back"};

byte difficultyItemsIndex = 0;
byte selectedDifficultyItemsIndex = 0;

const char* soundSetItems[] = {"Sound", "1.ON", "2.OF", "3.Back"};

byte soundItemsIndex = 0;
byte selectedSoundItemsIndex = 0;

byte matrix[MATRIX_SIZE][MATRIX_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

short moveInterval = 500;
unsigned long long lastMoved = 0;

bool matrixChanged = true;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

byte newRandomCol = 0;
byte newRandomRow = 0;

short currentScore = 0;

struct Settings {
	
	byte difficulty;
	byte lcdContrast;
	byte lcdLight;
	byte matrixLight;
	bool soundStatus;

} settingsData;

struct Highscores {

  char highscores[5][13];

} highscoresData;

char* highscoresItems[] = {
    "Highscores",
    highscoresData.highscores[0],
    highscoresData.highscores[1],
    highscoresData.highscores[2],
    highscoresData.highscores[3],
    highscoresData.highscores[4],
    "Back"
  };

byte highscoresItemsIndex = 0;
byte selectedHighscoresItemsIndex = 0;

byte currentState = STATE_WELCOME_MESSAGE;

struct Point {

  byte row;
  byte col;

} snake[MATRIX_SIZE * MATRIX_SIZE];

bool processedMove = false;

short xSnakePos;
short ySnakePos;

byte xJoystickSnake;
byte yJoystickSnake;

Point food;

byte snakeDotsCount = 0;

byte gameDifficulty = 1;

const byte buzzerPin = 3;
bool soundIsActive;

void setup() {

  Serial.begin(9600);

  pinMode(pinSW, INPUT_PULLUP);
  pinMode(lcdLightPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);


  writeDemoHighscores();
  
  readFromStorage();

  analogWrite(lcdLightPin, settingsData.lcdLight);

  lc.shutdown(0, false);
  lc.setIntensity(0, settingsData.matrixLight);
  lc.clearDisplay(0);
 
  lcd.begin(16, 2);
 
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.createChar(2, currentSelectionPointer);

}


void readFromStorage() {

  short index = 0;
  
  EEPROM.get(index, settingsData);  
  index += sizeof(settingsData);
  
  EEPROM.get(index, highscoresData);
}

void writeSettingsInStorage() {
  EEPROM.put(0, settingsData);
}

void writeHighscoresInStorage() {
  short index = sizeof(settingsData);
  EEPROM.put(index, highscoresData);
}

void displayWelcomeMessage() {

  String messageToDisplay = "You play the";

  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay);

  messageToDisplay = "BEST SNAKE EVER";

  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay);

  if (millis() - welcomeMessageTimer > STATE_WELCOME_MESSAGE_DURATION) {  
    currentState = STATE_MENU;
    lcd.clear();
  }
}

void processSWState() {

  getSWState();

  if (currentState == STATE_END_GAME) {

    byte newDirection = getJoystickState();

    if (newDirection == 0) {
      return;
    }

    lcd.clear();
    currentState = STATE_MENU;
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_MENU) {

    if (selectedMenuIndex == 1) {
       lcd.clear();
       currentState = STATE_INITIALIZE_GAME;
       swState = SW_RELEASED_STATE;
       return;
    }

    if (selectedMenuIndex == 2) {
      lcd.clear();
      currentState = STATE_HIGHSCORE;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedMenuIndex == 3) {
      lcd.clear();
      currentState = STATE_MENU_SETTINGS;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedMenuIndex == 4) {
      lcd.clear();
      currentState = STATE_ABOUT;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedMenuIndex == 5) {
      lcd.clear();
      currentState = STATE_HOW_TO_PLAY;
      swState = SW_RELEASED_STATE;
      return;
    }
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_MENU_SETTINGS) {

    if (selectedSettingsMenuIndex == 1) {
      lcd.clear();
      currentState = STATE_SET_DIFFICULTY;
      swState = SW_RELEASED_STATE;
      return;
    }
   
    if (selectedSettingsMenuIndex == 2) {
      lcd.clear();
      currentState = STATE_SET_LCD_CONTRAST;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedSettingsMenuIndex == 3) {
      lcd.clear();
      currentState = STATE_SET_LCD_LIGHT;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedSettingsMenuIndex == 4) {
      lcd.clear();
      currentState = STATE_SET_MATRIX_LIGHT;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedSettingsMenuIndex == 5) {
      lcd.clear();
      currentState = STATE_SET_SOUND_VOLUME;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedSettingsMenuIndex == 6) {
      lcd.clear();
      currentState = STATE_RESET_HIGHSCORES;
      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedSettingsMenuIndex == 7) {
      lcd.clear();
      currentState = STATE_MENU;
      swState = SW_RELEASED_STATE;
      return;
    }
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_ABOUT) {
    lcd.clear();
    currentState = STATE_MENU;
    swState = SW_RELEASED_STATE;
    return;
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_HOW_TO_PLAY) {
   
    lcd.clear();
    currentState = STATE_MENU;
    swState = SW_RELEASED_STATE;
    return;

  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_HIGHSCORE) {
   
    if (selectedHighscoresItemsIndex == 6) {
    lcd.clear();
    currentState = STATE_MENU;
    swState = SW_RELEASED_STATE;
    return;
    }

    if (!strlen(highscoresData.highscores[0])) {
      lcd.clear();
      currentState = STATE_MENU;
      swState = SW_RELEASED_STATE;
      return;
    }
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_GAME) {
   
    lcd.clear();
    currentState = STATE_MENU;
    swState = SW_RELEASED_STATE;
    return;
   
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_SET_DIFFICULTY) {

    if (selectedDifficultyItemsIndex == 1) {
      lcd.clear();
      gameDifficulty = 1;
      swState = SW_RELEASED_STATE;

      settingsData.difficulty = gameDifficulty;
      writeSettingsInStorage();
      return;
    }

    if (selectedDifficultyItemsIndex == 2) {
      lcd.clear();
      gameDifficulty = 2; 
      swState = SW_RELEASED_STATE;

      settingsData.difficulty = gameDifficulty;
      writeSettingsInStorage();
      return;
    }

    if (selectedDifficultyItemsIndex == 3) {
      lcd.clear();
      gameDifficulty = 3;
      swState = SW_RELEASED_STATE;

      settingsData.difficulty = gameDifficulty;
      writeSettingsInStorage();
      return;
    }

    if (selectedDifficultyItemsIndex == 4) {
      lcd.clear();
      currentState = STATE_MENU_SETTINGS;
      swState = SW_RELEASED_STATE;
      return;
    }

  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_SET_LCD_CONTRAST) {
    lcd.clear();
    currentState = STATE_MENU_SETTINGS;
    swState = SW_RELEASED_STATE;
    return;
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_RESET_HIGHSCORES) {
    lcd.clear();
    currentState = STATE_MENU_SETTINGS;
    swState = SW_RELEASED_STATE;
    return;
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_SET_LCD_LIGHT) {

    if (selectedLightLCDIndex == 1) {
      lcd.clear();
      lcdLight = 50;
      analogWrite(lcdLightPin, lcdLight);

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();

      swState = SW_RELEASED_STATE;
      return;
    }

    if (selectedLightLCDIndex == 2) {
      lcd.clear();
      lcdLight = 100;
      analogWrite(lcdLightPin, lcdLight);
      swState = SW_RELEASED_STATE;

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();

      return;
    }
   
    if (selectedLightLCDIndex == 3) {
      lcd.clear();
      lcdLight = 150;
      analogWrite(lcdLightPin, lcdLight);

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();

      swState = SW_RELEASED_STATE;
      
      return;
    }

    if (selectedLightLCDIndex ==4) {
      lcd.clear();
      lcdLight = 200;
      analogWrite(lcdLightPin, lcdLight);
      swState = SW_RELEASED_STATE;

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();
      
      return;
    }
   
    if (selectedLightLCDIndex == 5) {
      lcd.clear();
      lcdLight = 250;
      analogWrite(lcdLightPin, lcdLight);
      swState = SW_RELEASED_STATE;

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();

      return;
    }

    if (selectedLightLCDIndex == 6) {
      lcd.clear();
      currentState = STATE_MENU_SETTINGS;
      swState = SW_RELEASED_STATE;

      settingsData.lcdLight = lcdLight;
      writeSettingsInStorage();

      return;
    }
   
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_SET_MATRIX_LIGHT) {

    if (selectedLightMatrixIndex == 1) {
      matrixLight = 2;
      lc.setIntensity(0, matrixLight);
      swState = SW_RELEASED_STATE;

      settingsData.matrixLight = matrixLight;
      writeSettingsInStorage();

      return;
    }

    if (selectedLightMatrixIndex == 2) {
      matrixLight = 5;
      lc.setIntensity(0, matrixLight);
      swState = SW_RELEASED_STATE;

      settingsData.matrixLight = matrixLight;
      writeSettingsInStorage();

      return;
    }

    if (selectedLightMatrixIndex == 3) {
      matrixLight = 10;
      lc.setIntensity(0, matrixLight);
      swState = SW_RELEASED_STATE;

      settingsData.matrixLight = matrixLight;
      writeSettingsInStorage();

      return;
    }

    if (selectedLightMatrixIndex == 4) {
      matrixLight = 15;
      lc.setIntensity(0, matrixLight);
      swState = SW_RELEASED_STATE;

      settingsData.matrixLight = matrixLight;
      writeSettingsInStorage();
      
      return;
    }

    if (selectedLightMatrixIndex == 5) {
      lcd.clear();
      currentState = STATE_MENU_SETTINGS;
      swState = SW_RELEASED_STATE;
      return;
    }
  }

  if (swState == SW_PRESSED_STATE && currentState == STATE_SET_SOUND_VOLUME) {

    if (selectedSoundItemsIndex == 1) {
      lcd.clear();
      soundIsActive = true;
      swState = SW_RELEASED_STATE;

      settingsData.soundStatus = soundIsActive;
      writeSettingsInStorage();

      return;
    }

    if (selectedSoundItemsIndex == 2) {
      lcd.clear();
      soundIsActive = false;
      swState = SW_RELEASED_STATE;

      settingsData.soundStatus = soundIsActive;
      writeSettingsInStorage();
      return;
    }

    if (selectedSoundItemsIndex == 3) {
      lcd.clear();
      currentState = STATE_MENU_SETTINGS;
      swState = SW_RELEASED_STATE;
      return;
    }
  }

}

byte getSWState() {

  swState = !digitalRead(pinSW);
 
  if (swState != lastReading) {
    return SW_PRESSED_STATE;
  }

   lastReading = swState;
   return SW_RELEASED_STATE;
}

void readAxes() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
}

byte getJoystickState() {

  readAxes();

  if (xValue < JOYSTICK_MIN_TRESHOLD && !joystickMoved) {
    joystickMoved = true;
    return RIGHT;
  }

  if (xValue > JOYSTICK_MAX_TRESHOLD && !joystickMoved) {
    joystickMoved = true;
    return LEFT;
  }

  if (yValue < JOYSTICK_MIN_TRESHOLD && !joystickMoved) {
    joystickMoved = true;
    return UP;
  }

  if (yValue > JOYSTICK_MAX_TRESHOLD && !joystickMoved) {
    joystickMoved = true;
    return DOWN;
  }

  if (xValue >= JOYSTICK_MIN_TRESHOLD && xValue <= JOYSTICK_MAX_TRESHOLD && yValue >= JOYSTICK_MIN_TRESHOLD && yValue <= JOYSTICK_MAX_TRESHOLD) {
    joystickMoved = false;
    return 0;
  }

  return 0;
}

void menuProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedMenuIndex = newDirection == DOWN ? selectedMenuIndex + 1 : selectedMenuIndex - 1;
  selectedMenuIndex = constrain(selectedMenuIndex, 0, MENU_ITEMS_COUNT  - 1);

  if (newDirection == DOWN && selectedMenuIndex % 2 == 0) {
    menuIndex = selectedMenuIndex;
  }
  else if (newDirection == UP && selectedMenuIndex % 2 == 1) {
    menuIndex = selectedMenuIndex - 1;
  }

  menuIndex = constrain(menuIndex, 0, MENU_ITEMS_COUNT - 1);

  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  } 
}

void displayMenu() {
 
 
  lcd.setCursor(0, 0);  
  lcd.print(menuItems[menuIndex]);

  if (menuIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }


  if (menuIndex != MENU_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(menuItems[menuIndex + 1]);
  }

  if (menuIndex < MENU_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedMenuIndex) {
    byte currentLinePosition = selectedMenuIndex % 2;
    byte currentLineLength = strlen(menuItems[selectedMenuIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();
 
 
  menuProcessJoystickState();

  readFromStorage();
}

void settingsMenuProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedSettingsMenuIndex = newDirection == DOWN ? selectedSettingsMenuIndex + 1 : selectedSettingsMenuIndex - 1;
  selectedSettingsMenuIndex = constrain(selectedSettingsMenuIndex, 0, SETTINGS_MENU_ITEMS_COUNT - 1);

  if (newDirection == DOWN && selectedSettingsMenuIndex % 2 == 0) {
    settingsMenuIndex = selectedSettingsMenuIndex;
  }

  else if (newDirection == UP && selectedSettingsMenuIndex% 2 == 1) {
    settingsMenuIndex = selectedSettingsMenuIndex - 1;
  }

  settingsMenuIndex = constrain(settingsMenuIndex, 0, SETTINGS_MENU_ITEMS_COUNT - 1);
 
  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }
}

void displaySettingsMenu() {

  lcd.setCursor(0, 0);
  lcd.print(settingsItems[settingsMenuIndex]);

  if (settingsMenuIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }  

  if (settingsMenuIndex != SETTINGS_MENU_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(settingsItems[settingsMenuIndex + 1]);
  }
 
  if (settingsMenuIndex < SETTINGS_MENU_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedSettingsMenuIndex) {
    byte currentLinePosition = selectedSettingsMenuIndex % 2;
    byte currentLineLength = strlen(settingsItems[selectedSettingsMenuIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();

  settingsMenuProcessJoystickState();
 
}

void displayAbout() {

  lcd.setCursor(2, 0);
  lcd.print(aboutItems[0]);

  lcd.setCursor(0, 1);
  lcd.print(aboutItems[1]);

  processSWState();

}

void displayHowToPlay() {


  String messageToDisplay = "Run after food";

  lcd.setCursor(1, 0);
  lcd.print(messageToDisplay);

  processSWState();
}

void highscoresProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedHighscoresItemsIndex = newDirection == DOWN ? selectedHighscoresItemsIndex  + 1 : selectedHighscoresItemsIndex - 1;
  selectedHighscoresItemsIndex = constrain(selectedHighscoresItemsIndex , 0, HIGHSCORES_MENU_ITEMS_COUNT - 1);

  if (newDirection == DOWN && selectedHighscoresItemsIndex % 2 == 0) {
    highscoresItemsIndex = selectedHighscoresItemsIndex;
  }

  else if (newDirection == UP && selectedHighscoresItemsIndex % 2 == 1) {
    highscoresItemsIndex = selectedHighscoresItemsIndex - 1;
  }

  highscoresItemsIndex = constrain(highscoresItemsIndex, 0, HIGHSCORES_MENU_ITEMS_COUNT - 1);

  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }

}

void displayHighscore() {

  if (!strlen(highscoresData.highscores[0])) {
    
    String messageToDisplay = "!No highscore!";

    lcd.setCursor(1, 0);
    lcd.print(messageToDisplay);
    
    messageToDisplay = "Press to return";
    
    lcd.setCursor(0, 1);
    lcd.print(messageToDisplay);

    processSWState(); 
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print(highscoresItems[highscoresItemsIndex]);

  if (highscoresItemsIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }

  if (highscoresItemsIndex != HIGHSCORES_MENU_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(highscoresItems[highscoresItemsIndex + 1]);
  }

  if (highscoresItemsIndex < HIGHSCORES_MENU_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedHighscoresItemsIndex) {
    byte currentLinePosition = selectedHighscoresItemsIndex % 2;
    byte currentLineLength = strlen(highscoresItems[selectedHighscoresItemsIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  highscoresProcessJoystickState();

  processSWState();  
}

void displayContrast() {

  String messageToDisplay = "Just use the";

  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay);

  messageToDisplay = "potentiometer";

  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay);

  processSWState();
}

void displayLCDLightProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedLightLCDIndex = newDirection == DOWN ? selectedLightLCDIndex + 1 : selectedLightLCDIndex - 1;
  selectedLightLCDIndex = constrain(selectedLightLCDIndex, 0, LCD_LIGHT_ITEMS_COUNT - 1);

  if (newDirection == DOWN && selectedLightLCDIndex % 2 == 0) {
    lightLCDIndex = selectedLightLCDIndex;
  }

  else if (newDirection == UP && selectedLightLCDIndex % 2 == 1) {
    lightLCDIndex = selectedLightLCDIndex - 1;
  }

  lightLCDIndex = constrain(lightLCDIndex, 0, LCD_LIGHT_ITEMS_COUNT - 1);

  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }
}

void displayLCDLight() {
   
  lcd.setCursor(0, 0);
  lcd.print(lcdLightItems[lightLCDIndex]);

  if (lightLCDIndex  > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }

  if (lightLCDIndex != LCD_LIGHT_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(lcdLightItems[lightLCDIndex + 1]);
  }

  if (lightLCDIndex < LCD_LIGHT_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedLightLCDIndex > 0) {
    byte currentLinePosition = selectedLightLCDIndex % 2;
    byte currentLineLength = strlen(lcdLightItems[selectedLightLCDIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();

  displayLCDLightProcessJoystickState();

}

void matrixLightUp() {

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      lc.setLed(0, i, j, true);
    }
  }  
}

void matrixLightProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedLightMatrixIndex = newDirection == DOWN ? selectedLightMatrixIndex + 1 : selectedLightMatrixIndex - 1;
  selectedLightMatrixIndex = constrain(selectedLightMatrixIndex, 0, SETTINGS_MENU_ITEMS_COUNT - 1);

  if (newDirection == DOWN && selectedLightMatrixIndex % 2 == 0) {
    lightMatrixIndex = selectedLightMatrixIndex;
  }

  else if (newDirection == UP && selectedLightMatrixIndex% 2 == 1) {
    lightMatrixIndex = selectedLightMatrixIndex - 1;
  }

  lightMatrixIndex = constrain(lightMatrixIndex, 0, SETTINGS_MENU_ITEMS_COUNT - 1);
 
  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }
}

void displayMatrixLight() {

  matrixLightUp();

  lcd.setCursor(0, 0);
  lcd.print(matrixLightItems[lightMatrixIndex]);

  if (lightMatrixIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }

  if (lightMatrixIndex != MATRIX_LIGHT_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(matrixLightItems[lightMatrixIndex + 1]);
  }

  if (lightMatrixIndex < MATRIX_LIGHT_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedLightMatrixIndex) {
    byte currentLinePosition = selectedLightMatrixIndex % 2;
    byte currentLineLength = strlen(matrixLightItems[selectedLightMatrixIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();

  matrixLightProcessJoystickState();
}

void soundProcessJoystickState() {
  
    byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedSoundItemsIndex  = newDirection == DOWN ? selectedSoundItemsIndex  + 1 : selectedSoundItemsIndex  - 1;
  selectedSoundItemsIndex  = constrain(selectedSoundItemsIndex , 0, SOUND_VOLUME_ITEMS_COUNT  - 1);

  if (newDirection == DOWN && selectedSoundItemsIndex  % 2 == 0) {
    soundItemsIndex  = selectedSoundItemsIndex ;
  }
  else if (newDirection == UP && selectedSoundItemsIndex  % 2 == 1) {
    soundItemsIndex  = selectedSoundItemsIndex  - 1;
  }

  soundItemsIndex  = constrain(soundItemsIndex , 0, SOUND_VOLUME_ITEMS_COUNT - 1);

  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }
}

void displaySoundVolume() {

  lcd.setCursor(0, 0);  
  lcd.print(soundSetItems[soundItemsIndex]);

  if (soundItemsIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }


  if (soundItemsIndex != SOUND_VOLUME_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(soundSetItems[soundItemsIndex  + 1]);
  }

  if (menuIndex < SOUND_VOLUME_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedSoundItemsIndex ) {
    byte currentLinePosition = selectedSoundItemsIndex  % 2;
    byte currentLineLength = strlen(soundSetItems[selectedSoundItemsIndex ]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();

  soundProcessJoystickState();
}


void difficultyProcessJoystickState() {

  byte newDirection = getJoystickState();

  if (newDirection == 0 || (newDirection != UP && newDirection != DOWN)) {
    return;
  }

  selectedDifficultyItemsIndex  = newDirection == DOWN ? selectedDifficultyItemsIndex  + 1 : selectedDifficultyItemsIndex  - 1;
  selectedDifficultyItemsIndex  = constrain(selectedDifficultyItemsIndex , 0, DIFFICULTY_ITEMS_COUNT - 1);

  if (newDirection == DOWN && selectedDifficultyItemsIndex  % 2 == 0) {
    difficultyItemsIndex  = selectedDifficultyItemsIndex ;
  }
  else if (newDirection == UP && selectedDifficultyItemsIndex  % 2 == 1) {
    difficultyItemsIndex  = selectedDifficultyItemsIndex  - 1;
  }

  difficultyItemsIndex  = constrain(difficultyItemsIndex , 0, MENU_ITEMS_COUNT - 1);

  lcd.clear();

  if (settingsData.soundStatus == true) {
    menuSound();
  }

}

void displayDifficulty() {

  lcd.setCursor(0, 0);
  lcd.print(difficultySetItems[difficultyItemsIndex]);

  if (difficultyItemsIndex > 0) {
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
  }

  if (difficultyItemsIndex  != DIFFICULTY_ITEMS_COUNT - 1) {
    lcd.setCursor(0, 1);
    lcd.print(difficultySetItems[difficultyItemsIndex + 1]);
  } 

  if (difficultyItemsIndex < DIFFICULTY_ITEMS_COUNT - 2) {
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
  }

  if (selectedDifficultyItemsIndex) {
    byte currentLinePosition = selectedDifficultyItemsIndex % 2;
    byte currentLineLength = strlen(difficultySetItems[selectedDifficultyItemsIndex]);
    lcd.setCursor(currentLineLength, currentLinePosition);
    lcd.write((byte)2);
  }

  processSWState();

  difficultyProcessJoystickState();

}

void updateMatrix(Point food) {

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      if(food.row != i || food.col != j) {
      lc.setLed(0, i, j, matrix[i][j]);
      }
    }
  }
}

void generateFood() {

  bool retry = true;

  do 
  {
    retry = false;

    food.col = random(MATRIX_SIZE);
    food.row = random(MATRIX_SIZE);

   for (byte i = 0; i < snakeDotsCount; i++) {
     if (food.col == snake[i].col && food.row == snake[i].row) {
       retry = true;
       break;
     }
   }
  }while(retry == true);

}

void displayScore(int score) {

  String messageToDisplay = "Good luck!";

  lcd.setCursor(3, 0);
  lcd.print(messageToDisplay);

  messageToDisplay = "Score: ";

  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay);

  lcd.setCursor(7, 1);
  lcd.print(score);
}

void updatePosition() {

  if (xJoystickSnake == RIGHT) {
    ySnakePos -= 1;
  }
  
  if (xJoystickSnake == LEFT) {
    ySnakePos += 1;
  }

  if (yJoystickSnake == DOWN) {
    xSnakePos += 1;
  }
  
  if (yJoystickSnake == UP) {
    xSnakePos -= 1;
  }
  
  byte row = snake[snakeDotsCount - 1].row;
  byte col = snake[snakeDotsCount - 1].col;
  
  matrix[row][col] = 0;

  for (byte i = snakeDotsCount - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  snake[0] = {xSnakePos, ySnakePos};

  matrix[xSnakePos][ySnakePos] = 1;

  matrixChanged = true;

  lastMoved = millis();
}

void showFood() {
  lc.setLed(0, food.row, food.col, true);
}

void checkFoodEaten() {

  if (xSnakePos == food.row && ySnakePos == food.col) {

    matrix[food.row][food.col] = 1;

    matrixChanged = true;
    
    generateFood();

    snakeDotsCount += 1;

    if (gameDifficulty == 1) {
      currentScore += 1;
    }

    if (gameDifficulty == 2) {
      currentScore += 2;
     }

    if (gameDifficulty == 3) {
      currentScore += 3;
    }

    if (settingsData.soundStatus == true) {
    eatingSound();
    }
  }
}

void displayEndGameResult() {

  lcd.clear();

  String messageToDisplay = "Congrats";

  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay);

  messageToDisplay = "Score: ";

  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay);

  lcd.setCursor(7, 1);
  lcd.print(currentScore);

  delay(100);
  saveHighscore();
}

void writeDemoHighscores() {

  strcpy(highscoresData.highscores[0], "SC1:5");
  strcpy(highscoresData.highscores[1], "SC2:4");
  strcpy(highscoresData.highscores[2], "SC3:3");
  strcpy(highscoresData.highscores[3], "SC4:2");
  strcpy(highscoresData.highscores[4], "SC5:1");

  writeHighscoresInStorage();
}


void resetHighscores() {

  strcpy(highscoresData.highscores[0], "SC1:1");
  strcpy(highscoresData.highscores[1], "SC2:1");
  strcpy(highscoresData.highscores[2], "SC3:1");
  strcpy(highscoresData.highscores[3], "SC4:1");
  strcpy(highscoresData.highscores[4], "SC5:1");

  writeHighscoresInStorage();

  lcd.clear();
  displaySuccessfulHSReset();
}

void displaySuccessfulHSReset() {

  String messageToDisplay = "You reset the HS";

  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay);

  messageToDisplay = "Press to return";

  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay);
}

void saveHighscore() {

  byte recordBeatenIndex = getRecordBeatenIndex();

  if (recordBeatenIndex == -1) {

    lcd.clear();    
    String messageToDisplay = "Your score is not"; 

    lcd.setCursor(0, 0);
    lcd.print(messageToDisplay);
    
    messageToDisplay = "in our top :(";

    lcd.setCursor(0, 1);
    lcd.print(messageToDisplay);

    return;
  }

  lcd.clear();
  String messageToDisplay = "Your are in Top5"; 

  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay);

  modiffyHighscoreTable(recordBeatenIndex);

  //writeHighscoresInStorage();

}

void modiffyHighscoreTable(byte index) {

  short scoreCopy = currentScore;

  short reverseScore = 0;

  while (scoreCopy) {

    reverseScore = reverseScore * 10 + scoreCopy % 10;
    scoreCopy /= 10;    
  }

  int i = 4;

  while (reverseScore) {

    byte digit = reverseScore % 10;

    char charDigit = digit - '0';

    reverseScore /= 10;

    highscoresData.highscores[index][i] == charDigit;

    i++;
  }

  writeHighscoresInStorage();
}

byte getRecordBeatenIndex() {

  for (byte i = 0; i < HIGHSCORES_SAVED_COUNT; i++) {

    if (highscoresData.highscores[i][4] == '0') {
      return -1;
    }

    short score = 0;

    for(byte j = 4; j < strlen(highscoresData.highscores[i]); j++) {

      char digitChar = highscoresData.highscores[i][j];

      short digit = atoi(digitChar);

      score = score * 10 + digit;
    }

    if (currentScore > score) {
      return i;
    }
  }  

  return -1;
}

void closeTheMatrix() {

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      lc.setLed(0, i, j, false);
    }
  }
}

void checkGameOver() {

   if ( ySnakePos < 0 || ySnakePos > MATRIX_SIZE - 1 || xSnakePos < 0 || xSnakePos > MATRIX_SIZE - 1) {

    closeTheMatrix();
    displayEndGameResult();
    

    currentState = STATE_END_GAME;

    if (settingsData.soundStatus == true) {
      gameOverSound();
    }

    return;
  }

  for (int i = 1; i < snakeDotsCount; i++) {
    if (snake[i].row == xSnakePos && snake[i].col == ySnakePos) {
      
      closeTheMatrix();
      displayEndGameResult();
      
      currentState = STATE_END_GAME;
  
    if (settingsData.soundStatus == true) {
      gameOverSound();
    }

      return;
    }
  }
}

byte snakeProcessJoystickState() {

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (xValue >= JOYSTICK_MIN_TRESHOLD && xValue <= JOYSTICK_MAX_TRESHOLD && yValue >= JOYSTICK_MIN_TRESHOLD && yValue <= JOYSTICK_MAX_TRESHOLD) {
    processedMove = false;
    return 0;
  }

  if (xValue > JOYSTICK_MAX_TRESHOLD && processedMove == false && xJoystickSnake == 0) {
    processedMove = true;
    xJoystickSnake =  LEFT;
    yJoystickSnake = 0;
  }

  if (xValue < JOYSTICK_MIN_TRESHOLD && processedMove == false && xJoystickSnake == 0) {
    processedMove = true;
    xJoystickSnake =  RIGHT;
    yJoystickSnake = 0;
  }

  if (yValue > JOYSTICK_MAX_TRESHOLD && processedMove == false && yJoystickSnake == 0) {
    processedMove = true;
    xJoystickSnake = 0;
    yJoystickSnake = DOWN;
  }

  if (yValue < JOYSTICK_MIN_TRESHOLD && processedMove == false && yJoystickSnake == 0) {
    processedMove = true;
    xJoystickSnake = 0;
    yJoystickSnake = UP;
  }
}

void playGame() {

  checkGameOver();

  if (currentState == STATE_END_GAME){
    return;
  }

  displayScore(currentScore);

  if (currentScore == 5 && gameDifficulty < 3) {
    gameDifficulty = 2;
  }

  if (currentScore > 12) {
    gameDifficulty == 3;
  }

  if (gameDifficulty == 1) {
    moveInterval = SNAKE_SPEED_EASY_LEVEL;
  }

  if (gameDifficulty == 2) {
    moveInterval = SNAKE_SPEED_MEDIUM_LEVEL;
  }

  if (gameDifficulty == 3) {
    moveInterval = SNAKE_SPEED_HARD_LEVEL; 
  }

  if (millis() - lastMoved > moveInterval) {
     updatePosition();
  }

  showFood();

  snakeProcessJoystickState();

  checkFoodEaten();

  if (matrixChanged == true) {
    updateMatrix(food);
    matrixChanged = false;
  }

}

void initGame() {

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrix[i][j] = 0; 
    }
  }

  closeTheMatrix();

  snake[0] = Point{0, 2};
  snake[1] = Point{0, 1};
  snake[2] = Point{0, 0};

  currentScore = 0;

  xSnakePos = 0;
  ySnakePos = 2;
  snakeDotsCount = 3;

  readFromStorage();
  gameDifficulty = settingsData.difficulty;
  
  xJoystickSnake = LEFT;
  yJoystickSnake = 0;

  for (byte i = 0; i < snakeDotsCount; i++) {
    
    byte row = snake[i].row;
    byte col = snake[i].col;

    matrix[row][col] = 1;
  }

  generateFood();
  updateMatrix(food);

  matrixChanged = false;
  lastMoved = 0;

  currentState = STATE_GAME;
}

void menuSound() {
  tone(buzzerPin, SOUND_MENU, SOUND_MENU_DURATION);
}

void gameOverSound() {
  tone(buzzerPin, SOUND_GAME_OVER, SOUND_GAME_OVER_DURATION);
}

void eatingSound() {
  tone(buzzerPin, SOUND_GAME_OVER_DURATION, SOUND_EATING_DURATION);
}


void loop() {
 
  switch(currentState){

    case STATE_WELCOME_MESSAGE:
      displayWelcomeMessage();
      break;
   
    case STATE_MENU:
      delay(50);
      displayMenu();
      break;
   
    case STATE_MENU_SETTINGS:
      delay(50);
      displaySettingsMenu();
      break;
     
    case STATE_ABOUT:
      delay(50);
      displayAbout();
      break;

    case STATE_HOW_TO_PLAY:
      delay(50);
      displayHowToPlay();
      break;

    case STATE_HIGHSCORE:
      delay(50);
      displayHighscore();
      break;
     
    case STATE_SET_LCD_CONTRAST:
      delay(50);
      displayContrast();
      break;

    case STATE_SET_LCD_LIGHT:
      delay(50);
      displayLCDLight();
      break;

    case STATE_SET_MATRIX_LIGHT:
      delay(50);
      displayMatrixLight();
      break;

    case STATE_SET_SOUND_VOLUME:
      delay(50);
      displaySoundVolume();
      break;
   
    case STATE_SET_DIFFICULTY:
      delay(50);
      displayDifficulty();
      break;

    case STATE_INITIALIZE_GAME:
      delay(50);
      initGame();
      break;

    case STATE_GAME:
      delay(50);
      playGame();
      break;

    case STATE_END_GAME:
      delay(50);
      processSWState();
      break;

    case STATE_RESET_HIGHSCORES:
      delay(50);
      resetHighscores();
      processSWState();
      break;
  }
}