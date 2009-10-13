/*
 * volume.h
 *
 * Global definitions for VolPack example programs.
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

#include <stdlib.h>
#include <stdio.h>
#include <volpack.h>

#define BRAIN_FILE	"brainsmall.den"	/* input file */
#define BRAIN_HEADER	62			/* bytes of header in file */
#define BRAIN_XLEN	128			/* dimensions of volume */
#define BRAIN_YLEN	128
#define BRAIN_ZLEN	84

typedef struct {		/* contents of a voxel */
    short normal;		/*   encoded surface normal vector */
    unsigned char density;	/*   original density */
    unsigned char gradient;	/*   original gradient */
} RawVoxel;

RawVoxel *dummy_voxel;

#define BYTES_PER_VOXEL	sizeof(RawVoxel)	/* voxel size in bytes */
#define VOXEL_FIELDS	3	/* number of fields in voxel */
#define SHADE_FIELDS	2	/* number of fields used for shading
				   (normal and density); must be the
				   1st fields of RawVoxel */
#define CLSFY_FIELDS	2	/* number of fields used for classifying
				   (density and gradient); can be any fields
				   in the RawVoxel */

#define NORMAL_FIELD	0
#define NORMAL_OFFSET	vpFieldOffset(dummy_voxel, normal)
#define NORMAL_SIZE	sizeof(short)
#define NORMAL_MAX	VP_NORM_MAX

#define DENSITY_FIELD	1
#define DENSITY_OFFSET	vpFieldOffset(dummy_voxel, density)
#define DENSITY_SIZE	sizeof(unsigned char)
#define DENSITY_MAX	255

#define GRADIENT_FIELD	2
#define GRADIENT_OFFSET	vpFieldOffset(dummy_voxel, gradient)
#define GRADIENT_SIZE	sizeof(unsigned char)
#define GRADIENT_MAX	VP_GRAD_MAX

#define DENSITY_PARAM		0		/* classification parameters */
#define OCTREE_DENSITY_THRESH	4
#define GRADIENT_PARAM		1
#define OCTREE_GRADIENT_THRESH	4
#define OCTREE_BASE_NODE_SIZE	4

#define DENSITY_RAMP_POINTS 3			/* classification ramps */
int DensityRampX[] =    {  0,  24, 255};
float DensityRampY[] =  {0.0, 1.0, 1.0};

#define GRADIENT_RAMP_POINTS 4
int GradientRampX[] =   {  0,   5,  20, 221};
float GradientRampY[] = {0.0, 0.0, 1.0, 1.0};

#define IMAGE_WIDTH	256			/* image size */
#define IMAGE_HEIGHT	256

#define VOLUME_FILE	"brainsmall.rv"		/* volume data file */
#define OCTREE_FILE	"brainsmall.oct"	/* octree file */
#define CLVOLUME_FILE	"brainsmall.cv"		/* classified volume file */
