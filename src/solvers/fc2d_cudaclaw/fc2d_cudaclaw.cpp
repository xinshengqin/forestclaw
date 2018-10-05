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

#include "fc2d_cudaclaw.h"
#include "fc2d_cudaclaw_options.h"
#include "fc2d_cudaclaw_fort.h"

#include "cuda_source/cudaclaw_allocate.h"

#include <fclaw2d_clawpatch.hpp>
#include <fclaw2d_clawpatch.h>

#include <fclaw2d_update_single_step.h>
#include <fclaw2d_clawpatch_diagnostics.h>
#include <fclaw2d_clawpatch_options.h>
#include <fclaw2d_clawpatch_output_ascii.h> 
#include <fclaw2d_clawpatch_output_vtk.h>
#include <fclaw2d_clawpatch_fort.h>

#include <fclaw2d_patch.h>
#include <fclaw2d_global.h>
#include <fclaw2d_vtable.h>
#include <fclaw2d_defs.h>

#include "cuda_source/cudaclaw_allocate.h"
#include <fc2d_cuda_profiler.h>
#include "cuda_runtime_api.h"
#include "fc2d_cudaclaw_check.cu"

static fc2d_cudaclaw_vtable_t s_cudaclaw_vt;

/* --------------------- Clawpack solver functions (required) ------------------------- */

static
void cudaclaw_setprob(fclaw2d_global_t *glob)
{
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();
    if (cudaclaw_vt->fort_setprob != NULL)
    {
        cudaclaw_vt->fort_setprob();
    }
}


static
void cudaclaw_qinit(fclaw2d_global_t *glob,
                      fclaw2d_patch_t *this_patch,
                      int this_block_idx,
                      int this_patch_idx)
{
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();

    FCLAW_ASSERT(cudaclaw_vt->fort_qinit != NULL); /* Must be initialized */
    int mx,my,mbc,meqn,maux,maxmx,maxmy;
    double dx,dy,xlower,ylower;
    double *q, *aux;

    fclaw2d_clawpatch_grid_data(glob,this_patch,&mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    fclaw2d_clawpatch_soln_data(glob,this_patch,&q,&meqn);
    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);

    maxmx = mx;
    maxmy = my;

    /* Call to classic Clawpack 'qinit' routine.  This must be user defined */
    CUDACLAW_SET_BLOCK(&this_block_idx);
    cudaclaw_vt->fort_qinit(&maxmx,&maxmy,&meqn,&mbc,&mx,&my,&xlower,&ylower,&dx,&dy,q,
                          &maux,aux);
    CUDACLAW_UNSET_BLOCK();
}


static
void cudaclaw_bc2(fclaw2d_global_t *glob,
                    fclaw2d_patch_t *this_patch,
                    int this_block_idx,
                    int this_patch_idx,
                    double t,
                    double dt,
                    int intersects_phys_bdry[],
                    int time_interp)
{
    PROFILE_CUDA_GROUP("cudaclaw_bc2",6);
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();

    fc2d_cudaclaw_options_t *clawpack_options = fc2d_cudaclaw_get_options(glob);

    FCLAW_ASSERT(cudaclaw_vt->fort_bc2 != NULL);

    int mx,my,mbc,meqn,maux,maxmx,maxmy;
    double xlower,ylower,dx,dy;
    double *aux,*q;

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);

    maxmx = mx;
    maxmy = my;

    int *block_mthbc = clawpack_options->mthbc;

    /* Set a local copy of mthbc that can be used for a patch. */
    int mthbc[4];
    for(int i = 0; i < 4; i++)
    {
        if (intersects_phys_bdry[i])
        {
            mthbc[i] = block_mthbc[i];
        }
        else
        {
            mthbc[i] = -1;
        }
    }

    /*
      We may be imposing boundary conditions on time-interpolated data;
      and is being done just to help with fine grid interpolation.
      In this case, this boundary condition won't be used to update
      anything
    */
    fclaw2d_clawpatch_timesync_data(glob,this_patch,time_interp,&q,&meqn);

    CUDACLAW_SET_BLOCK(&this_block_idx);
    cudaclaw_vt->fort_bc2(&maxmx,&maxmy,&meqn,&mbc,&mx,&my,&xlower,&ylower,
                        &dx,&dy,q,&maux,aux,&t,&dt,mthbc);
    CUDACLAW_UNSET_BLOCK();
}


