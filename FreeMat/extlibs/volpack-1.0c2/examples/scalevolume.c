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

#include <stdio.h>
#include <volpack.h>
#include <malloc.h>

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


