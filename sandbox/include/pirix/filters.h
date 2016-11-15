/*---------------------------------------------------------------------------*/

#ifndef __PIREX_FILTERS_H__
#define __PIREX_FILTERS_H__

/*---------------------------------------------------------------------------*/

#include <stddef.h>

/*--------------------------------------------------------------------------*/

#define BUFFER_0		(float *) 0x00
#define BUFFER_1		(float *) 0x01
#define BUFFER_2		(float *) 0x02
#define BUFFER_3		(float *) 0x03
#define BUFFER_4		(float *) 0x04
#define BUFFER_5		(float *) 0x05
#define BUFFER_6		(float *) 0x06
#define BUFFER_7		(float *) 0x07

/*--------------------------------------------------------------------------*/

typedef enum operand_type_e
{
  OPERAND_COPY,

  OPERAND_CONV_V,
  OPERAND_CONV_H,

  OPERAND_HARRIS_1,
  OPERAND_HARRIS_2,

  OPERAND_HARRIS_LAPLACE_1,
  OPERAND_HARRIS_LAPLACE_2,
  OPERAND_HARRIS_LAPLACE_3,

  OPERAND_HESSIAN,

  OPERAND_LOG

} operand_type_t;

/*--------------------------------------------------------------------------*/

typedef struct image_kernel_s
{
    int image_width ;
    int image_stride;
    int image_height;

    int tile_max_extra_width;
    int tile_max_extra_height;

    int tile_width;
    int tile_height;
    int tile_w_last;
    int tile_h_last;
    int tile_w_nb;
    int tile_h_nb;

    float *buffers[8];

    struct operand_list_s *operand_list;

} image_kernel_t;

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*--------------------------------------------------------------------------*/

image_kernel_t *image_kernel_new(int width, int stride, int height);

void image_kernel_append(image_kernel_t *kernel, operand_type_t operand, ...);
void image_kernel_run(image_kernel_t *kernel);

void image_kernel_delete(image_kernel_t *kernel);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

