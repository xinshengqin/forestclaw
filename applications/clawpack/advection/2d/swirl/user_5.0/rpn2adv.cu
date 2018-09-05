__device__ void rpn2adv_cuda(int iface, int meqn,int mwaves, int maux,
     double ql[], double qr[], double auxl[], double auxr[],
     double wave[],double s[], double amdq[], double apdq[])
{
    int iface;

    wave[0][0] = qr[0] - ql[0];
    s[0] = auxr[iface];
    amdq[0] = min(auxr[iface], 0) * wave[1][1];
    apdq[0] = max(auxr[iface], 0) * wave[1][1];
}