/*
 * makeoctree.c
 *
 * Create a min-max octree for the brainsmall data set.
 *
 * Copyright (c) 1995-2008, Stanford University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Stanford University nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author:
 *    Phil Lacroute
 *    Computer Systems Laboratory
 *    Electrical Engineering Dept.
 *    Stanford University
 */

#include "volume.h"

main()
{
    vpContext *vpc;	/* rendering context */
    int volume_fd;	/* file descriptor for volume data (input) */
    int octree_fd;	/* file descriptor for octree (output) */

    /* create a context */
    vpc = vpCreateContext();

    /* load the volume */
    if ((volume_fd = open(VOLUME_FILE, 0)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", VOLUME_FILE);
	exit(1);
    }
    if (vpLoadRawVolume(vpc, volume_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	fprintf(stderr, "could not load the volume from file %s\n",
		VOLUME_FILE);
	exit(1);
    }
    close(volume_fd);

    /* compute the octree */
    vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD, NULL, 0);
    vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD, NULL, 0);
    vpMinMaxOctreeThreshold(vpc, DENSITY_PARAM, OCTREE_DENSITY_THRESH);
    vpMinMaxOctreeThreshold(vpc, GRADIENT_PARAM, OCTREE_GRADIENT_THRESH);
    if (vpCreateMinMaxOctree(vpc, 0, OCTREE_BASE_NODE_SIZE) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }

    /* store octree in a file */
    if ((octree_fd = creat(OCTREE_FILE, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", OCTREE_FILE);
	exit(1);
    }
    if (vpStoreMinMaxOctree(vpc, octree_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(octree_fd);

    return(0);
}
