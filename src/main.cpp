#include "wav.h"
#include "dsp/eq.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

static inline float clamp1(float x) { return std::max(-1.0f, std::min(1.0f, x)); }
static inline float softclip_tanh(float x) {
  const float k = 1.0f / std::tanh(1.0f);
  return clamp1(std::tanh(x) * k);
}

static void help() {
  std::cout <<
    "audio_eq_lab (MVP)\n"
    "Usage:\n"
    "  audio_eq_lab <in.wav> <out.wav> --freq <Hz> --q <Q> --gain_db <dB> [--preamp_db <dB>] [--softclip]\n";
}

int main(int argc, char** argv) {
  if (argc < 3) { help(); return 1; }

  std::string inPath = argv[1];
  std::string outPath = argv[2];

  float freq = 1000.0f;
  float q = 1.0f;
  float gainDb = 0.0f;
  float preampDb = 0.0f;
  bool softclip = false;

  for (int i = 3; i < argc; ) {
    std::string opt = argv[i];
    if (opt == "--freq" && i+1 < argc) { freq = std::stof(argv[i+1]); i += 2; }
    else if (opt == "--q" && i+1 < argc) { q = std::stof(argv[i+1]); i += 2; }
    else if (opt == "--gain_db" && i+1 < argc) { gainDb = std::stof(argv[i+1]); i += 2; }
    else if (opt == "--preamp_db" && i+1 < argc) { preampDb = std::stof(argv[i+1]); i += 2; }
    else if (opt == "--softclip") { softclip = true; i += 1; }
    else { std::cerr << "Unknown option: " << opt << "\n"; help(); return 1; }
  }

  WavData w;
  std::string err;
  if (!read_wav_pcm16(inPath, w, err)) { std::cerr << "Read error: " << err << "\n"; return 2; }

  EqChain eq;
  eq.setPreampDb(preampDb);
  eq.addPeaking((float)w.sampleRate, freq, q, gainDb);

  WavData out = w;

  for (size_t i = 0; i < w.samples.size(); i++) {
    float x = float(w.samples[i]) / 32768.0f;
    float y = eq.process(x);
    y = softclip ? softclip_tanh(y) : clamp1(y);

    int32_t s = (int32_t)std::lrint(clamp1(y) * 32767.0f);
    s = std::max(-32768, std::min(32767, s));
    out.samples[i] = (int16_t)s;
  }

  if (!write_wav_pcm16(outPath, out, err)) { std::cerr << "Write error: " << err << "\n"; return 3; }

  std::cout << "OK SR=" << w.sampleRate << " ch=" << w.channels
            << " peaking(freq=" << freq << ", Q=" << q << ", gain=" << gainDb << "dB)"
            << " preamp=" << preampDb << "dB"
            << " clip=" << (softclip ? "soft" : "hard") << "\n";
  return 0;
}
