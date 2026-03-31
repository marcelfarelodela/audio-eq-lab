# audio-eq-lab

**audio-eq-lab** is a portfolio project that implements a small, real-world **audio processing tool** in **modern C++**.  
The goal is to demonstrate practical engineering skills around **DSP fundamentals**, **file-based audio workflows**, and **clean, testable code**.

---

## What this project does

This tool reads a **WAV (RIFF) PCM 16-bit** audio file (mono or stereo), applies a **parametric peaking EQ** (a classic biquad filter), and writes a processed WAV output file.

It’s designed as a minimal but realistic “audio tool” you could find in a production workflow:
- deterministic processing (same input → same output),
- predictable output format,
- clear CLI configuration,
- safe output handling (optional soft clipping to avoid harsh digital clipping).

---

## How it works (high level)

1. **WAV input**  
   The program parses the WAV header (RIFF/WAVE) and reads the audio samples (16-bit PCM).

2. **Signal path (DSP chain)**  
   Audio samples are converted from **int16 → float [-1, 1]**, then processed through:
   - a **preamp gain** stage (in dB),
   - a **peaking EQ biquad** designed using the RBJ cookbook formulas (frequency, Q, gain).

3. **Output safety**  
   The processed signal can be either:
   - **hard-clipped** to [-1, 1], or
   - **soft-clipped** (tanh-based) for a smoother saturation.

4. **WAV output**  
   The signal is converted back to **int16** and saved as a WAV file with the same sample rate and channel count.

---

## What results you should expect

- If you set a **positive EQ gain**, energy around the target frequency band increases (you should hear a tonal change).
- **Preamp** can be used to compensate overall gain to avoid clipping when boosting.
- With `--softclip`, peaks near 0 dBFS are smoothly saturated instead of harshly clipped.

Typical output behavior:
- **Sample rate and duration stay the same**
- **Audio remains valid WAV PCM16**
- **Audible frequency emphasis/cut around the EQ band** based on `--freq`, `--q`, and `--gain_db`

---

## Build

    cmake -S . -B build
    cmake --build build -j

---

## Run (example)

    ./build/audio_eq_lab in.wav out.wav --freq 1000 --q 1.0 --gain_db 6 --preamp_db -3 --softclip

Arguments:
- `--freq <Hz>`: center frequency of the peaking EQ band
- `--q <Q>`: bandwidth control (higher Q = narrower band)
- `--gain_db <dB>`: boost/cut for the EQ band
- `--preamp_db <dB>`: gain applied before the filter (useful to prevent clipping)
- `--softclip`: enables smooth tanh-based clipping

---

## Current status (MVP)

- WAV PCM16 mono/stereo I/O
- Peaking EQ (biquad)
- CLI controls (freq/Q/gain/preamp)
- Optional soft clipping

Next planned upgrades:
- JSON config with multiple bands
- Streaming (chunk-based) processing for large files
- Automated regression tests (FFT-based EQ response checks)

