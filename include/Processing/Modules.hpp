#pragma once
#include "pch.hpp"
#include "Processing/Filter.hpp"

namespace Kaixo
{
    // Settings for a lookup table axis.
    struct TableAxis 
    { 
        int size; // Size of the axis
        double begin; // Start of the range of values for this axis
        double end;   // End of the range of values for this axis
        bool constrained = false; // Constrain value when looking up
        bool interpolate = false; // Interpolate result from lookup for smoother curve
    };

    // Simple lookup table, using a 1d array of doubles.
    template<TableAxis...>
    struct LookupTable;

    // 1 Dimensional lookup
    template<TableAxis Size>
    struct LookupTable<Size>
    {
        bool done = false;
        constexpr LookupTable(auto&& fun)
        {
            std::thread{ [&] {
                // Fill the table using the provided function.
                for (int i = 0; i < Size.size + 1; i++)
                    table[i] = fun((i / (double)Size.size) * (Size.end - Size.begin) + Size.begin);
                done = true;
            } }.detach();
        }

        constexpr double get(double val) const
        {   // Constrain value if necessary
            if constexpr (Size.constrained) val = constrain(val, Size.begin, Size.end);
            if constexpr (Size.interpolate) // If we need to interpolate
            {   // Get prefered value
                double v = Size.size * (val - Size.begin) / (Size.end - Size.begin);
                int a = (int)(v); // Round value using integer casting
                double r = v - a; // Get ratio towards next integer index
                return (1 - r) * table[a] + r * table[a + 1]; // Interpolate both indices using ratio
            }
            else // No interpolating, we cast to integer.
                return table[(int)(Size.size * (val - Size.begin) / (Size.end - Size.begin))];
        }

        std::array<float, Size.size + 1 + 1> table;
    };

    template<TableAxis Width, TableAxis Height>
    struct LookupTable<Width, Height>
    {
        bool done = false;
        constexpr LookupTable(auto&& fun)
        {
            std::thread{ [&] {
                // Fill table
                for (int i = 0; i < Width.size + 1; i++)
                    for (int j = 0; j < Height.size + 1; j++)
                        table[i + j * (Width.size + 1)] = // Convert 2d index to 1d index, and calculate correct argument
                        fun((i / (double)Width.size) * (Width.end - Width.begin) + Width.begin, // values using the range
                            (j / (double)Height.size) * (Height.end - Height.begin) + Height.begin); // provided in the TableAxis
                done = true;
            } }.detach();
        }

