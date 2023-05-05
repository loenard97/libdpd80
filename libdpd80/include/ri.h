/*
 * Resolved Instruments DPD80 interface library
 *
 * Copyright (C) 2016 - 2017 Resolved Instruments Inc.
 * All Rights Reserved.
 *
 */
/*! \file ri.h */

#ifndef RI_H
#define RI_H
#ifdef __cplusplus
extern "C" {
#endif


#if defined _WIN32
	#if defined LIBRI_STATIC
		#define LIBRI_API
	#elif defined LIBRI_EXPORT
		#define LIBRI_API __declspec(dllexport)
	#else
		#define LIBRI_API __declspec(dllimport)
	#endif
#else
	#define LIBRI_API
#endif


#include "stdint.h"
typedef unsigned int uint;

/*!
 * Enum containing the various USB connection speeds
 * that a device can operate in.
 * \see ri_get_usb_speed()
 */
typedef enum RI_USB_SPEED_t {
	RI_SPEED_UNKNOWN,
	RI_SPEED_LOW,
	RI_SPEED_FULL,
	RI_SPEED_HIGH,
	RI_SPEED_SUPER,

	RI_SPEED_ERROR = -1,
} RI_USB_SPEED_t;


/*!
 * Enum containing various error codes used by libri.
 *
 * Often a function returning non-zero signals an error
 * and value returned can give an indication where the error occured.
 */
typedef enum RI_ERROR_t {
	RI_SUCCESS = 0, //!< Operation completed succesfully
	RI_ERROR = -1, //!< Generic error occured


	RI_ERROR_PARAMS = 0x30, //!< Error occured with function parameters
	RI_ERROR_MEMORY, //!< Memory allocation error
	RI_ERROR_INVALID_DEVICE, //!< Invalid #ri_device
	RI_ERROR_BAD_CHECKSUM, //!< Invalid checksum or signature computed for stored data
	RI_ERROR_DEVICE_SETTINGS, //!< The factory settings stored on the device are not configured correctly
	RI_ERROR_NOT_IMPLEMENTED, //!< A needed feature is currently not implemented
	RI_ERROR_FIRMWARE, //!<< Error occured in firmware on the device.

	RI_ERROR_USB = 0x20, //!< Generic USB error
	RI_ERROR_USB_TIMEOUT, //!< USB timeout error
} RI_ERROR_t;

#ifndef RIPORTS_DEFINED
#define RIPORTS_DEFINED
/*! An enum identifying the possible DPD80 ports.
 *
 *
 */
typedef enum {
	RI_PORT_A, //!< Port A
	RI_PORT_B, //!< Port B
	RI_PORT_S, //!< Port S
	RI_PORT_T, //!< Port T
} RI_PORT_t;
#endif



// pack ri_version_t to byte so it's equivalent to a uint8_t[3].
#pragma pack(push)
#pragma pack(1)

/*! A structure containing versioning information.  This is represented in text as \a major.minor.release
 */
typedef struct ri_version_t {
	uint8_t major; //!< major version
	uint8_t minor; //!< minor version
	uint8_t release; //!< release number
} ri_version_t;

#pragma pack(pop)



/*! A structure containing basic information about an device
 *
 * \see ri_list_devices()
 */
typedef struct ri_device_info_t {
	char product[16]; //!< A null-terminated string describing the device
	char serial[16]; //!< A null-terminated string containing the device's serial number
	uint32_t samplerate; //!< the ADC samplerate of the connected device, in samples per second
	uint8_t bits; //!< the number of ADC bits in the connected device
	ri_version_t fw_version; //!< the firmware version of the connected device
} ri_device_info_t;




/*! \var LIBRI_VERSION
 * A global variable containing the version of the libri headers in use.
 * \sa ri_version()
 */
static const ri_version_t LIBRI_VERSION = {0, 9, 5};

/*! \struct ri_device
 * The device in use is stored as an opaque pointer with the type ri_device*.
 * When ri_device* is zero, no device exists and it's considered unopened.
 *
 * When a ri_device* is not equal to zero, it is assumed to be an opened device.
 * Therefore, always initialize the device pointer to zero on creation.
*/
typedef struct ri_device ri_device;


/*! Initializes libri.
 *
 * ri_init() should be called before any other libri functions
 * are called.  At exit, ri_exit() should be called.
 *
 * \returnerr
 * \see ri_exit()
 */
LIBRI_API int ri_init(void);


/*! Deinitialize libri.
 *
 * Should be called before program exit for proper shutdown of libri.
 *
 * \see ri_init()
 *
 */
LIBRI_API void ri_exit(void);




/*! Opens the first available device.
 *
 * If ri_open_device() returns zero, no devices were able to be found.  If it returns
 * a non-zero pointer, a device has been opened which should eventually be closed with ri_close_device().
 *
 * \return an #ri_device pointer
 * \see ri_device()
 * \see ri_close_device()
 */
LIBRI_API ri_device* ri_open_device(void);

/*! Closes an open #ri_device.
 *
 * After ri_close_device() is called the
 * #ri_device is invalid and the pointer should be set to zero.
 * For convenience, this function returns zero for the pointer to be set to.
 *
 * \devparam
 * \return a null *#ri_device pointer
 * \see #ri_device
 * \see ri_open_device()
 */
LIBRI_API ri_device* ri_close_device(ri_device *dev);

/*! Resets the device
 * \devparam
 * \returnerr
 */
LIBRI_API int ri_reset_device(ri_device* dev);

/*! List connected devices
 *
 * Returns an array of #ri_device_info_t of all devices found connected to the computer.
 * The pointer \a ndevices is set to the number of devices found and the size of the returned array.
 *
 * This does not open any devices, the serial number of the desired device can be found and the device can be opened with ri_open_from_serial().
 *
 * If no devices are found, or an error occurred, zero is returned.
 *
 * If succesfull, the #ri_device_info_t* array must be deallocated with a call to ri_free_device_list().
 *
 * \param ndevices stores number of devices found
 * \return c array of #ri_device_info_t with size ndevices
 * \see ri_free_device_list()
 *
*/
LIBRI_API ri_device_info_t* ri_list_devices(int *ndevices);

/*! Frees the array of #ri_device_info_t returned by ri_list_devices().
 *
 * \param devices pointer to the device list to free
 */
LIBRI_API void ri_free_device_list(ri_device_info_t *devices);


/*! Open a device based on serial number.
 *
 * Opens the first device which the string \a serial is found in the device's serial number.
 * Since it searches for a substring match, the entire serial does not have to be provided, just enough to provide a unique match versus other connected devices.
 *
 * \param serial string in the serial to search for
 *
 * \return #ri_device pointer of found device.  Zero if no device found or an error occured.
 */
LIBRI_API ri_device* ri_open_from_serial(const char* serial);

/*!  Collect raw ADC samples from the device.
*
* ri_get_raw_data() is the simplest method to get samples from the DPD80.  The function blocks and returns once the requested number of samples is collected and written to the supplied buffer \a buff.
*
* \devparam
* \param nsamples number of samples to collect
* \param *buff pointer to a location in memory to store the collected samples
* \returnerr
*/
LIBRI_API int ri_get_raw_data(ri_device *dev, uint64_t nsamples, uint16_t *buff);


/*! Enum to set the triggering mode when collecting data.
 *
 * \see ri_get_raw_data_triggered()
 */
typedef enum {
	RI_TRIG_AUTO, //!< Trigger immediatly.  Equivalent to no triggering like ri_get_raw_data()
	RI_TRIG_S_RISING, //!< Trigger when port S changes from *Low* to *High*
	RI_TRIG_S_FALLING, //!< Trigger when port S changes from  *High* to *Low*
	RI_TRIG_S_HIGH, //!< Trigger when port S is *High*
	RI_TRIG_S_LOW, //!< Trigger when port S is *Low*
	RI_TRIG_T_RISING, //!< Trigger when port T changes from *Low* to *High*
	RI_TRIG_T_FALLING, //!< Trigger when port T changes from *High* to *Low*
	RI_TRIG_T_HIGH, //!< Trigger when port T is *High*
	RI_TRIG_T_LOW, //!< Trigger when port T is *Low*
} RI_TRIGGER_MODE_t;

/*!  Collect raw ADC samples after a trigger signal is received.
*
* Wait for a trigger signal applied on the DPD80 port S or port T before collecting \a nsamples of raw ADC samples.
*
* See #RI_TRIGGER_MODE_t for a list of triggering modes.
* Once a triggering condition is reached, the entire \a nsamples of \a uint16_t samples are collected.
*
* \devparam
* \param nsamples number of samples to collect
* \param *buff pointer to a location in memory to store the collected samples
* \param mode the trigger mode, described by a #RI_TRIGGER_MODE_t value
* \returnerr
*/
LIBRI_API int ri_get_raw_data_triggered(ri_device *dev, uint64_t nsamples, uint16_t *buff, RI_TRIGGER_MODE_t mode);


/*!  Collect raw ADC samples after a trigger signal is received.
 *
 * Behaves like ri_get_raw_data_triggered, however on each trigger signal collect only \a samples_per_trigger samples, and repeat until the entire \nsamples are collected.
 *
 * See #RI_TRIGGER_MODE_t for a list of triggering modes.
 * Once a triggering condition is reached, \a samples_per_trigger of \a uint16_t samples are collected.  The function then waits again for a trigger condition, until the entire \a nsamples are collected.
 *
 * \devparam
 * \param nsamples total number of samples to collect
 * \param *buff pointer to a location in memory to store the collected samples
 * \param mode the trigger mode, described by a #RI_TRIGGER_MODE_t value
 * \param samples_per_trigger the number of samples to collect every time the trigger signal is received.
 *
 * \returnerr
 */
LIBRI_API int ri_get_raw_data_triggered_repeat(ri_device *dev, uint64_t nsamples, uint16_t *buff, RI_TRIGGER_MODE_t mode, uint64_t samples_per_trigger);


/*! callback function type for use with ri_start_continuous_transfer().  Called everytime after a set samples are collected.  The number of samples sent to this function is not guaranteed to be fixed.   This function is called repeatedly until it returns false, which cancells the continuous transfer operation.
 *
 * This function should return as quick as possible to prevent possible data loss.
 *
 * \param data buffer containing the collected samples.  The contents should be copied out of this buffer as the buffer may be deleted or reused once this function returns.
 * \param ndata number of 16 bit samples in \a data
 * \param dataloss boolean value indicating if data loss was detected since the last callback.
 * \param the \a userdata void pointer passed to ri_start_continuous_transfer()
 * \return boolean value indicating if data collection should continue
 */
typedef int(*ri_transfer_callback)(uint16_t* data, int ndata, int dataloss, void *userdata);


/*! Start continuous data collection using a callback function.
 *
 * ri_start_continuous_transfer() allows data to be transferred for a potentially unlimited
 * period of time. A callback function with the same type as #ri_transfer_callback should
 * be supplied which is called repeatedly whenever new data is available.
 *
 * this function will block, and data transfer will continue until the callback function returns false.
 *
 * \see The example \a example_data_streaming for an example on how to use this function.
 *
 * \devparam
 * \param callback a function pointer to a #ri_transfer_callback callback function
 * \param userdata a void pointer that will be passed to the callback function unchanged
 * \returnerr
 */
LIBRI_API int ri_start_continuous_transfer(ri_device *dev, ri_transfer_callback callback, void *userdata);



/*!
 * Gets a #ri_device_info_t struct with various values about
 * the connected ri_device.  This is the same structure returned
 * from the ri_list_devices() function.
 * \devparam
 * \return an #ri_device_info_t with information about the requested device.

 */
LIBRI_API ri_device_info_t ri_get_device_info(ri_device *dev);


/*! Gets the device's serial number.
 *
 * If an error occured, a null pointer is returned.  Use #ri_get_error() to find further information on the error.
 * \devparam
 * \return null-terminated string
 *
 */
const char* ri_get_serial(ri_device *dev);

/*! Gets the device's product string.
 *
 * If an error occured, a null pointer is returned.  Use ri_get_error() to find further information on the error.
 *
 * \devparam
 * \return null-terminated string
 */
const char* ri_get_product(ri_device *dev);

/*! Gets the device's firmware version.
 *
 * If an error occurs, returns #RI_BAD_VERSION.

 * \devparam
 * \return an #ri_version_t containing the firmware version information.
 */
ri_version_t ri_get_fwversion(ri_device *dev);

/*! Gets an intified version of the device's firmware version.
 *
 * Equivalent to calling #ri_version_intify on the firmware's version.
 *
 * If an error occurs, returns -1.
 *
 * \devparam
 * \returns the intified firmware version number.
 */
int ri_get_fwversion_int(ri_device *dev);

/*! Gets the device's sample rate.
 *
 * If an error occurs, returns #RI_BAD_UINT32.
 *
 * \devparam
 * \return the sample rate of the device, in samples per second.
 */
uint32_t ri_get_samplerate(ri_device *dev);

/*! Gets the number of bits of the device's ADC.
 *
 * \devparam
 * \return The number of bits of the device's Analog to Digital converter.
 *
 */
uint8_t ri_get_adcbits(ri_device *dev);


/*!
 * Returns the peak wavelength responsivity (in nm) of the model of photodiode on the device as an integer.  This is the typical peak sensitivity of the photodiode, not a per device calibrated sensitivity.
 *
 * If an error occured, return -1.
 *
 * \devparam
 * \return peak wavelength reponsivity in nm
 */
LIBRI_API int ri_get_peak_responsivity(ri_device *dev);

/*!
 * Returns the relative responsivity of the photodiode at a particular
 * wavelength from the peak responsivity.  The rel_responsivity at the
 * peak sensitivity is 1.  This is estimated from the datasheet curve and
 * due to polynomial representation internally may return a value slightly
 * greater then 1 at values near the peak wavelength.
 *
 * If the requested wavelength is out of range, zero is returned.
 *
 * \devparam
 * \param wavelength the wavelength (in nm) to calculate the relative responsivity at.
 */
LIBRI_API double ri_get_rel_responsivity(ri_device *dev, double wavelength);

/*! Enum describing the available types of calibration values.
 */
typedef enum {
	RI_CALIBRATION_DIGITAL_HIGHGAIN, //!< Highgain mode, ADC code to uW
	RI_CALIBRATION_DIGITAL_LOWGAIN, //!< Lowgain mode, ADC code to uW
	RI_CALIBRATION_ANALOG_HIGHGAIN, //!< Highgain mode, Volts to uW
	RI_CALIBRATION_ANALOG_LOWGAIN, //!< Lowgain mode, Volts to uW

	RI_CALIBRATION_DIGITAL_AUTO, //!< Detect calibration based on current gain setting
 } RI_CALIBRATIONTYPE_t;

/*! Struct conatining a linear calibration.
 *
 *  Contains two variables to linearly convert one measurement to different units using the formula Y = m*X + b.
 *
 */
typedef struct {
	float m; //!< Slope
	float b; //!< Offset
} ri_calibration_t;

LIBRI_API ri_calibration_t ri_get_calibration(ri_device *dev, int calibrationtype);

LIBRI_API ri_calibration_t ri_get_rel_calibration(ri_device *dev, int calibrationtype, double wavelength);



/*!
 * Returns the USB speed of the open #ri_device.  Data transfer operations
 * are only supported when the device is RI_SPEED_SUPER i.e. when in USB 3 mode.
 *
 * \devparam
 * \return an #RI_USB_SPEED_t value
 */
LIBRI_API RI_USB_SPEED_t ri_get_usb_speed(ri_device *dev);
LIBRI_API uint16_t ri_get_usbvid(ri_device *dev);
LIBRI_API uint16_t ri_get_usbpid(ri_device *dev);


/*!
 * A list of possible values that can be queried from a connected device.
 *
 *
 */
typedef enum RI_VALUE_NAME_t {
	// device information
	RI_VAL_SERIAL, //!< char[16]
	RI_VAL_PRODUCT, //!< char[16]
	RI_VAL_FWVERSION, //!< uint8[3]
	RI_VAL_SAMPLERATE, //!< uint32
	RI_VAL_ADCBITS, //!< uint8
	RI_VAL_PEAK_RESPONSIVITY, //!< uint32

	// usb information
	RI_VAL_USBVID = 0x20, //!< uint16
	RI_VAL_USBPID, //!< uint16
	//RI_VAL_USBSPEED,

	// calibration values
	// float32[2]
	RI_CAL_DIGITAL_HIGHGAIN = 0x30, //!< float[2]
	RI_CAL_DIGITAL_LOWGAIN, //!< float[2]
	RI_CAL_ANALOG_HIGHGAIN, //!< float[2]
	RI_CAL_ANALOG_LOWGAIN, //!< float[2]

} RI_VALUE_NAME_t;

/*!
 * ri_get_value() is an api to get various values from the attached device.
 *
 * available values to query are listed in the #RI_VALUE_NAME_t enum, and the data types they are copied as.
 *
 * since the type of \a *value is not declared, care must be taken \a *value points to enough memory to write the requested value.
 *
 * Due to type safety, the individual ri_get_ functions are recommended over ri_get_value().
 *
 * \devparam
 * \param name one of #RI_VALUE_NAME_t values
 * \param value a pointer to the place to write the value to
 * \returnerr
 */
LIBRI_API int ri_get_value(ri_device *dev, RI_VALUE_NAME_t name, void *value);

/*
 * The ri_set_ functions enable or disable various features / modes of the DPD80.
 * enable is a boolean argument (1 or 0) that enables the specific feature.
 *
 * these features are rest to their default value when the DPD80 is reset.
 *
 * highgain: turns on the highgain mode of the DPD80.  default 1.
 * antialias: turns on the 35 MHz antialias filter on the DPD80. default 1.
 * powerdown: powers down all the non-usb features on the DPD80.  default 0.
 * adcclock: turns on the 80 MHz ADC clock.  Usefull to turn off if using the analog
 *   output of the DPD80 and want to get rid of the 80 MHz clock peak. default 1.
 */

/*! Sets the \a highgain feature of the DPD80
 *
 * Controls wether the transimpedance amplifier is in \a highgain mode (1) or \a lowgain mode (0).  \a highgain mode increases amplification by a factor of 4, but reduces the available dynamic range.
 *
 * Default is \a highgain mode.
 *
 * Resets to default when the device is powered on.
 *
 * \devparam
 * \param enable if the feature should be enabled.
 * \returnerr
 */
LIBRI_API int ri_set_highgain(ri_device *dev, int enable);

/*! Sets the \a antialias feature of the DPD80
*
* Controls wether the 35 MHz antialiasing filter is enabled. Only effects the digitally colelcted data, not the analog RF output.
*
* Default is \a antialias mode on.
*
* Resets to default when the device is powered on.
*
* \devparam
* \param enable if the feature should be enabled.
* \returnerr
*/
LIBRI_API int ri_set_antialias(ri_device *dev, int enable);

/*! Sets the \a powerdown feature of the DPD80
*
* Controls wether the DPD80 is powered down.  When \a powerdown is enabled all the analog circuitry is powered off.
*
* Default is \a powerdown mode off.
*
* Resets to default when the device is powered on.
*
* \devparam
* \param enable if the feature should be enabled.
* \returnerr
*/
LIBRI_API int ri_set_powerdown(ri_device *dev, int enable);

/*! Sets the \a adcclock feature of the DPD80
*
* Controls wether the 80 MHz ADC clock is enabled.  When disabled digital data collection will not work, however it will eliminate the pickup of the 80 MHz clock in the analog RF output signal.
*
* Default is \a adcclock mode on.
*
* Resets to default when the device is powered on.
*
* \devparam
* \param enable if the feature should be enabled.
* \returnerr
*/
LIBRI_API int ri_set_adcclock(ri_device *dev, int enable);

/*!
 * Reads the state of the \a highgain feature.
 * If an error occured, returns -1.
 * \devparam
 * \return 0 or 1 on success, -1 on failure.
 * \sa ri_set_highgain()
 */
LIBRI_API int ri_read_highgain(ri_device *dev);

/*!
 * Reads the state of the \a antialias feature.
 * If an error occured, returns -1.
 * \devparam
 * \return 0 or 1 on success, -1 on failure.
 * \sa ri_set_antialias()
 */
LIBRI_API int ri_read_antialias(ri_device *dev);

/*!
 * Reads the state of the \a powerdown feature.
 * If an error occured, returns -1.
 * \devparam
 * \return 0 or 1 on success, -1 on failure.
 * \sa ri_set_powerdown()
 */
LIBRI_API int ri_read_powerdown(ri_device *dev);

/*!
 * Reads the state of the \a adcclock feature.
 * If an error occured, returns -1.
 * \devparam
 * \return 0 or 1 on success, -1 on failure.
 * \sa ri_set_adcclock()
 */
LIBRI_API int ri_read_adcclock(ri_device *dev);

/*
 * ri_port_ functions control the A, B, S, and T ports on the back of the DPD80.
 *
 * ports are addresable using the values
 *   RI_PORT_A
 *	 RI_PORT_B
 *	 RI_PORT_S
 *	 RI_PORT_T
 *
 * currently the S and T ports can only be configured in STREAM_IN mode where the
 * 3.3V TTL signals into these ports are recorded with the ADC samples in bits
 * 16 and 15.  This should be upgradable with future software updates to configure the S and T pins to act as general purpose ports too.
 *
 * ri_port_drive outputs the 3.3V TTL signal specified by value (1 or 0) on the port.
 * ri_port_read reads the 3.3V TTL signal applied to the port.
 * ri_port_pwm causes the port to output a square wave signal with the period
 *   of threshold clock cycles (clocked at ~400 MHz), and a duty cycle of threshold
 *   clock cycles.
 * ri_port_pulse causes a single pulse to be output on the port with a length of
 *   threshold clock cycles (clocked at ~400 MHz).
 *
 */

 /*!
  * Drives a 3.3 V logic signal on the specified port.
  *
  * \returnerr
  * \devparam
  * \param port the #RI_PORT_t port to control
  * \param value boolean value indicating wether 3.3 V (true) or 0 V (false) should be output.
  */
LIBRI_API int ri_port_drive(ri_device *dev, RI_PORT_t port, int value);

 /*!
  * Read a 3.3 V logic signal on the specified port.
  *
  * \return the logic signal applied to the port. -1 if and error occured.
  * \devparam
  * \param port the #RI_PORT_t port to control
  */
LIBRI_API int ri_port_read(ri_device *dev, RI_PORT_t port);

 /*!
  * Drive a square wave with a given duty cycle on the port.
  *
  * \a threshold and \a period are specified in clock cycles, which correspond to approximately 5 ns.
  *
  * \returnerr
  * \devparam
  * \param port the #RI_PORT_t port to control
  * \param threshold the on-time of the square wave (in clock cycles)
  * \param period the period of the square wave (in clock cycles)
  */
LIBRI_API int ri_port_pwm(ri_device *dev, RI_PORT_t port, uint32_t threshold, uint32_t period);

/*!
 * Drive a single square wave pulse on the specified port.
 *
 * \a threshold is specified in clock cycles, which correspond to approximately 5 ns.
 *
 * \returnerr
 * \devparam
 * \param port the #RI_PORT_t port to control
 * \param threshold the length of the pulse (in clock cycles)
 */
LIBRI_API int ri_port_pulse(ri_device *dev, RI_PORT_t port, uint32_t threshold);


/*
 * the ri_test_ functions set the adc to output predefined test values to verify
 * proper behavior.  Shouldn't need to be used by end users.
 *
 * ri_test_normal returns the device to normal operation.
 */
LIBRI_API void ri_test_pn9(ri_device *dev);
LIBRI_API void ri_test_checkerboard(ri_device *dev);
LIBRI_API void ri_test_user(ri_device *dev, uint16_t sample);
LIBRI_API void ri_test_normal(ri_device *dev);


/*! Returns the version of libri being linked,
 * as a ri_version_t containing the major, minor, and revision versions.
 *
 * This may differ from the variable #LIBRI_VERSION, if, for instance, a different version of libri.dll is being used compared with the version of ri.h when the program was compiled.
 */
LIBRI_API ri_version_t ri_version();

/*!
 * Turn a #ri_version_t into an integer so that version numbers can be compared using standard operators.
 *
 * \param version the #ri_version_t to convert
 * \returns an int corresponding to the version number
 */
LIBRI_API int ri_version_intify(ri_version_t version);


/*!
 * Clear the last error and set it to RI_SUCCESS.
 *
 * Useful if you need to check if an error occured in a function without
 * using it's error status.
 */
void ri_clear_errors();

/*!
 * Get a string describing the last error that occured.
 *
 * \return a null terminated string describing the previous error
 */
const char* ri_get_error_string();

/*!
 * Get the last error code that occured.  Error codes are
 * one of #RI_ERROR_t values.
 *
 * \return an #RI_ERROR_t value describing the last error
 */
int ri_get_error();

/*! A uint8_t value indicating an error occured.*/
static const uint8_t RI_BAD_UINT8 = 0xFFU;
/*! A uint16_t value indicating an error occured.*/
static const uint16_t RI_BAD_UINT16 = 0xFFFFU;
/*! A uint32_t value indicating an error occured.*/
static const uint32_t RI_BAD_UINT32 = 0xFFFFFFFFU;

/*! A float value indicating an error occured.*/
extern const float RI_BAD_FLOAT;
/*! A double value indicating an error occured.*/
extern const double RI_BAD_DOUBLE;
/*! A #ri_version_t value indicating an error occured.*/
static const ri_version_t RI_BAD_VERSION = {0xFF, 0xFF, 0xFF};

/*! Checks if the #ri_version_t matches #RI_BAD_VERSION. */
static inline int ri_is_bad_version(const ri_version_t version)
{
	return
		(version.major == RI_BAD_VERSION.major) &&
		(version.minor == RI_BAD_VERSION.minor) &&
		(version.release == RI_BAD_VERSION.release);
}

//#define INF ((float)(1e+300 * 1e+300))
//! Constant defining a #ri_calibration_t return value expressing an error.
static const ri_calibration_t RI_BAD_CALIBRATION = { 0, 0 };

/*! Checks if the #ri_calibration_t matches #RI_BAD_CALIBRATION. */
static inline int ri_is_bad_calibration(const ri_calibration_t cal)
{
	return (cal.m == RI_BAD_CALIBRATION.m) &&
		   (cal.b == RI_BAD_CALIBRATION.b);
}


#ifdef __cplusplus
}
#endif
#endif // RI_H
