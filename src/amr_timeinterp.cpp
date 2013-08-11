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

#include "amr_includes.H"


static
void cb_setup_time_interp(fclaw2d_domain_t *domain,
                          fclaw2d_patch_t *this_patch,
                          int this_block_idx,
                          int this_patch_idx,
                          void *user)
{
    /* This is called for all patches on the coarse level */
    ClawPatch *cp = get_clawpatch(this_patch);
    double &alpha = *((double*) user);

    /* This constructs a time interpolated version of the data on
       the coarser grid */
    cp->setup_for_time_interpolation(alpha);
}



/* ----------------------------------------------------------------------
   Main routine in this file.  This file assumes that both coarse and
   fine grids have valid interior data;  they only need to exchange (
   via interpolating and averaging) ghost cell values.
   -------------------------------------------------------------------- */

void timeinterp(fclaw2d_domain_t *domain,
                int level,double alpha)
{
    // Use separate timer for all exchanges
    fclaw2d_domain_data_t *ddata = get_domain_data(domain);

    fclaw_bool time_interp = alpha > 0; //
    int coarser_level = level - 1;

    /* Store time interpolated data into m_griddata_time_sync. */
    fclaw2d_domain_iterate_level(domain, coarser_level,cb_setup_time_interp,
                                     (void *) &alpha);
}