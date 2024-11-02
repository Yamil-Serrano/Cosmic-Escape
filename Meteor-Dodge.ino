#include <U8g2lib.h>  // Include the U8g2 library for handling the LCD display

// Initialize the ST7920 128x64 display in SPI mode with vertical orientation
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R1, /* clock (E)=*/ 14, /* data (R/W)=*/ 12, /* CS (RS)=*/ 13, /* reset=*/ U8X8_PIN_NONE);

// Triangle position and size variables
int triangleX = 27;             // X position of the triangle (player)
const int triangleWidth = 10;   // Width of the triangle (player)
const int triangleBaseY = 100;  // Y position of the base of the triangle
const int triangleTipY = 85;    // Y position of the tip of the triangle

// Timing variables
unsigned long previousMillis = 0;  // Store the last time the display was updated
const long interval = 16.67;        // Time interval for the game loop (about 60 frames per second)

// Maximum number of meteors in the game
const int maxMeteorCount = 5;

// Define a structure to represent a meteor
struct Meteor {
  int x;         // X position of the meteor
  int y;         // Y position of the meteor
  int size;      // Size (radius) of the meteor
  bool active;   // Status indicating if the meteor is active (visible)
};

// Array to hold multiple meteors
Meteor meteors[maxMeteorCount];

// Player's score variable
int score = 0;

// Pin numbers for the left and right buttons
const int buttonLeftPin = 5;   // Pin for the left button
const int buttonRightPin = 4;  // Pin for the right button

void setup() {
  u8g2.begin();                          // Initialize the display
  pinMode(buttonLeftPin, INPUT_PULLUP); // Set the left button pin as input with pull-up resistor
  pinMode(buttonRightPin, INPUT_PULLUP);// Set the right button pin as input with pull-up resistor

  // Initialize meteors with random positions and sizes
  for (int i = 0; i < maxMeteorCount; i++) {
    meteors[i].x = random(0, 64);       // Random X position within the screen width
    meteors[i].y = random(-127, 0);     // Random Y position starting above the screen
    meteors[i].size = random(2, 8);     // Random size for the meteor
    meteors[i].active = true;           // Set the meteor as active (visible)
  }
}

void loop() {
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  
  // Check if the time interval has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update the last update time

    // Clear the previous frame
    u8g2.clearBuffer();
    
    // Draw the score on the display
    u8g2.setFont(u8g2_font_5x8_tr);  // Set the font for the text
    u8g2.drawStr(5, 125, "Score:");   // Draw the "Score:" label
    u8g2.setCursor(40, 125);          // Set the cursor position for the score number
    u8g2.print(score);                 // Print the current score
    
    // Draw the game frame
    u8g2.drawFrame(0, 0, 64, 128);    // Draw a frame around the game area
    u8g2.drawLine(0, 115, 64, 115);    // Draw a line separating the game area from the score
    // Draw the triangle (player) on the screen
    u8g2.drawTriangle(triangleX, triangleBaseY, triangleX + 5, triangleTipY, triangleX + 10, triangleBaseY);

    // Loop through each meteor to update its position and draw it
    for (int i = 0; i < maxMeteorCount; i++) {
      if (meteors[i].active) {  // Check if the meteor is active
        u8g2.drawDisc(meteors[i].x, meteors[i].y, meteors[i].size); // Draw the meteor
        meteors[i].y += 4;  // Move the meteor downwards

        // If the meteor goes off the screen, reset its position and increase score
        if (meteors[i].y > 110) {
          meteors[i].y = 0;  // Reset the meteor to the top of the screen
          meteors[i].x = random(6, 58); // Randomize its X position
          score++;  // Increase score for each meteor that goes off screen
        }

        // Check for collision between the triangle and the meteor
        if (meteors[i].y + meteors[i].size >= triangleTipY && meteors[i].y - meteors[i].size <= triangleBaseY) {
          if (meteors[i].x >= triangleX && meteors[i].x <= triangleX + triangleWidth) {
            score = 0;  // Reset score if the meteor hits the triangle
            meteors[i].y = 0; // Reset the meteor position
          }
        }
      }
    }

    // Check if the left button is pressed
    if (digitalRead(buttonLeftPin) == LOW) {
      triangleX -= 5; // Move the triangle left
      if (triangleX < 0) triangleX = 0; // Prevent moving out of bounds
    }

    // Check if the right button is pressed
    if (digitalRead(buttonRightPin) == LOW) {
      triangleX += 5; // Move the triangle right
      if (triangleX > 54) triangleX = 54; // Prevent moving out of bounds
    }

    // Send the buffer to the display to render the graphics
    u8g2.sendBuffer();
  }
}
