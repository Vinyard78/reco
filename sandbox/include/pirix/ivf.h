/*---------------------------------------------------------------------------*/

#ifndef __PIRIX_IVF_H__
#define __PIRIX_IVF_H__

/*----------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include <openssl/md5.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <pirix/local_desc.h>
#include <pirix/geomfilter.h>

char *strtok_r (char * , const char *, char ** );
/*----------------------------------------------------------------------------*/

/* define the different types of weighting schemes */
#define TFIDF_NO              0
#define TFIDF_OXFORD          1
#define TFIDF_NISTER          2


/* Number of bits used to represent the quantized values of scales and angles */
#define NB_BITS_VAL_ID       21
#define NB_BITS_VAL_SCALE    5
#define NB_BITS_VAL_ANGLE    6
#define NB_BITS_VAL_DX       16
#define NB_BITS_VAL_DY       16
#define NB_BITS_VAL_BINSIGN  64 /* this value is a refinement of the position in a cluster */

/* Default log-scale quantization step */
#define LOG_SCALE_QUANTIZATION_STEP  1.2

/* wgc_type is a bitwise or of the 5 following constants */
/* 0) no geometry: ivfgeo_query_scale_angle does the same as ivfgeo_query */
#define AS_MAX_0 0

/* 1) we handle the whole 2D histogram */
#define AS_MAX_2D_HIST 6

/* 2) we want separate marginalized histograms */
#define AS_MAX_TWOHIST 5

/* 3) histograms are smoothed by these sizes in angle and scale */
#define AS_MAX_NEIGH_SIZE(na,ns) ((ns)<<16 | (na)<<8)

/* 4) before computing maximum, we multiply histogram bins with "Bayesian" coefficients */
#define AS_MAX_PREMULTIPLY 0xc0

/* 5) favor rotations of +/- 1/4 turn */
#define AS_MAX_PREMULTIPLY_QUARTTURN 0x40

/*----------------------------------------------------------------------------*/

#define IVFGEO_REALLOC_FACTOR 1.2
#define IVFGEO_REALLOC_NEWSIZE(oldsize) (((oldsize * 5) + 4) / 4)

/*----------------------------------------------------------------------------*/

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define HIST_DIM(as_max_type) ((as_max_type)==0 ? 0 : ((as_max_type) & 0x3f) == AS_MAX_2D_HIST ? 2 : 1)

/*----------------------------------------------------------------------------*/

/* Type used for the distance values */
typedef float distype_t;

/*----------------------------------------------------------------------------*/

/* The kind of normalization that is used */
enum norm_type_e {
  NORM_NONE = 0,		/* don't do normalization       */
  NORM_TYPE_1,			/* Manhantan distance           */
  NORM_TYPE_2,			/* Euclidean distance (default) */
  NORM_TYPE_AS,			/* normalize by score for self-query w/ angle-scale */
  NORM_TYPE_CDM			/* normalization in the spirit of the CDM */
};

/*----------------------------------------------------------------------------*/

typedef struct read_file_s
{
  void * fdptr;
  const void * descpath;
} read_file_t;



typedef struct image_desc_list_s
{   
  int size;
  int alloc_size;

  local_desc_list_t **data;

} image_desc_list_t;

/*----------------------------------------------------------------------------*/

typedef struct ivf_elem_s
{
  int id:NB_BITS_VAL_ID;		/* the identifier of the image   */
  unsigned int scale:NB_BITS_VAL_SCALE;		/* quantized scale of the region */
  int angle:NB_BITS_VAL_ANGLE;			/* quantized angle               */
//  unsigned int dx:NB_BITS_VAL_DX;		/* geometric dx */
//  unsigned int dy:NB_BITS_VAL_DY;		/* geometric dy */
  unsigned long long int binsign;		/* binsign */

} __attribute__ ((__packed__)) ivf_elem_t;

/*----------------------------------------------------------------------------*/

typedef struct ivf_match_elem_s
{
  unsigned int id;		/* the identifier of the vector/image   */
  unsigned char ds;		/* quantized scale of the region */
  unsigned char da;		/* quantized angle */
  unsigned int dx;		/* dx */
  unsigned int dy;		/* dy */
  float score;			/* the match strength */

} __attribute__ ((__packed__)) ivf_match_elem_t;


