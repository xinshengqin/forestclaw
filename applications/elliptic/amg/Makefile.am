## Makefile.am in applications/elliptic/amg

bin_PROGRAMS += applications/elliptic/amg/amg

applications_elliptic_amg_amg_SOURCES = \
	applications/elliptic/amg/amg_user.cpp \
	applications/elliptic/amg/amg_user.h \
	applications/elliptic/amg/amg.cpp \
	applications/elliptic/amg/fclaw2d_map_nomap.c \
	applications/elliptic/amg/amg_patch_fill.f \
	applications/elliptic/amg/amg_compute_residual.f

applications_elliptic_amg_amg_CPPFLAGS = $(AM_CPPFLAGS) \
	$(FCLAW_CLAWPACK46_CPPFLAGS) 

applications_elliptic_amg_amg_LDADD = \
        $(FCLAW_CLAWPACK46_LDADD) $(LDADD)


## @top_builddir@/src/solvers/fclaw2d_clawpack46/libclawpack5.la
