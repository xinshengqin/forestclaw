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

#include <fclaw2d_include_all.h>

void amg_link_solvers(fclaw2d_global_t *glob)
{
    fclaw2d_vtable_t *vt = fclaw2d_vt();

    const user_options_t* user = amg_get_options(glob);

    /* Nothing to do? */
}


void cb_fill_patches(fclaw2d_global_t* glob, 
                     fclaw2d_patch_t* this_patch,
                    int this_block_idx,
                    int this_patch_idx,
                    void *user)
{
    double* q = (double*) user;

    int patch_num, level;

    fclaw2d_patch_get_info(glob->domain,
                           this_patch, this_block_idx, this_patch_idx,
                           &patch_num, &level);

    /* Figure location in q for patchdata */

    /* Call fortran routine which fills in patch data into "this_patch" */
}

void cb_compute_residual(fclaw2d_global_t* glob, 
                         fclaw2d_patch_t* this_patch,
                         int this_block_idx,
                         int this_patch_idx,
                         void *user)
{
    double* r = (double*) user;

    /* Call Fortran routine to compute r = b - A*x  on this patch; store
    results in proper location in r.   */

    int patch_num, level;

    fclaw2d_patch_get_info(glob->domain,
                           this_patch, this_block_idx, this_patch_idx,
                           &patch_num, &level);

}



void amg_compute_residual(fclaw2d_global_t *glob, double * q, double *r)
{
    /* Iterate over all patches to fill patches with data from q */

    /* Call ghost_fill to fill all ghost cells */

    /* Iterate over patches to compute residual r = b - A*x

    This will require that we define our right hand side f 

    Return residual in *r */


}









