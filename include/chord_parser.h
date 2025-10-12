#ifndef CHORD_PARSER_H
#define CHORD_PARSER_H
#include "ear_trainer.h"
#define LINE_LENGTH 100

typedef struct
{
    ChordType **data;
    size_t *prog_length;
    size_t size;
    size_t capacity;
} Chord_Progs;

typedef struct
{
    ChordType *data;
    size_t size;
    size_t capacity;
} ChordTypes;

int append_chord_prog (Chord_Progs *prog, ChordType *element,
                       size_t element_len);
int append_chord (ChordTypes *CTs, ChordType CT);

ChordType convert_str_to_chordT (char *chord);

FILE *open_chord_file (const char *fileName);
int parse_chord_file (char *fileName);
ChordNotes convert_chordT_to_chordN (ChordType chordT, Notes base_note);

extern Chord_Progs progressions;
#endif // CHORD_PARSER_H
