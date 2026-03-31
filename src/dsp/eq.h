#pragma once
#include "biquad.h"
#include <vector>

class EqChain {
public:
  void setPreampDb(float db);
  void addPeaking(float sampleRate, float freqHz, float Q, float gainDb);
  float process(float x);

private:
  float preampLin = 1.0f;
  std::vector<Biquad> biquads;
};