static
void cudaclaw_b4step2(fclaw2d_global_t *glob,
                        fclaw2d_patch_t *this_patch,
                        int this_block_idx,
                        int this_patch_idx,
                        double t, double dt)

{
    PROFILE_CUDA_GROUP("cudaclaw_b4step2",1);
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();

    int mx,my,mbc,meqn, maux,maxmx,maxmy;
    double xlower,ylower,dx,dy;
    double *aux,*q;

    if (cudaclaw_vt->fort_b4step2 == NULL)
    {
        return;
    }

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    fclaw2d_clawpatch_soln_data(glob,this_patch,&q,&meqn);
    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);

    maxmx = mx;
    maxmy = my;

    CUDACLAW_SET_BLOCK(&this_block_idx);
    cudaclaw_vt->fort_b4step2(&maxmx,&maxmy,&mbc,&mx,&my,&meqn,q,&xlower,&ylower,
                            &dx,&dy,&t,&dt,&maux,aux);
    CUDACLAW_UNSET_BLOCK();
}

static
void cudaclaw_src2(fclaw2d_global_t *glob,
                     fclaw2d_patch_t *this_patch,
                     int this_block_idx,
                     int this_patch_idx,
                     double t,
                     double dt)
{
    PROFILE_CUDA_GROUP("cudaclaw_src2",7);
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();

    int mx,my,mbc,meqn, maux,maxmx,maxmy;
    double xlower,ylower,dx,dy;
    double *aux,*q;

    if (cudaclaw_vt->fort_src2 == NULL)
    {
        /* User has not set a fortran routine */
        return;
    }

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    fclaw2d_clawpatch_soln_data(glob,this_patch,&q,&meqn);
    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);

    maxmx = mx;
    maxmy = my;

    CUDACLAW_SET_BLOCK(&this_block_idx);
    cudaclaw_vt->fort_src2(&maxmx,&maxmy,&meqn,&mbc,&mx,&my,&xlower,&ylower,
                         &dx,&dy,q,&maux,aux,&t,&dt);
    CUDACLAW_UNSET_BLOCK();
}


/* This can be used as a value for patch_vt->patch_setup */
static
void cudaclaw_setaux(fclaw2d_global_t *glob,
                       fclaw2d_patch_t *this_patch,
                       int this_block_idx,
                       int this_patch_idx)
{
    PROFILE_CUDA_GROUP("cudaclaw_setaux",2);
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();

    if (cudaclaw_vt->fort_setaux == NULL)
    {
        return;
    }

    if (fclaw2d_patch_is_ghost(this_patch))
    {
        /* This is going to be removed at some point */
        return;
    }

    int mx,my,mbc,maux,maxmx,maxmy;
    double xlower,ylower,dx,dy;
    double *aux;

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);

    maxmx = mx;
    maxmy = my;

    CUDACLAW_SET_BLOCK(&this_block_idx);
    cudaclaw_vt->fort_setaux(&maxmx,&maxmy,&mbc,&mx,&my,&xlower,&ylower,&dx,&dy,
                           &maux,aux);
    CUDACLAW_UNSET_BLOCK();
}

