/*---------------------------------------------------------------------------*/

#ifndef __PIREX_TOOLS_H__
#define __PIREX_TOOLS_H__

/*----------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>

/*----------------------------------------------------------------------------*/

struct image_s;

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*----------------------------------------------------------------------------*/

typedef float mat22_t[4];

/* ---------- */

void mat22_cpy(mat22_t dest, mat22_t src);
void mat22_set(mat22_t dest, float *val);
void mat22_set_id(mat22_t dest);
void mat22_mul(mat22_t dest, mat22_t src1, mat22_t src2);
void mat22_scalar_mul(mat22_t dest, mat22_t src, float scalar);
void mat22_scalar_div(mat22_t dest, mat22_t src, float scalar);
void mat22_rot(mat22_t dest, mat22_t src, float angle);
float mat22_det(mat22_t src);
float mat22_trace(mat22_t src);
int mat22_inv(mat22_t dest, mat22_t src);
void mat22_get_polar(mat22_t mod, float *theta, mat22_t src);
int mat22_eigen_values(float *lambda_max, float *lambda_min, mat22_t src);
void mat22_print(mat22_t src);

/* ---------- */

float mat_sym22_norm_inv_sqrt(mat22_t src);
int mat_sym22_sqrt(mat22_t src);
int mat_sym22_get_eigen_space(float *eigen1, float *eigen2, float *theta, mat22_t src);
void mat_sym22_set_eigen_space(mat22_t dest, float eigen1, float eigen2, float theta);
int mat_sym22_pow(mat22_t matrix, double pow);

/*----------------------------------------------------------------------------*/

typedef struct convolution_s
{
    int order;

    float *coeffs;
    float *coeffs_accu;

} convolution_t;

/* ---------- */

convolution_t *convolution_new(int order, float *half_coeffs, int even);

void convolve_horiz(struct image_s *dest, struct image_s *src, convolution_t *conv);
void convolve_vert(struct image_s *dest, struct image_s *src, convolution_t *conv);

void convolution_delete(convolution_t *conv);

/*----------------------------------------------------------------------------*/

typedef enum derivation_order_e
{
    D0 = 0,
    D1 = 1,
    D2 = 2,
    D3 = 3

} derivation_order_t;

typedef struct cut_gauss_filter_s
{
    int order;
    float sigma;
    derivation_order_t deriv;

    float *coeff;
    convolution_t *conv;

} cut_gauss_filter_t;

/* ---------- */

struct image_s;

cut_gauss_filter_t *cut_gauss_filter_new(int order, float sigma, derivation_order_t deriv);

void cut_gauss_filter_vert(struct image_s *dest, struct image_s *src,
    cut_gauss_filter_t *filter);
void cut_gauss_filter_horiz(struct image_s *dest, struct image_s *src,
    cut_gauss_filter_t *filter);

void cut_gauss_filter_delete(cut_gauss_filter_t *filter);

/*----------------------------------------------------------------------------*/

typedef struct gauss_table_s
{
    int size;
    int inv_conv;

    float scale;

    float **data;

} gauss_table_t;

/* ---------- */

gauss_table_t *gauss_table_smooth_new(float scale);
gauss_table_t *gauss_table_dx_new(float scale);
gauss_table_t *gauss_table_dy_new(float scale);
float gauss_table_f(int x, int y, struct image_s *patch, gauss_table_t *gauss_table);
void gauss_table_delete(gauss_table_t *gauss_table);

/*----------------------------------------------------------------------------*/

void convert_mi_to_ellipse(float *a, float *b, float *c, mat22_t mi, float angle, float scale);
void convert_ellipse_to_mi(mat22_t mi, float *angle, float *scale, float a, float b, float c);

/*----------------------------------------------------------------------------*/

int is_noisy(struct image_s* img, int window_size);
int remove_noise(struct image_s *dest, struct image_s* img, int window_size);
int is_black(struct image_s *im);
int get_block_size(struct image_s* img);
void deblock(struct image_s *deblocked, struct image_s *src, int block_size );

/*----------------------------------------------------------------------------*/

typedef struct pirix_timer_s
{
    uint64_t time_start;
    uint64_t time_stop;

} pirix_timer_t;

void tic(void);
double toc(void);

pirix_timer_t *pirix_timer_new(void);
void pirix_timer_start(pirix_timer_t *timer);
void pirix_timer_stop(pirix_timer_t *timer);
void pirix_timer_clear(pirix_timer_t *timer);
double pirix_timer_get_time(pirix_timer_t *timer);
void pirix_timer_delete(pirix_timer_t *timer);

/*----------------------------------------------------------------------------*/

#ifdef USE_ASM
int probe_mmx(void);
int probe_sse(void);
int probe_sse2(void);
int probe_sse3(void);
int probe_ssse3(void);
int probe_sse4_1(void);
int probe_sse4_2(void);

void get_instruction_set_info(void);
#endif

/*----------------------------------------------------------------------------*/

void wiener_cpu(float *dest, float *src, int width);
 
typedef struct mem_cache_s mem_cache_t;

mem_cache_t *mem_cache_new(int size, int align, int objects_per_slab);
int mem_cache_delete(mem_cache_t *cache);

void *mem_cache_alloc(mem_cache_t *cache);
int mem_cache_free(mem_cache_t *cache, void *ptr);

/*--------------------------------------------------------------------------*/

double randn(void);

/*--------------------------------------------------------------------------*/

void clustering_kmeans_assign(int n, float *points, int nb_centroids,
  float *centroids, int max_iter, int **cluster_assign, int **cluster_size,
  int n_threads);

void clustering_kmeans(int n, float *points, int nb_centroids,
  float *centroids, int nb_iter_max, int n_threads);

int *quantize_codebook(int codebook_size, float *codebook_data,
  int point_size, float *points);
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*----------------------------------------------------------------------------*/

#endif

/*----------------------------------------------------------------------------*/