        constexpr double get(double x, double y) const
        {   // Constrain axis if necessary
            if constexpr (Width.constrained) x = constrain(x, Width.begin, Width.end);
            if constexpr (Height.constrained) y = constrain(y, Height.begin, Height.end);
            if constexpr (Width.interpolate && Height.interpolate) // If both interpolate
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                const float _xa1 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _yr] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * _yr];
                const float _xa2 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1)] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * (_yr + 1)];
                return _xa1 * (1 - _yrem) + _xa2 * _yrem;
            }      
            else if constexpr (Width.interpolate)
            {   // Single axis interpolation, works the same as in 1d lookup table
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                return (1 - _xrem) * table[_xr + (Width.size + 1) * _y] + 
                    _xrem * table[_xr + 1 + (Width.size + 1) * _y];
            }
            else if constexpr (Height.interpolate)
            {   // Single axis interpolation again.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr;
                return (1 - _yrem) * table[_x + (Width.size + 1) * _yr] + 
                    _yrem * table[_x + (Width.size + 1) * (_yr + 1)];
            }
            else
            {   // No interpolation, calculate 2d indices first, then convert to 1d index.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                return table[_x + (Width.size + 1) * _y];
            }
        }

        std::array<float, (Width.size + 1) * (Height.size + 1) + 1> table;
    };    
    
    template<TableAxis Width, TableAxis Height, TableAxis Depth>
    struct LookupTable<Width, Height, Depth>
    {
        bool done = false;
        constexpr LookupTable(auto&& fun)
        {
            std::thread{ [&] {
                // Fill table
                for (int i = 0; i < Width.size + 1; i++)
                    for (int j = 0; j < Height.size + 1; j++)
                        for (int k = 0; k < Depth.size + 1; k++)
                            table[i + j * (Width.size + 1) + k * (Width.size + 1) * (Height.size + 1)] = // Convert 3d index to 1d index, and calculate correct argument
                            fun((i / (double)Width.size) * (Width.end - Width.begin) + Width.begin, // values using the range
                                (j / (double)Height.size) * (Height.end - Height.begin) + Height.begin, // provided in the TableAxis
                                (k / (double)Depth.size) * (Depth.end - Depth.begin) + Depth.begin);
                done = true;
            } }.detach();
        }

        constexpr double get(double x, double y, double z) const
        {   // Constrain axis if necessary
            if constexpr (Width.constrained) x = constrain(x, Width.begin, Width.end);
            if constexpr (Height.constrained) y = constrain(y, Height.begin, Height.end);
            if constexpr (Depth.constrained) z = constrain(z, Depth.begin, Depth.end);
            if constexpr (Width.interpolate && Height.interpolate && Depth.interpolate) // If both interpolate
            {   
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; 
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                const float _xa11 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * _zr] +
                                         _xrem  * table[_xr + 1 + (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * _zr];
                const float _xa12 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr] +
                                         _xrem  * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _ya1 = _xa11 * (1 - _yrem) + _xa12 * _yrem;
                const float _xa21 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                         _xrem  * table[_xr + 1 + (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _xa22 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                         _xrem  * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _ya2 = _xa21 * (1 - _yrem) + _xa22 * _yrem;
                return _ya1 * (1 - _zrem) + _ya2 * _zrem;
            }  
            else if constexpr (Width.interpolate && Height.interpolate) // If both interpolate
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const float _xa1 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _z] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _z];
                const float _xa2 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _z] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _z];
                const float _ya = _xa1 * (1 - _yrem) + _xa2 * _yrem;
                return _ya;
            }            
            else if constexpr (Width.interpolate && Depth.interpolate) // If both interpolate
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                const float _xa1 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _zr] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _xa2 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                        _xrem  * table[_xr + 1 + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _za = _xa1 * (1 - _zrem) + _xa2 * _zrem;
                return _za;
            }            
            else if constexpr (Height.interpolate && Depth.interpolate) // If both interpolate
            {   // Calculate index and ratios for both indices
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                const float _ya1 = (1 - _yrem) * table[_x + (Width.size + 1) *  _yr +      (Width.size + 1) * (Height.size + 1) * _zr] +
                                        _yrem  * table[_x + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _ya2 = (1 - _yrem) * table[_x + (Width.size + 1) *  _yr +      (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                        _yrem  * table[_x + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _za = _ya1 * (1 - _zrem) + _ya2 * _zrem;
                return _za;
            }
            else if constexpr (Width.interpolate)
            {   // Single axis interpolation, works the same as in 1d lookup table
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                return (1 - _xrem) * table[_xr + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _z] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _z];
            }
            else if constexpr (Height.interpolate)
            {   // Single axis interpolation again.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr;
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                return (1 - _yrem) * table[_x + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _z] +
                    _yrem * table[_x + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _z];
            }
            else if constexpr (Depth.interpolate)
            {   // Single axis interpolation again.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                return (1 - _zrem) * table[_x + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _zr] +
                    _zrem * table[_x + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
            }
            else
            {   // No interpolation, calculate 2d indices first, then convert to 1d index.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Height.begin);
                return table[_x + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _z];
            }
        }

        std::array<float, (Width.size + 1) * (Height.size + 1) * (Depth.size + 1) + 1> table;
    };

    // Convert note number to frequency, uses lookup table to be more efficient and fast.
    double noteToFreq(double note);

    // Wave shapers
    namespace Shapers
    {
        // No shaper, for convenience in interpolating shapers
        inline double noShaper(double x, double amt) { return x; }

        // Sine shaper, goes from (-1, -1) to (1, 1) like a normal waveshaper
        // x: input signal
        // r: percentage of frequency
        // m: frequency
        inline double SineShaper(double x, double r, double m)
        {
            constexpr static auto pi = std::numbers::pi_v<double>;
            return x * std::cos((r + 1 / m) * x * x * pi * m - r * pi * m - pi);
        }

        // Powered Sine shaper, goes from (-1, -1) to (1, 1) like a normal waveshaper
        // x: input signal
        // r: percentage of frequency
        // m: frequency
        // p: power (p >= 0)
        inline double PoweredSineShaper(double x, double r, double m, int p)
        {
            return std::pow(SineShaper(x, r, m), p * 2. + 1);
        }

        inline double CenterExpander(double x)
        {
            return 2 - std::abs(x);
        }

        inline double SaturatorShaper(double x, double pow)
        {
            return std::tanh(x * (3 + pow));
        }

        inline double MinimizerShaper(double x, double pow, double pow2)
        {
            return SaturatorShaper(std::pow(std::abs(x), pow), pow2) * x;
        }

        inline double Shaper01(double x, double freq)
        {
            const double coefs[3]{ 2, 9, 19 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = SaturatorShaper(x, 3);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.4 + p4 * 0.2;
        }

        inline double Shaper02(double x, double freq)
        {
            const double coefs[3]{ 39, 4, 17 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = SaturatorShaper(x, 5);
            return p1 * 0.6 + p2 * 0.1 + p3 * 0.1 + p4 * 0.2;
        }

        inline double Shaper03(double x, double freq)
        {
            const double coefs[3]{ 5, 13, 47 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 2) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 5);
            const double p4 = SaturatorShaper(x, 2);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.3 + p4 * 0.3;
        }

        inline double Shaper04(double x, double freq)
        {
            const double coefs[3]{ 6, 12, 63 };
            const double p1 = PoweredSineShaper(x, freq, coefs[0], 5) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 2) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 1) * CenterExpander(x);
            const double p4 = SaturatorShaper(x, 5);
            return p1 * 0.3 + p2 * 0.2 + p3 * 0.3 + p4 * 0.2;
        }

        inline double Shaper11(double x, double freq)
        {
            const double coefs[3]{ 24, 7, 21 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = MinimizerShaper(x, 3, 2);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.4 + p4 * 0.2;
        }

        inline double Shaper12(double x, double freq)
        {
            const double coefs[3]{ 17, 16, 15 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]) * CenterExpander(x);
            const double p4 = MinimizerShaper(x, 5, 1);
            return p1 * 0.6 + p2 * 0.1 + p3 * 0.1 + p4 * 0.2;
        }

        inline double Shaper13(double x, double freq)
        {
            const double coefs[3]{ 8, 16, 32 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 1) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 3);
            const double p4 = MinimizerShaper(x, 2, 5);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.3 + p4 * 0.3;
        }

        inline double Shaper14(double x, double freq)
        {
            const double coefs[3]{ 10, 31, 97 };
            const double p1 = PoweredSineShaper(x, freq, coefs[0], 7) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 1) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 4) * CenterExpander(x);
            const double p4 = MinimizerShaper(x, 5, 3);
            return p1 * 0.3 + p2 * 0.2 + p3 * 0.3 + p4 * 0.2;
        }

        // All these shapers are arbitrary wave shapers pulled out of thin air
        // for the sake of creating weird harmonic content.
        // 1.2 * cos(159x)^2 + sin(59 + 132 * amt * x - 7.8)^3 + sin(59 + 132 * amt * x - 7.8) * cos(159x)
        inline double shaper1(double x, double amt)
        {
            const double cos1 = std::cos(amt * 159 * x);
            const double sin1 = std::sin(amt * (59 + 132 * amt * x - 7.8));
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1);
            return constrain(res, -1., 1.);
        }

        // cos((amt * 518 + 22) * x^2) * 0.64 * (0.64 * x)^5 + sin(10 * x^2) + x^7
        inline double shaper2(double x, double amt)
        {
            const double cos1 = std::cos(amt * (amt * 518 + 22) * x * x);
            const double sin1 = std::sin(amt * 10 * x * x);
            const double m1 = 0.64 * x;
            const double fp = 9 * m1 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.);
        }

        inline double shaper3(double x, double amt)
        {
            const double x1 = x * 5;
            const double cos1 = std::cos(amt * (42 + amt * 2) * x);
            const double cos2 = std::cos(amt * (23 + amt * 2) * x);
            const double sin1 = std::sin(amt * (74 + amt * 2) * x1 * x1 * x1);
            const double sin2 = std::sin(amt * (80 + 40 * amt) * x);
            const double res = (0.1 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.);
        }

        inline double shaper8(double x, double amt)
        {
            const double x1 = x * 8;
            const double cos1 = std::sin(amt * (3 + amt * 2) * x);
            const double cos2 = std::cos(amt * (6 + amt * 9) * x);
            const double sin1 = std::sin(amt * (2 + amt * 2) * x1 * x1 * x1 * x1);
            const double sin2 = std::cos(amt * (4 + 3 * amt) * x);
            const double res = (0.9 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.);
        }

        inline double shaper9(double x, double amt)
        {
            const double cos1 = std::cos(amt * (amt * 3 + 22) * x * 129);
            const double sin1 = std::sin(amt * 3 * x * x * x);
            const double m1 = 0.64 * x;
            const double fp = 3 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.);
        }

        inline double shaper7(double x, double amt)
        {
            const double cos1 = std::cos(amt * 19 * x * x * x);
            const double sin1 = std::sin(amt * (1 + 3 * amt * x * x - 7.8));
            const double sin2 = std::sin(amt * x * x);
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1 * sin2);
            return constrain(res, -1., 1.);
        }

        inline double shaper6(double x, double amt)
        {
            const double cos1 = std::sin(amt * (amt * 3 + 22) * x * x);
            const double sin1 = std::cos(amt * 54 * x * x * amt);
            const double m1 = 0.64 * x;
            const double m3 = 0.58 * x * cos1;
            const double fp = 3 * m1 * m1 * m3;
            const double sp = x * x * x * fp * m1;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.);
        }

        inline double shaper5(double x, double amt)
        {
            const double cos1 = std::cos(amt * (amt * 5 + 22) * x * x);
            const double sin1 = std::sin(amt * 312 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.);
        }

        inline double shaper0(double x, double amt)
        {
            const double cos1 = std::cos(amt * (amt * 3 + 4) * x * x * x);
            const double sin1 = std::sin(amt * 5 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.);
        }

        // Main wave shaper, is an interpolating shaper, going over 5 shapers
        // and the ability to morph between an aditional 5 shapers.
        double mainWaveShaper(double x, double amt, double morph);

        // Main phase shaper, the same as the wave shaper, except the output
        // is always constrained between 0 and 1.
        double mainPhaseShaper(double x, double amt, double morph);

        // Simple waveshaper that interpolates between x^(1/5), x, and x^5.
        inline double simpleshaper(double x, double amt)
        {   // If amt < 0.5 we have x^(1/5)
            if (amt < 0.5)
            {
                if (x < 0) // to prevent imaginary numbers, separate case for x < 0
                {
                    const double p2 = std::pow(-x, 1. / 5.);
                    double r = amt * 2;
                    return x * r + -p2 * (1 - r); // Interpolate between x^(1/5) and x
                }
                const double p2 = std::pow(x, 1. / 5.);
                double r = amt * 2;
                return x * r + p2 * (1 - r);
            }
            else
            {
                const double p1 = x * x * x * x * x;
                double r = (amt - 0.5) * 2;
                return p1 * r + x * (1 - r); // Interpolate between x^5 and x
            }
        }

        // Wavefolder, basically bounces back signal above 1 and below -1 so it folds
        // in on itself, creating aditional harmonics
        double fold(double x, double bias);

        // Simple drive algorithm with lookup table.
        double drive(double x, double gain, double amt);

        // power curve interpolates between x^(1/16), x, and x^16, and makes
        // sure the output curve perfectly outputs between 0 and 1.
        double powerCurve(double x, double curve);
    }

    using Wavetable = double(*)(double, double);
    namespace Wavetables
    {
        // Simple wavetables
        double sine(double phase);
        double saw(double phase, double f);
        double square(double phase, double f);
        double triangle(double phase, double f);

        // Basic wavetable combines sine, saw, square, and triangle into single wavetable.
        double basic(double phase, double wtpos, double f);
        bool basicLoaded();

        inline double sub(double phase, double wtpos)
        {   // Sub wavetable uses the simple shaper to add additional harmonics as its wtpos
            return Shapers::simpleshaper(sine(phase), -wtpos * 0.5 + 0.5);
        }
    }

    // Simple ADSR envelope, with curve and level settings.
    class ADSR
    {
    public:
        double SAMPLE_RATE = 44100.;

        struct Settings
        {
            double attack = 0.02; // seconds
            double decay = 0.1;   // seconds
            double sustain = 1.0; // between 0 and 1
            double release = 0.1; // seconds

            double attackCurve = 0.5;  
            double decayCurve = 0.5;  
            double releaseCurve = 0.5; 

            double attackLevel = 0;
            double decayLevel = 1;

            double timeMult = 1;
            
            bool legato = false;
        } settings;

        ADSR(const Settings& s = {}) : settings(s) {};

        double Apply(double s, size_t) { return sample * s; }
        void Generate(size_t);
        double Offset(double p);
        void Trigger();
        void Gate(bool g);
        bool Gate() { return m_Gate; }
        bool Done() { return m_Phase == -1; }

        double sample = 0;

        double m_Down = 0;
        double m_Phase = -1;
        bool m_Gate = false;
        bool m_SustainPhase = false;
    };

    // Specialized oscillator with several 
    // wave and phase shapers.
    class Oscillator
    {
    public:
        double SAMPLE_RATE = 44100.;

        struct Settings
        {
            double frequency = 440;
            double wtpos = 0;
            double sync = 0;
            double pw = 0.5;
            double bend = 0.5;
            double shaper = 0;
            double shaperMix = 0;
            double shaper2 = 0;
            double shaper2Mix = 0;
            double shaper3 = 0.5;
            double shaperMorph = 0;
        } settings;

        Oscillator(const Settings& s = {}) : settings(s) {}

        void Generate(size_t);
        double Offset(double phaseoffset, bool shaper, bool freeze); // Generate at phase offset, basic wavetable.
        double OffsetSimple(double phaseoffset); // Only simple shaper, at phase offset, basic wavetable.
        double OffsetClean(double phaseoffset); // No shapers, at phase offset, sub wavetable.

        double sample = 0;
        float phase = 0;
    };
}