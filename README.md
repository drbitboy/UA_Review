[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dc3d038ce7074b7bab093699d0806759)](https://www.codacy.com/app/jaredmales/MagAOX?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=magao-x/MagAOX&amp;utm_campaign=Badge_Grade)

# The MagAOX Software System

This is the software which runs the MagAOX ExAO system.  

## Dependencies

1. mxlib (https://github.com/jaredmales/mxlib)
2. libudev (for introspective device discovery)
3. libhdf5 (though not for anything currently implemented, but we will)

## Building

The build system has not been implemented. 

Some notes:

* The mxlib build system, as re-engineered by Joseph Long, is a good starting point.
* libMagAOX (part of this repository) is a c++ header-only library.  May not need to "install" it if everything uses rel paths.
* We need setuid for RT priority handling, access to ttys and FIFOs, etc.
  * This means we need a way to parse LD_LIBRARY_PATH and create a list of -Wl,-rpath,<path> args to the linker
  * This really should happen in the make file each time so it never gets out of date
  * This also means that a make install needs privileges.  See how this was handled for VisAO.

## System Setup

- [] Add docs here . . .

## Documentation

The code is more-or-less carefully documented with doxygen, though the doxyfile has not been created, etc.

- [] Init doxygen doc system
- [] Decide: do we use github pages, or host it on one of the snazzy magao-x domains?