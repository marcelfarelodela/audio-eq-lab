#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct WavData {
  uint16_t channels = 0;
  uint32_t sampleRate = 0;
  uint16_t bitsPerSample = 0;          // 16
  std::vector<int16_t> samples;        // interleaved if stereo
};

bool read_wav_pcm16(const std::string& path, WavData& out, std::string& err);
bool write_wav_pcm16(const std::string& path, const WavData& in, std::string& err);
