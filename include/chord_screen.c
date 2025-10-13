#include "chord_parser.h"
#include "ear_trainer.h"
#include <raylib.h>

static int finish = 0;
static unsigned int currentOctave = 0;
static int framesCounter = 0;
static size_t progression_counter = 0;
static ChordNotes current_chord_notes
    = { .chord = { [0 ... MAX_CHORD_SIZE - 1] = -1 } };
static ChordNotes former_chord_notes
    = { .chord = { [0 ... MAX_CHORD_SIZE - 1] = -1 } };
static int state = -1;
static int correct_chord = 0;
static int random_prog = 0;
static void CheckKeyPress (void);

void
InitChordScreen (void)
{
}

void
UpdateChordScreen (void)
{
    if (!IsMusicStreamPlaying (drone[key]))
        {
            PlayMusicStream (drone[key]);
        }
    UpdateMusicStream (drone[key]);

    correct_chord = 0;
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
            ChordNotes chord_notes = convert_chordT_to_chordN (
                progressions.data[random_prog].data[progression_counter], key);
            CheckKeyPress ();
            for (int i = 0; i < MAX_CHORD_SIZE; i++)
                {
                    if (ChordLength (chord_notes.chord)
                        != ChordLength (note_pool))
                        break;
                    for (int j = 0; j < MAX_CHORD_SIZE; j++)
                        {
                            if (chord_notes.chord[i] % OCTAVE
                                == note_pool[j] % OCTAVE)
                                {
                                    correct_chord = 1;
                                    break;
                                }
                            correct_chord = 0;
                        }
                    if (!correct_chord)
                        {
                            break;
                        }
                }

            if (correct_chord)
                {
                    if (progression_counter
                        == progressions.data[random_prog].size - 1)
                        {
                            state = -1;
                            progression_counter = 0;
                        }
                    else
                        {
                            progression_counter++;
                        }
                }
            if (IsKeyReleased (KEY_ENTER))
                {
                    PlayChordProg (key);
                    state = 1;
                }
        }

    else if (state == 1)
        {
            CheckKeyPress ();
            PlayChordProg (key);
        }
    else if (state == -1)
        {
            CheckKeyPress ();
            PlayChordProg (key);
            random_prog = GetRandomValue (0, progressions.size - 1);
            state = 1;
        }
}

void
DrawChordScreen (void)
{

    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; current_chord_notes.chord[i] > -1; i++)
        {
            DrawBoundingBoxAsCube (keyBoxes[(int)current_chord_notes.chord[i]],
                                   BLUE);
        }
    DrawGrid (10, 1);
    EndMode3D ();
    DrawText (TextFormat ("Current Key is %s", Key_to_text (key)), 30, 30, 24,
              RAYWHITE);
    if (state == 0)
        {
            for (size_t i = 0; i < progression_counter; i++)
                {
                    char s[16] = { 0 };
                    convert_chordT_to_str (
                        progressions.data[random_prog].data[i], s);
                    if (s[0] == 0)
                        {
                            break;
                        }
                    DrawTextEx (
                        font2, TextFormat ("%s", s),
                        (Vector2){
                            (float)GetRenderWidth ()
                                    / (progressions.data[random_prog].size + 1)
                                + i * (float)GetRenderWidth ()
                                      / (progressions.data[random_prog].size
                                         + 1),
                            80 },
                        64, 1, GREEN);
                }
        }
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
    if (progression_counter >= progressions.data[prog_number].size)
        {
            progression_counter = 0;
            state = 0;
            return 1;
        }
    former_chord_notes = current_chord_notes;
    current_chord_notes = convert_chordT_to_chordN (
        progressions.data[prog_number].data[progression_counter], key);
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

static void
CheckKeyPress (void)
{
    int key;
    if (state == 0)
        {
            while ((key = GetKeyPressed ()) != 0)
                {
                    if (hmget (Key_to_note, key) != -1)
                        {
                            int note = hmget (Key_to_note, key)
                                       + currentOctave * OCTAVE;
                            for (int i = 0; i < MAX_CHORD_SIZE; i++)
                                {
                                    if (note_pool[i] == -1)
                                        {
                                            note_pool[i] = note;
                                            PlaySound (sound[note]);
                                            break;
                                        }
                                }
                            continue;
                        }
                    if (key == KEY_RIGHT_SHIFT)
                        {
                            if (currentOctave >= 7)
                                continue;
                            currentOctave++;
                            continue;
                        }
                    if (key == KEY_LEFT_SHIFT)
                        {
                            if (currentOctave <= 0)
                                continue;
                            currentOctave--;
                            continue;
                        }
                }
        }
    for (int i = 0; i < hmlen (Key_to_note); i++)
        {
            KeyboardKey key = Key_to_note[i].key;
            if (IsKeyReleased (key))
                {
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    for (int i = 0; i < MAX_CHORD_SIZE; i++)
                        {
                            if (note_pool[i] == note)
                                {
                                    StopSound (sound[note]);
                                    note_pool[i] = -1;
                                }
                        }
                }
        }
}

int
ChordLength (int *chord)
{
    int count = 0;
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (chord[i] == -1)
                continue;
            count++;
        }

    return count;
}

int
FinishChordScreen (void)
{
    return finish;
}
