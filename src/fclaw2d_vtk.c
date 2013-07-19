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

#include "fclaw2d_vtk.h"

typedef struct fclaw2d_vtk_state
{
    int mx, my, meqn;
    int points_per_patch, cells_per_patch;
    int intsize, ndsize;
    int fits32;
    char filename[BUFSIZ];
    int64_t global_num_points, global_num_cells;
    int64_t global_num_connectivity;
    int64_t offset_position, psize_position;
    int64_t offset_connectivity, psize_connectivity;
    int64_t offset_offsets, psize_offsets;
    int64_t offset_types, psize_types;
    int64_t offset_mpirank, psize_mpirank;
    int64_t offset_blockno, psize_blockno;
    int64_t offset_patchno, psize_patchno;
    int64_t offset_meqn, psize_meqn;
    int64_t offset_end;
    const char *inttype;
    fclaw2d_vtk_patch_data_t coordinate_cb;
    fclaw2d_vtk_patch_data_t value_cb;
    MPI_File mpifile;
    MPI_Offset mpibegin;
    char *buf;
}
fclaw2d_vtk_state_t;

static int
fclaw2d_vtk_write_header (fclaw2d_domain_t * domain, fclaw2d_vtk_state_t * s)
{
    int retval;
    FILE *file;

    /* unconditionally open the file */
    file = fopen (s->filename, "wb");
    if (file == NULL)
    {
        return -1;
    }

    /* stop writing after first unsuccessful operation */
    retval = 0;
    retval = retval || fprintf (file, "<?xml version=\"1.0\"?>\n") < 0;
    retval = retval || fprintf (file, "<VTKFile type=\"UnstructuredGrid\" "
                                "version=\"0.1\" "
                                "byte_order=\"LittleEndian\">\n") < 0;
    retval = retval || fprintf (file, " <UnstructuredGrid>\n") < 0;
    retval = retval || fprintf (file, "  <Piece NumberOfPoints=\"%lld\" "
                                "NumberOfCells=\"%lld\">\n",
                                (long long) s->global_num_points,
                                (long long) s->global_num_cells) < 0;
    retval = retval || fprintf (file, "   <Points>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"Float64\" "
                                "Name=\"Position\" NumberOfComponents=\"3\" "
                                "format=\"appended\" offset=\"%lld\">\n",
                                (long long) s->offset_position) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "   </Points>\n") < 0;
    retval = retval || fprintf (file, "   <Cells>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"%s\" "
                                "Name=\"connectivity\" format=\"appended\" "
                                "offset=\"%lld\">\n", s->inttype,
                                (long long) s->offset_connectivity) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"%s\" "
                                "Name=\"offsets\" format=\"appended\" "
                                "offset=\"%lld\">\n", s->inttype,
                                (long long) s->offset_offsets) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"UInt8\" "
                                "Name=\"types\" format=\"appended\" "
                                "offset=\"%lld\">\n",
                                (long long) s->offset_types) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "   </Cells>\n") < 0;
    retval = retval || fprintf (file, "   <CellData Scalars=\"mpirank,"
                                "blockno,patchno\" Vectors=\"meqn\">\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"Int32\" "
                                "Name=\"mpirank\" format=\"appended\" "
                                "offset=\"%lld\">\n",
                                (long long) s->offset_mpirank) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"Int32\" "
                                "Name=\"blockno\" format=\"appended\" "
                                "offset=\"%lld\">\n",
                                (long long) s->offset_blockno) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"%s\" "
                                "Name=\"patchno\" format=\"appended\" "
                                "offset=\"%lld\">\n", s->inttype,
                                (long long) s->offset_patchno) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "    <DataArray type=\"Float32\" "
                                "Name=\"meqn\" NumberOfComponents=\"%d\" "
                                "format=\"appended\" offset=\"%lld\">\n",
                                s->meqn, (long long) s->offset_meqn) < 0;
    retval = retval || fprintf (file, "    </DataArray>\n") < 0;
    retval = retval || fprintf (file, "   </CellData>\n") < 0;
    retval = retval || fprintf (file, "   <PointData>\n") < 0;
    retval = retval || fprintf (file, "   </PointData>\n") < 0;
    retval = retval || fprintf (file, "  </Piece>\n") < 0;
    retval = retval || fprintf (file, " </UnstructuredGrid>\n") < 0;
    retval = retval || fprintf (file, " <AppendedData "
                                "encoding=\"raw\">\n  _") < 0;

    /* unconditionally close the file */
    retval = fclose (file) || retval;
    if (retval)
    {
        return -1;
    }
    return 0;
}

