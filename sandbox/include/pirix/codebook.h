/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_CODEBOOK_H__
#define __PIRIX_CODEBOOK_H__

/*---------------------------------------------------------------------------*/

#include <stddef.h>

#include <pirix/local_desc.h>

/*---------------------------------------------------------------------------*/

#define CODEBOOK_TYPE_NORMAL		0
#define CODEBOOK_TYPE_TWO_LEVELS	1

/*---------------------------------------------------------------------------*/

typedef struct centroid_tab_s
{
  int size;
  int *assign;
  float *centroids;

} centroid_tab_t;

/*---------------------------------------------------------------------------*/

typedef struct codebook_s
{
  int type;

  int size1;
  float *centroids1;

  int size2;
  float *centroids2;
  centroid_tab_t **centroid_tab;

  float *edges;

  /* Hamming embedding */
  int he_proj_nb;
  float *he_p;
  float *he_medians;

} codebook_t;

/*---------------------------------------------------------------------------*/
/*
typedef struct he_s
{
  int proj_nb;
  int codebook_size;
  float *p;
  float *medians;

} he_t;
*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*---------------------------------------------------------------------------*/

codebook_t *codebook_new(int type, int size1, int size2);

codebook_t *codebook_read(const char *fname);
void codebook_save(codebook_t *codebook, const char *fname);

codebook_t *codebook_read_from_memory(void *ptr, int size);
void codebook_save_to_memory(codebook_t *codebook, void **ptr, size_t *size);

void codebook_assign(local_desc_list_t *desc_list, codebook_t *codebook);

void codebook_delete(codebook_t *codebook);

/*--------------------------------------------------------------------------*/

//he_t *he_new(int proj_nb);

void he_build(codebook_t *cb, int points_nb, float *points, int codebook_size,
  int *cluster_size, int *cluster_assign);

//he_t *he_read(const char *fname);
//void he_save(he_t *he, const char *fname);

void he_assign(local_desc_list_t *desc_list, codebook_t *cb);

//void he_delete(he_t *he);

//void he_display(he_t *he);

/*--------------------------------------------------------------------------*/

void codebook_fill_centroid_tab(codebook_t *codebook);

void codebook_and_he_build(codebook_t *codebook,
  local_desc_list_t *desc_list, int codebook_iter_nb, int thread_nb);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

