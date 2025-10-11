#include "ear_trainer.h"
#include <raylib.h>

static int framesCounter = 0;
static Color note_color = BLUE;
static bool player_respond = false;
static char interval_feedback[8] = { 0 };
static unsigned int currentOctave = 0;
static int current_note = -1;
static int score = 0;
static int finish = 0;

void
UpdateIntervalScreen (void)
{
    finish = 0;
    if (!IsMusicStreamPlaying (drone[key]))
        {
            PlayMusicStream (drone[key]);
        }
    UpdateMusicStream (drone[key]);

    // interval logic
    if (current_note == -1)
        {
            current_note = GenNote (key, scale);
            PlaySound (sound[current_note]);
            framesCounter = 0;
        }

    bool playing = !IsSoundPlaying (sound[current_note]) && !player_respond;

    if (playing)
        {

            // check input
            for (int i = 0; i < hmlen (Key_to_note); i++)
                {
                    KeyboardKey key = Key_to_note[i].key;
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    if (IsKeyPressed (key))
                        {
                            PlaySound (sound[note]);
                            note_pool[0] = note;
                            player_respond = true;
                            framesCounter = 0;
                            note_color = RED;
                            strcpy (interval_feedback, "Wrong");
                            if (note % OCTAVE == current_note % OCTAVE)
                                {
                                    strcpy (interval_feedback, "Correct");
                                    note_color = GREEN;
                                    score += 1;
                                }
                        }
                }
        }
    else
        {
            framesCounter++;
            if (player_respond && framesCounter > 120)
                {
                    StopSound (sound[note_pool[0]]);
                    if (framesCounter > 150)
                        {
                            strcpy (interval_feedback, "");
                            framesCounter = 0;
                            player_respond = false;
                            current_note = GenNote (key, scale);
                            PlaySound (sound[current_note]);
                        }
                }
            if (IsSoundPlaying (sound[current_note]) && framesCounter > 60)
                {
                    StopSound (sound[current_note]);
                    framesCounter = 0;
                }
        }
    if (IsKeyPressed (KEY_RIGHT_SHIFT))
        {
            if (currentOctave < 7)
                currentOctave++;
        }
    if (IsKeyPressed (KEY_LEFT_SHIFT))
        {
            if (currentOctave > 0)
                currentOctave--;
        }

    // Update light position and view position
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0, 0,
        GetRenderHeight () / 20.0, no_of_buttons[INTERVAL_SCREEN],
        buttons[INTERVAL_SCREEN]);

    if (IsMenuButtonPressed (&buttons[INTERVAL_SCREEN][0]))
        {
            finish = 1;
        }
}
void
DrawIntervalScreen (void)
{

    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    if (note_pool[0] > -1)
        {
            DrawBoundingBoxAsCube (keyBoxes[note_pool[0]], note_color);
        }
    DrawBoundingBoxAsCube (keyBoxes[current_note], BLUE);
    /* DrawGrid(10, 1); */
    EndMode3D ();
    for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++)
        DrawMenuButton (buttons[INTERVAL_SCREEN][i]);
    DrawText (TextFormat ("Current Key is %s", Key_to_text (key)), 30, 30, 24,
              RAYWHITE);
}

void
UpdateIntSettingToInt (void)
{
    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0, 0,
        GetRenderHeight () / 20.0, no_of_buttons[INTERVAL_SCREEN],
        buttons[INTERVAL_SCREEN]);

    Vector3 target = { 0, 2, 2 };
    camera.position.x = Lerp (camera.position.x, target.x, 0.04);
    camera.position.y = Lerp (camera.position.y, target.y, 0.04);
    camera.position.z = Lerp (camera.position.z, target.z, 0.04);
    float tolerance = 0.1;
    target.x += tolerance;
    target.y += tolerance;
    target.z += tolerance;
    if (camera.position.x <= target.x && camera.position.y <= target.y
        && camera.position.z <= target.z)
        {
            screen_transition = false;
            printf ("done\n");
        }
}

void
UpdatePauseToInt (void)
{
    float target = GetRenderHeight ();
    camera2d.offset.y = Lerp (camera2d.offset.y, target, 0.1);
    target -= 5;
    if (camera2d.offset.y >= target)
        {
            screen_transition = false;
        }
}

void
DrawPauseToInt (void)
{
    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < POOL_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0, 0,
        GetRenderHeight () / 20.0, no_of_buttons[INTERVAL_SCREEN],
        buttons[INTERVAL_SCREEN]);
    for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++)
        DrawMenuButton (buttons[INTERVAL_SCREEN][i]);
    BeginMode2D (camera2d);

    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[PAUSE_SCREEN],
                        buttons[PAUSE_SCREEN]);

    DrawBackgroundImage (images[1]);
    for (int i = 0; i < no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (buttons[PAUSE_SCREEN][i]);
    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    EndMode2D ();
}

const char *
Key_to_text (int key)
{
    switch (key)
        {
        case 0:
            return "C";
        case 1:
            return "C#";
        case 2:
            return "D";
        case 3:
            return "D#";
        case 4:
            return "E";
        case 5:
            return "F";
        case 6:
            return "F#";
        case 7:
            return "G";
        case 8:
            return "G#";
        case 9:
            return "A";
        case 10:
            return "A#";
        case 11:
            return "B";
        default:
            return "";
        }
}

int
GenNote (int key, Scale scale)
{
    Notes note[NOTE_N] = { 0 };
    int current = key;
    note[0] = current;
    if (scale == MAJOR)
        {
            for (int i = 1; i < NOTE_N; i++)
                {
                    if (i % SCALE_LENGTH == 3 || i % SCALE_LENGTH == 0)
                        current += MINOR_SECOND;
                    else
                        current += MAJOR_SECOND;
                    note[i] = current;
                }
        }

    else if (scale == MINOR)
        {
            for (int i = 1; i < NOTE_N; i++)
                {
                    if (i % SCALE_LENGTH == 2 || i % SCALE_LENGTH == 5)
                        current += MINOR_SECOND;
                    else
                        current += MAJOR_SECOND;
                    note[i] = current;
                }
        }
    return note[GetRandomValue (0, NOTE_N - 1)];
}

int
FinishIntScreen (void)
{
    return finish;
}
