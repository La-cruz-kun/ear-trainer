#include "chord_parser.h"
#include "ear_trainer.h"
#include <raylib.h>

static int finish = 0;
static int framesCounter = 0;
static size_t progression_counter = 0;
static ChordNotes current_chord_notes = { .chord = { -1, -1, -1, -1 } };
static ChordNotes former_chord_notes = { .chord = { -1, -1, -1, -1 } };
static int state = 0;

void
InitChordScreen (void)
{
}

void
UpdateChordScreen (void)
{
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    if (state == 0)
        {
            current_chord_notes = (ChordNotes){ .chord = { -1, -1, -1, -1 } };
            if (IsKeyReleased (KEY_ENTER))
                state = 1;
        }

    else if (state == 1)
        {
            PlayChordProg (key);
        }
}

void
DrawChordScreen (void)
{

    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    /* if (note_pool[0] > -1) */
    /*     { */
    /*         DrawBoundingBoxAsCube (keyBoxes[note_pool[0]], note_color); */
    /*     } */
    for (int i = 0; current_chord_notes.chord[i] > -1; i++)
        {
            DrawBoundingBoxAsCube (keyBoxes[(int)current_chord_notes.chord[i]],
                                   BLUE);
        }
    DrawGrid (10, 1);
    EndMode3D ();
    /* for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++) */
    /*     DrawMenuButton (buttons[INTERVAL_SCREEN][i]); */
    DrawText (TextFormat ("Current Key is %s", Key_to_text (key)), 30, 30, 24,
              RAYWHITE);
}

void
UpdateIntSettingToChord (void)
{
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

int
GenChord (int key, int prog_number)
{
    if (progression_counter >= progressions.prog_length[0])
        {
            progression_counter = 0;
            state = 0;
            return 1;
        }
    former_chord_notes = current_chord_notes;
    current_chord_notes = convert_chordT_to_chordN (
        progressions.data[0][progression_counter], key);
    if (current_chord_notes.chord[0] == -1)
        {
            progression_counter = 0;
            state = 0;
            return 1;
        }
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (current_chord_notes.chord[i] != -1)
                {
                    current_chord_notes.chord[i]
                        = current_chord_notes.chord[i] + OCTAVE * 4;
                }
        }
    progression_counter++;
    return 0;
}

void
PlayChordProg (int key)
{
    key = key + OCTAVE * 3;
    int random_prog = GetRandomValue (0, progressions.size - 1);
    if (framesCounter == 80)
        {
            if (!GenChord (key, random_prog))
                {
                    TransitionChord ();
                    PlayChord (current_chord_notes);
                }
            framesCounter = 0;
        }
    framesCounter++;
}

void
TransitionChord (void)
{
    if (former_chord_notes.chord[0] == -1)
        {
            for (int i = 1; i < MAX_CHORD_SIZE; i++)
                {
                    if (current_chord_notes.chord[i] == -1)
                        {
                            break;
                        }
                    else if (current_chord_notes.chord[i]
                             < current_chord_notes.chord[i - 1])
                        {
                            current_chord_notes.chord[i] += OCTAVE;
                        }
                }
        }

    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (current_chord_notes.chord[i] == -1)
                {
                    break;
                }
            int note_distance = OCTAVE;
            for (int j = 0; j < MAX_CHORD_SIZE; j++)
                {
                    int new_distance = current_chord_notes.chord[i]
                                       - former_chord_notes.chord[j];
                    if (current_chord_notes.chord[i] % 12
                        == former_chord_notes.chord[j] % 12)
                        {
                            note_distance = 0;
                            current_chord_notes.chord[i]
                                = former_chord_notes.chord[j];
                            break;
                        }
                    if (abs (new_distance) > AUGUMENTED_FOURTH)
                        {
                            if (new_distance < -AUGUMENTED_FOURTH)
                                new_distance = new_distance + OCTAVE;
                            else
                                new_distance = new_distance - OCTAVE;
                        }
                    if (abs (new_distance) < note_distance)
                        {
                            note_distance = new_distance;
                            current_chord_notes.chord[i]
                                = former_chord_notes.chord[j] + note_distance;
                            break;
                        }
                }
        }
}

void
PlayChord (ChordNotes current_chord_notes)
{
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (current_chord_notes.chord[i] > -1
                && current_chord_notes.chord[i] < NO_OF_NOTES)
                {
                    PlaySound (sound[current_chord_notes.chord[i]]);
                }
        }
}

int
FinishChordScreen (void)
{
    return finish;
}
