# Resolved Instruments DPD80 Driver

This project consists of a driver library for the Photodiode DPD80 by Resolved Instruments ([Produkt Page](https://resolvedinstruments.com/dpd80-photodetector))
You can download the driver from [here](https://github.com/loenard97/libdpd80/releases).

## Usage
Starting the executable runs a measurement for a specific number of samples.
The data stream from the photodiode is then collected and printed to stdout.
```sh
$ .\libdpd80.exe
META: REQUEST COUNTER SAMPLES 80000000
META: START_OF_STREAM
DATA: 10240;4198065
DATA: 10240;4192395
DATA: 10240;4191134
...
DATA: 10240;4170839
DATA: 10240;4176194
DATA: 10240;4173860
META: END_OF_STREAM
META: TRANSFERED / MB 160.0
META: ELAPSED TIME / s 1.056
META: SPEED / MBPS 151.52
```

## Compiling
(Adapted from the official documentation [here](https://resolvedinstruments.com/docs/libri-intro.html#libri-intro))

This project was build with VisualStudio 2022.
To compile with libri in a new project, the follow settings should be configured in the project properties:
 1. C/C++ > General > Additional Include Directories: add the libri include directory
 2. Linker > General > Additional Library Directories: add the libri lib or lib64 directory depending on platform of the build.
 3. Linker > Input > Additional Dependencies: add libri.lib to beginning of list
Additionally the libri.dll file will have to be copied from lib/ or lib64/ and put in the same directory as the executable for the examples to run.