static
double cudaclaw_update(fclaw2d_global_t *glob,
                         fclaw2d_patch_t *this_patch,
                         int this_block_idx,
                         int this_patch_idx,
                         double t,
                         double dt,
                         void* user)
{
    PROFILE_CUDA_GROUP("cudaclaw_update",3);
    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = fc2d_cudaclaw_vt();
    int iter, total, patch_buffer_len;

    const fc2d_cudaclaw_options_t* clawpack_options;
    clawpack_options = fc2d_cudaclaw_get_options(glob);
    double maxcfl = 0.0;


    fclaw2d_single_step_buffer_data_t *buffer_data = 
              (fclaw2d_single_step_buffer_data_t*) user;

    patch_buffer_len = FC2D_CUDACLAW_BUFFER_LEN;
    iter = buffer_data->iter;
    total = buffer_data->total_count; 
#if 0
    if (cudaclaw_vt->b4step2 != NULL)
    {
        fclaw2d_timer_start (&glob->timers[FCLAW2D_TIMER_ADVANCE_B4STEP2]);       
        cudaclaw_vt->b4step2(glob,
                           this_patch,
                           this_block_idx,
                           this_patch_idx,t,dt);
        fclaw2d_timer_stop (&glob->timers[FCLAW2D_TIMER_ADVANCE_B4STEP2]);       
    }
#endif
    fclaw2d_timer_start (&glob->timers[FCLAW2D_TIMER_ADVANCE_STEP2]);  
    // if (iter == 0)
    // {
    //     /* Create array to store pointers to patch data */
    //     buffer_data->user = FCLAW_ALLOC(cudaclaw_fluxes_t,
    //                                     patch_buffer_len*sizeof(cudaclaw_fluxes_t));
    // }  

    /* Be sure to save current step! */
    fclaw2d_clawpatch_save_current_step(glob, this_patch);


    /* Memcopy data to qold_dev, aux_dev;  store pointer to fluxes */
    fc2d_cudaclaw_store_buffer(glob,this_patch,this_patch_idx,total,
                              iter, (cudaclaw_fluxes_t*) s_array_fluxes_struct[batch_iter%NUM_STREAMS]);

    maxcfl = 0;
    if ((iter+1) % patch_buffer_len == 0)
    {
        maxcfl = cudaclaw_step2_batch(glob,
                                      patch_buffer_len,t,dt);
    }
    else if (iter == total-1)
    {
        maxcfl = cudaclaw_step2_batch(glob,
                                      total%patch_buffer_len,t,dt);        
    }

    // if (iter == total-1)
    // {
    //     FCLAW_FREE(buffer_data->user);
    // }

    fclaw2d_timer_stop (&glob->timers[FCLAW2D_TIMER_ADVANCE_STEP2]);       

    if (clawpack_options->src_term > 0 && cudaclaw_vt->src2 != NULL)
    {
        cudaclaw_vt->src2(glob,
                        this_patch,
                        this_block_idx,
                        this_patch_idx,t,dt);
    }

    if (iter == total-1)
    {
        // TODO: Here we assume num_batch < NUM_STREAMS during the entire run.
        CHECK(cudaDeviceSynchronize());
        PROFILE_CUDA_GROUP("Copy back to patches loop",2);
        int num_batch = total/FC2D_CUDACLAW_BUFFER_LEN+1 ;
        for (int ib = 0; ib < num_batch; ++ib)
        {
            /* -------------------------- Copy q back to host ----------------------------------*/ 
            cudaclaw_fluxes_t* array_fluxes_struct = s_array_fluxes_struct[batch_iter%NUM_STREAMS];
            for (int i = 0; i < FC2D_CUDACLAW_BUFFER_LEN; ++i)    
            {     
                cudaclaw_fluxes_t* fluxes = &(array_fluxes_struct[i]);
                int I_q = i*fluxes->num;

                memcpy(fluxes->qold,&s_membuffer[batch_iter%NUM_STREAMS][I_q],fluxes->num_bytes);

            }        
        }

    }
    return maxcfl;
}


/* ---------------------------------- Output functions -------------------------------- */

static
void cudaclaw_output(fclaw2d_global_t *glob, int iframe)
{
    const fc2d_cudaclaw_options_t* clawpack_options;
    clawpack_options = fc2d_cudaclaw_get_options(glob);

    if (clawpack_options->ascii_out != 0)
    {
        fclaw2d_clawpatch_output_ascii(glob,iframe);
    }

    if (clawpack_options->vtk_out != 0)
    {
        fclaw2d_clawpatch_output_vtk(glob,iframe);
    }

}

/* ---------------------------------- Virtual table  ---------------------------------- */

static
fc2d_cudaclaw_vtable_t* cudaclaw_vt_init()
{
    FCLAW_ASSERT(s_cudaclaw_vt.is_set == 0);
    return &s_cudaclaw_vt;
}

