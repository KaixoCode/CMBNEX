#pragma once
#include "pch.hpp"

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
        {   // Load data in detached thread to prevent blocking
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

    // 2d lookup table
    template<TableAxis Width, TableAxis Height>
    struct LookupTable<Width, Height>
    {
        bool done = false;
        constexpr LookupTable(auto&& fun)
        {   // Load data in detached thread to prevent blocking
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
                const float _xa1 = (1 - _xrem) * table[_xr + (Width.size + 1) * _yr] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * _yr];
                const float _xa2 = (1 - _xrem) * table[_xr + (Width.size + 1) * (_yr + 1)] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * (_yr + 1)];
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

        std::array<float, (Width.size + 1)* (Height.size + 1) + 1> table;
    };

    // 3d lookup table
    template<TableAxis Width, TableAxis Height, TableAxis Depth>
    struct LookupTable<Width, Height, Depth>
    {
        bool done = false;
        constexpr LookupTable(auto&& fun)
        {   // Load data in detached thread to prevent blocking
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
            if constexpr (Width.interpolate && Height.interpolate && Depth.interpolate)
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
                                          _xrem * table[_xr + 1 + (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * _zr];
                const float _xa12 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr] +
                                          _xrem * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _ya1 = _xa11 * (1 - _yrem) + _xa12 * _yrem;
                const float _xa21 = (1 - _xrem) * table[_xr +     (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                          _xrem * table[_xr + 1 + (Width.size + 1) * _yr +       (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _xa22 = (1 - _xrem) * table[_xr +     (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                                          _xrem * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _ya2 = _xa21 * (1 - _yrem) + _xa22 * _yrem;
                return _ya1 * (1 - _zrem) + _ya2 * _zrem;
            }
            else if constexpr (Width.interpolate && Height.interpolate)
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const float _xa1 = (1 - _xrem) * table[_xr + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _z] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _z];
                const float _xa2 = (1 - _xrem) * table[_xr + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _z] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _z];
                const float _ya = _xa1 * (1 - _yrem) + _xa2 * _yrem;
                return _ya;
            }
            else if constexpr (Width.interpolate && Depth.interpolate)
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                const float _xa1 = (1 - _xrem) * table[_xr + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _zr] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _xa2 = (1 - _xrem) * table[_xr + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                    _xrem * table[_xr + 1 + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
                const float _za = _xa1 * (1 - _zrem) + _xa2 * _zrem;
                return _za;
            }
            else if constexpr (Height.interpolate && Depth.interpolate)
            {   // Calculate index and ratios for both indices
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                const double _z = Depth.size * (z - Depth.begin) / (Depth.end - Depth.begin);
                const int _zr = (int)(_z);
                const double _zrem = _z - _zr;
                const float _ya1 = (1 - _yrem) * table[_x + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * _zr] +
                    _yrem * table[_x + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * _zr];
                const float _ya2 = (1 - _yrem) * table[_x + (Width.size + 1) * _yr + (Width.size + 1) * (Height.size + 1) * (_zr + 1)] +
                    _yrem * table[_x + (Width.size + 1) * (_yr + 1) + (Width.size + 1) * (Height.size + 1) * (_zr + 1)];
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
            {   // No interpolation, calculate 3d indices first, then convert to 1d index.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _z = Depth.size * (z - Depth.begin) / (Depth.end - Height.begin);
                return table[_x + (Width.size + 1) * _y + (Width.size + 1) * (Height.size + 1) * _z];
            }
        }

        std::array<float, (Width.size + 1)* (Height.size + 1)* (Depth.size + 1) + 1> table;
    };
}