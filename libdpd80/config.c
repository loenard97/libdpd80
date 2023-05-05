#include <stdio.h>
#include "config.h"
#include "libdpd80.h"

ERROR_STATUS parse_config(int argc, char* argv[], Config* config) {
	// default settings if no args are given
	if (argc == 1) {
		config->measurement_type = COUNTER;
		config->n_samples = 80 * 1000 * 1000;	// 1s of measurement time
	}
	else if (argc == 2) {
		if (strcmp(argv[1], "histogram") == 0) {
			config->measurement_type = HISTOGRAM;
		}
	}
	else {
		return STATUS_FAILURE;
	}

	return STATUS_SUCCESS;
}