typedef struct ivf_match_s
{
  int n;				/* number of matches found */
  int nmax;				/* allocated number of matches */
  int nbvw;				/* number of visual words */
  ivf_match_elem_t *matchs;		/* the matches */
  int *vw_start;			/* to store visual word segments */
  int *vw_end;				/* to store visual word segments */

} ivf_match_t;

/*----------------------------------------------------------------------------*/

typedef struct ivf_s
{
  pthread_mutex_t mut_ivf;	/* ivf mutex */
  char * descpath;      /* Path of descriptors base directory */
  char * commitlogpath; /* Path of commitlog file */
  int nb_ro_threads;    /* Number of threads accessing the ivf in readonly mode. -1 if doing readwrite operation */

  int nbvw;			    /* number of visual words */
  int nbvec;			/* number of images stored (with holes) */
  int nbvec_real;		/* real number of images stored */
  int nbvec_max;		/* maximum number of vectors stored in the inverted file */
  int norm_type;		/* modify the default normalization scheme for the frequency vector normalization */

  ivf_elem_t **elems;		/* the data corresponding to the frequency vectors */
  int *segsize;			/* the amount (number of elements) of data allocated for a given vw */
  int *nbelems;			/* the number of elements stored for a given vw */

  distype_t *norms;		/* number of descriptors of the frequency vectors stored (allocated size:nbvec_max) */
  int *labels;			/* some labels that are associated with the images (optional) */
  char **ids;			/* some unique string that are associated with eht images (optional) */
  char **summaries;		/* some unique string that are associated with eht images (optional) */
  unsigned char **md5s;		/* md5 of each image */
  int *nbdesc;   		/* number of descriptors per image, < 0 => image deleted */
  int *geo_image_w;		/* Image width (allocated size: nbvec_max) */
  int *geo_image_h;		/* Image height (allocated size: nbvec_max) */

  int tfidf;			/* define the kind of tf-idf scheme which is applied */
  distype_t *tfidf_weights;	/* tf-idf weights (allocated size: nbvw) */

  int he_dist_w;		/* !=0: weight bins is a function of the Hamming distance */
  int he_dist_var;		/* Variance when using he_dist_t == 2 */
  int he_thres;			/* Hamming comparison of binary signatures threshold */
  double * he_dis_weights;	/* weights applied for the different values of the Hamming distance */

  int query_type;		/* Define query type (0 = tfidf, 1 = tfidf + wgc, 2 = tfidf + geo */

  int wgc_type;			/* method to extract maximum from the angle / scale histogram(s) */
  distype_t *wgc_weights;	/* "Bayesian" weighting */

  double scale_w;		/* weighting applied to favor bigger scales */
  double *scale_weights;	/* weights applied for the different scales */

  int geo_type;			/* Type of weigtening */
  int geo_div_x;		/* Width of the step division of the image in pixels */
  int geo_div_y;		/* Height of the step division of the image in pixels */
  double *geo_weights;		/* Weights applied for the different locations */

//  image_desc_list_t *image_desc_list;	/* Descriptors for all index images for geometric filter */

  bool gf_enable;			/* Enable geometric filter */
  int gf_pos_thresh_hamming;		/* Hamming threshold for geometric filter */
  lowehough_parameters_t gf_params;	/* Geometric filter params */

  /* Layers */
  int nblayers;
  int nblayers_max;
  char ** layers;
  bool ** belongtolayer;

  double * text_probs;

} ivf_t;

typedef struct additional_params_s
{
  float text_filter_var;
  char * layers;
} additional_params_t;

/*----------------------------------------------------------------------------*/

typedef struct ivf_result_s
{
  int mxid;
  char *id;
  char *summary;
  float score;
  float dx;
  float dy;

} ivf_result_t;

/*----------------------------------------------------------------------------*/

typedef struct ivf_result_list_s
{
  int size;
  int alloc_size;

  ivf_result_t **data;

  int is_result_valid;

} ivf_result_list_t;

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*----------------------------------------------------------------------------*/

#define IVF_RESULT_LIST_MIN_SIZE             10
#define IVF_RESULT_LIST_INC_FACTOR           2

