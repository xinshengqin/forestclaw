#ifndef FC2D_CUDA_PROFILER_H
#define FC2D_CUDA_PROFILER_H

#include <stdint.h>
#include "nvToolsExt.h"

extern const uint32_t colors[]; 
extern const int num_colors;

#ifdef __cplusplus
    class CudaTracer {
        public:
            CudaTracer(const char* name, int cid = 0);
            ~CudaTracer();
    };
    
    #ifdef FCLAW_ENABLE_DEBUG
    #define PROFILE_CUDA(fname) CudaTracer uniq_name_using_macros__(fname);
    #define PROFILE_CUDA_GROUP(fname, groupid) CudaTracer uniq_name_using_macros__(fname, groupid);
    #else
    #define PROFILE_CUDA(fname) ;
    #define PROFILE_CUDA_GROUP(fname, groupid) ;
    #endif // end FLCAW_ENABLE_DEBUG

#else // no c++

    #ifdef FCLAW_ENABLE_DEBUG
    #define PROFILE_CUDA_START(name,cid) { \
            int color_id = cid; \
            color_id = color_id%num_colors;\
            nvtxEventAttributes_t eventAttrib = {0}; \
            eventAttrib.version = NVTX_VERSION; \
            eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE; \
            eventAttrib.colorType = NVTX_COLOR_ARGB; \
            eventAttrib.color = colors[color_id]; \
            eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; \
            eventAttrib.message.ascii = name; \
            nvtxRangePushEx(&eventAttrib); \
    }
    #define PROFILE_CUDA_STOP nvtxRangePop();
    #else
    #define PROFILE_CUDA_START(name,cid) ;
    #define PROFILE_CUDA_STOP ;
    #endif // FLCAW_ENABLE_DEBUG

#endif // __cplusplus


#endif
