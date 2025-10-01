#ifndef EAR_TRAINER_H
#define EAR_TRAINER_H
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

enum Screen
{
    HOME_SCREEN,
    INTERVAL_SCREEN,
    INTERVAL_SETTING_SCREEN,
    OPTION_SCREEN,
    PAUSE_SCREEN,
    FREE_SCREEN,
    NO_OF_SCREEN
};

typedef enum
{
    NOT_HOVER,
    HOVER,
    CLICK
} MenuButtonAction;

typedef struct
{
    Texture image;
    Rectangle bound;
    const char *text;
    Color text_color;
    MenuButtonAction action;
    bool is;
} MenuButton;

typedef enum
{
    C1 = 0,
    CS1,
    D1,
    DS1,
    E1,
    F1,
    FS1,
    G1,
    GS1,
    A1,
    AS1,
    B1,
    C2,
    CS2,
    D2,
    DS2,
    E2,
    F2,
    FS2,
    G2,
    GS2,
    A2,
    AS2,
    B2,
    C3,
    CS3,
    D3,
    DS3,
    E3,
    F3,
    FS3,
    G3,
    GS3,
    A3,
    AS3,
    B3,
    C4,
    CS4,
    D4,
    DS4,
    E4,
    F4,
    FS4,
    G4,
    GS4,
    A4,
    AS4,
    B4,
    C5,
    CS5,
    D5,
    DS5,
    E5,
    F5,
    FS5,
    G5,
    GS5,
    A5,
    AS5,
    B5,
    C6,
    CS6,
    D6,
    DS6,
    E6,
    F6,
    FS6,
    G6,
    GS6,
    A6,
    AS6,
    B6,
    C7,
    CS7,
    D7,
    DS7,
    E7,
    F7,
    FS7,
    G7,
    GS7,
    A7,
    AS7,
    B7,
    C8,
    NO_OF_NOTES
} Notes;
typedef enum
{
    C,
    CS,
    D,
    DS,
    E,
    F,
    FS,
    G,
    GS,
    A,
    AS,
    B
} Actual_Note;

typedef enum
{
    MAJOR = 0,
    MINOR = 1,
    SCALE_LENGTH = 7
} Scale;

typedef enum
{
    UNISON = 0,
    MINOR_SECOND,
    MAJOR_SECOND,
    MINOR_THIRD,
    MAJOR_THIRD,
    FOURTH,
    AUGUMENTED_FOURTH,
    FIFTH,
    MINOR_SIXTH,
    MAJOR_SIXTH,
    MINOR_SEVENTH,
    MAJOR_SEVENTH,
    OCTAVE
} Intervals;
// draw.c
void DrawBackgroundImage (Texture image);
void DrawMenuButton (MenuButton button);

// input.c
bool IsMenuButtonPressed (MenuButton *button);

// util.c
MenuButton CreateMenuButton (Texture image, const char *text, Color text_color,
                             Rectangle bound);
void calcMenuButtonPosition (MenuButton *button, ...);
#endif // EAR_TRAINER_H
