#include <stdio.h>
#include "MIC.h"

void calibration_baseline(float* wave_data, int frame_size) {
    float sum = 0, average_data = 0;
    for (int i = 0; i < frame_size; i++)
        sum += wave_data[i];
    average_data = sum / frame_size;
    for (int j = 0; j < frame_size; j++)
        wave_data[j] = (wave_data[j] - average_data) / 1000;
}

int sgn(float data) {
    if (data >= 0)
        return 1;
    else
        return 0;
}
float cal_zero_crossing_rate(float* wave_data, int frame_size) {
    int sum = 0;
    for (int i = 0; i < frame_size - 1; i++)
        sum += abs(sgn(wave_data[i]) - sgn(wave_data[i + 1]));
    return sum / 2;
}


float cal_short_time_energy(float* wave_data, int frame_size) {
    float sum = 0;
    for (int i = 0; i < frame_size; i++)
        sum += wave_data[i] * wave_data[i];
    return sum;
}

bool is_probable_snoring(float* wave_data, int frame_size) {
    calibration_baseline(wave_data, frame_size);
    float shortTimeEnergy = cal_short_time_energy(wave_data, frame_size);
    float zeroCrossingRate = cal_zero_crossing_rate(wave_data, frame_size);
    if (zeroCrossingRate < 10 && shortTimeEnergy > 5)
        return true;
    else
        return false;
}