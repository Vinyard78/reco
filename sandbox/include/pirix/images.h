/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_IMAGES_H__
#define __PIRIX_IMAGES_H__

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include <pirix/tools.h>

/*---------------------------------------------------------------------------*/

#define IMAGE_LIST_MIN_SIZE		10
#define IMAGE_LIST_INC_FACTOR		2

#define JPEG_DEFAULT_QUALITY		80

/*---------------------------------------------------------------------------*/

typedef struct image_s
{
    int width;
    int height;
    int stride;

    float *data;

} image_t;

/*---------------------------------------------------------------------------*/

typedef struct image_list_s
{
    int size;
    int alloc_size;

    image_t **data;

} image_list_t;

/*---------------------------------------------------------------------------*/

typedef struct image_pyramid_s
{
    float scale_factor;
    int min_size;

    image_list_t *images;

} image_pyramid_t;

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*---------------------------------------------------------------------------*/

image_t *image_new(int width, int height);

image_t *image_cpy(image_t *src);

image_t *image_load(const char *fname);

bool image_save(image_t *image, const char *fname);

void image_delete(image_t *image);

float image_get_value(image_t *img, float x, float y);

int image_interpolate(image_t *patch, image_t *image, mat22_t mi, float tx, float ty);

void image_resize_horiz(image_t *dest, image_t *src, float scale);
void image_resize_vert(image_t *dest, image_t *src, float scale);
void image_resize(image_t *dest, image_t *src, float scale);
image_t *image_resize_test(image_t *src);
image_t *image_resize_test_2(image_t *src);
image_t *image_resize_test_3(image_t *src);

image_t *image_rotate(image_t *src, int ori);

void image_normalize(image_t *img);

/*---------------------------------------------------------------------------*/

image_list_t *image_list_new(void);
void image_list_append(image_list_t *list, image_t *image);
void image_list_delete(image_list_t *list);

/*---------------------------------------------------------------------------*/

image_pyramid_t *image_pyramid_new(void);
image_pyramid_t *image_pyramid_build(image_t *image, float scale_factor, int min_size, int smooth_before_rescale);
int image_pyramid_get_aff_patch(image_t *patch, image_pyramid_t *pyramid, mat22_t mi, float tx, float ty);
int image_pyramid_get_index(image_pyramid_t *pyramid, float scale);

float image_pyramid_get_scale(image_pyramid_t *pyramid, int index);

void image_pyramid_delete(image_pyramid_t *pyramid);

/*---------------------------------------------------------------------------*/

void image_draw_pixel(image_t *image, int x, int y, float intensity);
void image_draw_line(image_t *image, int x0, int y0, int x1, int y1, float intensity);
void image_draw_rec(image_t *image, int x, int y, int width, int height, float intensity);
void image_draw_ellipse(image_t *image, int x0, int y0, float a, float b, float theta, int steps, float intensity);

/*---------------------------------------------------------------------------*/

image_t *h_detector(image_t *img, float threshold, int norm);
image_t *v_detector(image_t *img, float threshold, int norm);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

