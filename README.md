# SDFtracks

A tool to extract particle tracks from [EPOCH](http://www.ccpp.ac.uk/codes.html) output files,
converting them to timeseries data in HDF5 format.

Selection critera may be specified to allow extraction of only certain particles.  This selection
may be bounded to a given time window, but trajectories will be extracted for the whole range of
sdf files found.

*Note that EPOCH must have been run with particle ids and these must be present in the sdf files*

## Usage

SDFtracks has no real command line interface, but accepts a single `.ini` formatted configuration
file which contains all options.  This is a deliberate design decision to encourage documentation
of analysis steps.

```
user@machine $ sdftracks -h 
Usage: sdftracks [-h] <configfile>
Allowed Options:
  -h [ --help ]         Print this help message
  <configfile>          Configuration .ini file
```

## Config File Format

The configuration file format is `.ini`, with `key = value` pairs:

```ini
;; specified values are defaults which will be used if not overridden

; epoch species name to consider
species = 'electron'
; minimum gamma particle must achieve (at any point within time window)
; to be selected
gamma_min = 10.0 

; simulation time during which begin selecting particles for trajectory extraction
time_min = 0.0
time_max = 1.79769e+308

; Select at random this fraction of the total particles to output
; (this is useful if there is too much data to handle all selected trajectories)
rand_frac = 1.0

; name of output file
output_filename = 'output.h5'

; prefix appended to all epoch sdf files, e.g "sim_" will look for files called
; "sim_0000.sdf" and so on. defaults to looking for "0000.sdf" etc.
prefix = ""

; convert units to OSIRIS for input to radiation solver code 
; (this is mostly for use by the Author)
radt_mode = false
```

## HDF5 File Structure

The output file is structured with one group per electron, the subgroups for each electron then
contain all available timeseries data.

## Installation

Prerequisites for installation are

* A C++11 capable compiler (e.g gcc 6 or greater)
* [Cmake](https://cmake.org) version 3.6 or later
* [Boost](https://boost.org) version 1.58 or later
* [HDF5](https://portal.hdfgroup.org/display/support) version 1.10 recommended

Installation is then performed by checking out the code, running cmake and then make:

```
user@machine $ git clone https://github.com/ptooley/sdftracks.git

user@machine $ cd sdftracks

user@machine $ cmake .

user@machine $ make
```

Sdftracks can then be found in the `./bin/` directory.
