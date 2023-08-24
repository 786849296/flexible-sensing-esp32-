#pragma once
#ifndef FIND_PEAKS
#define FIND_PEAKS

int* fun_findAllPeaks(float* signal, int length, int* peaks_count);
int* fun_selectbyDistance(float* signal, int* peaks, int peaks_size, int distance, int* peaks_count);
int* fun_selectbyHeight(float* signal, int* peaks, int peaks_size, int* peaks_count);

#endif
