/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2017 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include "simd.hpp"
#include "common.hpp"

#ifdef MANGO_SIMD_CONVERT_AVX

namespace mango {
namespace simd {

    // -----------------------------------------------------------------
    // reinterpret
    // -----------------------------------------------------------------

    static inline int32x4 int32x4_reinterpret(uint32x4 s)
    {
        return uint32x4::type(s);
    }

    static inline int32x4 int32x4_reinterpret(float32x4 s)
    {
        return _mm_castps_si128(s);
    }

    static inline uint32x4 uint32x4_reinterpret(int32x4 s)
    {
        return int32x4::type(s);
    }

    static inline uint32x4 uint32x4_reinterpret(float32x4 s)
    {
        return _mm_castps_si128(s);
    }

    static inline float32x4 float32x4_reinterpret(int32x4 s)
    {
        return _mm_castsi128_ps(s);
    }

    static inline float32x4 float32x4_reinterpret(uint32x4 s)
    {
        return _mm_castsi128_ps(s);
    }

    // -----------------------------------------------------------------
    // float32
    // -----------------------------------------------------------------

    static inline float32x4 float32x4_convert(int32x4 s)
    {
        return _mm_cvtepi32_ps(s);
    }

    static inline float32x4 float32x4_convert(uint32x4 s)
    {
        const __m128i mask = _mm_set1_epi32(0x0000ffff);
        const __m128i onep39 = _mm_set1_epi32(0x53000000);
        const __m128i x0 = _mm_or_si128(_mm_srli_epi32(s, 16), onep39);
        const __m128i x1 = _mm_and_si128(s, mask);
        const __m128 f1 = _mm_cvtepi32_ps(x1);
        const __m128 f0 = _mm_sub_ps(_mm_castsi128_ps(x0), _mm_castsi128_ps(onep39));
        return _mm_add_ps(f0, f1);
    }

    static inline int32x4 int32x4_convert(float32x4 s)
    {
        return _mm_cvtps_epi32(s);
    }

    static inline int32x4 int32x4_truncate(float32x4 s)
    {
        return _mm_cvttps_epi32(s);
    }

    // -----------------------------------------------------------------
    // float64
    // -----------------------------------------------------------------

    static inline float64x4 float64x4_convert(int32x4 s)
    {
        return _mm256_cvtepi32_pd(s);
    }

    static inline float64x4 float64x4_convert(float32x4 s)
    {
        return _mm256_cvtps_pd(s);
    }

    static inline int32x4 int32x4_convert(float64x4 s)
    {
        return _mm256_cvtpd_epi32(s);
    }

    static inline float32x4 float32x4_convert(float64x4 s)
    {
        return _mm256_cvtpd_ps(s);
    }

#if defined(MANGO_ENABLE_AVX2)

    static inline float64x4 float64x4_convert(uint32x4 ui)
    {
        const __m256d bias = _mm256_set1_pd((1ll << 52) * 1.5);
        const __m256i xyzw = _mm256_cvtepu32_epi64(ui);
        __m256d v = _mm256_castsi256_pd(xyzw);
        v = _mm256_or_pd(v, bias);
        v = _mm256_sub_pd(v, bias);
        return v;
    }

#else

    // clang workaround
    #define simd_mm256_set_m128i(hi, lo) _mm256_insertf128_si256(_mm256_castsi128_si256(lo), hi, 1)

    static inline float64x4 float64x4_convert(uint32x4 ui)
    {
        const __m256d bias = _mm256_set1_pd((1ll << 52) * 1.5);
        const __m128i mask = _mm_set1_epi32(0x43380000);
        const __m128i xy = _mm_unpacklo_epi32(ui, mask);
        const __m128i zw = _mm_unpackhi_epi32(ui, mask);
        const __m256i xyzw = simd_mm256_set_m128i(zw, xy);
        __m256d v = _mm256_castsi256_pd(xyzw);
        v = _mm256_sub_pd(v, bias);
        return v;
    }

    #undef simd_mm256_set_m128i

#endif

    static inline uint32x4 uint32x4_convert(float64x4 d)
    {
        const __m256d bias = _mm256_set1_pd((1ll << 52) * 1.5);
        const __m256d v = _mm256_add_pd(d, bias);
        const __m128d xxyy = _mm256_castpd256_pd128(v);
        const __m128d zzww = _mm256_extractf128_pd(v, 1);
        __m128 xyzw = _mm_shuffle_ps(_mm_castpd_ps(xxyy), _mm_castpd_ps(zzww), 0x88);
        return _mm_castps_si128(xyzw);
    }

