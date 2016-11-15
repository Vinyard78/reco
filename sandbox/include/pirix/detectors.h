/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_DETECTORS_H__
#define __PIRIX_DETECTORS_H__

/*---------------------------------------------------------------------------*/

#include <stddef.h>

#include <pirix/images.h>
#include <pirix/local_desc.h>

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*---------------------------------------------------------------------------*/

void maxima_detector_old(local_desc_list_t *desc_list, image_pyramid_t *scale_pyramid,
    image_pyramid_t *space_pyramid, float pos_threshold, float scale_threshold);

void maxima_detector(local_desc_list_t *desc_list, image_pyramid_t *scale_pyramid,
    image_pyramid_t *pos_pyramid, float pos_threshold, float scale_threshold);

local_desc_list_t *detector_harris(image_pyramid_t *pyramid, float treshold);

local_desc_list_t *detector_harris_laplace(image_pyramid_t *pyramid, float treshold);

local_desc_list_t *detector_hessian(image_pyramid_t *pyramid, float treshold);

local_desc_list_t *detector_log(image_pyramid_t *pyramid, float treshold);

local_desc_list_t *detector_dense(image_pyramid_t *pyramid, int div_x,
    int dix_y, float overlap_ratio, int multiscale);

local_desc_list_t *detector_dense1(image_pyramid_t *pyramid, int pix_x,
    int pix_y, float overlap_ratio, int multiscale);

local_desc_list_t *detector_mser(image_t *image, int min_size, float max_size,
    int delta, float q_max, float exclusion_factor);

/*---------------------------------------------------------------------------*/
/*
void hessian_cpu(image_t *hes, image_t *lap, image_t *src);
void harris_cpu(image_t *dest, image_t *src);

local_desc_list_t *image_maxima_detector(image_t *src, float threshold);
*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

