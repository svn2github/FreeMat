/*
 * makevolume.c
 *
 * Create a volume from the brainsmall data set.
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

#include <sys/types.h>
#include "volume.h"

int main()
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
