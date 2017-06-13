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

#include "amg_user.h"

#include <fclaw2d_clawpatch.h>
#include <fclaw_options.h>
#include <fclaw2d_ghost_fill.h>

void amg_link_solvers(fclaw2d_global_t *glob)
{

 #if 0
    fclaw2d_vtable_t *vt = fclaw2d_vt();

    const user_options_t* user = amg_get_options(glob);
#endif    

    /* Nothing to do? */
}


static
void cb_fill_patches(fclaw2d_domain_t* domain, 
                     fclaw2d_patch_t* this_patch,
                     int this_block_idx,
                     int this_patch_idx,
                     void *user)
{
    fclaw2d_global_iterate_t *g = (fclaw2d_global_iterate*) user;
    double* q = (double*) g->user;
    fclaw2d_global_t *glob = g->glob;

    double *u;
    int mx,my,mbc;

    int patch_num, level;

    fclaw2d_patch_get_info(glob->domain,
                           this_patch, this_block_idx, this_patch_idx,
                           &patch_num, &level);

    fclaw2d_clawpatch_options_t *clawpatch_opt = fclaw2d_clawpatch_get_options(glob);
    mx = clawpatch_opt->mx;
    my = clawpatch_opt->my;
    mbc = clawpatch_opt->mbc;

    int idx = patch_num*mx*my;  /* Should be long int?  */

    u = fclaw2d_clawpatch_get_q(glob,this_patch);

    AMG_FILL_PATCH(&mx,&my,&mbc,&q[idx],u);
}

static
void cb_compute_residual(fclaw2d_domain_t* domain, 
                         fclaw2d_patch_t* this_patch,
                         int this_block_idx,
                         int this_patch_idx,
                         void *user)
{
    fclaw2d_global_iterate_t *g = (fclaw2d_global_iterate*) user;
    double* r = (double*) g->user;
    fclaw2d_global_t *glob = g->glob;

    double *u;
    double xlower,ylower,dx,dy;
    int mx,my,mbc;
    int patch_num, level;
    int idx;

    /* Call Fortran routine to compute r = b - A*x  on this patch; store
    results in proper location in r.   */


    fclaw2d_patch_get_info(glob->domain,
                           this_patch, this_block_idx, this_patch_idx,
                           &patch_num, &level);

    u = fclaw2d_clawpatch_get_q(glob,this_patch);

    fclaw2d_clawpatch_grid_data(glob, this_patch,
                                &mx,&my,&mbc,&xlower,&ylower,
                                &dx,&dy);

    idx = patch_num*mx*my;  /* Should be long int?  */


    AMG_COMPUTE_RESIDUAL(&mx,&my,&mbc,&xlower,&ylower,&dx,&dy,u,&r[idx]);
}



void amg_compute_residual(fclaw2d_global_t *glob, double *q, double *r)
{
    fclaw_options_t *fclaw_opt = fclaw2d_get_options(glob);

    fclaw2d_global_iterate_patches(glob,cb_fill_patches,q);

    double t = 0;
    int time_interp = 0;
    fclaw2d_ghost_update(glob, fclaw_opt->minlevel,
                         fclaw_opt->maxlevel,t, 
                         time_interp, 
                         FCLAW2D_TIMER_ADVANCE);

    fclaw2d_global_iterate_patches(glob,cb_compute_residual,r);
}









