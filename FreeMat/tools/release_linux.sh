#!/bin/sh
# Master release script
# Usage: release_linux <path to source> <path to build>
SourceDir=$1
BuildDir=$2
DepsDir=$2/deps
echo "Making build dir $BuildDir"
mkdir -p $BuildDir
echo "Making deps dir $DepsDir"
mkdir -p $DepsDir
cd $DepsDir
cmake $SourceDir/tools/dependencies
make
cd $BuildDir
cmake $SourceDir
make
cpack -G RPM
cpack -G TGZ --config CPackSourceConfig.cmake
cpack -G ZIP --config CPackSourceConfig.cmake

