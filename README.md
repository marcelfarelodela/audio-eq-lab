# audio-eq-lab

Portfolio project: **Parametric EQ (biquad)** in C++.

## MVP
- WAV PCM16 mono/stereo I/O
- Peaking EQ (RBJ biquad)
- CLI: freq / Q / gain / preamp
- Optional soft clipping

## Build
    cmake -S . -B build
    cmake --build build -j

## Run
    ./build/audio_eq_lab in.wav out.wav --freq 1000 --q 1.0 --gain_db 6 --preamp_db -3 --softclip
