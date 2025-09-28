#ifndef PLUG_H
#define PLUG_H
#include <raylib.h>
#include "ear_trainer.h"
#define NO_OF_IMAGES 40
#define MAX_NUM_OF_BUTTON 15
#define NO_OF_MUSIC 5
#define POOL_SIZE 6
#define MENU_BUTTON_WIDTH 0.16 * GetRenderWidth()
#define MENU_BUTTON_HEIGHT 0.08 * GetRenderWidth()



typedef struct {
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

typedef void (*plug_init_t)(Plug *plug);
typedef void (*plug_update_t)(Plug *plug);

void DrawBoundingBoxAsCube(BoundingBox box, Color color);
void MapPianoNotes(void);
void LoadPianoModel(void);
void DrawPiano(void);
void DrawPlaneModel(void);
void LoadPlaneModel(void);
void LoadUi(Plug *plug);
void LoadResources(Plug *plug);
void LoadSoundInstrument(void);
void CheckKeyPress(void);
int GenNote(int key, Scale scale);

void ScreenTransitionIntSettingToInt(Plug *plug);
void ScreenTransitionHomeToIntSetting(Plug *plug);
void ScreenTransitionIntToPause(Plug *plug);
void ScreenTransitionPauseToInt(Plug *plug);
void ScreenTransitionFreeToPause(Plug *plug);

void AlignScreenButtons(float height, float width, float x, float y, float padding, int no_of_buttons, MenuButton *buttons);
const char* Key_to_text(int key);
#endif //PLUG_H