static void
write_position_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                   int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    MPI_Status mpistatus;

    s->coordinate_cb (domain, patch, blockno, patchno, s->buf);
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_position, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_connectivity_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                       int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int i, j;
    MPI_Status mpistatus;

    if (s->fits32)
    {
        const int32_t pbefore = s->points_per_patch *
            (domain->global_num_patches_before +
             domain->blocks[blockno].num_patches_before + patchno);

        int32_t *idata = (int32_t *) s->buf;
        int32_t k;
        for (j = 0; j < s->my; ++j)
        {
            for (i = 0; i < s->mx; ++i)
            {
                k = pbefore + i + j * (s->mx + 1);
                *idata++ = k;
                *idata++ = k + 1;
                *idata++ = k + s->mx + 2;
                *idata++ = k + s->mx + 1;
            }
        }
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
    mpiret =
        MPI_File_write (s->mpifile, s->buf, s->psize_connectivity, MPI_BYTE,
                        &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_offsets_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int c;
    MPI_Status mpistatus;

    if (s->fits32)
    {
        const int32_t cbefore = s->cells_per_patch *
            (domain->global_num_patches_before +
             domain->blocks[blockno].num_patches_before + patchno);

        int32_t *idata = (int32_t *) s->buf;
        int32_t k = 4 * (cbefore + 1);
        for (c = 0; c < s->cells_per_patch; k += 4, ++c)
        {
            *idata++ = k;
        }
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_offsets, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_types_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int c;
    MPI_Status mpistatus;

    char *cdata = s->buf;
    for (c = 0; c < s->cells_per_patch; ++c)
    {
        *cdata++ = 9;
    }
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_types, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_mpirank_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int c;
    MPI_Status mpistatus;

    int *idata = (int *) s->buf;
    for (c = 0; c < s->cells_per_patch; ++c)
    {
        *idata++ = domain->mpirank;
    }
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_mpirank, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_blockno_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int c;
    MPI_Status mpistatus;

    int *idata = (int *) s->buf;
    for (c = 0; c < s->cells_per_patch; ++c)
    {
        *idata++ = blockno;
    }
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_blockno, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_patchno_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    int c;
    MPI_Status mpistatus;

    if (s->fits32)
    {
        const int32_t gpno =
            domain->global_num_patches_before +
            domain->blocks[blockno].num_patches_before + patchno;

        int32_t *idata = (int32_t *) s->buf;
        for (c = 0; c < s->cells_per_patch; ++c)
        {
            *idata++ = gpno;
        }
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_patchno, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
write_meqn_cb (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
               int blockno, int patchno, void *user)
{
    fclaw2d_vtk_state_t *s = (fclaw2d_vtk_state_t *) user;
    int mpiret;
    MPI_Status mpistatus;

    s->value_cb (domain, patch, blockno, patchno, s->buf);
    mpiret = MPI_File_write (s->mpifile, s->buf, s->psize_meqn, MPI_BYTE,
                             &mpistatus);
    SC_CHECK_MPI (mpiret);
}

static void
fclaw2d_vtk_write_field (fclaw2d_domain_t * domain, fclaw2d_vtk_state_t * s,
                         int64_t offset_field, int64_t psize_field,
                         fclaw2d_patch_callback_t cb)
{
    int mpiret;
    int32_t bcount;
    MPI_Offset mpipos;
#ifdef P4EST_DEBUG
    MPI_Offset mpinew;
#endif
    MPI_Status mpistatus;

    s->buf = P4EST_ALLOC (char, psize_field);
    mpipos = s->mpibegin + offset_field;
    if (domain->mpirank > 0)
    {
        /* account for byte count */
        mpipos += s->ndsize + psize_field * domain->global_num_patches_before;
    }
    mpiret = MPI_File_seek (s->mpifile, mpipos, MPI_SEEK_SET);
    SC_CHECK_MPI (mpiret);
    if (domain->mpirank == 0)
    {
        /* write byte count */
        bcount = psize_field * domain->global_num_patches;
#if 0
        P4EST_LDEBUGF ("offset %lld psize %lld bcount %d %o %x\n",
                       (long long) offset_field, (long long) psize_field,
                       bcount, bcount, bcount);
#endif
        mpiret = MPI_File_write (s->mpifile, &bcount, 1, MPI_INT, &mpistatus);
        SC_CHECK_MPI (mpiret);
    }
    fclaw2d_domain_iterate_patches (domain, cb, s);
    P4EST_FREE (s->buf);

#ifdef P4EST_DEBUG
    mpiret = MPI_File_get_position (s->mpifile, &mpinew);
    SC_CHECK_MPI (mpiret);
    P4EST_ASSERT (mpinew - mpipos ==
                  (domain->mpirank == 0 ? s->ndsize : 0) +
                  domain->local_num_patches * psize_field);
    P4EST_ASSERT (domain->mpirank < domain->mpisize - 1 ||
                  mpinew - s->mpibegin ==
                  offset_field + s->ndsize +
                  psize_field * domain->global_num_patches);
#endif
}

static void
fclaw2d_vtk_write_data (fclaw2d_domain_t * domain, fclaw2d_vtk_state_t * s)
{
    int mpiret;
    MPI_Offset mpipos;

    /* collectively open the file in append mode and reserve space */
    mpiret = MPI_File_open (domain->mpicomm, s->filename,
                            MPI_MODE_WRONLY | MPI_MODE_APPEND |
                            MPI_MODE_UNIQUE_OPEN, MPI_INFO_NULL, &s->mpifile);
    SC_CHECK_MPI (mpiret);
    mpiret = MPI_File_get_position (s->mpifile, &s->mpibegin);
    SC_CHECK_MPI (mpiret);
    mpipos = s->mpibegin + (MPI_Offset) s->offset_end;
    mpiret = MPI_File_preallocate (s->mpifile, mpipos);
    SC_CHECK_MPI (mpiret);

    /* write meta data fields */
    fclaw2d_vtk_write_field (domain, s, s->offset_position, s->psize_position,
                             write_position_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_connectivity,
                             s->psize_connectivity, write_connectivity_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_offsets, s->psize_offsets,
                             write_offsets_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_types, s->psize_types,
                             write_types_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_mpirank, s->psize_mpirank,
                             write_mpirank_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_blockno, s->psize_blockno,
                             write_blockno_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_patchno, s->psize_patchno,
                             write_patchno_cb);
    fclaw2d_vtk_write_field (domain, s, s->offset_meqn, s->psize_meqn,
                             write_meqn_cb);

    /* collectively close the file */
    mpiret = MPI_File_close (&s->mpifile);
    SC_CHECK_MPI (mpiret);
}

static int
fclaw2d_vtk_write_footer (fclaw2d_domain_t * domain, fclaw2d_vtk_state_t * s)
{
    int retval;
    FILE *file;

    /* unconditionally open the file */
    file = fopen (s->filename, "ab");
    if (file == NULL)
    {
        return -1;
    }

    /* stop writing after first unsuccessful operation */
    retval = 0;
    retval = retval || fprintf (file, "\n </AppendedData>\n</VTKFile>\n") < 0;

    /* unconditionally close the file */
    retval = fclose (file) || retval;
    if (retval)
    {
        return -1;
    }
    return 0;
}

int
fclaw2d_vtk_write_file (fclaw2d_domain_t * domain, const char *basename,
                        int mx, int my, int meqn,
                        double vtkspace, int vtkwrite,
                        fclaw2d_vtk_patch_data_t coordinate_cb,
                        fclaw2d_vtk_patch_data_t value_cb)
{
    int retval, gretval;
    int mpiret;
    fclaw2d_vtk_state_t ps, *s = &ps;

    /* set up VTK internal information */
    s->mx = mx;
    s->my = my;
    s->meqn = meqn;
    s->points_per_patch = (mx + 1) * (my + 1);
    s->cells_per_patch = mx * my;
    snprintf (s->filename, BUFSIZ, "%s.vtu", basename);
    s->global_num_points = s->points_per_patch * domain->global_num_patches;
    s->global_num_cells = s->cells_per_patch * domain->global_num_patches;
    s->global_num_connectivity = 4 * s->global_num_cells;
    s->fits32 = s->global_num_points <= INT32_MAX
        && s->global_num_connectivity <= INT32_MAX;
    s->inttype = s->fits32 ? "Int32" : "Int64";
    s->intsize = s->fits32 ? sizeof (int32_t) : sizeof (int64_t);
    s->ndsize = 4;              /* TODO: use VTK header_type if necessary */
    s->coordinate_cb = coordinate_cb;
    s->value_cb = value_cb;

    /* compute data size per patch for the various VTK data arrays */
    s->psize_position = s->points_per_patch * 3 * sizeof (double);
    s->psize_connectivity = s->cells_per_patch * 4 * s->intsize;
    s->psize_offsets = s->cells_per_patch * s->intsize;
    s->psize_types = s->cells_per_patch * 1;
    s->psize_mpirank = s->cells_per_patch * 4;
    s->psize_blockno = s->cells_per_patch * 4;
    s->psize_patchno = s->cells_per_patch * s->intsize;
    s->psize_meqn = s->cells_per_patch * s->meqn * sizeof (float);

    /* compute offsets in bytes after beginning of appended data section */
    s->offset_position = 0;
    s->offset_connectivity = s->ndsize +
        s->offset_position + s->psize_position * domain->global_num_patches;
    s->offset_offsets = s->ndsize +
        s->offset_connectivity +
        s->psize_connectivity * domain->global_num_patches;
    s->offset_types = s->ndsize +
        s->offset_offsets + s->psize_offsets * domain->global_num_patches;
    s->offset_mpirank = s->ndsize +
        s->offset_types + s->psize_types * domain->global_num_patches;
    s->offset_blockno = s->ndsize +
        s->offset_mpirank + s->psize_mpirank * domain->global_num_patches;
    s->offset_patchno = s->ndsize +
        s->offset_blockno + s->psize_blockno * domain->global_num_patches;
    s->offset_meqn = s->ndsize +
        s->offset_patchno + s->psize_patchno * domain->global_num_patches;
    s->offset_end = s->ndsize +
        s->offset_meqn + s->psize_meqn * domain->global_num_patches;

    /* write header meta data and check for error */
    retval = 0;
    if (domain->mpirank == 0)
    {
        retval = fclaw2d_vtk_write_header (domain, s);
    }
    mpiret = MPI_Allreduce (&retval, &gretval, 1, MPI_INT, MPI_MIN,
                            domain->mpicomm);
    SC_CHECK_MPI (mpiret);
    if (gretval < 0)
    {
        return -1;
    }

    /* write mesh and numerical data using MPI I/O */
    fclaw2d_vtk_write_data (domain, s);

    /* write footer information and check for error */
    retval = 0;
    if (domain->mpirank == 0)
    {
        retval = fclaw2d_vtk_write_footer (domain, s);
    }
    mpiret = MPI_Allreduce (&retval, &gretval, 1, MPI_INT, MPI_MIN,
                            domain->mpicomm);
    SC_CHECK_MPI (mpiret);
    if (gretval < 0)
    {
        return -1;
    }

    return 0;
}