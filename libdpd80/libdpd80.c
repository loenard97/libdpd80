#define DEBUG

#include <stdio.h>
#include <time.h>
#include "ri.h"
#include "libdpd80.h"
#include "callbacks.h"
#include "config.h"

ERROR_STATUS main(int argc, char* argv[]) {
#ifdef DEBUG
	printf("META: ARGC %d\n", argc);
	for (int i = 0; i < argc; ++i) {
		printf("META: ARGV[%d] %s\n", i, argv[i]);
	}
#endif

	// parse config
	Config config;
	if (parse_config(argc, argv, &config) != STATUS_SUCCESS) {
		printf("ERR!: ARGS INCORRECT");
		return 1;
	}
#ifdef DEBUG
	printf("META: CONFIG MEASUREMENT TYPE %d\n", config.measurement_type);
#endif

	// initialize device
	ri_init();

	int err = 0;
	ri_device* device = 0;

	device = ri_open_device();
	if (device == 0) {
		fprintf(stderr, "Error: device not found\n");
		return 1;
	}

	// run measurement
	if (config.measurement_type == COUNTER) {
		const int64_t samples_to_transfer = config.n_samples;
		double initial_time = (double)clock() / CLOCKS_PER_SEC;

		int64_t samples_left = samples_to_transfer;

		printf("META: REQUEST COUNTER SAMPLES %lld\n", samples_to_transfer);
		printf("META: START_OF_STREAM\n");
		ri_start_continuous_transfer(device, callback_counter, &samples_left);
		printf("META: END_OF_STREAM\n");

		double final_time = (double)clock() / CLOCKS_PER_SEC;
		double MBs_transferred = samples_to_transfer * 2. / 1000000.;
		printf("META: TRANSFERED / MB %.1f\n", MBs_transferred);
		printf("META: ELAPSED TIME / s %g\n", final_time - initial_time);
		printf("META: SPEED / MBPS %.2f\n", MBs_transferred / (final_time - initial_time));
	}
	else {
		printf("ERR!: MEASUREMENT TYPE UNKNOWN");
		return 1;
	}

	// close device
	device = ri_close_device(device);
	ri_exit();
	return 0;
}
