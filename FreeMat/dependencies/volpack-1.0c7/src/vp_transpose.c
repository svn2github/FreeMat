/*
 * vp_transpose.c
 *
 * Routines to transpose a raw volume.
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

#include "vp_global.h"

/* AML static int TransposeBlock ANSI_ARGS((void *src, int src_xstride, */
static void TransposeBlock ANSI_ARGS((void *src, int src_xstride,
    int src_ystride, int src_zstride, void *dst, int dst_xstride,
    int dst_ystride, int dst_zstride, int xlen, int ylen, int zlen,
    int bytes_per_voxel));

/*
 * vpTranspose
 *
 * Transpose the raw volume data.
 */

vpResult
vpTranspose(vpc, kaxis)
vpContext *vpc;	/* context */
int kaxis;	/* axis which will have the largest stride after transposing */
{
    void *blk;			/* buffer to store data during transpose */
    int xlen, ylen, zlen;	/* volume size */
    int src_xstride, src_ystride, src_zstride; /* strides of src voxels */
    int dst_xstride, dst_ystride, dst_zstride; /* strides of dst voxels */
    int bytes_per_voxel;	/* size of voxel */
    int retcode;

    /* XXX replace with a blocked algorithm to conserve memory and
       improve cache performance */

    /* check for errors */
    if ((retcode = VPCheckRawVolume(vpc)) != VP_OK)
	return(retcode);

    /* decide on the new strides */
    xlen = vpc->xlen;
    ylen = vpc->ylen;
    zlen = vpc->zlen;
    src_xstride = vpc->xstride;
    src_ystride = vpc->ystride;
    src_zstride = vpc->zstride;
    bytes_per_voxel = vpc->raw_bytes_per_voxel;
    switch (kaxis) {
    case VP_X_AXIS:
	dst_xstride = ylen*zlen*bytes_per_voxel;
	dst_ystride = bytes_per_voxel;
	dst_zstride = ylen*bytes_per_voxel;
	break;
    case VP_Y_AXIS:
	dst_xstride = zlen*bytes_per_voxel;
	dst_ystride = zlen*xlen*bytes_per_voxel;
	dst_zstride = bytes_per_voxel;
	break;
    case VP_Z_AXIS:
	dst_xstride = bytes_per_voxel;
	dst_ystride = xlen*bytes_per_voxel;
	dst_zstride = xlen*ylen*bytes_per_voxel;
	break;
    default:
	return(VPSetError(vpc, VPERROR_BAD_OPTION));
    }
    if (src_xstride == dst_xstride && src_ystride == dst_ystride &&
	src_zstride == dst_zstride)
	return(VP_OK);

    /* transpose volume */
    Alloc(vpc, blk, void *, xlen*ylen*zlen*bytes_per_voxel,
	  "transpose_tmp");
    TransposeBlock(vpc->raw_voxels, src_xstride, src_ystride, src_zstride,
		   blk, dst_xstride, dst_ystride, dst_zstride,
		   xlen, ylen, zlen, bytes_per_voxel);
    memcpy(vpc->raw_voxels, blk, xlen*ylen*zlen*bytes_per_voxel);
    Dealloc(vpc, blk);
    vpc->xstride = dst_xstride;
    vpc->ystride = dst_ystride;
    vpc->zstride = dst_zstride;
    return(VP_OK);
}

/*
 * TransposeBlock
 *
 * Transpose a block of volume data by copying it from a source array
 * to a destination array using the indicated strides.
 */

/* AML static int */
void
TransposeBlock(src, src_xstride, src_ystride, src_zstride, dst, dst_xstride,
	       dst_ystride, dst_zstride, xlen, ylen, zlen, bytes_per_voxel)
void *src;		/* source array */
int src_xstride;	/* strides for source array */
int src_ystride;
int src_zstride;
void *dst;		/* destination array */
int dst_xstride;	/* strides for destination array */
int dst_ystride;
int dst_zstride;
int xlen, ylen, zlen;	/* size of block in voxels per side */
int bytes_per_voxel;	/* size of a voxel */
{
    int x, y, z, b;
    unsigned char *src_ptr;
    unsigned char *dst_ptr;

    src_ptr = src;
    dst_ptr = dst;
    for (z = 0; z < zlen; z++) {
	for (y = 0; y < ylen; y++) {
	    for (x = 0; x < xlen; x++) {
		for (b = 0; b < bytes_per_voxel; b++)
		    dst_ptr[b] = src_ptr[b];
		src_ptr += src_xstride;
		dst_ptr += dst_xstride;
	    }
	    src_ptr += src_ystride - xlen*src_xstride;
	    dst_ptr += dst_ystride - xlen*dst_xstride;
	}
	src_ptr += src_zstride - ylen*src_ystride;
	dst_ptr += dst_zstride - ylen*dst_ystride;
    }
}
