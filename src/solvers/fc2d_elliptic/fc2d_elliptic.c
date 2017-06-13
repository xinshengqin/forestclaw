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

#include "fc2d_elliptic.h"


#include <fclaw2d_global.h>
#include <fclaw2d_vtable.h>
#include <fclaw2d_defs.h>

#include <fclaw2d_clawpatch.hpp>
#include <fclaw2d_clawpatch_output_ascii.h>
#include <fclaw2d_clawpatch_output_vtk.h>

static fc2d_elliptic_vtable_t classic_vt;

static
fc2d_elliptic_vtable_t* fc2d_elliptic_vt_init()
{
    FCLAW_ASSERT(classic_vt.is_set == 0);
    return &classic_vt;
}

/* -----------------------------------------------------------
   Public interface to routines in this file
   ----------------------------------------------------------- */

fc2d_elliptic_vtable_t* fc2d_elliptic_vt()
{
    FCLAW_ASSERT(classic_vt.is_set != 0);
    return &classic_vt;
}

void fc2d_elliptic_vtable_initialize()
{
    fclaw2d_clawpatch_vtable_initialize();

    fclaw2d_vtable_t*                fclaw_vt = fclaw2d_vt();
    fclaw2d_patch_vtable_t*          patch_vt = fclaw2d_patch_vt();  /* initialized in clawpatch_init */
    fclaw2d_clawpatch_vtable_t*  clawpatch_vt = fclaw2d_clawpatch_vt();

    fc2d_elliptic_vtable_t*  claw46_vt = fc2d_elliptic_vt_init();


    elliptic_vt->is_set = 1;
}

void fc2d_elliptic_bc2(fclaw2d_global_t *glob,
                         fclaw2d_patch_t *this_patch,
                         int this_block_idx,
                         int this_patch_idx,
                         double t,
                         double dt,
                         int intersects_phys_bdry[],
                         int time_interp)
{
    fc2d_elliptic_options_t *clawpack_options = fc2d_elliptic_get_options(glob);

    FCLAW_ASSERT(elliptic_vt.bc2 != NULL);

    int mx,my,mbc,meqn,maux,maxmx,maxmy;
    double xlower,ylower,dx,dy;
    double *u;

    fclaw2d_clawpatch_grid_data(glob,this_patch, &mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    int *block_mthbc = elliptic_options->mthbc;

    /* Set a local copy of mthbc that can be used for a patch. */
    int mthbc[NumFaces];
    for(int i = 0; i < NumFaces; i++)
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

    elliptic_vt.bc2(&mx,&my,&meqn,&mbc,&mx,&my,&xlower,&ylower,
                   &dx,&dy,u,&t,&dt,mthbc);
}

