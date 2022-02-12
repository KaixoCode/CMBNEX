#include "pch.hpp"

#include <bitset>
namespace Kaixo
{
    template<class Ty>
    concept simd_type = std::floating_point<Ty> || std::integral<Ty>;

    template<simd_type Ty = float, size_t Bits = 256>
    struct SIMD;

    // 128: SSE & SSE2 & SSE4.1 & AVX & AVX2
    // 256: AVX & AVX2
    // 512: AVX512F & (AVX512F | KNCNI) & AVX512DQ

    // Choose SIMD path
    enum SIMDPATH { s0, s128, s256, s512 };
    static inline int ChoosePath()
    {
        alignas(4) union {
            int data[4]; 
            struct { int EAX, EBX, ECX, EDX; };
        } info, infoex;
        __cpuid(info.data, 1); // Normal features
        __cpuidex(infoex.data, 7, 0); // Extended features

#define feature(x, y) (x & (0x1ull << y))

        // Find relevant features
        bool _sse = feature(info.EDX, 25);
        bool _sse2 = feature(info.EDX, 26);
        bool _sse3 = feature(info.ECX, 0);
        bool _sse41 = feature(info.ECX, 19);
        bool _sse42 = feature(info.ECX, 20);
        bool _avx = feature(info.ECX, 28);
        bool _avx2 = feature(infoex.EBX, 5);
        bool _avx512f = feature(infoex.EBX, 16);
        bool _avx512dq = feature(infoex.EBX, 17);
        bool _avx512vl = feature(infoex.EBX, 31);

        if (_avx512f && _avx512dq)
            return SIMDPATH::s512;
        else if (_avx && _avx2)
            return SIMDPATH::s256;
        else if (_sse && _sse2 && _sse41 && _avx && _avx2)
            return SIMDPATH::s128;
        else
            return SIMDPATH::s0;
    }


    // SSE & SSE2 & SSE4.1
    template<>
    struct SIMD<int, 128>
    {
        constexpr static size_t BITS = 128;
        using Base = int;
        using Type = __m128i;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm_set1_epi32(UINT_MAX) : _mm_set1_epi32(0))
        {}

        inline SIMD(Base* v)
            : value(_mm_set_epi32(v[0], v[1], v[2], v[3]))
        {}

