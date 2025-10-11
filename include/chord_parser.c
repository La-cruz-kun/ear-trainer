#include "chord_parser.h"
#include "ear_trainer.h"

static char buffer[LINE_LENGTH];

FILE *
open_chord_file (const char *fileName)
{
    FILE *file = fopen (fileName, "r");
    if (file == NULL)
        {
            perror (TextFormat ("Unable to open file %s\n", fileName));
            return NULL;
        }
    return file;
}

int
append_chord_prog (Chord_Progs *prog, ChordType *element)
{ // element must be an array of 4 values unused value should be -1
    if (prog->data == NULL)
        {
            prog->capacity = 4;
            prog->size = 0;
            prog->data = (ChordType **)malloc (
                prog->capacity
                * sizeof (ChordType *)); // initialize with 4 Chords[4] arrays
            if (prog->data == NULL)
                {
                    printf ("could initialize linked list\n");
                    return -1;
                }
        }
    if (prog->size == prog->capacity)
        {
            prog->capacity = prog->capacity * 2;
            prog->data
                = realloc (prog->data, prog->capacity * sizeof (ChordType *));
            if (prog->data == NULL)
                {
                    printf ("counld not resize the linked list\n");
                    return -1;
                }
        }

    prog->data[prog->size] = element;
    prog->size++;
    return 0;
}

ChordType
convert_str_to_chordT (char *chord)
{
    if (strcmp (chord, "I") == 0)
        return I;
    else if (strcmp (chord, "ii") == 0)
        return ii;
    else if (strcmp (chord, "iii") == 0)
        return iii;
    else if (strcmp (chord, "IV") == 0)
        return IV;
    else if (strcmp (chord, "V") == 0)
        return V;
    else if (strcmp (chord, "vi") == 0)
        return vi;
    else if (strcmp (chord, "vii") == 0)
        return vii;
    return -1;
}

ChordNotes
convert_chordT_to_chordN (ChordType chordT, Notes base_note)
{
    ChordNotes chordN = { .chord = { -1, -1, -1, -1 } };
    switch (chordT)
        {
        case I:
            {
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case ii:
            {
                base_note = base_note + MAJOR_SECOND;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case iii:
            {
                base_note = base_note + MAJOR_THIRD;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case IV:
            {
                base_note = base_note + FOURTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case V:
            {
                base_note = base_note + FIFTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case vi:
            {
                base_note = base_note + MAJOR_SIXTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        case vii:
            {
                base_note = base_note + MAJOR_SEVENTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
                return chordN;
            }
        default:
            return chordN;
        }
}

int
parse_chord_file (char *fileName)
{
    FILE *file = open_chord_file (fileName);
    if (!file)
        return 1;

    while (fgets (buffer, LINE_LENGTH, file))
        {
            int p_counter = 0; // parser counter
            int c_counter = 0; // chord counter
            char temp[16]
                = { 0 }; // Hoping no chord name is longer than 16 characters
            ChordType *line_prog
                = (ChordType *)malloc (4 * sizeof (ChordType));
            memset (line_prog, -1, 4);
            for (int i = 0; i < LINE_LENGTH; i++)
                {
                    if (buffer[i] == 0 || buffer[i] == EOF || buffer[i] == ' '
                        || buffer[i] == '\0')
                        {
                            append_chord_prog (&progressions, line_prog);
                            break;
                        }
                    if (buffer[i] == '-' || buffer[i] == '\n')
                        {
                            p_counter = 0;
                            line_prog[c_counter]
                                = convert_str_to_chordT (temp);
                            memset (temp, 0, 16);
                            c_counter++;
                        }
                    else
                        {
                            temp[p_counter] = buffer[i];
                            p_counter++;
                        }
                }
        }
    return 0;
}