    static inline int32x4 int32x4_truncate(float64x4 s)
    {
        return _mm256_cvttpd_epi32(s);
    }

    // -----------------------------------------------------------------
    // float16
    // -----------------------------------------------------------------

#ifdef MANGO_ENABLE_F16C

    static inline float32x4 float32x4_convert(float16x4 h)
    {
        const __m128i* p = reinterpret_cast<const __m128i *>(&h);
        return _mm_cvtph_ps(_mm_loadl_epi64(p));
    }

    static inline float16x4 float16x4_convert(float32x4 f)
    {
        float16x4 h;
        __m128i* p = reinterpret_cast<__m128i *>(&h);
        _mm_storel_epi64(p, _mm_cvtps_ph(f, 0));
        return h;
    }

#else

    static inline float32x4 float32x4_convert(float16x4 h)
    {
        const __m128i* p = reinterpret_cast<const __m128i *>(&h);
        const int32x4 u = _mm_unpacklo_epi16(_mm_loadl_epi64(p), _mm_setzero_si128());

        int32x4 no_sign  = int32x4_and(u, int32x4_set1(0x7fff));
        int32x4 sign     = int32x4_and(u, int32x4_set1(0x8000));
        int32x4 exponent = int32x4_and(u, int32x4_set1(0x7c00));
        int32x4 mantissa = int32x4_and(u, int32x4_set1(0x03ff));

        // NaN or Inf
        int32x4 a = int32x4_or(int32x4_set1(0x7f800000), int32x4_sll(mantissa, 13));

        // Zero or Denormal
        const int32x4 magic = int32x4_set1(0x3f000000);
        int32x4 b;
        b = int32x4_add(magic, mantissa);
        b = int32x4_reinterpret(float32x4_sub(float32x4_reinterpret(b), float32x4_reinterpret(magic)));

        // Numeric Value
        int32x4 c = int32x4_add(int32x4_set1(0x38000000), int32x4_sll(no_sign, 13));

        // Select a, b, or c based on exponent
        int32x4 mask;
        int32x4 result;

        mask = int32x4_compare_eq(exponent, int32x4_zero());
        result = int32x4_select(mask, b, c);

        mask = int32x4_compare_eq(exponent, int32x4_set1(0x7c00));
        result = int32x4_select(mask, a, result);

        // Sign
        result = int32x4_or(result, int32x4_sll(sign, 16));

        return float32x4_reinterpret(result);
    }

    static inline float16x4 float16x4_convert(float32x4 f)
    {
        const float32x4 magic = float32x4_set1(Float(0, 15, 0).f);
        const int32x4 vinf = int32x4_set1(31 << 23);

        const int32x4 u = int32x4_reinterpret(f);
        const int32x4 sign = int32x4_srl(int32x4_and(u, int32x4_set1(0x80000000)), 16);

        const int32x4 vexponent = int32x4_set1(0x7f800000);

        // Inf / NaN
        const int32x4 s0 = int32x4_compare_eq(int32x4_and(u, vexponent), vexponent);
        int32x4 mantissa = int32x4_and(u, int32x4_set1(0x007fffff));
        int32x4 x0 = int32x4_compare_eq(mantissa, int32x4_zero());
        mantissa = int32x4_select(x0, int32x4_zero(), int32x4_sra(mantissa, 13));
        const int32x4 v0 = int32x4_or(int32x4_set1(0x7c00), mantissa);

        int32x4 v1 = int32x4_and(u, int32x4_set1(0x7ffff000));
        v1 = int32x4_reinterpret(float32x4_mul(float32x4_reinterpret(v1), magic));
        v1 = int32x4_add(v1, int32x4_set1(0x1000));

#if defined(MANGO_ENABLE_SSE4_1)
        v1 = _mm_min_epi32(v1, vinf);
        v1 = int32x4_sra(v1, 13);

        int32x4 v = int32x4_select(s0, v0, v1);
        v = int32x4_or(v, sign);
        v = _mm_packus_epi32(v, v);
#else
        v1 = int32x4_select(int32x4_compare_gt(v1, vinf), vinf, v1);
        v1 = int32x4_sra(v1, 13);

        int32x4 v = int32x4_select(s0, v0, v1);
        v = int32x4_or(v, sign);
        v = _mm_slli_epi32 (v, 16);
        v = _mm_srai_epi32 (v, 16);
        v = _mm_packs_epi32 (v, v);
#endif

        float16x4 h;
        _mm_storel_epi64(reinterpret_cast<__m128i *>(&h), v);
        return h;
    }

#endif // MANGO_ENABLE_F16C

} // namespace simd
} // namespace mango

#endif // MANGO_SIMD_CONVERT_AVX