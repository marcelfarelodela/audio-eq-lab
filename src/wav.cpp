#include "wav.h"
#include <fstream>
#include <string>

static bool read_exact(std::ifstream& f, void* dst, size_t n) {
  return static_cast<bool>(f.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(n)));
}

static void skip(std::ifstream& f, uint32_t n) {
  f.seekg(static_cast<std::streamoff>(n), std::ios::cur);
}

bool read_wav_pcm16(const std::string& path, WavData& out, std::string& err) {
  std::ifstream fin(path, std::ios::binary);
  if (!fin) { err = "Cannot open input file."; return false; }

  char riff[4]; uint32_t riffSize = 0; char wave[4];
  if (!read_exact(fin, riff, 4) || !read_exact(fin, &riffSize, 4) || !read_exact(fin, wave, 4)) {
    err = "Invalid header (cannot read RIFF/WAVE)."; return false;
  }
  if (std::string(riff,4) != "RIFF" || std::string(wave,4) != "WAVE") {
    err = "Not a RIFF/WAVE file."; return false;
  }

  bool gotFmt = false, gotData = false;
  uint16_t audioFormat = 0;
  uint32_t dataBytes = 0;
  std::streampos dataPos = 0;

  while (fin && !(gotFmt && gotData)) {
    char id[4]; uint32_t size = 0;
    if (!read_exact(fin, id, 4) || !read_exact(fin, &size, 4)) break;

    std::string chunkId(id,4);

    if (chunkId == "fmt ") {
      if (size < 16) { err = "Invalid fmt chunk size."; return false; }

      uint32_t byteRate = 0;
      uint16_t blockAlign = 0;

      if (!read_exact(fin, &audioFormat, 2) ||
          !read_exact(fin, &out.channels, 2) ||
          !read_exact(fin, &out.sampleRate, 4) ||
          !read_exact(fin, &byteRate, 4) ||
          !read_exact(fin, &blockAlign, 2) ||
          !read_exact(fin, &out.bitsPerSample, 2)) {
        err = "Cannot read fmt chunk."; return false;
      }

      if (size > 16) skip(fin, size - 16);
      gotFmt = true;
    }
    else if (chunkId == "data") {
      dataPos = fin.tellg();
      dataBytes = size;
      skip(fin, size);
      gotData = true;
    }
    else {
      skip(fin, size);
    }

    if (size % 2 == 1) skip(fin, 1);
  }

  if (!gotFmt || !gotData) { err = "Missing fmt or data chunk."; return false; }
  if (audioFormat != 1) { err = "Only PCM (format=1) supported."; return false; }
  if (out.bitsPerSample != 16) { err = "Only 16-bit PCM supported."; return false; }
  if (out.channels != 1 && out.channels != 2) { err = "Only mono/stereo supported."; return false; }
  if (out.sampleRate == 0) { err = "Invalid sample rate."; return false; }
  if (dataBytes == 0) { err = "Empty data chunk."; return false; }

  out.samples.clear();
  out.samples.resize(dataBytes / sizeof(int16_t));

  fin.clear();
  fin.seekg(dataPos, std::ios::beg);
  if (!fin.read(reinterpret_cast<char*>(out.samples.data()),
                static_cast<std::streamsize>(dataBytes))) {
    err = "Failed to read audio data."; return false;
  }

  return true;
}

bool write_wav_pcm16(const std::string& path, const WavData& in, std::string& err) {
  if (in.bitsPerSample != 16 || (in.channels != 1 && in.channels != 2) || in.sampleRate == 0) {
    err = "Invalid WAV parameters for writing."; return false;
  }

  std::ofstream fout(path, std::ios::binary);
  if (!fout) { err = "Cannot open output file."; return false; }

  const uint16_t audioFormat = 1;
  const uint16_t blockAlign = static_cast<uint16_t>(in.channels * (in.bitsPerSample / 8));
  const uint32_t byteRate = in.sampleRate * blockAlign;
  const uint32_t dataBytes = static_cast<uint32_t>(in.samples.size() * sizeof(int16_t));
  const uint32_t riffSize = 36 + dataBytes;

  fout.write("RIFF", 4);
  fout.write(reinterpret_cast<const char*>(&riffSize), 4);
  fout.write("WAVE", 4);

  fout.write("fmt ", 4);
  uint32_t fmtSize = 16;
  fout.write(reinterpret_cast<const char*>(&fmtSize), 4);
  fout.write(reinterpret_cast<const char*>(&audioFormat), 2);
  fout.write(reinterpret_cast<const char*>(&in.channels), 2);
  fout.write(reinterpret_cast<const char*>(&in.sampleRate), 4);
  fout.write(reinterpret_cast<const char*>(&byteRate), 4);
  fout.write(reinterpret_cast<const char*>(&blockAlign), 2);
  fout.write(reinterpret_cast<const char*>(&in.bitsPerSample), 2);

  fout.write("data", 4);
  fout.write(reinterpret_cast<const char*>(&dataBytes), 4);
  fout.write(reinterpret_cast<const char*>(in.samples.data()),
             static_cast<std::streamsize>(dataBytes));

  if (!fout) { err = "Write error while writing WAV."; return false; }
  return true;
}
