/*
 * vp_check.c
 *
 * Consistency and error checking routines.
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

/* error strings for vpGetErrorString() */
static char *ErrorString[] = {
    "limit exceeded",
    "singular matrix or vector",
    "I/O error",
    "invalid buffer size",
    "invalid image definition",
    "invalid shader definition",
    "invalid classifier definition",
    "invalid volume definition",
    "invalid voxel definition",
    "invalid option",
    "argument out of range",
    "invalid file",
    "cannot compute shadow buffer",
    "memory allocation failed",
};

/*
 * VPCheckVoxelFields
 *
 * Check the voxel field information for validity.
 */

vpResult
VPCheckVoxelFields(vpc)
vpContext *vpc;
{
    int f;
    int size, offset;

    if (vpc->raw_bytes_per_voxel <= 0)
	return(VPSetError(vpc, VPERROR_BAD_VOXEL));
    if (vpc->num_voxel_fields <= 0)
	return(VPSetError(vpc, VPERROR_BAD_VOXEL));
    for (f = 0; f < vpc->num_voxel_fields; f++) {
	size = vpc->field_size[f];
	offset = vpc->field_offset[f];
	if (size != 1 && size != 2 && size != 4)
	    return(VPSetError(vpc, VPERROR_BAD_VOXEL));
	if (offset < 0 || offset + size > vpc->raw_bytes_per_voxel)
	    return(VPSetError(vpc, VPERROR_BAD_VOXEL));
	if (f > 0 && offset < vpc->field_size[f-1] + vpc->field_offset[f-1])
	    return(VPSetError(vpc, VPERROR_BAD_VOXEL));
    }
    return(VP_OK);
}

/*
 * VPCheckRawVolume
 *
 * Check the raw volume for consistency.
 */

vpResult
VPCheckRawVolume(vpc)
vpContext *vpc;
{
    int retcode;

    if ((retcode = VPCheckVoxelFields(vpc)) != VP_OK)
	return(retcode);
    if (vpc->xlen <= 0 || vpc->ylen <= 0 || vpc->zlen <= 0)
	return(VPSetError(vpc, VPERROR_BAD_VOLUME));
    if (vpc->raw_voxels == NULL)
	return(VPSetError(vpc, VPERROR_BAD_VOLUME));
    if (vpc->raw_bytes_per_voxel * vpc->xlen * vpc->ylen * vpc->zlen !=
	vpc->raw_voxels_size)
	return(VPSetError(vpc, VPERROR_BAD_VOLUME));
    return(VP_OK);
}

/*
 * VPCheckClassifiedVolume
 *
 * Check the classified volume for consistency.
 */

vpResult
VPCheckClassifiedVolume(vpc, axis)
vpContext *vpc;
int axis;
{
    int retcode;

    if ((retcode = VPCheckVoxelFields(vpc)) != VP_OK)
	return(retcode);
    if (vpc->xlen <= 0 || vpc->ylen <= 0 || vpc->zlen <= 0)
	return(VPSetError(vpc, VPERROR_BAD_VOLUME));
    if (vpc->rle_bytes_per_voxel == 0)
	return(VPSetError(vpc, VPERROR_BAD_VOLUME));
    switch (axis) {
    case VP_X_AXIS:
	if (vpc->rle_x == NULL)
	    return(VPSetError(vpc, VPERROR_BAD_VOLUME));
	ASSERT(vpc->rle_x->ilen == vpc->ylen);
	ASSERT(vpc->rle_x->jlen == vpc->zlen);
	ASSERT(vpc->rle_x->klen == vpc->xlen);
	break;
    case VP_Y_AXIS:
	if (vpc->rle_y == NULL)
	    return(VPSetError(vpc, VPERROR_BAD_VOLUME));
	ASSERT(vpc->rle_y->ilen == vpc->zlen);
	ASSERT(vpc->rle_y->jlen == vpc->xlen);
	ASSERT(vpc->rle_y->klen == vpc->ylen);
	break;
    case VP_Z_AXIS:
	if (vpc->rle_z == NULL)
	    return(VPSetError(vpc, VPERROR_BAD_VOLUME));
	ASSERT(vpc->rle_z->ilen == vpc->xlen);
	ASSERT(vpc->rle_z->jlen == vpc->ylen);
	ASSERT(vpc->rle_z->klen == vpc->zlen);
	break;
    default:
	VPBug("bad axis in VPCheckClassifiedVolume");
    }
    return(VP_OK);
}

/*
 * VPCheckClassifier
 *
 * Check the classification parameters for consistency.
 */

vpResult
VPCheckClassifier(vpc)
vpContext *vpc;
{
    int p, f;
    int retcode;

    if ((retcode = VPCheckVoxelFields(vpc)) != VP_OK)
	return(retcode);
    if (vpc->num_shade_fields <= 0 ||
	vpc->num_shade_fields > vpc->num_voxel_fields)
	return(VPSetError(vpc, VPERROR_BAD_VOXEL));
    if (vpc->num_clsfy_params <= 0 ||
	vpc->num_clsfy_params > vpc->num_voxel_fields)
	return(VPSetError(vpc, VPERROR_BAD_VOXEL));
    for (p = 0; p < vpc->num_clsfy_params; p++) {
	f = vpc->param_field[p];
	if (f < 0 || f >= vpc->num_voxel_fields)
	    return(VPSetError(vpc, VPERROR_BAD_CLASSIFIER));
	if (vpc->clsfy_table[p] == NULL || vpc->clsfy_table_size[p] !=
	    (vpc->field_max[f]+1)*sizeof(float))
	    return(VPSetError(vpc, VPERROR_BAD_CLASSIFIER));
	if (p > 0 && f <= vpc->param_field[p-1])
	    return(VPSetError(vpc, VPERROR_BAD_CLASSIFIER));
    }
    return(VP_OK);
}

