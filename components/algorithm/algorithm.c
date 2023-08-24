#include "algorithm.h"

inline int cal_diff(float data[], int len)
{
	//float* data_diff = malloc(sizeof(float) * (len - 2));
	for (int i = 1; i < len - 1; i++)
		data[i - 1] = (data[i + 1] - data[i - 1]) / 2.0;
	return len - 2;
}

inline int peak_diff(int data[], int len)
{
	//float* peaks_diff = malloc(sizeof(float) * (len - 1));
	for (int i = 0; i < len - 1; i++)
		data[i] = data[i + 1] - data[i];
	return len - 1;
}

inline int exp_list(float data[], int len)
{
	//float* data_exp = malloc(sizeof(float) * len);
	for (int i = 0; i < len; i++)
		data[i] = exp(data[i]) - 1;
	return len;
}

inline int peak2_zip(int data[], int len)
{
	//float* peak_zip = malloc(sizeof(float) * (len - 1));
	for (int i = 0; i < len - 1; i++)
		data[i] = data[i] + data[i + 1];
	return len - 1;
}

inline int peak3_zip(int data[], int len)
{
	//float* peak_zip = malloc(sizeof(float) * (len - 2));
	for (int i = 0; i < len - 2; i++)
		data[i] = data[i] + data[i + 1] + data[i + 2];
	return len - 2;
}

bool is_onBed(int data[], int len)
{
	for (int i = 0; i < len; i++)
		if (data[i] > 100)
			return true;
	return false;
}

bool is_bodyMove(int data1[], int data2[], int len)
{
	for (int i = 0; i < len; i++)
		if (abs(data1[i] - data2[i]) > 250)
			return true;
	return false;
}