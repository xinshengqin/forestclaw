__global__  void cudaclaw5_step2_GPU(int mx,int my,int meqn,int maux,int mbc,
                                     double *qold, double *aux, double dx,
                                     double dy,double dt, double cflgrid,
                                     double fm[], double fp[], 
                                     double gm[],double gp[], 
                                     cudaclaw5_rpn2_t cuda_rpn2,
                                     cudaclaw5_rpt2_t cuda_rpt2, 
                                     cudaclaw5_flux2_t cuda_flux2)
{

    int i,j
    double precision dtdx,dtdy,cfl1d

    cflgrid = 0.0;
    dtdx = dt/dx;
    dtdy = dt/dy;

#if 0   
    /* Allocate on GPU; Set to zero */
    fm = 0.d0
    fp = 0.d0
    gm = 0.d0
    gp = 0.d0
#endif    


}
