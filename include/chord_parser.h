// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#ifndef CHORD_PARSER_H
#define CHORD_PARSER_H
#include "ear_trainer.h"
#define LINE_LENGTH 100

typedef struct
{
    ChordType *data;
    size_t size;
    size_t capacity;
} ChordTypes;

typedef struct
{
    ChordTypes *data;
    size_t size;
    size_t capacity;
} Chord_Progs;

int append_chord (ChordTypes *CTs, ChordType CT);
int append_chord_prog (Chord_Progs *prog, ChordTypes element);

ChordType convert_str_to_chordT (char *chord);
void convert_chordT_to_str (ChordType chordT, char *r_str);

FILE *open_chord_file (const char *fileName);
int parse_chord_file (char *fileName);
ChordNotes convert_chordT_to_chordN (ChordType chordT, Notes base_note);

extern Chord_Progs progressions;
#endif // CHORD_PARSER_H
