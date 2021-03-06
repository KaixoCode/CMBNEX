#pragma once
#include "pch.hpp"

namespace Kaixo
{
	// Some small util functions.
	constexpr auto constrain(auto x, auto y, auto z) { return (x < y ? y : x > z ? z : x); } //std::clamp(x, y, z)
	static inline auto fastlog2(double x) { return std::log2(x); }
	constexpr auto myfmod1(auto a) { return a - (int)a; }
	constexpr auto db2lin(auto db) { return std::powf(10.0f, 0.05 * (db)); }
	constexpr auto lin2db(auto lin) { return (20.0f * std::log10f(static_cast<float>(lin))); }

	// Some constexpr maths functions
	namespace Constexpr
	{
		constexpr auto abs(auto a) -> decltype(a) { return a < 0 ? -a : a; }
		constexpr auto floor(auto a) -> decltype(a) { return static_cast<decltype(a)>(a < 0 ? (int64_t)(a - 1) : (int64_t)a); }
		
		// test values within machine epsilon range, used for termination
		//constexpr bool near_equal(auto x, decltype(x) y) { return abs(x - y) <= std::numeric_limits<decltype(x)>::epsilon(); }
		constexpr bool near_equal(auto x, decltype(x) y) { return abs(x - y) <= 0.0001; }

		// Square root by Newton-Raphson method
		constexpr double sqrt_g(std::floating_point auto x, decltype(x) guess) {
			return near_equal(guess, (guess + x / guess) / decltype(x){ 2 }) ? guess : sqrt_g(x, (guess + x / guess) / decltype(x){ 2 });
		}

		constexpr double sqrt(std::floating_point auto x) { return x == 0 ? 0 : x > 0 ? sqrt_g(x, x) : throw "Complex Number!!"; }
		constexpr double sqrt(std::integral auto x) { return sqrt(static_cast<double>(x)); }

		constexpr double exp_g(auto x, decltype(x) sum, decltype(x) n, uint64_t i, decltype(x) t) {
			return near_equal(sum, sum + t / n) ? sum : exp_g(x, sum + t / n, n * i, i + 1, t * x);
		}

		constexpr double exp(std::floating_point auto x) { return exp_g(x, decltype(x){ 1 }, decltype(x){ 1 }, 2, x); }
		constexpr double exp(std::integral auto x) { return exp(static_cast<double>(x)); }

		constexpr double trig_series(auto x, decltype(x) sum, decltype(x) n, uint64_t i, int64_t s, decltype(x) t) {
			return near_equal(sum, sum + t * s / n) ? sum : trig_series(x, sum + t * s / n, n * i * (i + 1), i + 2, -s, t * x * x);
		}

		constexpr double sin(std::floating_point auto x) { return trig_series(x, x, decltype(x){ 6 }, 4ull, -1ll, x * x * x); }
		constexpr double sin(std::integral auto x) { return sin(static_cast<double>(x)); }
		constexpr double cos(std::floating_point auto x) { return trig_series(x, decltype(x){ 1 }, decltype(x){ 2 }, 3ull, -1ll, x * x); }
		constexpr double cos(std::integral auto x) { return cos(static_cast<double>(x)); }

		constexpr double log_iter(auto x, decltype(x) y) { return y + decltype(x){ 2 } *(x - exp(y)) / (x + exp(y)); }
		constexpr double log_g(auto x, decltype(x) y) { return near_equal(y, log_iter(x, y)) ? y : log_g(x, log_iter(x, y)); }
		constexpr double log_gt(auto x) { return x > decltype(x){ 0.25 } ? log_g(x, decltype(x){ 0 }) : log_gt(x *
			std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)>) - decltype(x){ 5 }; }
		constexpr double log_lt(auto x) { return x < decltype(x){ 1024 } ? log_g(x, decltype(x){ 0 }) : log_lt(x /
			(std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)> * std::numbers::e_v<decltype(x)>)) + decltype(x){ 5 }; }

		constexpr double log(std::floating_point auto x) { return x < 0 ? throw "Log Domain Error" : x >= decltype(x){ 1024 } ? log_lt(x) : log_gt(x); }
		constexpr double log(std::integral auto x) { return log(static_cast<double>(x)); }

		constexpr double pow(auto x, auto y) { return exp(log(x) * y); }
	}


	struct Color
	{
		Color() {}
		Color(CColor color)
			: r(color.red), g(color.green),
			b(color.blue), a(color.alpha)
		{}

		operator CColor() const { return { (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a }; }

		union { double r, h = 0; };
		union { double g, s = 0; };
		union { double b, v = 0; };
		double a = 255;
	};

	static inline Color rgb2hsv(Color in)
	{
		Color       out;
		double      min, max, delta;
		out.a = in.a;

		min = in.r < in.g ? in.r : in.g;
		min = min < in.b ? min : in.b;

		max = in.r > in.g ? in.r : in.g;
		max = max > in.b ? max : in.b;

		out.v = max;                                // v
		delta = max - min;
		if (delta < 0.00001)
		{
			out.s = 0;
			out.h = 0; // undefined, maybe nan?
			return out;
		}
		if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
			out.s = (delta / max);                  // s
		}
		else {
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			out.s = 0.0;
			out.h = NAN;                            // its now undefined
			return out;
		}
		if (in.r >= max)                           // > is bogus, just keeps compilor happy
			out.h = (in.g - in.b) / delta;        // between yellow & magenta
		else
			if (in.g >= max)
				out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
			else
				out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

		out.h *= 60.0;                              // degrees

		if (out.h < 0.0)
			out.h += 360.0;

		return out;
	}

	static inline Color hsv2rgb(Color in)
	{
		double      hh, p, q, t, ff;
		long        i;
		Color       out;
		out.a = in.a;

		if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		hh = in.h;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0 - in.s);
		q = in.v * (1.0 - (in.s * ff));
		t = in.v * (1.0 - (in.s * (1.0 - ff)));

		switch (i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
		}
		return out;
	}
}