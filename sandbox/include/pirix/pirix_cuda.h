/*--------------------------------------------------------------------------*/

#ifndef __PIRIX_CUDA_H__
#define __PIRIX_CUDA_H__

/*--------------------------------------------------------------------------*/

#include <pirix/images.h>
#include <pirix/local_desc.h>

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*--------------------------------------------------------------------------*/

struct codebook_s;
struct he_s;

struct pirix_timing_s;
struct cuda_codebook_s;

/*--------------------------------------------------------------------------*/

typedef struct pirix_cuda_s
{
  int desc_nb;

  struct cuda_image_s *src;
  struct cuda_detector_s *detector;

  struct cuda_local_desc_s *d_desc_info;
  float *d_descs_data;

  float *d_var;
  float *d_mean;
  float *d_patch;

  struct cuda_codebook_s *codebook;

} pirix_cuda_t;

/*--------------------------------------------------------------------------*/

int cuda_get_free_device(void);
bool cuda_device_init(int id);

pirix_cuda_t *pirix_cuda_new(int device_id, struct codebook_s *cb);

local_desc_list_t *pirix_cuda_img2dsc(pirix_cuda_t *obsidian, image_t *src, float det_thresh, float desc_T, struct pirix_timing_s *timing);

void pirix_cuda_desc2vws(pirix_cuda_t *obsidian, local_desc_list_t *desc_list, struct pirix_timing_s *timing);

local_desc_list_t *pirix_cuda_img2vws(pirix_cuda_t *obsidian, image_t *src, int det_thresh, float desc_T, struct pirix_timing_s *timing);

void pirix_cuda_delete(pirix_cuda_t *obsidian);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