ivf_result_t *ivf_result_new(int mxid, char *id, char *summary, float score, float dx, float dy);
void ivf_result_delete(ivf_result_t *res);

ivf_result_list_t *ivf_result_list_new(void);
void ivf_result_list_append(ivf_result_list_t *list, ivf_result_t *res);
void ivf_result_list_delete(ivf_result_list_t *list);

/*----------------------------------------------------------------------------*/

/* number of distinct scales and angles resulting from the previous quantization scheme */
extern const int ivf_nb_scales;
extern const int ivf_nb_angles;

/*----------------------------------------------------------------------------*/

struct pirix_config_s;

/*
 * Operation for locking and unlocking the ivf for concurent access
 */
int ivf_lock_ro(ivf_t *ivf);
int ivf_lock_rw(ivf_t *ivf);
void ivf_unlock_ro(ivf_t *ivf);
void ivf_unlock_rw(ivf_t *ivf);

/*----------------------------------------------------------------------------*/

int hash_buildparams();

ivf_t *ivf_new(int nbvw, int maxn, int query_type, int geo_div_x, int geo_div_y, const char * descpath);

void *ivf_read_from_file(void * read_struct);
void ivf_save_to_file(void * ivfptr, void * fdptr);
void ivf_save_to_memory(ivf_t *ivf, void **ptr, size_t *size);

int ivf_add(ivf_t *ivf, local_desc_list_t *desc_list, int label,
  const char *id, const char *summary, unsigned char * md5, int recovering);
int ivf_del(ivf_t *ivf, int mxid, int recovering);

ivf_result_list_t *ivf_query(ivf_t *ivf, local_desc_list_t *desc_list, int n);
/* use_geom disabled: none, 1: enabled, 2: depend on ivf params */
ivf_result_list_t *ivf_query_wgc(ivf_t *ivf,  local_desc_list_t *desc_list, int n, int use_geom, additional_params_t * additional_params);

void ivf_delete(ivf_t *ivf);

void ivf_print (ivf_t *ivf);

void ivf_update_summary(ivf_t *ivf, int mxid, const char *summary, int recovering);

/*--------------------------------------------------------------------------*/

double ivf_self_wgc_score(ivf_t *ivf, local_desc_list_t *desc_list);

void ivf_compute_tfidf(ivf_t *ivf);
void ivf_set_wgc_type(ivf_t *ivf, int wgc_type);
void ivf_compute_norms(ivf_t *ivf);
void ivf_compute_scale_weights(ivf_t *ivf);
void ivf_compute_he_weights(ivf_t *ivf);
void ivf_compute_geo_weights(ivf_t *ivf);

int quantize_scale(local_desc_t *desc);
int quantize_angle(local_desc_t *desc);

int binsign_hamming(unsigned long long int bs1, unsigned long long int bs2);

void ivf_display(ivf_t *ivf);

/*----------------------------------------------------------------------------*/

image_desc_list_t *image_desc_list_new();
void image_desc_list_append(image_desc_list_t *list, local_desc_list_t *desc_list);
void image_desc_list_delete(image_desc_list_t *list);

/*--------------------------------------------------------------------------*/

local_desc_list_t * ivf_get_local_desc_list(ivf_t * ivf, int i);
void ivf_save_local_desc_list(ivf_t * ivf, local_desc_list_t *  desc_list, int imgid);
void ivf_delete_local_desc_list(ivf_t * ivf, int imgid);

int ivf_get_layer(ivf_t * ivf, char * layername);
int ivf_add_layer(ivf_t * ivf, char * layername);
void ivf_assign_doc_layers(ivf_t * ivf, int mxid, int * layers, int * size);
void ivf_set_doc_layers(ivf_t * ivf, int mxid, char * summary);
int * readlayers(ivf_t * ivf, char * req, int * len);

void ivf_recover_add2commitlog(char* operation, ivf_t *ivf, local_desc_list_t *desc_list, int label,
  const char *id, const char *summary, unsigned char * md5);
void ivf_recover_recover_ivf(ivf_t *ivf);
void ivf_recover_clean(char* descpath, char* commitlogpath);
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*----------------------------------------------------------------------------*/

#endif

/*----------------------------------------------------------------------------*/

