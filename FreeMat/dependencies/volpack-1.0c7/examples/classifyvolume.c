/*
 * classifyvolume.c
 *
 * Create a classified volume from the brainsmall data set.
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

main(argc, argv)
int argc;
char **argv;
{
    vpContext *vpc;	/* rendering context */
    int volume_fd;	/* file descriptor for volume (input) */
    int octree_fd;	/* file descriptor for octree (input) */
    int density_fd;	/* file descriptor for raw volume data (input) */
    int output_fd;	/* file descriptor for classified volume (output) */
    int use_rawdata;	/* if true, use raw data instead of volume */
    int use_octree;	/* if true, use octree with the volume */
    unsigned char *density;	/* buffer for density data */
    unsigned density_size;/* size of density data */
    float density_ramp[DENSITY_MAX+1];	/* opacity as a function of density */
    float gradient_ramp[GRADIENT_MAX+1];/* opacity as a function 
					   of gradient magnitude */
    void *malloc();

    /* check command-line arguments */
    use_octree = 0;
    use_rawdata = 0;
    if (argc > 1) {
	if (!strcmp(argv[1], "-octree"))
	    use_octree = 1;
	else if (!strcmp(argv[1], "-rawdata"))
	    use_rawdata = 1;
	else {
	    fprintf(stderr, "Usage: %s [-octree | -rawdata]\n", argv[0]);
	    exit(1);
	}
    }

    /* create a context */
    vpc = vpCreateContext();

    /* load input data: either raw data, or an unclassified volume with no
       octree, or an unclassified volume with an octree */
    if (use_rawdata) {
	/* describe the layout of the volume */
	vpSetVolumeSize(vpc, BRAIN_XLEN, BRAIN_YLEN, BRAIN_ZLEN);
	vpSetVoxelSize(vpc, BYTES_PER_VOXEL, VOXEL_FIELDS,
		       SHADE_FIELDS, CLSFY_FIELDS);
	vpSetVoxelField(vpc, NORMAL_FIELD, NORMAL_SIZE, NORMAL_OFFSET,
			NORMAL_MAX);
	vpSetVoxelField(vpc, DENSITY_FIELD, DENSITY_SIZE, DENSITY_OFFSET,
			DENSITY_MAX);
	vpSetVoxelField(vpc, GRADIENT_FIELD, GRADIENT_SIZE, GRADIENT_OFFSET,
			GRADIENT_MAX);

	/* allocate space for the raw data */
	density_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN;
	density = malloc(density_size);
	if (density == NULL) {
	    fprintf(stderr, "out of memory\n");
	    exit(1);
	}

	/* load the raw data */
	if ((density_fd = open(BRAIN_FILE, 0)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", BRAIN_FILE);
	    exit(1);
	}
	if (lseek(density_fd, BRAIN_HEADER, 0) < 0) {
	    perror("seek");
	    fprintf(stderr, "could not read data from %s\n", BRAIN_FILE);
	    exit(1);
	}
	if (read(density_fd, density, density_size) != density_size) {
	    perror("read");
	    fprintf(stderr, "could not read data from %s\n", BRAIN_FILE);
	    exit(1);
	}
	close(density_fd);
    } else {
	/* load the unclassified volume data */
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
    }

    /* set the classification function */
    vpRamp(density_ramp, sizeof(float), DENSITY_RAMP_POINTS, DensityRampX,
	   DensityRampY);
    vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD, density_ramp,
			 sizeof(density_ramp));
    vpRamp(gradient_ramp, sizeof(float), GRADIENT_RAMP_POINTS, GradientRampX,
	   GradientRampY);
    vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD,
			 gradient_ramp, sizeof(gradient_ramp));
    vpSetd(vpc, VP_MIN_VOXEL_OPACITY, 0.05);

    /* load the octree */
    if (use_octree) {
	/* load the octree */
	if ((octree_fd = open(OCTREE_FILE, 0)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", OCTREE_FILE);
	    exit(1);
	}
	if (vpLoadMinMaxOctree(vpc, octree_fd) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    fprintf(stderr, "could not load the octree from file %s\n",
		    OCTREE_FILE);
	    exit(1);
	}
	close(octree_fd);
    }

    /* classify */
    if (use_rawdata) {
	if (vpClassifyScalars(vpc, density, density_size, DENSITY_FIELD,
			      GRADIENT_FIELD, NORMAL_FIELD) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    exit(1);
	}
    } else {
	if (vpClassifyVolume(vpc) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    exit(1);
	}
    }

    /* store the classified volume */
    if ((output_fd = creat(CLVOLUME_FILE, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", CLVOLUME_FILE);
	exit(1);
    }
    if (vpStoreClassifiedVolume(vpc, output_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(output_fd);

    return(0);
}
