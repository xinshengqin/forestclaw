/*
  Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FC2D_ELLIPTIC_H
#define FC2D_ELLIPTIC_H

#include <fclaw_base.h>   /* Needed for FCLAW_F77_FUNC */
#include <fclaw2d_transform.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

struct fclaw2d_global;
struct fclaw2d_patch;

typedef void (*fc2d_elliptic_setprob_t)(void);

typedef void (*fc2d_elliptic_bc2_t)(const int* maxmx, const int* maxmy,
                                      const int* meqn, const int* mbc,
                                      const int* mx, const int* my,
                                      const double* xlower, const double* ylower,
                                      const double* dx, const double* dy,
                                      const double q[], const int* maux,
                                      const double aux[], const double* t,
                                      const double* dt, const int mthbc[]);

typedef  void (*fc2d_elliptic_qinit_t)(const int* maxmx, const int* maxmy,
                                         const int* meqn,const int* mbc,
                                         const int* mx, const int* my,
                                         const double* xlower, const double* ylower,
                                         const double* dx, const double* dy,
                                         double q[], const int* maux, double aux[]);

typedef struct fc2d_elliptic_vtable
{

    /* Fortran routines */
    fc2d_elliptic_setprob_t   setprob;
    fc2d_elliptic_bc2_t       bc2;
    fc2d_elliptic_qinit_t     qinit;
    
    int is_set;

} fc2d_elliptic_vtable_t;

void fc2d_elliptic_vtable_initialize(void);

fc2d_elliptic_vtable_t* fc2d_elliptic_vt(void);

#define ELLIPTIC_BC2_DEFAULT FCLAW_F77_FUNC(elliptic_bc2_default,ELLIPTIC_BC2_DEFAULT)
void ELLIPTIC_BC2_DEFAULT(const int* mx, const int* my, const int* meqn,
                     const int* mbc, 
                     const double* xlower, const double* ylower,
                     const double* dx, const double* dy, const double q[],
                     const double* t, const double* dt, const int mthbc[]);

/* --------------------------------------------------------------------------------------
   Regridding functions
   -------------------------------------------------------------------------------------- */

#define FC2D_ELLIPTIC_FORT_TAG4REFINEMENT FCLAW_F77_FUNC(fc2d_elliptic_fort_tag4refinement, \
                                                           FC2D_ELLIPTIC_FORT_TAG4REFINEMENT)

void FC2D_ELLIPTIC_FORT_TAG4REFINEMENT(const int* mx,const int* my,
                                         const int* mbc,const int* meqn,
                                         const double* xlower, const double* ylower,
                                         const double* dx, const double* dy,
                                         const int* blockno,
                                         double q[],
                                         const double* tag_threshold,
                                         const int* init_flag,
                                         int* tag_patch);



#define FC2D_ELLIPTIC_FORT_TAG4COARSENING FCLAW_F77_FUNC(fc2d_elliptic_fort_tag4coarsening, \
                                                          FC2D_ELLIPTIC_FORT_TAG4COARSENING)

void FC2D_ELLIPTIC_FORT_TAG4COARSENING(const int* mx, const int* my,
                                         const int* mbc, const int* meqn,
                                         const double* xlower, const double* ylower,
                                         const double* dx, const double* dy,
                                         const int* blockno,
                                         double q0[],double q1[],
                                         double q2[],double q3[],
                                         const double* tag_threshold,
                                         int* tag_patch);

#define FC2D_ELLIPTIC_FORT_INTERPOLATE2FINE FCLAW_F77_FUNC(fc2d_elliptic_fort_interpolate2fine, \
                                                FC2D_ELLIPTIC_FORT_INTERPOLATE2FINE)
void FC2D_ELLIPTIC_FORT_INTERPOLATE2FINE(const int* mx,const int* my,
                                           const int* mbc, const int* meqn,
                                           double qcoarse[], double qfine[],
                                           double areacoarse[], double areafine[],
                                           const int* igrid, const int* manifold);

#define FC2D_ELLIPTIC_FORT_AVERAGE2COARSE FCLAW_F77_FUNC(fc2d_elliptic_fort_average2coarse, \
                                                FC2D_ELLIPTIC_FORT_AVERAGE2COARSE)