void fc2d_cudaclaw_solver_initialize()
{
    int claw_version = 4;
    fclaw2d_clawpatch_vtable_initialize(claw_version);

    fclaw2d_vtable_t*                fclaw_vt = fclaw2d_vt();
    fclaw2d_patch_vtable_t*          patch_vt = fclaw2d_patch_vt();  

    fc2d_cudaclaw_vtable_t*  cudaclaw_vt = cudaclaw_vt_init();

    /* ForestClaw vtable items */
    fclaw_vt->output_frame                   = cudaclaw_output;
    fclaw_vt->problem_setup                  = cudaclaw_setprob;    

    /* These could be over-written by user specific settings */
    patch_vt->initialize                     = cudaclaw_qinit;
    patch_vt->setup                          = cudaclaw_setaux;  
    patch_vt->physical_bc                    = cudaclaw_bc2;
    patch_vt->single_step_update             = cudaclaw_update;

    /* Set user data */
    patch_vt->create_user_data  = cudaclaw_allocate_fluxes;
    patch_vt->destroy_user_data = cudaclaw_deallocate_fluxes;

    /* Wrappers so that user can change argument list */
    cudaclaw_vt->b4step2                       = cudaclaw_b4step2;
    cudaclaw_vt->src2                          = cudaclaw_src2;

    /* Required functions  - error if NULL */
    cudaclaw_vt->fort_bc2       = CUDACLAW_BC2_DEFAULT;
    cudaclaw_vt->fort_qinit     = NULL;
    cudaclaw_vt->fort_rpn2      = NULL;
    cudaclaw_vt->fort_rpt2      = NULL;

    /* Optional functions - call only if non-NULL */
    cudaclaw_vt->fort_setprob   = NULL;
    cudaclaw_vt->fort_setaux    = NULL;
    cudaclaw_vt->fort_b4step2   = NULL;
    cudaclaw_vt->fort_src2      = NULL;

    cudaclaw_vt->is_set = 1;
}


/* ----------------------------- User access to solver functions --------------------------- */


/* These are here in case the user wants to call Clawpack routines directly */

fc2d_cudaclaw_vtable_t* fc2d_cudaclaw_vt()
{
    FCLAW_ASSERT(s_cudaclaw_vt.is_set != 0);
    return &s_cudaclaw_vt;
}

/* This should only be called when a new fclaw2d_clawpatch_t is created. */
void fc2d_cudaclaw_set_capacity(fclaw2d_global_t *glob,
                                  fclaw2d_patch_t *this_patch,
                                  int this_block_idx,
                                  int this_patch_idx)
{
    int mx,my,mbc,maux,mcapa;
    double dx,dy,xlower,ylower;
    double *aux, *area;
    fc2d_cudaclaw_options_t *clawopt;

    clawopt = fc2d_cudaclaw_get_options(glob);
    mcapa = clawopt->mcapa;

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    area = fclaw2d_clawpatch_get_area(glob,this_patch);

    fclaw2d_clawpatch_aux_data(glob,this_patch,&aux,&maux);
    FCLAW_ASSERT(maux >= mcapa && mcapa > 0);

    CUDACLAW_SET_CAPACITY(&mx,&my,&mbc,&dx,&dy,area,&mcapa,
                            &maux,aux);
}



/* -------------------------- Public interface to Clawpack wrappers --------------------*/

/* These are overkill;  it isn't obvious why the user would want these */

void fc2d_cudaclaw_setprob(fclaw2d_global_t *glob)
{
    cudaclaw_setprob(glob);
}

/* This can be set to cudaclaw_vt->src2 */
void fc2d_cudaclaw_src2(fclaw2d_global_t* glob,
                          fclaw2d_patch_t *this_patch,
                          int this_block_idx,
                          int this_patch_idx,
                          double t,
                          double dt)
{
    cudaclaw_src2(glob,this_patch,this_block_idx,this_block_idx,t,dt);
}


void fc2d_cudaclaw_setaux(fclaw2d_global_t *glob,
                            fclaw2d_patch_t *this_patch,
                            int this_block_idx,
                            int this_patch_idx)
{
    cudaclaw_setaux(glob,this_patch,this_block_idx,this_patch_idx);
}


void fc2d_cudaclaw_qinit(fclaw2d_global_t *glob,
                           fclaw2d_patch_t *this_patch,
                           int this_block_idx,
                           int this_patch_idx)
{
    cudaclaw_qinit(glob,this_patch,this_block_idx,this_patch_idx);
}

void fc2d_cudaclaw_b4step2(fclaw2d_global_t* glob,
                             fclaw2d_patch_t *this_patch,
                             int this_block_idx,
                             int this_patch_idx,
                             double t,
                             double dt)
{
    cudaclaw_b4step2(glob,this_patch,this_block_idx,this_patch_idx,t,dt);
}

void fc2d_cudaclaw_bc2(fclaw2d_global_t *glob,
                         fclaw2d_patch_t *this_patch,
                         int this_block_idx,
                         int this_patch_idx,
                         double t,
                         double dt,
                         int intersects_bc[],
                         int time_interp)
{
    cudaclaw_bc2(glob,this_patch,this_block_idx,this_block_idx,t,dt,
                   intersects_bc,time_interp);
}













