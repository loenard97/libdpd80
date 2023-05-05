# Resloved Instruments DPD80 Driver

This project consists of a driver library for the Photodiode DPD80 by Resolved Instruments ([Produkt Page](https://resolvedinstruments.com/dpd80-photodetector))

## Compiling
(Adapted from the official documentation [here](https://resolvedinstruments.com/docs/libri-intro.html#libri-intro))

This project was build with VisualStudio 2022.
To compile with libri in a new project, the follow settings should be configured in the project properties:
 1. C/C++ > General > Additional Include Directories: add the libri include directory
 2. Linker > General > Additional Library Directories: add the libri lib or lib64 directory depending on platform of the build.
 3. Linker > Input > Additional Dependencies: add libri.lib to beginning of list
Additionally the libri.dll file will have to be copied from lib/ or lib64/ and put in the same directory as the executable for the examples to run.
