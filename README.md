# jsns2

## Description

This is the code repository for the optics, material, and geometry files
for JSNS2 simulations with RAT. This repository should be cloned into the
data directory of your ratpac-kpipe distribution.

There is analysis code in the analyze directory and there are some useful
macros in the mac directory.

## Files

MATERIALS_JSNS2.ratdb - a RAT Database file containing the material 
properties for materials that are not included in the default MATERIALS
file for RAT.

makeOPTICSFile.py - a python script that builds the OPTICS table for
the liquid scintillator in JSNS2. There are some calculations that have
to be done that require a script to make the file as opposed to just
typing in the values.

jsns2.geo - the geometry file for the detector. Specifies detector surfaces
and tells RAT where to look for the gdml file.

build_geometry.py - a python script that builds the gdml file and the two
PMTINFO files (PMTINFO.root, PMTINFO.ratdb). Calls both gdml_parts.py and
build_pmtinfo.py to do this.

gdml_parts.py - a python file containing a few string objects that contain
the parts of the gdml file that don't need to be iterated (i.e. the material
definitions, solids, and some of the volumes).

build_pmtinfo.py - a python script that is used to build the PMTINFO.ratdb
file used by RAT for PMT look-ups and also builds a look-up tree for easier
use with the analysis code.

jsns2PMTTest.geo - a geometry file for a simple configuration with just
a single PMT and nothing else. Used to make sure the PMT model looks good.

PMTTest.gdml - a gdml file for the PMTTest geometry. Contains the definition
of the solids and volumes used in making a single PMT.