        inline SIMD(Base v)
            : value(_mm_set1_epi32(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;
        auto toFloat() { return _mm_cvtepi32_ps(value); }

        inline SIMD operator~() { return _mm_xor_si128(value, _mm_set1_epi32(UINT_MAX)); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm_add_epi32(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm_sub_epi32(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm_mullo_epi32(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm_div_epi32(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm_and_si128(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm_or_si128(a.value, b.value); }
        friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm_cmplt_epi32(a.value, b.value); }
        friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm_cmpgt_epi32(a.value, b.value); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm_min_epi32(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm_max_epi32(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    // AVX & AVX2 & AVX512F + AVX512VL
    template<>
    struct SIMD<int, 256>
    {
        constexpr static size_t BITS = 256;
        using Base = int;
        using Type = __m256i;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm256_set1_epi32(UINT_MAX) : _mm256_set1_epi32(0))
        {}

        inline SIMD(Base* v)
            : value(_mm256_set_epi32(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]))
        {}

        inline SIMD(Base v)
            : value(_mm256_set1_epi32(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;
        auto toFloat() { return _mm256_cvtepi32_ps(value); }

        inline SIMD operator~() { return _mm256_xor_si256(value, _mm256_set1_epi32(UINT_MAX)); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm256_add_epi32(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm256_sub_epi32(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm256_mullo_epi32(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm256_div_epi32(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm256_and_si256(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm256_or_si256(a.value, b.value); }
        //friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm256_cmplt_epi32_mask(a.value, b.value); }
        //friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm256_cmpgt_epi32_mask(a.value, b.value); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm256_min_epi32(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm256_max_epi32(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    // AVX512F
    template<>
    struct SIMD<int, 512>
    {
        constexpr static size_t BITS = 512;
        using Base = int;
        using Type = __m512i;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm512_set1_epi32(UINT_MAX) : _mm512_set1_epi32(0))
        {}

        inline SIMD(Base* v)
            : value(_mm512_loadu_epi32(v))
        {}

        inline SIMD(Base v)
            : value(_mm512_set1_epi32(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;
        auto toFloat() { return _mm512_cvtepi32_ps(value); }

        inline SIMD operator~() { return _mm512_xor_si512(value, _mm512_set1_epi32(UINT_MAX)); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm512_add_epi32(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm512_sub_epi32(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm512_mullo_epi32(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm512_div_epi32(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm512_and_si512(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm512_or_si512(a.value, b.value); }
        friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm512_cmplt_epi32_mask(a.value, b.value); }
        friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm512_cmpgt_epi32_mask(a.value, b.value); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm512_min_epi32(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm512_max_epi32(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    // SSE & SSE2 & SSE4.1 & AVX
    template<>
    struct SIMD<float, 128>
    {
        constexpr static size_t BITS = 128;
        constexpr static size_t COUNT = 4;
        using Base = float;
        using Type = __m128;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm_castsi128_ps(_mm_set1_epi32(UINT_MAX)) : _mm_set1_ps(0))
        {}

        inline SIMD(Base* v)
            : value(_mm_loadu_ps(v))
        {}

        inline SIMD(Base v)
            : value(_mm_set1_ps(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;

        inline void get(float(&data)[COUNT]) { return _mm_storeu_ps(data, value); }

        inline SIMD floor() const { return _mm_floor_ps(value); }
        inline SIMD log2() const { return _mm_log2_ps(value); }

        inline SIMD<int, BITS> toInt() const { return _mm_cvtps_epi32(value); }

        inline SIMD operator~() { return _mm_xor_ps(value, _mm_castsi128_ps(_mm_set1_epi32(UINT_MAX))); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm_add_ps(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm_sub_ps(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm_mul_ps(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm_div_ps(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm_and_ps(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm_or_ps(a.value, b.value); }
        friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm_cmp_ps(a.value, b.value, _CMP_LT_OS); }
        friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm_cmp_ps(a.value, b.value, _CMP_GT_OS); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm_min_ps(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm_max_ps(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    // AVX & AVX2
    template<>
    struct SIMD<float, 256>
    {
        constexpr static size_t BITS = 256;
        constexpr static size_t COUNT = 8;
        using Base = float;
        using Type = __m256;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm256_castsi256_ps(_mm256_set1_epi32(UINT_MAX)) : _mm256_set1_ps(0))
        {}

        inline SIMD(Base* v)
            : value(_mm256_loadu_ps(v))
        {}

        inline SIMD(Base v)
            : value(_mm256_set1_ps(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;

        inline void get(float(&data)[COUNT]) { return _mm256_storeu_ps(data, value); }

        inline SIMD floor() const { return _mm256_floor_ps(value); }
        inline SIMD log2() const { return _mm256_log2_ps(value); }

        inline SIMD<int, BITS> toInt() const { return _mm256_cvtps_epi32(value); }

        inline SIMD operator~() { return _mm256_xor_ps(value, _mm256_castsi256_ps(_mm256_set1_epi32(UINT_MAX))); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm256_add_ps(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm256_sub_ps(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm256_mul_ps(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm256_div_ps(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm256_and_ps(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm256_or_ps(a.value, b.value); }
        friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm256_cmp_ps(a.value, b.value, _CMP_LT_OS); }
        friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm256_cmp_ps(a.value, b.value, _CMP_GT_OS); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm256_min_ps(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm256_max_ps(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    // AVX512F & (AVX512F | KNCNI) & AVX512DQ
    template<>
    struct SIMD<float, 512>
    {
        constexpr static size_t BITS = 512;
        constexpr static size_t COUNT = 16;
        using Base = float;
        using Type = __m512;

        inline SIMD()
            : value{}
        {}

        explicit inline SIMD(bool v)
            : value(v ? _mm512_castsi512_ps(_mm512_set1_epi32(UINT_MAX)) : _mm512_set1_ps(0))
        {}

        inline SIMD(Base* v)
            : value(_mm512_loadu_ps(v))
        {}

        inline SIMD(Base v)
            : value(_mm512_set1_ps(v))
        {}

        inline SIMD(const Type& v)
            : value(v)
        {}

        Type value;

        inline void get(float(&data)[COUNT]) { return _mm512_storeu_ps(data, value); }

        inline SIMD floor() const { return _mm512_floor_ps(value); }
        inline SIMD log2() const { return _mm512_log2_ps(value); }

        inline SIMD<int, BITS> toInt() const { return _mm512_cvtps_epi32(value); }

        inline SIMD operator~() { return _mm512_xor_ps(value, _mm512_castsi512_ps(_mm512_set1_epi32(UINT_MAX))); }

        friend inline SIMD operator+(const SIMD& a, const SIMD& b) { return _mm512_add_ps(a.value, b.value); }
        friend inline SIMD operator-(const SIMD& a, const SIMD& b) { return _mm512_sub_ps(a.value, b.value); }
        friend inline SIMD operator*(const SIMD& a, const SIMD& b) { return _mm512_mul_ps(a.value, b.value); }
        friend inline SIMD operator/(const SIMD& a, const SIMD& b) { return _mm512_div_ps(a.value, b.value); }
        friend inline SIMD operator&(const SIMD& a, const SIMD& b) { return _mm512_and_ps(a.value, b.value); }
        friend inline SIMD operator|(const SIMD& a, const SIMD& b) { return _mm512_or_ps(a.value, b.value); }
        friend inline SIMD operator<(const SIMD& a, const SIMD& b) { return _mm512_cmplt_ps_mask(a.value, b.value); }
        friend inline SIMD operator>(const SIMD& a, const SIMD& b) { return _mm512_cmpnle_ps_mask(a.value, b.value); }

        friend inline SIMD min(const SIMD& a, const SIMD& b) { return _mm512_min_ps(a.value, b.value); }
        friend inline SIMD max(const SIMD& a, const SIMD& b) { return _mm512_max_ps(a.value, b.value); }
        friend inline SIMD minmax(const SIMD& a, const SIMD& b, const SIMD& c) { return min(max(a, b), c); }
    };

    template<size_t Bits>
    static inline SIMD<float, Bits> lookup(const SIMD<int, Bits>& index, auto& table)
    {
        if constexpr (Bits == 512) return _mm512_i32gather_ps(index.value, table.data(), sizeof(float));
        else if constexpr (Bits == 256) return _mm256_i32gather_ps(table.data(), index.value, sizeof(float));
        else if constexpr (Bits == 128) return _mm_i32gather_ps(table.data(), index.value, sizeof(float));
    }

    template<size_t Bits>
    static inline auto interpolate(SIMD<float, Bits>& num, SIMD<int, Bits>& a, SIMD<float, Bits>& r)
    {
        SIMD<float, Bits> _start = num.floor(); // Calculate index 1
        r = num - _start; // Calculate how far along it is to the next index
        a = _start.toInt();
    };

    template<size_t Bits>
    static inline auto simdmyfmod1(const SIMD<float, Bits>& num)
    {
        SIMD<float, Bits>  _start = num.floor();
        return num - _start;
    };

    template<size_t Bits>
    static inline auto shaperLookup(const SIMD<float, Bits>& x, const SIMD<float, Bits>& y,
        const SIMD<float, Bits>& z, const std::array<float, 4097 * 9 * 1025 + 1>& table)
    {
        SIMD<float, Bits> _x = (x + 1) * 2048;
        SIMD<int, Bits> _xr;
        SIMD<float, Bits> _xrem;
        interpolate(_x, _xr, _xrem);

        SIMD<float, Bits> _y = y * 8;
        SIMD<int, Bits> _yr;
        SIMD<float, Bits> _yrem;
        interpolate(_y, _yr, _yrem);

        SIMD<int, Bits> _z = minmax(z * (1024 / 3.f), 0.f, 1024.f).toInt();

        // Calculate indices to interpolate
        // Get data at all indices, and interpolate xaxis using xrem
        SIMD<int, Bits> _zi = _z * ((4096 + 1) * (8 + 1));
        SIMD<int, Bits> _yi1 = _yr * (4096 + 1) + _zi;
        SIMD<int, Bits> _yi2 = (_yr + 1) * (4096 + 1) + _zi;
        SIMD<float, Bits> _xa11 = lookup(_xr + _yi1, table) * (1 - _xrem)
            + lookup(_xr + 1 + _yi1, table) * _xrem;
        SIMD<float, Bits> _xa12 = lookup(_xr + _yi2, table) * (1 - _xrem)
            + lookup(_xr + 1 + _yi2, table) * _xrem;

        return _xa11 * (1 - _yrem) + _xa12 * _yrem;
    };

    template<size_t Bits>
    static inline auto wavetableLookup(const SIMD<float, Bits>& x, const SIMD<float, Bits>& y,
        const SIMD<float, Bits>& z, const std::array<float, 2049 * 4 * 33 + 1>& table)
    {
        SIMD<float, Bits> _x = x * 2048;
        SIMD<int, Bits> _xr;
        SIMD<float, Bits> _xrem;
        interpolate(_x, _xr, _xrem);

        SIMD<float, Bits> _y = y * 3;
        SIMD<int, Bits> _yr;
        SIMD<float, Bits> _yrem;
        interpolate(_y, _yr, _yrem);

        SIMD<int, Bits> _z = z.toInt();

        // Calculate indices to interpolate
        // Get data at all indices, and interpolate xaxis using xrem
        SIMD<int, Bits> _zi = _z * ((2048 + 1) * (3 + 1));
        SIMD<int, Bits> _yi1 = _yr * (2048 + 1) + _zi;
        SIMD<int, Bits> _yi2 = (_yr + 1) * (2048 + 1) + _zi;
        SIMD<float, Bits> _xa11 = lookup(_xr + _yi1, table) * (1 - _xrem)
            + lookup(_xr + 1 + _yi1, table) * _xrem;
        SIMD<float, Bits> _xa12 = lookup(_xr + _yi2, table) * (1 - _xrem)
            + lookup(_xr + 1 + _yi2, table) * _xrem;
        return _xa11 * (1 - _yrem) + _xa12 * _yrem;
    };

    template<size_t Bits>
    static inline auto driveLookup(const SIMD<float, Bits>& x, const SIMD<float, Bits>& y,
        const std::array<float, 100001 * 2 + 1>& table)
    {
        SIMD<int, Bits> _x = ((x + 10) * 5000).toInt();

        SIMD<float, Bits>  _y = y;
        SIMD<int, Bits> _yr;
        SIMD<float, Bits>  _yrem;
        interpolate(_y, _yr, _yrem);

        // Calculate indices to interpolate
        // Get data at all indices, and interpolate xaxis using xrem
        SIMD<int, Bits> _yi1 = _yr * (100000 + 1);
        SIMD<int, Bits> _yi2 = (_yr + 1) * (100000 + 1);
        return lookup(_x + _yi1, table) * (1 - _yrem)
            + lookup(_x + _yi2, table) * _yrem;
    };

    template<size_t Bits>
    static inline auto powerCurveLookup(const SIMD<float, Bits>& x, const SIMD<float, Bits>& y,
        const std::array<float, 1001 * 1001 + 1>& table)
    {
        SIMD<float, Bits>  _x = x * 1000;
        SIMD<int, Bits> _xr;
        SIMD<float, Bits>  _xrem;
        interpolate(_x, _xr, _xrem);

        SIMD<int, Bits> _y = (y * 500.f + 500.f).toInt() * (1000 + 1);

        // Calculate indices to interpolate
        // Get data at all indices, and interpolate xaxis using xrem
        return lookup(_xr + _y, table) * (1 - _xrem)
            + lookup(_xr + 1 + _y, table) * _xrem;
    };

    template<size_t Bits>
    static inline auto foldLookup(const SIMD<float, Bits>& x, const std::array<float, 100001 + 1>& table)
    {
        SIMD<int, Bits> _x = ((x + 5) * 10000).toInt();
        return lookup(_x, table);
    };
}