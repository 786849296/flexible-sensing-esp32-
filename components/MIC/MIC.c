#include <stdio.h>
#include "MIC.h"
// #include <stdbool.h>

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
    int sum = 0;
    for (int i = 0; i < frame_size; i++)
        sum += wave_data[i] * wave_data[i];
    return sum;
}


_Bool is_probable_snoring(float zeroCrossingRate, float shortTimeEnergy) {
    if (zeroCrossingRate < 40 && shortTimeEnergy > 0.2)
        return 1;
    else
        return 0;
}