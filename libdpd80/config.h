#ifndef CONFIG_H
#define CONFIG_H

#include "libdpd80.h"

typedef enum measurement_type {
	COUNTER,
	HISTOGRAM,
} MeasurementType;

typedef struct config {
	MeasurementType measurement_type;
	unsigned long n_samples;
} Config;

ERROR_STATUS parse_config(int argc, char* argv[], Config* config);

#endif
