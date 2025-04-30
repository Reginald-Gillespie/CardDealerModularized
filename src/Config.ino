#include <Arduino.h>
#ifndef GameConfig
#define GameConfig

// HANDY TOGGLES AND VALUES
#define verbose false                                  // Disable most verbose serial prints because they take up a ton of ram.
bool useSerial = true;                                 // Enables serial output for debugging. Set to false to disable serial output. Some statements need manual uncommenting for memory reasons.
bool scrollInstructions = true;                        // Enables/disables the instructions that switch between the initial animation and the games selection menu.
bool motorStartRoutine = true;                         // Enables/disables each of the motors going back and forth at boot. Useful for debugging, but can be turned off to save a little energy for deals.
uint8_t riggedColor = 1;                               // Can be used to changed the color tag that marked cards are dealt towards. RED = 1; YELLOW = 2; BLUE = 3; GREEN = 4.
uint16_t textSpeedInterval = 160;                      // How fast do you read?? Amount of time (in ms) between frames of scrolling text (Lower number = faster text scrolling).
uint16_t textStartHoldTime = 800;                      // Amount of time (in ms) scrolling text should pause before advancing.
uint16_t textEndHoldTime = 800;                        // Amount of time (in ms) that scrolling text should pause at the end of a scroll.
const unsigned long timeUntilScreensaverStart = 55000; // When this amount of time expires (in milliseconds), the intro animation starts as a screensaver.
const unsigned long markedLEDTimeout = 600;            // The Nano's onboard LED lights up when DEALR detects a marked card. This is the number of milliseconds it lights for.
const unsigned long expressionDuration = 500;          // DEALR makes faces when it deals cards. This value determines the amount of time it makes the face for.

#endif // GameConfig