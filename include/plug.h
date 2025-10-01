// Hot reloading tsoding style
#ifndef PLUG_H
#define PLUG_H
#include "ear_trainer.h"
#include <raylib.h>
#define NO_OF_IMAGES 40
#define MAX_NUM_OF_BUTTON 15
#define NO_OF_MUSIC 5
#define POOL_SIZE 6
#define MENU_BUTTON_WIDTH 0.16 * GetRenderWidth ()
#define MENU_BUTTON_HEIGHT 0.08 * GetRenderWidth ()

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

typedef struct
{

    BoundingBox keyBoxes[NO_OF_NOTES];
    Sound sound[NO_OF_NOTES];
    bool key_pressed;
    KeyboardHashMap *Key_to_note;
    int note_pool[POOL_SIZE];
    unsigned int currentOctave;
    Font MenuFont;
    Color note_color;

    Model models[NO_OF_MODELS];
    Light lights;
    Shader shaders[NO_OF_SHADERS];
    int shaderLoc[NO_OF_SHADER_LOC];
    Texture2D maps[NO_OF_MAPS];
    /* Model piano_model; */
    /* Model plane_model; */
    /* Model lightCube; */
    /* Shader pbrShader; */
    /* Shader planeShader; */
    /* Shader lightShader; */
    /* int lightPosLoc; */
    /* int lightAmbLoc; */
    /* int lightDiffLoc; */
    /* int lightSpecLoc; */
    /* int lightConstLoc; */
    /* int lightLinearLoc; */
    /* int lightQuadraticLoc; */
    /* int viewPosLoc; */
    /* Texture2D pianoAlbedo; */
    /* Texture2D pianoMetallicRoughness; */
    /* Texture2D pianoNormal; */

    /* Texture2D planeAlbedo; */
    /* Texture2D planeRoughness; */
    /* Texture2D planeNormal; */
    /* Texture2D planeMetallic; */
    /* Texture2D planeAO; */

    /* // Material1: Bench */
    /* Texture2D benchAlbedo; */
    /* Texture2D benchMetallicRoughness; */
    /* Texture2D benchNormal; */

    enum Screen current_screen;
    enum Screen former_screen;
    Texture images[NO_OF_IMAGES];
    Music music[NO_OF_MUSIC];
    const char *music_file[NO_OF_MUSIC];
    int current_music_index;
    int current_note;
    MenuButton buttons[NO_OF_SCREEN][MAX_NUM_OF_BUTTON];
    int no_of_buttons[NO_OF_SCREEN];
    int key;
    int scale;
    int score;
    bool player_respond;
    char interval_feedback[8];
    Camera camera;
    Camera2D camera2d;
    float time;
    bool screen_transition;
    bool quit;
    bool looped_once;
} Plug;

typedef void (*plug_init_t) (Plug *plug);
typedef void (*plug_update_t) (Plug *plug);

void DrawBoundingBoxAsCube (BoundingBox box, Color color);
void MapPianoNotes (Plug *plug);
void LoadPianoModel (Plug *plug);
void DrawPiano (Plug *plug);
void DrawPlaneModel (Plug *plug);
void LoadPlaneModel (Plug *plug);
void LoadUi (Plug *plug);
void LoadResources (Plug *plug);
void LoadSoundInstrument (Plug *plug);
void CheckKeyPress (Plug *plug);
int GenNote (int key, Scale scale);

void ScreenTransitionIntSettingToInt (Plug *plug);
void ScreenTransitionHomeToIntSetting (Plug *plug);
void ScreenTransitionIntToPause (Plug *plug);
void ScreenTransitionPauseToInt (Plug *plug);
void ScreenTransitionFreeToPause (Plug *plug);

void AlignScreenButtons (float height, float width, float x, float y,
                         float padding, int no_of_buttons,
                         MenuButton *buttons);
const char *Key_to_text (int key);
#endif // PLUG_H
