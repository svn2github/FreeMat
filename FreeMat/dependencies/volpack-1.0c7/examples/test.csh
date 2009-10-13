#!/bin/csh -f

echo "*** makevolume ***"
time ./makevolume
echo "*** rendervolume ***"
time ./rendervolume
mv brainsmall.ppm brainsmall1.ppm

echo "*** makeoctree ***"
time ./makeoctree
echo "*** rendervolume -octree ***"
time ./rendervolume -octree
mv brainsmall.ppm brainsmall2.ppm

echo "*** classifyvolume ***"
time ./classifyvolume
echo "*** rendervolume -classified ***"
time ./rendervolume -classified
mv brainsmall.ppm brainsmall3.ppm

echo "*** classifyvolume -octree ***"
time ./classifyvolume -octree
echo "*** rendervolume -classified ***"
time ./rendervolume -classified
mv brainsmall.ppm brainsmall4.ppm