void FC2D_ELLIPTIC_FORT_AVERAGE2COARSE(const int* mx, const int* my,
                                         const int* mbc, const int* meqn,
                                         double qcoarse[],double qfine[],
                                         double areacoarse[],double areafine[],
                                         const int* igrid, const int* manifold);

#define FC2D_ELLIPTIC_FORT_COPY_FACE FCLAW_F77_FUNC(fc2d_elliptic_fort_copy_face, \
                                                     FC2D_ELLIPTIC_FORT_COPY_FACE)

void FC2D_ELLIPTIC_FORT_COPY_FACE(const int* mx, const int* my, const int* mbc, const int* meqn,
                                   double qthis[],double qneighbor[], const int* a_idir,
                                   fclaw2d_transform_data_t** transform_cptr);


#define FC2D_ELLIPTIC_FORT_AVERAGE_FACE FCLAW_F77_FUNC(fc2d_elliptic_fort_average_face, \
                                                        FC2D_ELLIPTIC_FORT_AVERAGE_FACE)
void FC2D_ELLIPTIC_FORT_AVERAGE_FACE(const int* mx, const int* my, const int* mbc,
                                      const int* meqn,
                                      double qcoarse[],double qfine[],
                                      double areacoarse[], double areafine[],
                                      const int* idir, const int* iside,
                                      const int* num_neighbors,
                                      const int* refratio, const int* igrid,
                                      const int* manifold, fclaw2d_transform_data_t** transform_cptr);

#define FC2D_ELLIPTIC_FORT_INTERPOLATE_FACE FCLAW_F77_FUNC(fc2d_elliptic_fort_interpolate_face, \
                                                            FC2D_ELLIPTIC_FORT_INTERPOLATE_FACE)
void FC2D_ELLIPTIC_FORT_INTERPOLATE_FACE(const int* mx, const int* my, const int* mbc,
                                          const int* meqn,
                                          double qcoarse[],double qfine[],
                                          const int* idir, const int* iside,
                                          const int* num_neighbors,
                                          const int* refratio, const int* igrid,
                                          fclaw2d_transform_data_t** transform_cptr);

#define FC2D_ELLIPTIC_FORT_COPY_CORNER FCLAW_F77_FUNC(fc2d_elliptic_fort_copy_corner, \
                                                       FC2D_ELLIPTIC_FORT_COPY_CORNER)
void FC2D_ELLIPTIC_FORT_COPY_CORNER(const int* mx, const int* my, const int* mbc,
                                     const int* meqn, double this_q[],double neighbor_q[],
                                     const int* a_corner,fclaw2d_transform_data_t** transform_cptr);

#define FC2D_ELLIPTIC_FORT_AVERAGE_CORNER FCLAW_F77_FUNC(fc2d_elliptic_fort_average_corner, \
                                                          FC2D_ELLIPTIC_FORT_AVERAGE_CORNER)
void FC2D_ELLIPTIC_FORT_AVERAGE_CORNER(const int* mx, const int* my, const int* mbc,
                                        const int* meqn, const int* a_refratio,
                                        double qcoarse[], double qfine[],
                                        double areacoarse[], double areafine[],
                                        const int* manifold,
                                        const int* a_corner, fclaw2d_transform_data_t** transform_cptr);

#define FC2D_ELLIPTIC_FORT_INTERPOLATE_CORNER FCLAW_F77_FUNC(fc2d_elliptic_fort_interpolate_corner, \
                                                             FC2D_ELLIPTIC_FORT_INTERPOLATE_CORNER)
void FC2D_ELLIPTIC_FORT_INTERPOLATE_CORNER(const int* mx, const int* my, const int* mbc,
                                            const int* meqn, const int* a_refratio, double this_q[],
                                            double neighbor_q[], const int* a_corner,
                                            fclaw2d_transform_data_t** transform_cptr);

/* --------------------------------------------------------------------------------------
   Output functions
   -------------------------------------------------------------------------------------- */
#define  FC2D_ELLIPTIC_FORT_OUTPUT_ASCII FCLAW_F77_FUNC(fc2d_elliptic_fort_output_ascii, \
                                                       FC2D_ELLIPTIC_FORT_OUTPUT_ASCII)
void  FC2D_ELLIPTIC_FORT_OUTPUT_ASCII(char* matname1,
                                     int* mx,        int* my,
                                     int* meqn,      int* mbc,
                                     double* xlower, double* ylower,
                                     double* dx,     double* dy,
                                     double q[],
                                     int* patch_num, int* level,
                                     int* blockno,   int* mpirank);

