#pragma once
#include "pch.hpp"

namespace Kaixo
{
	auto constrain(auto x, auto y, auto z) { return (x < y ? y : x > z ? z : x); } //std::clamp(x, y, z)
	auto myfmod1(auto a) { return a - (int)a; }
	auto db2lin(auto db) { return std::powf(10.0f, 0.05 * (db)); }
	auto lin2db(auto lin) { return (20.0f * std::log10f(static_cast<float>(lin))); }
}