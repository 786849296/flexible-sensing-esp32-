#pragma once

float cal_zero_crossing_rate(float* wave_data, int frame_size);
float cal_short_time_energy(float* wave_data, int frame_size);
_Bool is_probable_snoring(float zeroCrossingRate, float shortTimeEnergy);