#define FC2D_ELLIPTIC_FORT_HEADER_ASCII FCLAW_F77_FUNC(fc2d_elliptic_fort_header_ascii, \
                                                        FC2D_ELLIPTIC_FORT_HEADER_ASCII)
void FC2D_ELLIPTIC_FORT_HEADER_ASCII(char* matname1, char* matname2,
                                      double* time, int* meqn, int* maux, int* ngrids);


#define FC2D_ELLIPTIC_FORT_CONSERVATION_CHECK FCLAW_F77_FUNC(fc2d_elliptic_fort_conservation_check, \
                                                              FC2D_ELLIPTIC_FORT_CONSERVATION_CHECK)
void FC2D_ELLIPTIC_FORT_CONSERVATION_CHECK(int *mx, int *my, int* mbc, int* meqn,
                                            double *dx, double *dy,
                                            double* area, double *q, double* sum);

#define FC2D_ELLIPTIC_FORT_COMPUTE_PATCH_AREA FCLAW_F77_FUNC(fc2d_elliptic_fort_compute_patch_area, \
                                                              FC2D_ELLIPTIC_FORT_COMPUTE_PATCH_AREA)

double FC2D_ELLIPTIC_FORT_COMPUTE_PATCH_AREA(int *mx, int* my, int*mbc, double* dx,
                                              double* dy, double area[]);



#if 0
#define FC2D_ELLIPTIC_FORT_COMPUTE_ERROR FCLAW_F77_FUNC(fc2d_elliptic_fort_compute_error, \
                                                     FC2D_ELLIPTIC_FORT_COMPUTE_ERROR)

void FC2D_ELLIPTIC_FORT_COMPUTE_ERROR(int* blockno, int *mx, int *my, int* mbc, int* meqn,
                                        double *dx, double *dy, double *xlower,
                                        double *ylower, double *t, double q[],
                                        double error[]);
#endif

#define FC2D_ELLIPTIC_FORT_COMPUTE_ERROR_NORM FCLAW_F77_FUNC(fc2d_elliptic_fort_compute_error_norm, \
                                                              FC2D_ELLIPTIC_FORT_COMPUTE_ERROR_NORM)

void FC2D_ELLIPTIC_FORT_COMPUTE_ERROR_NORM(int *mx, int *my, int *mbc, int *meqn,
                                            double *dx, double *dy, double area[],
                                            double error[], double error_norm[]);

#define FC2D_ELLIPTIC_FORT_GHOSTPACK_QAREA FCLAW_F77_FUNC(fc2d_elliptic_fort_ghostpack_qarea, \
                                                            FC2D_ELLIPTIC_FORT_GHOSTPACK_QAREA)
void  FC2D_ELLIPTIC_FORT_GHOSTPACK_QAREA(int *mx, int *my, int *mbc,
                                           int *meqn, int *mint,
                                           double qdata[], double area[],
                                           double qpack[], int *psize,
                                           int *packmode, int *ierror);

#define FC2D_ELLIPTIC_FORT_TIMEINTERP FCLAW_F77_FUNC (fc2d_elliptic_fort_timeinterp, \
                                                       FC2D_ELLIPTIC_FORT_TIMEINTERP)
void FC2D_ELLIPTIC_FORT_TIMEINTERP(const int *mx, const int* my, const int* mbc,
                                    const int *meqn, const int* psize,
                                    double qcurr[], double qlast[],
                                    double qinterp[],const double* alpha,
                                    const int* ierror);


/* -------------------------------------------------------------------------
   Routines that won't change
   ------------------------------------------------------------------------- */
void
    fc2d_elliptic_setprob(struct fclaw2d_global* glob);

void
    fc2d_elliptic_qinit(struct fclaw2d_global* glob,
                          struct fclaw2d_patch *this_patch,
                          int this_block_idx,
                          int this_patch_idx);

void
    fc2d_elliptic_bc2(struct fclaw2d_global *glob,
                        struct fclaw2d_patch *this_patch,
                        int this_block_idx,
                        int this_patch_idx,
                        double t,
                        double dt,
                        int intersects_bc[],
                        int time_interp);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif


#endif /* !FC2D_CLAWPACH46_H */
