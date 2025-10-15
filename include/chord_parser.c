// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#include "chord_parser.h"
#include "ear_trainer.h"
#include <stddef.h>

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
append_chord_prog (Chord_Progs *prog, ChordTypes element)
{ // element must be an array of 4 values unused value should be -1
    if (prog->data == NULL)
        {
            prog->capacity = 4;
            prog->size = 0;
            prog->data = (ChordTypes *)malloc (
                prog->capacity
                * sizeof (ChordTypes)); // initialize with 4 Chords[4] arrays
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
                = realloc (prog->data, prog->capacity * sizeof (ChordTypes));
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

int
append_chord (ChordTypes *CTs, ChordType CT)
{
    if (CTs->data == NULL)
        {
            CTs->capacity = 4;
            CTs->size = 0;
            CTs->data = (ChordType *)malloc (
                CTs->capacity
                * sizeof (ChordType)); // initialize with 4 Chords[4] arrays
            if (CTs->data == NULL)
                {
                    printf ("could initialize linked list\n");
                    return -1;
                }
        }
    if (CTs->size == CTs->capacity)
        {
            CTs->capacity = CTs->capacity * 2;
            CTs->data
                = realloc (CTs->data, CTs->capacity * sizeof (ChordType));
            if (CTs->data == NULL)
                {
                    printf ("counld not resize the linked list\n");
                    return -1;
                }
        }

    CTs->data[CTs->size] = CT;
    CTs->size++;
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

void
convert_chordT_to_str (ChordType chordT, char *str)
{
    switch (chordT)
        {
        case I:
            str[0] = 'I';
            break;
        case ii:
            str[0] = 'i';
            str[1] = 'i';
            break;
        case iii:
            str[0] = 'i';
            str[1] = 'i';
            str[2] = 'i';
            break;
        case IV:
            str[0] = 'I';
            str[1] = 'V';
            break;
        case V:
            str[0] = 'V';
            break;
        case vi:
            str[0] = 'v';
            str[1] = 'i';
            break;
        case vii:
            str[0] = 'v';
            str[1] = 'i';
            str[2] = 'i';
            break;
        default:
            break;
        }
}

ChordNotes
convert_chordT_to_chordN (ChordType chordT, Notes base_note)
{
    ChordNotes chordN = { .chord = { [0 ... MAX_CHORD_SIZE - 1] = -1 } };
    switch (chordT)
        {
        case I:
            {
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case ii:
            {
                base_note = base_note + MAJOR_SECOND;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case iii:
            {
                base_note = base_note + MAJOR_THIRD;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case IV:
            {
                base_note = base_note + FOURTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case V:
            {
                base_note = base_note + FIFTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MAJOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case vi:
            {
                base_note = base_note + MAJOR_SIXTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        case vii:
            {
                base_note = base_note + MAJOR_SEVENTH;
                chordN.chord[0] = base_note;
                chordN.chord[1] = base_note + MINOR_THIRD;
                chordN.chord[2] = base_note + FIFTH;
            }
            break;
        default:
            return chordN;
        }
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (chordN.chord[i] != -1)
                {
                    chordN.chord[i] = chordN.chord[i] % OCTAVE;
                }
        }
    return chordN;
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
            char temp[16]
                = { 0 }; // Hoping no chord name is longer than 16 characters
            ChordTypes line_prog = { 0 };
            for (int i = 0; i < LINE_LENGTH; i++)
                {
                    if (buffer[i] == 0 || buffer[i] == EOF || buffer[i] == ' '
                        || buffer[i] == '\0')
                        {
                            for (size_t i = line_prog.size;
                                 i < line_prog.capacity; i++)
                                {
                                    line_prog.data[i] = -1;
                                }
                            append_chord_prog (&progressions, line_prog);
                            break;
                        }
                    if (buffer[i] == '-' || buffer[i] == '\n')
                        {
                            p_counter = 0;
                            append_chord (&line_prog,
                                          convert_str_to_chordT (temp));
                            memset (temp, 0, 16);
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
