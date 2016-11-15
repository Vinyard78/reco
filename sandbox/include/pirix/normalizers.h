/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_NORMALIZERS_H__
#define __PIRIX_NORMALIZERS_H__

/*---------------------------------------------------------------------------*/

#include <stddef.h>

#include <pirix/images.h>
#include <pirix/local_desc.h>

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*---------------------------------------------------------------------------*/

void normalizer_second_moment(local_desc_list_t *desc_list, image_pyramid_t *pyramid);

void normalizer_grad_ori(local_desc_list_t *desc_list, image_pyramid_t *pyramid);
void normalizer_grad_ori_multiple(local_desc_list_t *desc_list, image_pyramid_t *pyramid);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

