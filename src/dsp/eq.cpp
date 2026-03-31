#include "eq.h"
#include <cmath>

void EqChain::setPreampDb(float db) {
  preampLin = std::pow(10.0f, db / 20.0f);
}

void EqChain::addPeaking(float sampleRate, float freqHz, float Q, float gainDb) {
  Biquad b;
  b.setPeaking(sampleRate, freqHz, Q, gainDb);
  biquads.push_back(b);
}

float EqChain::process(float x) {
  float y = x * preampLin;
  for (auto& b : biquads) y = b.process(y);
  return y;
}
