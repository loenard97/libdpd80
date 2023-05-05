/*
	Callback functions, that get triggered by main when new data arrives.
	Each data stream starts with "START_OF_STREAM\n" and ends with "END_OF_STREAM\n".
*/

#include <stdio.h>
#include "ri.h"
#include "callbacks.h"

int transfer_callback(uint16_t* data, int ndata, int dataloss, void* userdata)
{
	int64_t* samples_left = (int64_t*)userdata;

	if (dataloss)
		printf("data loss detected\n");

	printf("%d\n", data[0] - 49152);	// remove bit mask

	*samples_left -= ndata;
	return *samples_left > 0;
}

/*
	Return tuple ndata;sum for each package.
*/
int callback_counter(uint16_t* data, int ndata, int dataloss, void* userdata)
{
	unsigned long long sum = 0;
	int64_t* samples_left = (int64_t*)userdata;

	if (dataloss)
		printf("ERR!: DATA LOSS DETECTED\n");

	for (int i = 0; i < ndata; ++i)
	{
		sum += data[i] & 0x03ff;	// apply data bit mask
	}
	printf("DATA: %d;%llu\n", ndata, sum);

	*samples_left -= ndata;

	return *samples_left > 0;
}
