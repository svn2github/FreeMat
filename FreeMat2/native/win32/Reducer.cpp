// This code is adopted from reduce.c in xloadimage version 4.1 by
// Jim Frost.  Here is the original copyright for the code:
//
// * Copyright 1989, 1993 Jim Frost
// *
// * Permission to use, copy, modify, distribute, and sell this software
// * and its documentation for any purpose is hereby granted without fee,
// * provided that the above copyright notice appear in all copies and
// * that both that copyright notice and this permission notice appear
// * in supporting documentation.  The author makes no representations
// * about the suitability of this software for any purpose.  It is
// * provided "as is" without express or implied warranty.
// *
// * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
// * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
// * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// * USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Here are the comments at the top of the original reduce.c file:
/* reduce.c:
 *
 * reduce an image's colormap usage to a set number of colors.  this also
 * translates a true color image to a TLA-style image of `n' colors.
 *
 * this uses an algorithm by Paul Heckbert discussed in `Color Image
 * Quantization for Frame Buffer Display,' _Computer Graphics_ 16(3),
 * pp 297-307.  this implementation is based on one discussed in
 * 'A Few Good Colors,' _Computer Language_, Aug. 1990, pp 32-41 by
 * Dave Pomerantz.
 *
 * this function cannot reduce to any number of colors larger than 32768.
 *
 * jim frost 04.18.91
 *
 * Copyright 1991 Jim Frost.
 * See included file "copyright.h" for complete copyright information.
 */

#include "Reducer.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DIST(A, B) ((A) < (B) ? (B) - (A) : (A) - (B))

/* find the distance between two colors.  we loose some accuracy here because
 * a triple squared short may not fit in a long.  we use a table lookup
 * to help speed this up; it's an O(exp(n,2)) algorithm.
 */

unsigned int  squareInit= 0;
unsigned long squareTable[32768];

void initSquareTable()
{ unsigned long a;

  for (a= 0; a < 32768; a++)
    squareTable[a]= a * a;
  squareInit= 1;
}

/* this converts a 24-bit true color pixel into a 15-bit true color pixel
 */

#define RGB_TO_15BIT(r,g,b) \
  ((((int) r) & 0x0000f8) << 7) | \
  ((((int) g) & 0x0000f8) << 2) | \
  ((((int) b) & 0x0000f8) >> 3)
							

/* these macros extract color intensities from a 15-bit true color pixel
 */

#define RED_INTENSITY(P)   (((P) & 0x7c00) >> 10)
#define GREEN_INTENSITY(P) (((P) & 0x03e0) >> 5)
#define BLUE_INTENSITY(P)   ((P) & 0x001f)

/* this structure defines a color area which is made up of an array of pixel
 * values and a count of the total number of image pixels represented by
 * the area.  color areas are kept in a list sorted by the number of image
 * pixels they represent.
 */

struct color_area {
  unsigned short    *pixels;       /* array of pixel values in this area */
  unsigned short     num_pixels;   /* size of above array */
  int              (*sort_func)(const void*, const void*); /* predicate func to sort with before
								    * splitting */
  unsigned long      pixel_count;  /* # of image pixels we represent */
  struct color_area *prev, *next;
};

/* predicate functions for qsort
 */

static int sortRGB(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (red1 == red2)
    if (green1 == green2)
      if (blue1 < blue2)
	return(-1);
      else
	return(1);
    else if (green1 < green2)
      return(-1);
    else
      return(1);
  else if (red1 < red2)
    return(-1);
  else
    return(1);
}

static int sortRBG(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (red1 == red2)
    if (blue1 == blue2)
      if (green1 < green2)
	return(-1);
      else
	return(1);
    else if (blue1 < blue2)
      return(-1);
    else
      return(1);
  else if (red1 < red2)
    return(-1);
  else
    return(1);
}

static int sortGRB(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (green1 == green2)
    if (red1 == red2)
      if (blue1 < blue2)
	return(-1);
      else
	return(1);
    else if (red1 < red2)
      return(-1);
    else
      return(1);
  else if (green1 < green2)
    return(-1);
  else
    return(1);
}

static int sortGBR(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (green1 == green2)
    if (blue1 == blue2)
      if (red1 < red2)
	return(-1);
      else
	return(1);
    else if (blue1 < blue2)
      return(-1);
    else
      return(1);
  else if (green1 < green2)
    return(-1);
  else
    return(1);
}

