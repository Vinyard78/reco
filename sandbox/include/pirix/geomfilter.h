/*--------------------------------------------------------------------------*/

#ifndef FILTER_SHORTLIST_H_INCLUDED
#define FILTER_SHORTLIST_H_INCLUDED

/*--------------------------------------------------------------------------*/

#include <pirix/local_desc.h>

/*--------------------------------------------------------------------------*/

typedef local_desc_list_t imdesc_t;

/*--------------------------------------------------------------------------*/

typedef struct shortlist_t shortlist_t;

shortlist_t* shortlist_new(imdesc_t **imdescs,int nimdesc);

void shortlist_set_query(shortlist_t *sl, local_desc_list_t *query);

void shortlist_match_points_vw(shortlist_t *,int hamming_thresh, float text_filter_var, double * probs);

void shortlist_delete(shortlist_t *);

/*--------------------------------------------------------------------------*/

typedef struct pointmatch_t
{
  struct pointmatch_t *next;
  float score;  /* in [0,1] bigger = better */
  int qpt, dbpt; /* point numbers in their respective image descriptors */

} pointmatch_t;

/*--------------------------------------------------------------------------*/

typedef struct
{
  /* ---------- used internally */
  int npt; /* nb of pts in image */
  int ptofs; /* index of first point */
  struct fast_alloc_t *fa_pointmatch;

  /* ---------- search results */
  /* linked list of point matches for this image */
  pointmatch_t *ptmatches;   

  /* query in lowDim dimensions */
  int stage1_nmatch; 

  /* in 128 dim */
  int stage2_nmatch; 
  double stage2_votes;

  /* after geometric verification */
  int stage3_nmatch;
  double stage3_votes;

  /* new improved voting system */
  double final_votes;

  /* affine matrix (defined iff stage3_nmatch>0) */
  double affine[6];
  
} imagematch_t;

/*--------------------------------------------------------------------------*/

imagematch_t *shortlist_get_imagematch(shortlist_t *,int imno);

typedef struct
{

  /* use an anisotropic scaling model in 2 dimensions instead of a
     similarity model during the Hough transform stage? */
  int use_2aff_model;

  /* layout of bin sizes:
     scale (in log) 
     angle 
     translation x 
     translation y 
  */
  double bin_sizes[4];

  /* layout of bin sizes:
     scaling factor in x (log)
     translation in x
     scaling factor in y (log)
     translation in y
  */
  double bin_sizes_2aff[4];

  /* don't handle more than this bins */
  int max_nbin;

  /* min nb of matches before we try to estimate an affine transform (>=3) */
  int min_match_before_affine;

  /* when searching for agreeing point matches, allow this tolerance
     (in pixels) */
  double pos_error_in_affine;

  /* need at least this number of agreeing matches */ 
  int min_match_after_affine;  

  /* when verifying if points are not too close to each other, use
     this threshold (in pixels) */
  double distinct_tolerance;

  /* the bigger, the more diagnostic messages are written to stdout
     (0=silent) */
  int verbose;
  
  /* a priori weighting of deformations */

  /* enabled? */
  int weight_deformation;

  /* tolerances on parameters */
  double sigma_logscale;  /* scaling */
  double sigma_logar;     /* aspect ratio change */
  double sigma_a1;        /* rotation angle of horizontal axis */
  double sigma_a12;       /* difference of rotation of the vertical axis */
  int weight_portrait;    /* are pictures in portrait format weighted
                             the same as landscape? */
  double min_weight;      /* strip weights below this  */


  /* old weighting with hard thresholds (not used if weight_deformation) */

  /* absolute val of determinant of affine matrix should be in 
     [1/max_det,max_det] */
  double max_det;

  /* aspect ratio (small axis/big axis) of ellipse should be bigger
     than this */ 
  double min_ar;
 
  /* finally estimated affine matrix should not transform (0,0) more
     than this many bins away from the initial bin */
  double max_bin_deviation;
  
  
} lowehough_parameters_t;

/*--------------------------------------------------------------------------*/

void lowehough_parameters_default(lowehough_parameters_t *params);

void shortlist_filter_lowehough(shortlist_t *,lowehough_parameters_t *params);

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