/*
 * VPCheckShader
 *
 * Check the shading parameters for consistency.
 */

vpResult
VPCheckShader(vpc)
vpContext *vpc;
{
    if (vpc->shading_mode == LOOKUP_SHADER) {
	if (vpc->color_field < 0 ||
	    vpc->color_field >= vpc->num_voxel_fields ||
	    vpc->color_field >= vpc->num_shade_fields)
	    return(VPSetError(vpc, VPERROR_BAD_SHADER));
	if (vpc->shade_color_table == NULL)
	    return(VPSetError(vpc, VPERROR_BAD_SHADER));
	if (vpc->shade_color_table_size != vpc->color_channels*sizeof(float)*
		(vpc->field_max[vpc->color_field]+1)*vpc->num_materials)
	    return(VPSetError(vpc, VPERROR_BAD_SHADER));
	if (vpc->field_size[vpc->color_field] != 2)
	    return(VPSetError(vpc, VPERROR_BAD_SHADER));
	if (vpc->num_materials < 1)
	    return(VPSetError(vpc, VPERROR_BAD_SHADER));
	if (vpc->num_materials > 1) {
	    if (vpc->weight_field < 0 ||
		vpc->weight_field >= vpc->num_voxel_fields ||
		vpc->weight_field >= vpc->num_shade_fields)
		return(VPSetError(vpc, VPERROR_BAD_SHADER));
	    if (vpc->shade_weight_table == NULL)
		return(VPSetError(vpc, VPERROR_BAD_SHADER));
	    if (vpc->shade_weight_table_size !=
		(vpc->field_max[vpc->weight_field]+1) * sizeof(float) * 
		vpc->num_materials)
		return(VPSetError(vpc, VPERROR_BAD_SHADER));
	    if (vpc->field_size[vpc->weight_field] != 1)
		return(VPSetError(vpc, VPERROR_BAD_SHADER));
	}
    }
    return(VP_OK);
}

/*
 * VPCheckImage
 *
 * Check the image buffer for validity.
 */

vpResult
VPCheckImage(vpc)
vpContext *vpc;
{
    if (vpc->image == NULL || vpc->image_width <= 0 || vpc->image_height <= 0)
	return(VPSetError(vpc, VPERROR_BAD_IMAGE));
    switch (vpc->pixel_type) {
    case VP_ALPHA:
	break;
    case VP_LUMINANCE:
    case VP_LUMINANCEA:
	if (vpc->color_channels != 1)
	    return(VPSetError(vpc, VPERROR_BAD_OPTION));
	break;
    case VP_RGB:
    case VP_BGR:
    case VP_RGBA:
    case VP_ABGR:
	if (vpc->color_channels != 3)
	    return(VPSetError(vpc, VPERROR_BAD_OPTION));
	break;
    default:
	return(VPSetError(vpc, VPERROR_BAD_OPTION));
    }
    return(VP_OK);
}

/*
 * VPCheckShadows
 *
 * Check the shadow specification for validity.
 */

vpResult
VPCheckShadows(vpc)
vpContext *vpc;
{
    if (vpc->enable_shadows) {
	if (vpc->shadow_light_num < VP_LIGHT0 ||
	    vpc->shadow_light_num > VP_LIGHT5)
	    return(VPSetError(vpc, VPERROR_BAD_OPTION));
	if (!vpc->light_enable[vpc->shadow_light_num - VP_LIGHT0])
	    vpc->enable_shadows = 0;
	if (vpc->shadow_color_table_size != vpc->shade_color_table_size ||
	    vpc->shadow_color_table == NULL)
	    return(VPSetError(vpc, VPERROR_BAD_SIZE));
    }
    return(VP_OK);
}

/*
 * vpGetError
 *
 * Return the error code from the first invalid command since the last
 * call to vpGetError().
 */

vpResult
vpGetError(vpc)
vpContext *vpc;
{
    vpResult code;

    code = vpc->error_code;
    vpc->error_code = VP_OK;
    return(code);
}

/*
 * vpGetErrorString
 *
 * Return a descriptive string for an error code.
 */

char *
vpGetErrorString(code)
vpResult code;
{
    if (code == VP_OK)
	return("no error");
    else if (code < VPERROR_FIRST || code > VPERROR_LAST)
	return(NULL);
    else
	return(ErrorString[code - VPERROR_FIRST]);
}

/*
 * VPSetError
 *
 * Set the error code in vpc.
 */

vpResult
VPSetError(vpc, code)
vpContext *vpc;
vpResult code;
{
    if (vpc->error_code == VP_OK)
	vpc->error_code = code;
    return(code);
}
