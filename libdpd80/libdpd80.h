#ifndef LIBDPD80_H
#define LIBDPD80_H

typedef enum error_status {
	STATUS_SUCCESS = 0, 
	STATUS_FAILURE = 1, 
} ERROR_STATUS;

ERROR_STATUS main(int argc, char* argv[]);

#endif
