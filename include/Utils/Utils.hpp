#pragma once
#include "pch.hpp"

namespace Kaixo
{
	constexpr static inline auto constrain(auto x, auto y, auto z) { return (x < y ? y : x > z ? z : x); } //std::clamp(x, y, z)
	constexpr static inline auto myfmod1(auto a) { return a - (int)a; }
	constexpr static inline auto db2lin(auto db) { return std::powf(10.0f, 0.05 * (db)); }
	constexpr static inline auto lin2db(auto lin) { return (20.0f * std::log10f(static_cast<float>(lin))); }
}