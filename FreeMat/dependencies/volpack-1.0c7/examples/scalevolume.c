/*
 * scalevolume.c
 *
 * Scale a volume in .den format to a different resolution.
 * Usage: scalevolume input.den output.den xscale yscale zscale [option]
 * Options:
 *    -b	use box filter
 *    -t	use triangle filter
 *    -c	use cubic bspline filter
 *    -m	use Mitchell filter
 *    -g	use Gaussian filter
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

main(argc, argv)
int argc;
char **argv;
{
    char *src_file;				/* input file name */
    char *dst_file;				/* output file name */
    double xscale, yscale, zscale;		/* scale factors */
    int filter;					/* filter type code */
    unsigned char *src_volume;			/* buffer for input data */
    unsigned char *dst_volume;			/* buffer for output data */
    unsigned src_xlen, src_ylen, src_zlen;	/* size of input data */
    unsigned dst_xlen, dst_ylen, dst_zlen;	/* size of output data */
    unsigned dst_size;				/* size in bytes of output */
    int code;
    double atof();
    unsigned char *read_den();

    /* parse arguments */
    if (argc < 6 || argc > 7) {
	fprintf(stderr, "Usage: scalevolume input.den output.den ");
	fprintf(stderr, "xscale yscale zscale [-b -t -c -m -g]\n");
	exit(1);
    }
    src_file = argv[1];
    dst_file = argv[2];
    xscale = atof(argv[3]);
    yscale = atof(argv[4]);
    zscale = atof(argv[5]);
    filter = VP_LINEAR_FILTER;
    if (argc == 7) {
	if (!strcmp(argv[6], "-b")) {
	    filter = VP_BOX_FILTER;
	} else if (!strcmp(argv[6], "-t")) {
	    filter = VP_LINEAR_FILTER;
	} else if (!strcmp(argv[6], "-c")) {
	    filter = VP_BSPLINE_FILTER;
	} else if (!strcmp(argv[6], "-m")) {
	    filter = VP_MITCHELL_FILTER;
	} else if (!strcmp(argv[6], "-g")) {
	    filter = VP_GAUSSIAN_FILTER;
	} else {
	    fprintf(stderr, "invalid filter option %s\n", argv[6]);
	    exit(1);
	}
    }

    /* load input volume */
    src_volume = read_den(src_file, &src_xlen, &src_ylen, &src_zlen);
    if (src_volume == NULL)
	exit(1);

    /* allocate space for the output volume */
    dst_xlen = src_xlen * xscale;
    dst_ylen = src_ylen * yscale;
    dst_zlen = src_zlen * zscale;
    dst_size = dst_xlen * dst_ylen * dst_zlen;
    dst_volume = (unsigned char *)malloc(dst_size);
    if (dst_volume == NULL) {
	fprintf(stderr, "out of memory\n");
	exit(1);
    }

    /* scale the volume */
    if ((code = vpResample3D(src_volume, src_xlen, src_ylen, src_zlen,
			     dst_volume, dst_xlen, dst_ylen, dst_zlen,
			     VP_UCHAR, filter)) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n", vpGetErrorString(code));
	exit(1);
    }

    /* store the output volume */
    if (!write_den(dst_file, dst_volume, dst_xlen, dst_ylen, dst_zlen))
	exit(1);

    exit(0);
}


