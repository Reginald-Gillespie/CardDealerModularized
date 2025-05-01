// File to set card faces.
// Ideally, faces used multiple times are stored in a single variable to reduce flash useage

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
EDITING ANIMATIONS
DEALR comes stock with two animations: one quick blinking animation it does right on boot, and one "screensaver" animation it does when it's bored.
If you look under "Initial blinking animation," you'll notice a series of symbols in quotes. Each four-character section between the quotes is a "frame."
For example: "O  O" is two wide eyes separated by two spaces. You can change these sections to anything you want, as long as you have exactly 4 characters.
So "X  X" works, "MARK" works, but "GUS" is too short and would need to be " GUS" or "GUS ". Each "frame" corresponds with an interval, or the amount of
time that frame should be displayed for in milliseconds. So if you want a frame to say "MARK" for 1 second, look at the next line, find the corresponding interval,
and type 1000.

The number of frames must equal the number of intervals, so if you add frames to the end of an animation, make sure you remember to add new intervals for those frames.

You can create new animations and call them in the script, but if you're just getting started, try changing some frames in the existing animations to see what happens!
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef Faces_H
#define Faces_H

#include <Arduino.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// Stored strings of all faces go here to same ram.
const char* EFFORT =       "X  X";   // The face DEALR makes in unrigged games while dealing a card
const char* MONEY =        "$  $";   // The face DEALR makes in rigged games while dealing a marked card
const char* LOOK_SMALL =   "o  o";   // The face DEALR makes in unrigged games while dealing an unmarked card
const char* LEFT =         ">  >";   // The face DEALR makes when rotating clockwise
const char* RIGHT =        "<  <";   // The face DEALR makes when rotating counter-clockwise
const char* LOOK_BIG =     "O  O";   // The face DEALR makes right before dealing a card in a regular game
const char* BLINK =        "-  -";
const char* LOOK_CLOSED =  "_  _";
const char* a__a =         "a  a";
const char* WILD =         "@  @";    // The face DEALR makes right before dealing a marked card in a rigged game
const char* SNEAKY =       "=  =";    // The face DEALR makes right before dealing an unmarked card in a rigged game


// This little block has to come before the animation definitions, which let you change the faces DEALR makes.
struct DisplayAnimation {
    const char** frames;            // Pointer to array of frames
    const unsigned long* intervals; // Pointer to array of timing intervals
    uint8_t numFrames;              // Number of frames in the animation
};



const char* introFrames[] = {
    LOOK_BIG, // Frame 1
    BLINK, // Frame 2
    LOOK_BIG, // Frame 3
    BLINK, // Frame 4
    LOOK_BIG // Frame 5
}; 
const unsigned long introIntervals[] = {
    1100, // Interval 1
    75,   // Interval 2
    180,  // Interval 3
    75,   // Interval 4
    1100
}; // Interval 5
const DisplayAnimation initialBlinking = { introFrames, introIntervals, ARRAY_SIZE(introFrames) };

// Screensaver blinking animation
const char* screensaveFrames[] = {
    LOOK_BIG, // Frame 1
    BLINK, // Frame 2
    LOOK_BIG, // Frame 3
    BLINK, // Frame 4
    a__a, // Frame 5
    LOOK_CLOSED, // Frame 6
    BLINK, // Frame 7
    LOOK_CLOSED
}; // Frame 8
const unsigned long screensaveIntervals[] = {
    2000, // Interval 1
    75,   // Interval 2
    3000, // Interval 3
    75,   // Interval 4
    3000, // Interval 5
    3000, // Interval 6
    1500, // Interval 7
    4000
}; // Interval 8
const DisplayAnimation screensaverBlinking = { screensaveFrames, screensaveIntervals, ARRAY_SIZE(screensaveFrames) };

// Cheating blinking animation
const char* evilScreensaveFrames[] = {
    MONEY, // Frame 1
    BLINK, // Frame 2
    MONEY, // Frame 3
    BLINK, // Frame 4
    WILD, // Frame 5
    LOOK_CLOSED, // Frame 6
    BLINK, // Frame 7
    LOOK_CLOSED
}; // Frame 8
const unsigned long evilScreensaveIntervals[] = {
    2000, // Interval 1
    75,   // Interval 2
    3000, // Interval 3
    75,   // Interval 4
    3000, // Interval 5
    3000, // Interval 6
    1500, // Interval 7
    4000
}; // Interval 8

// Initial blinking animation
const DisplayAnimation evilScreensaverBlinking = { evilScreensaveFrames, evilScreensaveIntervals, ARRAY_SIZE(evilScreensaveFrames) };



#endif