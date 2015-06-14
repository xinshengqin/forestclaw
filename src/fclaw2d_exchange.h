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

#ifndef FCLAW2D_EXCHANGE_H
#define FCLAW2D_EXCHANGE_H

#include <fclaw2d_forestclaw.h>
#include <fclaw2d_partition.h>
#include <fclaw2d_convenience.h>
#include <fclaw2d_vtable.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

void fclaw2d_exchange_ghost_patches(fclaw2d_domain_t* domain,
                                    int minlevel,
                                    int maxlevel,
                                    int time_interp);

#if 0
void fclaw2d_exchange_ghost_patches_all(fclaw2d_domain_t* domain);

void fclaw2d_exchange_ghost_patch_partial(fclaw2d_domain_t* domain,
                                          int exchange_minlevel,
                                          int exchange_maxlevel);

void fclaw2d_exchange_ghost_patch_partial_new(fclaw2d_domain_t* domain,
                                              int exchange_minlevel,
                                              int exchange_maxlevel);
#endif

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif