/*
 * rendervolume.c
 *
 * Render the brainsmall data set.
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

#include "volume.h"

main(argc, argv)
int argc;
char **argv;
{
    vpContext *vpc;	/* rendering context */
    int volume_fd;	/* file descriptor for volume (input) */
    int octree_fd;	/* file descriptor for octree (input) */
    int clvolume_fd;	/* file descriptor for classified volume (input) */
    int use_octree;	/* if true, use octree with the unclassified volume */
    int use_clvolume;	/* if true, use the classified volume */
    float density_ramp[DENSITY_MAX+1];	/* opacity as a function of density */
    float gradient_ramp[GRADIENT_MAX+1];/* opacity as a function 
					   of gradient magnitude */
    int numframes;	/* number of frames to render */
    float shade_table[NORMAL_MAX+1];	/* shading lookup table */
    unsigned char image[IMAGE_WIDTH][IMAGE_HEIGHT];	/* output image */
    char filename[512];	/* output file name */
    int n;

    /* check command-line arguments */
    use_octree = 0;
    use_clvolume = 0;
    numframes = 1;
    while (--argc > 0) {
	argv++;
	if (!strcmp(*argv, "-octree"))
	    use_octree = 1;
	else if (!strcmp(*argv, "-classified"))
	    use_clvolume = 1;
	else if (atoi(*argv) > 0)
	    numframes = atoi(*argv);
	else {
	    fprintf(stderr, "Usage: %s [-octree | -classified] [numframes]\n",
		    argv[0]);
	    exit(1);
	}
    }

    /* create a context */
    vpc = vpCreateContext();

    /* load input data: either a classified volume, or an unclassified
       volume with no octree, or an unclassified volume with an octree */
    if (use_clvolume) {
	/* load the classified volume data */
	if ((clvolume_fd = open(CLVOLUME_FILE, 0)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", CLVOLUME_FILE);
	    exit(1);
	}
	if (vpLoadClassifiedVolume(vpc, clvolume_fd) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    fprintf(stderr, "could not load the volume from file %s\n",
		    CLVOLUME_FILE);
	    
	    exit(1);
	}
	close(clvolume_fd);
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

	/* set the classification function */
	vpRamp(density_ramp, sizeof(float), DENSITY_RAMP_POINTS, DensityRampX,
	       DensityRampY);
	vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD, density_ramp,
			     sizeof(density_ramp));
	vpRamp(gradient_ramp, sizeof(float), GRADIENT_RAMP_POINTS,
	       GradientRampX, GradientRampY);
	vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD,
			     gradient_ramp, sizeof(gradient_ramp));
	vpSetd(vpc, VP_MIN_VOXEL_OPACITY, 0.05);

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
    }

    /* set the shading parameters */
    vpSetLookupShader(vpc, 1, 1, NORMAL_FIELD, shade_table,
		      sizeof(shade_table), 0, NULL, 0);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_AMBIENT, VP_BOTH_SIDES,
		  0.18, 0.18, 0.18);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_DIFFUSE, VP_BOTH_SIDES,
		  0.35, 0.35, 0.35);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_SPECULAR, VP_BOTH_SIDES,
		  0.39, 0.39, 0.39);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_SHINYNESS, VP_BOTH_SIDES,10.0,0.0,0.0);
    vpSetLight(vpc, VP_LIGHT0, VP_DIRECTION, 0.3, 0.3, 1.0);
    vpSetLight(vpc, VP_LIGHT0, VP_COLOR, 1.0, 1.0, 1.0);
    vpEnable(vpc, VP_LIGHT0, 1);
    vpSetDepthCueing(vpc, 1.4, 1.5);
    vpEnable(vpc, VP_DEPTH_CUE, 1);

    /* set the initial viewing parameters */
    vpSeti(vpc, VP_CONCAT_MODE, VP_CONCAT_LEFT);
    vpRotate(vpc, VP_Y_AXIS, 130.0);
    vpRotate(vpc, VP_X_AXIS, -15.0);
    vpCurrentMatrix(vpc, VP_PROJECT);
    vpIdentityMatrix(vpc);
    vpWindow(vpc, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
    vpCurrentMatrix(vpc, VP_MODEL);

    /* set the image buffer */
    vpSetImage(vpc, (unsigned char *)image, IMAGE_WIDTH, IMAGE_HEIGHT,
	       IMAGE_WIDTH, VP_LUMINANCE);

    /* render and store the images */
    vpSetd(vpc, VP_MAX_RAY_OPACITY, 0.95);
    for (n = 0; n < numframes; n++) {
	/* compute shading lookup table */
	if (vpShadeTable(vpc) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    exit(1);
	}

	/* render */
	if (use_clvolume) {
	    if (vpRenderClassifiedVolume(vpc) != VP_OK) {
		fprintf(stderr, "VolPack error: %s\n",
			vpGetErrorString(vpGetError(vpc)));
		exit(1);
	    }
	} else {
	    if (vpRenderRawVolume(vpc) != VP_OK) {
		fprintf(stderr, "VolPack error: %s\n",
			vpGetErrorString(vpGetError(vpc)));
		exit(1);
	    }
	}

	/* store image */
	if (numframes == 1)
	    strcpy(filename, "brainsmall.ppm");
	else
	    sprintf(filename, "brainsmall_%d.ppm", n + 1000);
	StorePGM(image, IMAGE_WIDTH, IMAGE_HEIGHT, filename);

	/* rotate by 5 degrees for next image */
	vpRotate(vpc, VP_Y_AXIS, 5.0);
    }

    return(0);
}

StorePGM(image, width, height, filename)
char *image;
int width, height;
char *filename;
{
    FILE *image_fp;	/* file descriptor for image (output) */

#define PGM_MAGIC1	'P'
#define RPGM_MAGIC2	'5'

    if ((image_fp = fopen(filename, "w")) == NULL) {
	fprintf(stderr, "cannot open output file %s\n", filename);
	exit(1);
    }
    fprintf(image_fp, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, RPGM_MAGIC2,
	    width, height, 255);
    fwrite(image, 1, width*height, image_fp);
    fclose(image_fp);
}
