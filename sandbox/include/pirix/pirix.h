/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_H__
#define __PIRIX_H__

/*---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdbool.h>

#include <pirix/images.h>
#include <pirix/codebook.h>
#include <pirix/local_desc.h>
#include <pirix/ivf.h>

#include <pirix/pirix_cuda.h>

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*----------i----------------------------------------------------------------*/

typedef struct pirix_config_s
{
  /* Cuda usage */
  bool use_gpu;
  int cuda_device_id;

  /* Pyramid */
  float pyr_scale_factor;
  int pyr_min_size;

  /* Detector */
  enum {DET_TYPE_HARRIS, DET_TYPE_HARRIS_LAPLACE, DET_TYPE_HESSIAN} det_type;
  float det_threshold;

  /* Selector */
  int max_desc;

  /* Normalisation */
  enum {NORM_AFF_NONE, NORM_AFF_SECOND_MOMENT} norm_aff;
  enum {NORM_ORI_NONE, NORM_ORI_SINGLE, NORM_ORI_MULTIPLE} norm_ori;

  /* Decription */
  float desc_T;
  float desc_scale_one;

  /* Codebook */
  int codebook_type;
  int codebook_size1;
  int codebook_size2;
  int codebook_iter_nb;

  /* Hamming Embedding */
  int he_proj_nb;

} pirix_config_t;

/*---------------------------------------------------------------------------*/

pirix_config_t *pirix_config_new(void);

void pirix_config_delete(pirix_config_t *config);

/*---------------------------------------------------------------------------*/

typedef struct pirix_s
{
  /* Config structure */
  pirix_config_t *config;

  /* GPU structure */
  pirix_cuda_t *pirix_cuda;

  /* Codebook and Hamming embedding */
  codebook_t *codebook;

  /* Inverted file */
//  ivf_t *ivf;

} pirix_t;

/*---------------------------------------------------------------------------*/

typedef struct pirix_timing_s
{
  float decompress;
  float resize;
  float copy_cpu_to_gpu;
  float pyramid;
  float detector;
  float copy_gpu_to_cpu;
  float selector;
  float aff_norm;
  float ori_norm;
  float descriptor;
  float assign;
  float ivf;

} pirix_timing_t;

/*---------------------------------------------------------------------------*/

//pirix_t *pirix_new(pirix_config_t *config, codebook_t *codebook, ivf_t *ivf);
pirix_t *pirix_new(pirix_config_t *config, codebook_t *codebook);

local_desc_list_t *pirix_img2dsc(pirix_t *pirix, image_t *src, pirix_timing_t *timing);
local_desc_list_t *pirix_img2vws(pirix_t *pirix, image_t *src, pirix_timing_t *timing);
void pirix_desc2vws(pirix_t *pirix, local_desc_list_t *desc_list, pirix_timing_t *timing);

void pirix_delete(pirix_t *pirix);

/*--------------------------------------------------------------------------*/

pirix_timing_t *pirix_timing_new(void);

void pirix_timing_add(pirix_timing_t *dest, pirix_timing_t *src);
void pirix_timing_scal_mul(pirix_timing_t *timing, float scal);

float pirix_timing_get_global_time(pirix_timing_t *timing);

void pirix_timing_delete(pirix_timing_t *timing);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