static int sortBRG(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (blue1 == blue2)
    if (red1 == red2)
      if (green1 < green2)
	return(-1);
      else
	return(1);
    else if (red1 < red2)
      return(-1);
    else
      return(1);
  else if (blue1 < blue2)
    return(-1);
  else
    return(1);
}

static int sortBGR(const void *q1, const void *q2) { 
  unsigned int red1, green1, blue1, red2, green2, blue2;
  unsigned short *p1, *p2;
  p1 = (unsigned short *) q1;
  p2 = (unsigned short *) q2;

  red1= RED_INTENSITY(*p1);
  green1= GREEN_INTENSITY(*p1);
  blue1= BLUE_INTENSITY(*p1);
  red2= RED_INTENSITY(*p2);
  green2= GREEN_INTENSITY(*p2);
  blue2= BLUE_INTENSITY(*p2);

  if (blue1 == blue2)
    if (green1 == green2)
      if (red1 < red2)
	return(-1);
      else
	return(1);
    else if (green1 < green2)
      return(-1);
    else
      return(1);
  else if (blue1 < blue2)
    return(-1);
  else
    return(1);
}

/* this does calculations on a color area following a split and inserts
 * the color area in the list of color areas.
 */

static void insertColorArea(unsigned long *pixel_counts,
			    struct color_area **rlargest, 
			    struct color_area **rsmallest, 
			    struct color_area *area) { 
  int a;
  unsigned int red, green, blue;
  unsigned int min_red, min_green, min_blue;
  unsigned int max_red, max_green, max_blue= 0;
  struct color_area *largest, *smallest, *tmp_area;

  min_red= min_green= min_blue= 31;
  max_red= max_green= max_blue= 0;

  /* update pixel count for this area and find RGB intensity widths
   */

  area->pixel_count= 0;
  for (a= 0; a < area->num_pixels; a++) {
    area->pixel_count += pixel_counts[area->pixels[a]];
    red= RED_INTENSITY(area->pixels[a]);
    green= GREEN_INTENSITY(area->pixels[a]);
    blue= BLUE_INTENSITY(area->pixels[a]);
    if (red < min_red)
      min_red= red;
    if (red > max_red)
      max_red= red;
    if (green < min_green)
      min_green= green;
    if (green > max_green)
      max_green= green;
    if (blue < min_blue)
      min_blue= blue;
    if (blue > max_blue)
      max_blue= blue;
  }

  /* calculate widths and determine which predicate function to use based
   * on the result
   */

  red= max_red - min_red;
  green= max_green - min_green;
  blue= max_blue - min_blue;

  if (red > green)
    if (green > blue)
      area->sort_func= sortRGB;
    else if (red > blue)
      area->sort_func= sortRBG;
    else
      area->sort_func= sortBRG;
  else if (green > blue)
    if (red > blue)
      area->sort_func= sortGRB;
    else
      area->sort_func= sortGBR;
  else
    area->sort_func= sortBGR;

  /* insert color area in color area list sorted by number of pixels that
   * the area represents
   */

  largest= *rlargest;
  smallest= *rsmallest;

  if (!largest) {
    largest= smallest= area;
    area->prev= area->next= (struct color_area *)NULL;
  }

  /* if we only have one element, our pixel count is immaterial so we get
   * stuck on the end of the list.
   */

  else if (area->num_pixels < 2) {
    smallest->next= area;
    area->prev= smallest;
    area->next= (struct color_area *)NULL;
    smallest= area;
  }

  /* insert node into list
   */

  else {
    for (tmp_area= largest; tmp_area; tmp_area= tmp_area->next)
      if ((area->pixel_count > tmp_area->pixel_count) ||
	  (tmp_area->num_pixels < 2)) {
	area->prev= tmp_area->prev;
	area->next= tmp_area;
	tmp_area->prev= area;
	if (area->prev)
	  area->prev->next= area;
	else
	  largest= area;
	break;
      }
    if (!tmp_area) {
      area->prev= smallest;
      area->next= (struct color_area *)NULL;
      smallest->next= area;
      smallest= area;
    }
  }
  *rlargest= largest;
  *rsmallest= smallest;
}

