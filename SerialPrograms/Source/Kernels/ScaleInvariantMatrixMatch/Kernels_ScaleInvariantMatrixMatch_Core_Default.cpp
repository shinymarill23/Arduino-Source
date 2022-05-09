/*  Scale Invariant Matrix Match (Default)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <immintrin.h>
#include "Kernels/Kernels_x64_SSE41.h"
#include "Kernels_ScaleInvariantMatrixMatch_Routines.h"

namespace PokemonAutomation{
namespace Kernels{
namespace ScaleInvariantMatrixMatch{



struct SumATA2_Default{
    using vtype = float;

    float sum_AT = 0;
    float sum_A2 = 0;

    PA_FORCE_INLINE float scale() const{
        return sum_AT / sum_A2;
    }

    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* T){
        float sum_as0 = 0;
        float sum_as1 = 0;
        float sum_at0 = 0;
        float sum_at1 = 0;

        size_t lc = length / 2;
        while (lc--){
            float a0 = A[0];
            float a1 = A[1];
            sum_as0 += a0 * a0;
            sum_as1 += a1 * a1;
            sum_at0 += a0 * T[0];
            sum_at1 += a1 * T[1];
            A += 2;
            T += 2;
        }

        if (length % 2){
            float a0 = A[0];
            sum_as0 += a0 * a0;
            sum_at0 += a0 * T[0];
        }

        sum_A2 += sum_as0 + sum_as1;
        sum_AT += sum_at0 + sum_at1;
    }
    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* TW, const float* W){
        float sum_at0 = 0;
        float sum_at1 = 0;
        float sum_as0 = 0;
        float sum_as1 = 0;

        size_t lc = length / 2;
        while (lc--){
            float a0 = A[0] * W[0];
            float a1 = A[1] * W[1];
            sum_as0 += a0 * a0;
            sum_as1 += a1 * a1;
            sum_at0 += a0 * TW[0];
            sum_at1 += a1 * TW[1];
            A += 2;
            TW += 2;
            W += 2;
        }

        if (length % 2){
            float a0 = A[0] * W[0];
            sum_as0 += a0 * a0;
            sum_at0 += a0 * TW[0];
        }

        sum_A2 += sum_as0 + sum_as1;
        sum_AT += sum_at0 + sum_at1;
    }
};
struct SumError_Default{
    using vtype = float;

    float scale;
    float sum = 0;

    PA_FORCE_INLINE SumError_Default(float p_scale)
        : scale(p_scale)
    {}

    PA_FORCE_INLINE float sum_sqr() const{
        return sum;
    }

    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* T){
        float sum0 = 0;
        for (size_t c = 0; c < length; c++){
            float a0 = scale * A[c] - T[c];
            sum0 += a0 * a0;
        }
        sum += sum0;
    }
    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* TW, const float* W){
        float sum0 = 0;
        for (size_t c = 0; c < length; c++){
            float a0 = scale * A[c] * W[c] - TW[c];
            sum0 += a0 * a0;
        }
        sum += sum0;
    }
};



float compute_scale_Default(
    size_t width, size_t height,
    float const* const* A,
    float const* const* T
){
    return compute_scale<SumATA2_Default>(width, height, A, T);
}
float compute_scale_Default(
    size_t width, size_t height,
    float const* const* A,
    float const* const* TW,
    float const* const* W
){
    return compute_scale<SumATA2_Default>(width, height, A, TW, W);
}
float compute_error_Default(
    size_t width, size_t height,
    float scale,
    float const* const* A,
    float const* const* T
){
    return compute_error<SumError_Default>(width, height, scale, A, T);
}
float compute_error_Default(
    size_t width, size_t height,
    float scale,
    float const* const* A,
    float const* const* TW,
    float const* const* W
){
    return compute_error<SumError_Default>(width, height, scale, A, TW, W);
}






}
}
}
