#include <U8g2lib.h>

// Buzzer pin definition
const int buzzerPin = 15;

// Note frequencies (in Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_REST 0

// Space theme melody
const int melody[] = {
  NOTE_C4, NOTE_F4, NOTE_G4, NOTE_REST,
  NOTE_E4, NOTE_A4, NOTE_G4, NOTE_REST,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_REST,
  NOTE_F4, NOTE_E4, NOTE_D4, NOTE_REST
};

// Duration of each note (in milliseconds)
const int noteDurations[] = {
  200, 200, 200, 100,
  200, 200, 200, 100,
  200, 200, 200, 100,
  200, 200, 200, 100
};

// Keep track of the current note
int currentNote = 0;
unsigned long lastNoteTime = 0;

// Initialize the ST7920 128x64 display in SPI mode with vertical orientation
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R1, /* clock (E)=*/ 14, /* data (R/W)=*/ 12, /* CS (RS)=*/ 13, /* reset=*/ U8X8_PIN_NONE);

// Triangle position and size variables
int triangleX = 27;             
const int triangleWidth = 10;   
const int triangleBaseY = 100;  
const int triangleTipY = 85;    

// Timing variables
unsigned long previousMillis = 0;
const long interval = 16.67;    

// Maximum number of meteors in the game
const int maxMeteorCount = 5;

// Define a structure to represent a meteor
struct Meteor {
  int x;        
  int y;        
  int size;     
  bool active;  
};

// Array to hold multiple meteors
Meteor meteors[maxMeteorCount];

// Player's score variable
int score = 0;

// Pin numbers for the left and right buttons
const int buttonLeftPin = 5;   
const int buttonRightPin = 4;  

void playNote() {
  unsigned long currentTime = millis();
  
  // Check if it's time to play the next note
  if (currentTime - lastNoteTime >= noteDurations[currentNote]) {
    // Stop the previous note
    noTone(buzzerPin);
    
    // Move to the next note
    currentNote = (currentNote + 1) % (sizeof(melody) / sizeof(melody[0]));
    
    // Play the current note if it's not a rest
    if (melody[currentNote] != NOTE_REST) {
      tone(buzzerPin, melody[currentNote]);
    }
    
    lastNoteTime = currentTime;
  }
}

void setup() {
  u8g2.begin();                         
  pinMode(buttonLeftPin, INPUT_PULLUP);
  pinMode(buttonRightPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);           // Set up the buzzer pin

  // Initialize meteors with random positions and sizes
  for (int i = 0; i < maxMeteorCount; i++) {
    meteors[i].x = random(0, 64);      
    meteors[i].y = random(-127, 0);    
    meteors[i].size = random(2, 8);    
    meteors[i].active = true;          
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Play background music
  playNote();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    u8g2.clearBuffer();
    
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(5, 125, "Score:");
    u8g2.setCursor(40, 125);
    u8g2.print(score);
    
    u8g2.drawFrame(0, 0, 64, 128);
    u8g2.drawLine(0, 115, 64, 115);
    u8g2.drawTriangle(triangleX, triangleBaseY, triangleX + 5, triangleTipY, triangleX + 10, triangleBaseY);

    for (int i = 0; i < maxMeteorCount; i++) {
      if (meteors[i].active) {
        u8g2.drawDisc(meteors[i].x, meteors[i].y, meteors[i].size);
        meteors[i].y += 4;

        if (meteors[i].y > 110) {
          meteors[i].y = 0;
          meteors[i].x = random(6, 58);
          score++;
        }

        if (meteors[i].y + meteors[i].size >= triangleTipY && meteors[i].y - meteors[i].size <= triangleBaseY) {
          if (meteors[i].x >= triangleX && meteors[i].x <= triangleX + triangleWidth) {
            score = 0;
            meteors[i].y = 0;
          }
        }
      }
    }

    if (digitalRead(buttonLeftPin) == LOW) {
      triangleX -= 5;
      if (triangleX < 0) triangleX = 0;
    }

    if (digitalRead(buttonRightPin) == LOW) {
      triangleX += 5;
      if (triangleX > 54) triangleX = 54;
    }

    u8g2.sendBuffer();
  }
}
