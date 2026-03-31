#pragma once

class Biquad {
public:
  void reset();
  float process(float x);

  void setPeaking(float sampleRate, float freqHz, float Q, float gainDb);

private:
  float b0=1, b1=0, b2=0, a1=0, a2=0;
  float x1=0, x2=0, y1=0, y2=0;
};
