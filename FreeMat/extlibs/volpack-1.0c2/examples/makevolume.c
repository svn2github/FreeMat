/*
 * makevolume.c
 *
 * Create a volume from the brainsmall data set.
 *
 * Copyright (c) 1994 The Board of Trustees of The Leland Stanford
 * Junior University.  All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice and this permission notice appear in
 * all copies of this software and that you do not sell the software.
 * Commercial licensing is available by contacting the author.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author:
 *    Phil Lacroute
 *    Computer Systems Laboratory
 *    Electrical Engineering Dept.
 *    Stanford University
 */

/*
 * $Date$
 * $Revision$
 */

#include <sys/types.h>
#include "volume.h"

main()
{
    vpContext *vpc;	/* rendering context */
    unsigned char *density; /* buffer for density data */
    unsigned density_size;/* size of density data */
    char *volume;	/* volume data */
    unsigned volume_size;/* size of volume */
    int density_fd;	/* file descriptor for density file (input) */
    int volume_fd;	/* file descriptor for volume (output) */
    void *malloc();
    off_t brain_header_size = BRAIN_HEADER;

    /* create a context */
    vpc = vpCreateContext();

    /* describe the layout of the volume */
    vpSetVolumeSize(vpc, BRAIN_XLEN, BRAIN_YLEN, BRAIN_ZLEN);
    vpSetVoxelSize(vpc, BYTES_PER_VOXEL, VOXEL_FIELDS,
		   SHADE_FIELDS, CLSFY_FIELDS);
    vpSetVoxelField(vpc, NORMAL_FIELD, NORMAL_SIZE, NORMAL_OFFSET, NORMAL_MAX);
    vpSetVoxelField(vpc, DENSITY_FIELD, DENSITY_SIZE, DENSITY_OFFSET,
		    DENSITY_MAX);
    vpSetVoxelField(vpc, GRADIENT_FIELD, GRADIENT_SIZE, GRADIENT_OFFSET,
		    GRADIENT_MAX);

    /* allocate space for the raw data and the volume */
    density_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN;
    density = malloc(density_size);
    volume_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN * BYTES_PER_VOXEL;
    volume = malloc(volume_size);
    if (density == NULL || volume == NULL) {
	fprintf(stderr, "out of memory\n");
	exit(1);
    }
    vpSetRawVoxels(vpc, volume, volume_size, BYTES_PER_VOXEL,
		   BRAIN_XLEN * BYTES_PER_VOXEL,
		   BRAIN_YLEN * BRAIN_XLEN * BYTES_PER_VOXEL);

    /* load the raw data */
    if ((density_fd = open(BRAIN_FILE, 0)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", BRAIN_FILE);
	exit(1);
    }
    if (lseek(density_fd, brain_header_size, 0) < 0) {
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

    /* compute surface normals (for shading) and
       gradient magnitudes (for classification) */
    if (vpVolumeNormals(vpc, density, density_size, DENSITY_FIELD,
			GRADIENT_FIELD, NORMAL_FIELD) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }

    /* store volume in a file */
    if ((volume_fd = creat(VOLUME_FILE, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", VOLUME_FILE);
	exit(1);
    }
    if (vpStoreRawVolume(vpc, volume_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(volume_fd);

    return(0);
}
