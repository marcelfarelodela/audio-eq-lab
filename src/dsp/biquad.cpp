#include "biquad.h"
#include <cmath>
#include <algorithm>

void Biquad::reset() { x1=x2=y1=y2=0.0f; }

float Biquad::process(float x) {
  float y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;
  x2 = x1; x1 = x;
  y2 = y1; y1 = y;
  return y;
}

void Biquad::setPeaking(float sampleRate, float freqHz, float Q, float gainDb) {
  freqHz = std::max(10.0f, std::min(freqHz, 0.49f * sampleRate));
  Q = std::max(0.05f, Q);

  float A = std::pow(10.0f, gainDb / 40.0f);
  float w0 = 2.0f * float(M_PI) * (freqHz / sampleRate);
  float cosw0 = std::cos(w0);
  float sinw0 = std::sin(w0);
  float alpha = sinw0 / (2.0f * Q);

  float bb0 = 1.0f + alpha * A;
  float bb1 = -2.0f * cosw0;
  float bb2 = 1.0f - alpha * A;
  float aa0 = 1.0f + alpha / A;
  float aa1 = -2.0f * cosw0;
  float aa2 = 1.0f - alpha / A;

  b0 = bb0 / aa0;
  b1 = bb1 / aa0;
  b2 = bb2 / aa0;
  a1 = aa1 / aa0;
  a2 = aa2 / aa0;

  reset();
}
