/*
 * vp_renderC.c
 *
 * Function to render classified volumes.
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

#define COMP_AC1PB_FUNC		VPCompAC1PB
extern void VPCompAC1PB();

#define COMP_AC3PB_FUNC		VPCompAC3PB
extern void VPCompAC3PB();

#ifdef COMP_AC11B
#define COMP_AC11B_FUNC		VPCompAC11B
extern void VPCompAC11B();
#else
#define COMP_AC11B_FUNC		VPCompAC1NB
#endif

#ifdef COMP_AC31B
#define COMP_AC31B_FUNC		VPCompAC31B
extern void VPCompAC31B();
#else
#define COMP_AC31B_FUNC		VPCompAC3NB
#endif

#ifdef COMP_AC12B
#define COMP_AC12B_FUNC		VPCompAC12B
extern void VPCompAC12B();
#else
#define COMP_AC12B_FUNC		VPCompAC1NB
#endif

#ifdef COMP_AC32B
#define COMP_AC32B_FUNC		VPCompAC32B
extern void VPCompAC32B();
#else
#define COMP_AC32B_FUNC		VPCompAC3NB
#endif

#define COMP_AC1NB_FUNC		VPCompAC1NB
extern void VPCompAC1NB();

#define COMP_AC3NB_FUNC		VPCompAC3NB
extern void VPCompAC3NB();


#define COMP_AC1PS_FUNC		VPCompAC1PB

#define COMP_AC3PS_FUNC		VPCompAC3PB

#ifdef COMP_AC11S
#define COMP_AC11S_FUNC		VPCompAC11S
extern void VPCompAC11S();
#else
#define COMP_AC11S_FUNC		VPCompAC1NS
#endif

#ifdef COMP_AC31S
#define COMP_AC31S_FUNC		VPCompAC31S
extern void VPCompAC31S();
#else
#define COMP_AC31S_FUNC		VPCompAC3NS
#endif

#ifdef COMP_AC12S
#define COMP_AC12S_FUNC		VPCompAC12S
extern void VPCompAC12S();
#else
#define COMP_AC12S_FUNC		VPCompAC1NS
#endif

#ifdef COMP_AC32S
#define COMP_AC32S_FUNC		VPCompAC32S
extern void VPCompAC32S();
#else
#define COMP_AC32S_FUNC		VPCompAC3NS
#endif

#define COMP_AC1NS_FUNC		VPCompAC1NS
extern void VPCompAC1NS();

#define COMP_AC3NS_FUNC		VPCompAC3NS
extern void VPCompAC3NS();

#ifdef INDEX_VOLUME
extern void VPCompAI11B();
#endif

#define SHADOWS_OFF		0
#define SHADOWS_ON		1
#define SHADOW_OPTS		2

#define MATERIAL_CALLBACK	0
#define MATERIAL_ONE		1
#define MATERIAL_TWO		2
#define MATERIAL_MORE		3
#define MATERIAL_OPTS		4

#define COLOR_GRAY		0
#define COLOR_RGB		1
#define COLOR_OPTS		2

static void (*AffineProcTable[SHADOW_OPTS][MATERIAL_OPTS][COLOR_OPTS])() = {
    {
	{ COMP_AC1PB_FUNC, COMP_AC3PB_FUNC },
	{ COMP_AC11B_FUNC, COMP_AC31B_FUNC },
	{ COMP_AC12B_FUNC, COMP_AC32B_FUNC },
	{ COMP_AC1NB_FUNC, COMP_AC3NB_FUNC }
    },
    {
	{ COMP_AC1PS_FUNC, COMP_AC3PS_FUNC },
	{ COMP_AC11S_FUNC, COMP_AC31S_FUNC },
	{ COMP_AC12S_FUNC, COMP_AC32S_FUNC },
	{ COMP_AC1NS_FUNC, COMP_AC3NS_FUNC }
    }
};

/*
 * vpRenderClassifiedVolume
 *
 * Render a classified, run-length encoded volume using the shear-warp
 * algorithm.
 */

vpResult
vpRenderClassifiedVolume(vpc)
vpContext *vpc;
{
    int retcode;
    void (*composite_func)();
    int shadow_option, material_option, color_option;
#ifdef INDEX_VOLUME
    int have_index;
#endif

    /* check for errors and initialize */
    if ((retcode = VPCheckShader(vpc)) != VP_OK)
	return(retcode);
    if ((retcode = VPFactorView(vpc)) != VP_OK)
	return(retcode);
    if ((retcode = VPCheckClassifiedVolume(vpc, vpc->best_view_axis)) != VP_OK)
	return(retcode);
    if ((retcode = VPCheckImage(vpc)) != VP_OK)
	return(retcode);
    if ((retcode = VPCheckShadows(vpc)) != VP_OK)
	return(retcode);

#ifdef INDEX_VOLUME
    switch (vpc->best_view_axis) {
    case VP_X_AXIS:
	if (vpc->rle_x->voxel_index != NULL)
	    have_index = 1;
	else
	    have_index = 0;
	break;
    case VP_Y_AXIS:
	if (vpc->rle_y->voxel_index != NULL)
	    have_index = 1;
	else
	    have_index = 0;
	break;
    case VP_Z_AXIS:
	if (vpc->rle_z->voxel_index != NULL)
	    have_index = 1;
	else
	    have_index = 0;
	break;
    default:
	VPBug("invalid viewing axis in vpRenderClassifiedVolume");
    }
#endif /* INDEX_VOLUME */

    Debug((vpc, VPDEBUG_RENDER, "Algorithm: affine RLEvolume\n"));

    /* determine which options are enabled */
    if (vpc->enable_shadows)
	shadow_option = SHADOWS_ON;
    else
	shadow_option = SHADOWS_OFF;
    if (vpc->shading_mode == CALLBACK_SHADER)
	material_option = MATERIAL_CALLBACK;
    else if (vpc->num_materials == 1)
	material_option = MATERIAL_ONE;
    else if (vpc->num_materials == 2)
	material_option = MATERIAL_TWO;
    else
	material_option = MATERIAL_MORE;
    if (vpc->color_channels == 1)
	color_option = COLOR_GRAY;
    else
	color_option = COLOR_RGB;

    /* render */
    if (vpc->affine_view) {
	/* choose a compositing function */
	composite_func = AffineProcTable[shadow_option][material_option]
					[color_option];
#ifdef INDEX_VOLUME
	if (have_index && shadow_option == SHADOWS_OFF &&
	    material_option == MATERIAL_ONE && color_option == COLOR_GRAY) {
	    composite_func = VPCompAI11B;
	}
#endif
	VPRenderAffine(vpc, USE_RLEVOLUME, composite_func);
    } else {
	/* XXX perspective rendering not available yet */
	return(VPSetError(vpc, VPERROR_BAD_OPTION));
    }

    return(VP_OK);
}
