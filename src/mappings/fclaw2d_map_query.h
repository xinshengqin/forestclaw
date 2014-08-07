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


#ifndef FCLAW2D_MAP_QUERY_H
#define FCLAW2D_MAP_QUERY_H

#include <fclaw2d_map.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}                               /* need this because indent is dumb */
#endif
#endif

#define FCLAW2D_MAP_IS_CART FCLAW_F77_FUNC_(fclaw2d_map_is_cart,FCLAW2D_MAP_IS_CART)
int FCLAW2D_MAP_IS_CART(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_DISK FCLAW_F77_FUNC_(fclaw2d_map_is_disk,FCLAW2D_MAP_IS_DISK)
int FCLAW2D_MAP_IS_DISK(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_PILLOWDISK FCLAW_F77_FUNC_(fclaw2d_map_is_pillowdisk,FCLAW2D_MAP_IS_PILLOWDISK)
int FCLAW2D_MAP_IS_PILLOWDISK(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_SQUAREDDISK FCLAW_F77_FUNC_(fclaw2d_map_is_squareddisk,FCLAW2D_MAP_IS_SQUAREDDISK)
int FCLAW2D_MAP_IS_SQUAREDDISK(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_PILLOWSPHERE FCLAW_F77_FUNC_(fclaw2d_map_is_pillowsphere,FCLAW2D_MAP_IS_PILLOWSPHERE)
int FCLAW2D_MAP_IS_PILLOWSPHERE(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_CUBEDSPHERE FCLAW_F77_FUNC_(fclaw2d_map_is_cubedsphere,FCLAW2D_MAP_IS_CUBEDSPHERE)
int FCLAW2D_MAP_IS_CUBEDSPHERE(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_FLAT FCLAW_F77_FUNC_(fclaw2d_map_is_flat,FCLAW2D_MAP_IS_FLAT)
int FCLAW2D_MAP_IS_FLAT(fclaw2d_map_context_t** cont);

#define FCLAW2D_MAP_IS_SPHERE FCLAW_F77_FUNC_(fclaw2d_map_is_sphere,FCLAW2D_MAP_IS_SPHERE)
int FCLAW2D_MAP_IS_SPHERE(fclaw2d_map_context_t** cont);

#ifdef __cplusplus
#if 0
{                               /* need this because indent is dumb */
#endif
}
#endif


#endif
