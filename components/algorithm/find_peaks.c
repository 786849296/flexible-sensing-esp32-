#include<stdio.h>
#include<stdlib.h>

// 数组分区 quicksort函数调用
int partition(float* arr, int* indices, int low, int high) {
    float pivot = arr[indices[high]];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[indices[j]] < pivot) {
            i++;
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
    }

    int temp = indices[i + 1];
    indices[i + 1] = indices[high];
    indices[high] = temp;

    return (i + 1);
}

// 快速排序 argsort函数调用
void quicksort(float* arr, int* indices, int low, int high) {
    if (low < high) {
        int pi = partition(arr, indices, low, high);

        quicksort(arr, indices, low, pi - 1);
        quicksort(arr, indices, pi + 1, high);
    }
}

// 将arr升序排列后的索引保存在indices中 fun_selectbyDistance函数调用
void argsort(float* arr, int* indices, int length) {
    for (int i = 0; i < length; i++) {
        indices[i] = i;
    }

    quicksort(arr, indices, 0, length - 1);
}

// 定位所有波峰
// signal:信号数组, length:信号数组长度, peaks_count:用于保存峰值个数
// float signal[] = { };
// int length = sizeof(signal) / sizeof(signal[0]);
// int peaks_count = 0;
// int* peaks = fun_findAllPeaks(signal, length, &peaks_count);
int* fun_findAllPeaks(float* signal, int length, int* peaks_count) {
    int i = 1;
    int i_max = length - 1;
    int* peaks = (int*)malloc(length * sizeof(int));
    int peaks_found = 0;

    while (i < i_max) {
        if (signal[i - 1] < signal[i]) {
            int i_ahead = i + 1;
            while (i_ahead < i_max && signal[i_ahead] == signal[i]) {
                i_ahead += 1;
            }
            if (signal[i_ahead] < signal[i]) {
                if (peaks != NULL) {
                    peaks[peaks_found] = i;
                    peaks_found++;
                    i = i_ahead;
                }
            }
        }
        i += 1;
    }

    *peaks_count = peaks_found;
    return peaks;
}


// 根据height筛选峰值
// signal:信号数组, peaks:峰值数组, peaks_size:峰值个数, peaks_count:用于保存峰值个数
// float signal[] = { };
// int peaks[] = { };
// int peaks_size = sizeof(peaks) / sizeof(peaks[0]);
// int peaks_count = 0;
// int* fun_selectbyHeight(signal, peaks, peaks_size, &peaks_count);
int* fun_selectbyHeight(float* signal, int* peaks, int peaks_size, int* peaks_count) {
    float num = 0;
    float maxpeaks = 0;
    for (int i = 0; i < peaks_size; i++) {
        num += signal[peaks[i]];
        if (signal[peaks[i]] > maxpeaks) {
            maxpeaks = signal[peaks[i]];
        }
    }

    // 计算peaksheight
    float peaksheight = num / peaks_size;
    // float peaksheight = (num - maxpeaks) / (*peaks_count - 1);

    int* new_peaks = (int*)malloc(peaks_size * sizeof(int));
    int new_peaks_found = 0;
    for (int i = 0; i < peaks_size; i++) {
        if (signal[peaks[i]] > peaksheight && new_peaks != NULL) {
            new_peaks[new_peaks_found] = peaks[i];
            new_peaks_found++;
        }
    }
    *peaks_count = new_peaks_found;
    return new_peaks;
}


// 根据distance筛选峰值
// signal:信号数组, peaks:峰值数组, peaks_size:峰值个数, distance:峰值最小距离, peaks_count:用于保存峰值个数
// float signal[] = { };
// int peaks[] = { };
// int peaks_size = sizeof(peaks) / sizeof(peaks[0]);
// int distance = 59;
// int peaks_count = 0;
// int* new_peaks = fun_selectbyDistance(signal, peaks, peaks_size, distance, &peaks_count);
int* fun_selectbyDistance(float* signal, int* peaks, int peaks_size, int distance, int* peaks_count) {
    int* new_peaks = (int*)malloc(peaks_size * sizeof(int)); // newpeaks分配空间
    int* keep = (int*)malloc(peaks_size * sizeof(int)); // 峰值条件判断布尔数组
    for (int i = 0; i < peaks_size; i++) {
        keep[i] = 1;
    }
    float* priority = (float*)malloc(peaks_size * sizeof(float));
    for (int i = 0; i < peaks_size; i++) {
        priority[i] = signal[peaks[i]];
    }

    int* priority_argsort = (int*)malloc(peaks_size * sizeof(int));
    argsort(priority, priority_argsort, peaks_size);

    for (int i = peaks_size - 1; i >= 0; i--) {
        if (priority_argsort != NULL) {
            int j = priority_argsort[i];
            if (keep[j] == 0) {
                continue;
            }
            // 删除左侧不满足峰值
            int k = j - 1;
            while (k >= 0 && peaks[j] - peaks[k] < distance) {
                keep[k] = 0;
                k--;
            }
            // 删除右侧不满足峰值
            k = j + 1;
            while (k < peaks_size && peaks[k] - peaks[j] < distance) {
                keep[k] = 0;
                k++;
            }
        }
    }

    int new_peaks_found = 0;
    for (int i = 0; i < peaks_size; i++) {
        if (keep[i] == 1) {
            new_peaks[new_peaks_found] = peaks[i];
            new_peaks_found++;
        }
    }

    free(keep);
    free(priority);
    free(priority_argsort);

    *peaks_count = new_peaks_found;
    return new_peaks;
}
