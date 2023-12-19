#pragma once

#include <stdbool.h>

void calibration_baseline(float* wave_data, int frame_size);
float cal_zero_crossing_rate(float* wave_data, int frame_size);
float cal_short_time_energy(float* wave_data, int frame_size);
bool is_probable_snoring(float* wave_data, int frame_size);
