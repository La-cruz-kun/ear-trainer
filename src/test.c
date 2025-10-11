#include "ear_trainer.h"
#include <chord_parser.h>
#include <stdio.h>

void
GenChord (int key, ChordNotes *chord_notes, Chord_Progs progressions)
{
    int octave = GetRandomValue (3, 6);
    key = key + octave;
    *chord_notes = convert_chordT_to_chordN (progressions.data[0][0], key);
}

int
main (void)
{
    Chord_Progs prog = { 0 };
    ChordNotes chord = { 0 };
    prog = parse_chord_file ("resources/chordprogression.txt");
    GenChord (C4, &chord, prog);

    return 0;
}