int ColorReduce(unsigned char *source_data, int width, int height,
		int color_count, unsigned short *output_colors, 
		unsigned short *output_data) {
  unsigned long pixel_counts[32768]; /* pixel occurrance histogram */
  unsigned short pixel_array[32768];
  unsigned long count, midpoint;
  int x, y, num_pixels, allocated, depth, ncolors;
  unsigned char *pixel;
  unsigned short *dpixel;
  struct color_area *areas, *largest_area, *smallest_area;
  struct color_area *new_area, *old_area;
  char buf[BUFSIZ];
  int n;

  n = color_count;

  if (n > 32768) /* max # of colors we can handle */
    n = 32768;

  /* create a histogram of particular pixel occurrances
   */

  memset(pixel_counts, 0, 32768 * sizeof(unsigned long));
  pixel = source_data;
  for (y= 0; y < height; y++)
    for (x= 0; x < width; x++) {
      int p;
      p = RGB_TO_15BIT(pixel[0],pixel[1],pixel[2]);
      pixel_counts[p]++;
      pixel += 3;
    }

  /* create array of 15-bit pixel values that actually occur in the image
   */

  num_pixels= 0;
  for (x= 0; x < 32768; x++)
    if (pixel_counts[x] > 0)
      pixel_array[num_pixels++]= (short)x;
  /* create color area array and initialize first element
   */

  areas= (struct color_area *)malloc(n * sizeof(struct color_area));
  areas[0].pixels= pixel_array;
  areas[0].num_pixels= num_pixels;
  largest_area= smallest_area= (struct color_area *)NULL;
  insertColorArea(pixel_counts, &largest_area, &smallest_area, areas);
  allocated= 1;

  /* keep splitting the color area until we have as many color areas as we
   * need
   */

  while (allocated < n) {

    /* if our largest area can't be broken down, we can't even get the
     * number of colors they asked us to
     */

    if (largest_area->num_pixels < 2)
      break;

    /* find midpoint of largest area and do split
     */

    qsort(largest_area->pixels, largest_area->num_pixels, sizeof(short),
	  largest_area->sort_func);
    count= 0;
    midpoint= largest_area->pixel_count / 2;
    for (x= 0; x < largest_area->num_pixels; x++) {
      count += pixel_counts[largest_area->pixels[x]];
      if (count > midpoint)
	break;
    }
    if (x == 0) /* degenerate case; divide in half */
      x= 1;
    new_area= areas + allocated;
    new_area->pixels= largest_area->pixels + x;
    new_area->num_pixels= largest_area->num_pixels - x;
    largest_area->num_pixels= x;
    old_area= largest_area;
    largest_area= largest_area->next;
    if (largest_area)
      largest_area->prev= (struct color_area *)NULL;
    else
      smallest_area= (struct color_area *)NULL;

    /* recalculate for each area of split and insert in the area list
     */

    insertColorArea(pixel_counts, &largest_area, &smallest_area, old_area);
    insertColorArea(pixel_counts, &largest_area, &smallest_area, new_area);

    allocated++;
  }


  /* calculate RGB table from each color area.  this should really calculate
   * a new color by weighting the intensities by the number of pixels, but
   * it's a pain to scale so this just averages all the intensities.  it
   * works pretty well regardless.
   */

  for (x= 0; x < allocated; x++) {
    long red, green, blue, count, pixel;

    red= green= blue= 0;
    count= areas[x].pixel_count;
    for (y= 0; y < areas[x].num_pixels; y++) {
      pixel= areas[x].pixels[y];
      red += RED_INTENSITY(pixel);
      green += GREEN_INTENSITY(pixel);
      blue += BLUE_INTENSITY(pixel);
      pixel_counts[pixel]= x;
    }
    red /= areas[x].num_pixels;
    green /= areas[x].num_pixels;
    blue /= areas[x].num_pixels;
    output_colors[3*x] = (unsigned short)(red << 11);
    output_colors[3*x+1]= (unsigned short)(green << 11);
    output_colors[3*x+2]= (unsigned short)(blue << 11);
  };

  free(areas);

  /* copy old image into new image
   */

  pixel = source_data;
  dpixel = output_data;
  
  for (y= 0; y < height; y++)
    for (x= 0; x < width; x++) {
      int p;
      p = RGB_TO_15BIT(pixel[0],pixel[1],pixel[2]);
      *dpixel = (unsigned short) (pixel_counts[p]);
      pixel += 3;
      dpixel++;
    }
  return allocated;
}
