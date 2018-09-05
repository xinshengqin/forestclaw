__global__ void cudaclaw5_step2_wrap(int maxm, int meqn, int maux, int mbc,
                                     int method[], int mthlim[], int mcapa, 
                                     int mwaves, int mx, int my, 
                                     double qold[],
                                     double aux[], 
                                     double dx, double dy, double dt,
                                     double cfl, double xlower, double ylower, 
                                     int level, double t, 
                                     cudaclaw5_rpn2_t rpn2,
                                     cudaclaw5_rpt2_t rpt2, 
                                     cudaclaw5_flux2_t flux2,
                                     int block_corner_count[], int ierror)
{
    double dtdx, dtdy
    int i,j,m

    /* Needed by Riemann solvers.  This should be fixed later by 
       a 'context' for a Riemann solver. */

    ierror = 0;


    /* GPU call : Distribute grid to blocks */
    int m = (mx+3);
    int n = (my+3); 
    dim3 block(32,32);  
    dim3 grid((m+block.x-1)/block.x,(n+block.y-1)/block.y);

    cudaclaw5_step2_GPU<<grid,block>>(mx,my,meqn,maux,mbc,
                                      qold_dev,aux_dev,
                                      dx,dy,dt,cfl,
                                      fm_dev,fp_dev,gm_dev,gp_dev,
                                      cuda_rpn2,rpt2,ierror);

    /* update q */
    dtdx = dt/dx
    dtdy = dt/dy
    for(m = 0; m < meqn; m++)
    {
        for(i = 0; i < mx; i++)
        {            
            for(j = 0; j < my; j++)
            {
               if (mcapa == 0)
               {
                  /* no capa array.  Standard flux differencing: */
                  qold[m][i][j] = qold[m][i][j]
                        - dtdx * (fm[m][i+1][j] - fp[m][i][j])
                        - dtdy * (gm[m][i][j+1] - gp[m][i][j]);
                }
                else if 
                {
                    /* with capa array. */
                    qold[m][i][j] = qold[m][i][j]
                                -(dtdx*(fm[m][i+1][j] - fp[m][i][j])
                                + dtdy*(gm[m][i][j+1] - gp[m][i][j]))/aux[mcapa][i][j];
               }
           }
        }
    }
}