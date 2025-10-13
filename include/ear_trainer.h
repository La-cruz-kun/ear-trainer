#ifndef EAR_TRAINER_H
#define EAR_TRAINER_H
#include "parser.h"
#include "stb_ds.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#define NO_OF_IMAGES 50
#define MAX_NUM_OF_BUTTON 15
#define NO_OF_MUSIC 5
#define MAX_CHORD_SIZE 6
#define MENU_BUTTON_WIDTH 0.16 * GetRenderWidth ()
#define MENU_BUTTON_HEIGHT 0.08 * GetRenderWidth ()
#define NOTE_N 6 * SCALE_LENGTH

enum Screen
{
    LOADING_SCREEN,
    LOGO_SCREEN,
    HOME_SCREEN,
    INTERVAL_SCREEN,
    INTERVAL_SETTING_SCREEN,
    OPTION_SCREEN,
    PAUSE_SCREEN,
    FREE_SCREEN,
    CHORD_SCREEN,
    SETTING_SCREEN,
    NO_OF_SCREEN,
    NONE
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

typedef struct
{
    KeyboardKey key;
    int value;
} KeyboardHashMap;

enum Models
{
    PIANO,
    PLANE,
    LIGHT_CUBE,
    NO_OF_MODELS
};

enum Shaders
{
    PBR_SHADER,
    PLANE_SHADER,
    LIGHT_SHADER,
    NO_OF_SHADERS
};

enum ShaderLoc
{
    LIGHT_POS_LOC,
    LIGHT_AMB_LOC,
    LIGHT_DIFF_LOC,
    LIGHT_SPEC_LOC,
    LIGHT_CONST_LOC,
    LIGHT_LINEAR_LOC,
    LIGHT_QUADRATIC_LOC,
    VIEW_POS_LOC,
    NO_OF_SHADER_LOC
};

enum Maps
{
    PIANO_ALBEDO,
    PIANO_METALLIC_ROUGHNESS,
    PIANO_NORMAL,

    PLANE_ALBEDO,
    PLANE_ROUGHNESS,
    PLANE_NORMAL,
    PLANE_METALLIC,
    PLANE_AO,

    // Material1: Bench
    BENCH_ALBEDO,
    BENCH_METALLIC_ROUGHNESS,
    BENCH_NORMAL,

    NO_OF_MAPS
};

typedef struct
{
    Vector3 lightPosition;
    Vector3 lightAmbient;
    Vector3 lightDiffuse;
    Vector3 lightSpecular;

    float lightConstant;
    float lightLinear;
    float lightQuadratic;

} Light;

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

typedef enum
{
    I,
    ii,
    iii,
    IV,
    V,
    vi,
    vii
} ChordType;

typedef struct
{
    int chord[MAX_CHORD_SIZE];
} ChordNotes;

// draw.c
void DrawBackgroundImage (Texture image);
void DrawMenuButton (MenuButton button);

// input.c
bool IsMenuButtonPressed (MenuButton *button);

// util.c
MenuButton CreateMenuButton (Texture image, const char *text, Color text_color,
                             Rectangle bound);
void calcMenuButtonPosition (MenuButton *button, ...);

void UpdateHomeScreen (void);

void DrawBoundingBoxAsCube (BoundingBox box, Color color);
int MapPianoNotes (void);
int LoadPianoModel (void);
void DrawPiano (void);
void DrawPlaneModel (void);
int LoadPlaneModel (void);
int LoadUi (void);
int LoadResources (void);
int LoadSoundInstrument (void);

int GenNote (int key, Scale scale);
int GenChord (int key, int prog_number);
void PlayChordProg (int key);
void PlayChord (ChordNotes chord_notes);
void TransitionChord (void);
int ChordLength (int *chord);

void AlignScreenButtons (float height, float width, float x, float y,
                         float padding, int no_of_buttons,
                         MenuButton *buttons);
const char *Key_to_text (int key);

int init (void);
void InitLoadingScreen (void);
void InitChordScreen (void);

void update (void);
void UpdateLoadingScreen (void);
void UpdateFreeScreen (void);
void UpdateIntervalSettingScreen (void);
void UpdateIntervalScreen (void);
void UpdatePauseScreen (void);
void UpdateSettingScreen (void);
void UpdateChordScreen (void);
void UpdateTransition (void);

void UpdateHomeToIntSetting (void);
void UpdateIntSettingToInt (void);
void UpdateHomeToFree (void);
void UpdateIntToPause (void);
void UpdateFreeToPause (void);
void UpdatePauseToInt (void);
void UpdatePauseToFree (void);
void UpdatePauseToHome (void);
void UpdateIntSettingToChord (void);

void DrawLoadingScreen (void);
void DrawHomeScreen (void);
void DrawFreeScreen (void);
void DrawIntervalSettingScreen (void);
void DrawIntervalScreen (void);
void DrawPauseScreen (void);
void DrawSettingScreen (void);
void DrawChordScreen (void);
void DrawTransition (void);

void DrawHomeToIntSetting (void);
void DrawIntToPause (void);
void DrawFreeToPause (void);
void DrawPauseToInt (void);
void DrawPauseToFree (void);
void DrawPauseToHome (void);

bool FinishLoadingScreen (void);
int FinishHomeScreen (void);
int FinishIntSettingScreen (void);
int FinishPauseScreen (void);
int FinishIntScreen (void);
int FinishFreeScreen (void);
int FinishSettingScreen (void);

void ChangeToScreen (enum Screen screen);
void TransitionToScreen (enum Screen screen);

extern Camera2D camera2d;
extern Camera camera;
extern Music music[NO_OF_MUSIC];
extern Music drone[OCTAVE];
extern const char *music_file[NO_OF_MUSIC];
extern MenuButton buttons[NO_OF_SCREEN][MAX_NUM_OF_BUTTON];
extern enum Screen current_screen;
extern enum Screen after_int_set_screen;
extern enum Screen former_screen;
extern bool looped_once;
extern bool screen_transition;
extern bool quit;
extern Texture images[NO_OF_IMAGES];
extern Shader shaders[NO_OF_SHADERS];

extern Vector3 lightPosition;
extern Vector3 lightAmbient;
extern Vector3 lightDiffuse;
extern Vector3 lightSpecular;

extern int shaderLoc[NO_OF_SHADER_LOC];
extern int current_music_index;
extern int no_of_buttons[NO_OF_SCREEN];
extern int note_pool[MAX_CHORD_SIZE];
extern int key;
extern int scale;

extern Sound sound[NO_OF_NOTES];
extern KeyboardHashMap *Key_to_note;
extern Model models[NO_OF_MODELS];
extern BoundingBox keyBoxes[NO_OF_NOTES];

extern Font font1;
extern Font font2;
extern Font font3;

#endif // EAR_TRAINER_H
