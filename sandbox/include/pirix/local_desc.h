/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_LOCAL_DESC_H__
#define __PIRIX_LOCAL_DESC_H__

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>

#include <pirix/tools.h>
#include <pirix/images.h>

/*---------------------------------------------------------------------------*/

#define LOCAL_DESC_LIST_MIN_SIZE		10
#define LOCAL_DESC_LIST_INC_FACTOR		2

/*---------------------------------------------------------------------------*/

typedef struct local_desc_s
{
    int image_w;
    int image_h;

    float x;
    float y;

    float scale;
    float angle;
    mat22_t mi;
    float cornerness;

    float patch_mean;
    float patch_var;

    float *desc;

    int vw;
    long long unsigned int binsign;

} local_desc_t;

typedef struct local_desc_summary_s
{
//    int image_w;
//    int image_h;

    float x;
    float y;

    float scale;
    float angle;
    mat22_t mi;
    float cornerness;

//    float patch_mean;
//    float patch_var;

//    float *desc;

//    int vw;
//    long long unsigned int binsign;

} local_desc_summary_t;

/*---------------------------------------------------------------------------*/

typedef struct local_desc_list_s
{
    int size;
    int alloc_size;

    local_desc_t **data;

} local_desc_list_t;

typedef struct local_desc_summary_list_s
{
    int size;
    int alloc_size;

    local_desc_t **data;

} local_desc_summary_list_t;

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*---------------------------------------------------------------------------*/

local_desc_t *local_desc_new(bool withVws);

local_desc_t *local_desc_copy(local_desc_t *desc, bool withVws);
void local_desc_save(local_desc_t *desc, FILE *fp);
void local_descsummary_save(local_desc_t *desc, FILE *fp);

void local_desc_delete(local_desc_t *desc);

void local_desc_image_mapping(local_desc_t *desc, mat22_t mi);

/*---------------------------------------------------------------------------*/

local_desc_list_t *local_desc_list_new();

local_desc_list_t *local_desc_list_copy(local_desc_list_t *desc_list, bool withVws);
local_desc_list_t *local_desc_list_read(const char *fname);
local_desc_list_t *local_descsummary_list_read(const char *fnamem, int * nb);
void local_descsummary_list_save(local_desc_list_t *desc_list, const char *fname);

local_desc_t * local_descsummary_read(FILE *fp);
int local_desc_list_read_add(local_desc_list_t *desc_list, const char *fname);
void local_desc_list_append(local_desc_list_t *list, local_desc_t *desc);
void local_desc_list_save(local_desc_list_t *desc, const char *fname);

double local_desc_list_norm1(local_desc_list_t *desc_list);
double local_desc_list_norm2(local_desc_list_t *desc_list);

void local_desc_list_delete(local_desc_list_t *list);

void local_desc_list_sort_by_vw(local_desc_list_t *list);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

