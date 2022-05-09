/*  Scale Invariant Matrix Match (x86 AVX512)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifdef PA_AutoDispatch_17_Skylake

#include <immintrin.h>
#include "Kernels_ScaleInvariantMatrixMatch_Routines.h"

namespace PokemonAutomation{
namespace Kernels{
namespace ScaleInvariantMatrixMatch{



struct SumATA2_min16_x86_AVX512{
    using vtype = __m512;

    __m512 sum_AT = _mm512_setzero_ps();
    __m512 sum_A2 = _mm512_setzero_ps();

    PA_FORCE_INLINE float scale() const{
        return _mm512_reduce_add_ps(sum_AT) / _mm512_reduce_add_ps(sum_A2);
    }

    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* T){
        __m512 sum_as0 = _mm512_setzero_ps();
        __m512 sum_as1 = _mm512_setzero_ps();
        __m512 sum_as2 = _mm512_setzero_ps();
        __m512 sum_as3 = _mm512_setzero_ps();
        __m512 sum_at0 = _mm512_setzero_ps();
        __m512 sum_at1 = _mm512_setzero_ps();
        __m512 sum_at2 = _mm512_setzero_ps();
        __m512 sum_at3 = _mm512_setzero_ps();

        size_t align = (size_t)T % 64;
        if (align){
            align /= sizeof(float);
            A -= align;
            T -= align;

            __mmask16 mask = ~(((uint16_t)1 << align) - 1);

            __m512 a0 = _mm512_maskz_load_ps(mask, A);
            __m512 t0 = _mm512_maskz_load_ps(mask, T);
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
            sum_at0 = _mm512_fmadd_ps(a0, t0, sum_at0);

            A += 16;
            T += 16;
            length -= 16 - align;
        }

        const __m512* ptrA = (const __m512*)A;
        const __m512* ptrT = (const __m512*)T;

        size_t lc = length / 64;
        if (lc){
            do{
                __m512 a0 = ptrA[0];
                __m512 a1 = ptrA[1];
                __m512 a2 = ptrA[2];
                __m512 a3 = ptrA[3];
                sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
                sum_as1 = _mm512_fmadd_ps(a1, a1, sum_as1);
                sum_as2 = _mm512_fmadd_ps(a2, a2, sum_as2);
                sum_as3 = _mm512_fmadd_ps(a3, a3, sum_as3);
                sum_at0 = _mm512_fmadd_ps(a0, ptrT[0], sum_at0);
                sum_at1 = _mm512_fmadd_ps(a1, ptrT[1], sum_at1);
                sum_at2 = _mm512_fmadd_ps(a2, ptrT[2], sum_at2);
                sum_at3 = _mm512_fmadd_ps(a3, ptrT[3], sum_at3);
                ptrA += 4;
                ptrT += 4;
            }while (--lc);
            sum_as0 = _mm512_add_ps(sum_as0, sum_as1);
            sum_at0 = _mm512_add_ps(sum_at0, sum_at1);
            sum_as2 = _mm512_add_ps(sum_as2, sum_as3);
            sum_at2 = _mm512_add_ps(sum_at2, sum_at3);
            sum_as0 = _mm512_add_ps(sum_as0, sum_as2);
            sum_at0 = _mm512_add_ps(sum_at0, sum_at2);
        }

        length %= 64;
        while (length >= 16){
            __m512 a0 = ptrA[0];
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
            sum_at0 = _mm512_fmadd_ps(a0, ptrT[0], sum_at0);
            ptrA += 1;
            ptrT += 1;
            length -= 16;
        }
        if (length){
            __mmask16 mask = ((uint16_t)1 << length) - 1;
            __m512 t0 = _mm512_maskz_load_ps(mask, ptrT);
            __m512 a0 = _mm512_maskz_load_ps(mask, ptrA);
            sum_at0 = _mm512_fmadd_ps(a0, t0, sum_at0);
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
        }

        sum_A2 = _mm512_add_ps(sum_A2, sum_as0);
        sum_AT = _mm512_add_ps(sum_AT, sum_at0);
    }
    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* TW, const float* W){
        __m512 sum_as0 = _mm512_setzero_ps();
        __m512 sum_as1 = _mm512_setzero_ps();
        __m512 sum_as2 = _mm512_setzero_ps();
        __m512 sum_as3 = _mm512_setzero_ps();
        __m512 sum_at0 = _mm512_setzero_ps();
        __m512 sum_at1 = _mm512_setzero_ps();
        __m512 sum_at2 = _mm512_setzero_ps();
        __m512 sum_at3 = _mm512_setzero_ps();

        size_t align = (size_t)TW % 64;
        if (align){
            align /= sizeof(float);
            A -= align;
            TW -= align;
            W -= align;

            __mmask16 mask = ~(((uint16_t)1 << align) - 1);

            __m512 a0 = _mm512_maskz_load_ps(mask, A);
            __m512 t0 = _mm512_maskz_load_ps(mask, TW);
            __m512 w0 = _mm512_maskz_load_ps(mask, W);
            a0 = _mm512_mul_ps(a0, w0);
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
            sum_at0 = _mm512_fmadd_ps(a0, t0, sum_at0);

            A += 16;
            TW += 16;
            W += 16;
            length -= 16 - align;
        }

        const __m512* ptrA = (const __m512*)A;
        const __m512* ptrT = (const __m512*)TW;
        const __m512* ptrW = (const __m512*)W;

        size_t lc = length / 64;
        if (lc){
            do{
                __m512 a0 = ptrA[0];
                __m512 a1 = ptrA[1];
                __m512 a2 = ptrA[2];
                __m512 a3 = ptrA[3];
                a0 = _mm512_mul_ps(a0, ptrW[0]);
                a1 = _mm512_mul_ps(a1, ptrW[1]);
                a2 = _mm512_mul_ps(a2, ptrW[2]);
                a3 = _mm512_mul_ps(a3, ptrW[3]);
                sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
                sum_as1 = _mm512_fmadd_ps(a1, a1, sum_as1);
                sum_as2 = _mm512_fmadd_ps(a2, a2, sum_as2);
                sum_as3 = _mm512_fmadd_ps(a3, a3, sum_as3);
                sum_at0 = _mm512_fmadd_ps(a0, ptrT[0], sum_at0);
                sum_at1 = _mm512_fmadd_ps(a1, ptrT[1], sum_at1);
                sum_at2 = _mm512_fmadd_ps(a2, ptrT[2], sum_at2);
                sum_at3 = _mm512_fmadd_ps(a3, ptrT[3], sum_at3);
                ptrA += 4;
                ptrT += 4;
                ptrW += 4;
            }while (--lc);
            sum_as0 = _mm512_add_ps(sum_as0, sum_as1);
            sum_at0 = _mm512_add_ps(sum_at0, sum_at1);
            sum_as2 = _mm512_add_ps(sum_as2, sum_as3);
            sum_at2 = _mm512_add_ps(sum_at2, sum_at3);
            sum_as0 = _mm512_add_ps(sum_as0, sum_as2);
            sum_at0 = _mm512_add_ps(sum_at0, sum_at2);
        }

        length %= 64;
        while (length >= 16){
            __m512 a0 = ptrA[0];
            a0 = _mm512_mul_ps(a0, ptrW[0]);
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
            sum_at0 = _mm512_fmadd_ps(a0, ptrT[0], sum_at0);
            ptrA += 1;
            ptrT += 1;
            ptrW += 1;
            length -= 16;
        }
        if (length){
            __mmask16 mask = ((uint16_t)1 << length) - 1;
            __m512 a0 = _mm512_maskz_load_ps(mask, ptrA);
            __m512 t0 = _mm512_maskz_load_ps(mask, ptrT);
            __m512 w0 = _mm512_maskz_load_ps(mask, ptrW);
            a0 = _mm512_mul_ps(a0, w0);
            sum_as0 = _mm512_fmadd_ps(a0, a0, sum_as0);
            sum_at0 = _mm512_fmadd_ps(a0, t0, sum_at0);
        }

        sum_A2 = _mm512_add_ps(sum_A2, sum_as0);
        sum_AT = _mm512_add_ps(sum_AT, sum_at0);
    }
};
struct SumError_min16_x86_AVX512{
    using vtype = __m512;

    __m512 scale;
    __m512 sum = _mm512_setzero_ps();

    PA_FORCE_INLINE SumError_min16_x86_AVX512(float p_scale)
        : scale(_mm512_set1_ps(p_scale))
    {}

    PA_FORCE_INLINE float sum_sqr() const{
        return _mm512_reduce_add_ps(sum);
    }

    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* T){
        __m512 sum0 = _mm512_setzero_ps();
        __m512 sum1 = _mm512_setzero_ps();
        __m512 sum2 = _mm512_setzero_ps();
        __m512 sum3 = _mm512_setzero_ps();

        size_t align = (size_t)T % 64;
        if (align){
            align /= sizeof(float);
            A -= align;
            T -= align;

            __mmask16 mask = ~(((uint16_t)1 << align) - 1);

            __m512 a0 = _mm512_maskz_load_ps(mask, A);
            __m512 t0 = _mm512_maskz_load_ps(mask, T);
            a0 = _mm512_fmsub_ps(scale, a0, t0);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);

            A += 16;
            T += 16;
            length -= 16 - align;
        }

        const __m512* ptrA = (const __m512*)A;
        const __m512* ptrT = (const __m512*)T;

        size_t lc = length / 64;
        if (lc){
            do{
                __m512 a0 = _mm512_fmsub_ps(scale, ptrA[0], ptrT[0]);
                __m512 a1 = _mm512_fmsub_ps(scale, ptrA[1], ptrT[1]);
                __m512 a2 = _mm512_fmsub_ps(scale, ptrA[2], ptrT[2]);
                __m512 a3 = _mm512_fmsub_ps(scale, ptrA[3], ptrT[3]);
                sum0 = _mm512_fmadd_ps(a0, a0, sum0);
                sum1 = _mm512_fmadd_ps(a1, a1, sum1);
                sum2 = _mm512_fmadd_ps(a2, a2, sum2);
                sum3 = _mm512_fmadd_ps(a3, a3, sum3);
                ptrA += 4;
                ptrT += 4;
            }while (--lc);
            sum0 = _mm512_add_ps(sum0, sum1);
            sum2 = _mm512_add_ps(sum2, sum3);
            sum0 = _mm512_add_ps(sum0, sum2);
        }

        length %= 64;
        while (length >= 16){
            __m512 a0 = _mm512_fmsub_ps(scale, ptrA[0], ptrT[0]);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);
            ptrA += 1;
            ptrT += 1;
            length -= 16;
        }
        if (length){
            __mmask16 mask = ((uint16_t)1 << length) - 1;
            __m512 t0 = _mm512_maskz_load_ps(mask, ptrT);
            __m512 a0 = _mm512_maskz_load_ps(mask, ptrA);
            a0 = _mm512_fmsub_ps(scale, a0, t0);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);
        }

        sum = _mm512_add_ps(sum, sum0);
    }
    PA_FORCE_INLINE void accumulate(size_t length, const float* A, const float* TW, const float* W){
        __m512 sum0 = _mm512_setzero_ps();
        __m512 sum1 = _mm512_setzero_ps();
        __m512 sum2 = _mm512_setzero_ps();
        __m512 sum3 = _mm512_setzero_ps();

        size_t align = (size_t)TW % 64;
        if (align){
            align /= sizeof(float);
            A -= align;
            TW -= align;
            W -= align;

            __mmask16 mask = ~(((uint16_t)1 << align) - 1);

            __m512 a0 = _mm512_maskz_load_ps(mask, A);
            __m512 t0 = _mm512_maskz_load_ps(mask, TW);
            __m512 w0 = _mm512_maskz_load_ps(mask, W);
            a0 = _mm512_mul_ps(scale, a0);
            a0 = _mm512_fmsub_ps(a0, w0, t0);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);

            A += 16;
            TW += 16;
            W += 16;
            length -= 16 - align;
        }

        const __m512* ptrA = (const __m512*)A;
        const __m512* ptrT = (const __m512*)TW;
        const __m512* ptrW = (const __m512*)W;

        size_t lc = length / 64;
        if (lc){
            do{
                __m512 a0 = _mm512_mul_ps(scale, ptrA[0]);
                __m512 a1 = _mm512_mul_ps(scale, ptrA[1]);
                __m512 a2 = _mm512_mul_ps(scale, ptrA[2]);
                __m512 a3 = _mm512_mul_ps(scale, ptrA[3]);
                a0 = _mm512_fmsub_ps(a0, ptrW[0], ptrT[0]);
                a1 = _mm512_fmsub_ps(a1, ptrW[1], ptrT[1]);
                a2 = _mm512_fmsub_ps(a2, ptrW[2], ptrT[2]);
                a3 = _mm512_fmsub_ps(a3, ptrW[3], ptrT[3]);
                sum0 = _mm512_fmadd_ps(a0, a0, sum0);
                sum1 = _mm512_fmadd_ps(a1, a1, sum1);
                sum2 = _mm512_fmadd_ps(a2, a2, sum2);
                sum3 = _mm512_fmadd_ps(a3, a3, sum3);
                ptrA += 4;
                ptrT += 4;
                ptrW += 4;
            }while (--lc);
            sum0 = _mm512_add_ps(sum0, sum1);
            sum2 = _mm512_add_ps(sum2, sum3);
            sum0 = _mm512_add_ps(sum0, sum2);
        }

        length %= 64;
        while (length >= 16){
            __m512 a0 = _mm512_mul_ps(scale, ptrA[0]);
            a0 = _mm512_fmsub_ps(a0, ptrW[0], ptrT[0]);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);
            ptrA += 1;
            ptrT += 1;
            ptrW += 1;
            length -= 16;
        }
        if (length){
            __mmask16 mask = ((uint16_t)1 << length) - 1;
            __m512 a0 = _mm512_maskz_load_ps(mask, ptrA);
            __m512 t0 = _mm512_maskz_load_ps(mask, ptrT);
            __m512 w0 = _mm512_maskz_load_ps(mask, ptrW);
            a0 = _mm512_mul_ps(scale, a0);
            a0 = _mm512_fmsub_ps(a0, w0, t0);
            sum0 = _mm512_fmadd_ps(a0, a0, sum0);
        }

        sum = _mm512_add_ps(sum, sum0);
    }
};


float compute_scale_min16_x86_AVX512(
    size_t width, size_t height,
    float const* const* A,
    float const* const* T
){
    return compute_scale<SumATA2_min16_x86_AVX512>(width, height, A, T);
}
float compute_scale_min16_x86_AVX512(
    size_t width, size_t height,
    float const* const* A,
    float const* const* TW,
    float const* const* W
){
    return compute_scale<SumATA2_min16_x86_AVX512>(width, height, A, TW, W);
}
float compute_error_min16_x86_AVX512(
    size_t width, size_t height,
    float scale,
    float const* const* A,
    float const* const* T
){
    return compute_error<SumError_min16_x86_AVX512>(width, height, scale, A, T);
}
float compute_error_min16_x86_AVX512(
    size_t width, size_t height,
    float scale,
    float const* const* A,
    float const* const* TW,
    float const* const* W
){
    return compute_error<SumError_min16_x86_AVX512>(width, height, scale, A, TW, W);
}



}
}
}
#endif
