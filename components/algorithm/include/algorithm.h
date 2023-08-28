#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "filter.h"

#define FILTER_N 5
#define FILTER_FS 100.0
#define FILTER_LOW_WC 0.4
#define FILTER_BP_WC_LOW 1.1
#define FILTER_BP_WC_HIGH 12.0

int cal_diff(float data[], int len);
int peak_diff(int data[], int len);
int exp_list(float data[], int len);
int peak2_zip(int data[], int len);
int peak3_zip(int data[], int len);
bool is_onBed(int data[], int len);
bool is_bodyMove(int data1[], int data2[], int len);
