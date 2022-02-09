#include "Processing/Processor.hpp"
#include <cassert>

template<auto Fun, size_t Args>
struct AlgorithmRangeTest
{
	struct Settings
	{
		double min = -1.;
		double max = 1.;

		struct Range 
		{
			double precision = 0.001;
			double begin = 0;
			double end = 1;
		} args[Args];
	};

	static inline void Run(Settings settings)
	{
		RunExp(settings, std::make_index_sequence<Args>{});
	}

private:
	template<size_t ...Is>
	static inline void RunExp(Settings& settings, std::index_sequence<Is...>)
	{
		double _current[Args]{ settings.args[Is].begin... };
		size_t _index = 0;
		bool _done = false;
		while (!_done)
		{
			auto _result = Fun(_current[Is]...);
			TestResult(settings, _result);

			_current[_index] += settings.args[_index].precision; // Increment

			// While we encounter current index larger than end.
			while (_index < Args && _current[_index] >= settings.args[_index].end)
			{
				_current[_index] = settings.args[_index].begin; // Reset back to begin
				_index++; // Go to next index
				if (_index == Args) 
				{ 
					_done = true; 
					break; 
				} // if we reset end, done
				_current[_index] += settings.args[_index].precision; // increment by precision
			}

			_index = 0;
		}
	}

	static inline void TestResult(Settings& settings, double result)
	{
		bool _passRange = result >= settings.min && result <= settings.max;
		bool _passNan = !std::isnan(result);
		bool _passInf = !std::isinf(result);
		assert(_passRange);
		assert(_passNan);
		assert(_passInf);
	}
};

// Test the input and output ranges of algorithms
void TestAlgorithmRanges()
{
	std::cout << "mainWaveShaper\n";
	AlgorithmRangeTest<&Kaixo::Shapers::mainWaveShaper, 3>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.01, .begin = -1., .end = 1. }, // X
			{.precision = 0.01, .begin = 0., .end = 1. },  // Amount
			{.precision = 0.01, .begin = 0., .end = 2. }   // Morph
		} 
	});	

	std::cout << "mainPhaseShaper\n";
	AlgorithmRangeTest<&Kaixo::Shapers::mainPhaseShaper, 3>::Run({
		.min = 0., .max = 1., // Output range
		.args
		{
			{.precision = 0.01, .begin = 0., .end = 1. }, // X
			{.precision = 0.01, .begin = 0., .end = 1. }, // Amount
			{.precision = 0.01, .begin = 0., .end = 2. }  // Morph
		} 
	});	

	std::cout << "drive\n";
	AlgorithmRangeTest<&Kaixo::Shapers::drive, 3>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.01, .begin = -12., .end = 12. }, // X
			{.precision = 0.01, .begin = 1., .end = 4. },    // Gain
			{.precision = 0.01, .begin = 0., .end = 1. },    // Amount
		} 
	});

	std::cout << "fold\n";
	AlgorithmRangeTest<&Kaixo::Shapers::fold, 2>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.001, .begin = -12., .end = 12. }, // X
			{.precision = 0.01, .begin = -1., .end = 1. }, // Bias
		} 
	});

	std::cout << "powerCurve\n";
	AlgorithmRangeTest<&Kaixo::Shapers::powerCurve, 2>::Run({
		.min = 0., .max = 1., // Output range
		.args
		{
			{.precision = 0.001, .begin = 0., .end = 1. }, // X
			{.precision = 0.01, .begin = -1., .end = 1. }, // Curve
		} 
	});

	std::cout << "simpleshaper\n";
	AlgorithmRangeTest<&Kaixo::Shapers::simpleshaper, 2>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.001, .begin = -1., .end = 1. }, // X
			{.precision = 0.01, .begin = 0., .end = 1. }, // Amount
		} 
	});

	std::cout << "Wavetables::basic\n";
	AlgorithmRangeTest<&Kaixo::Wavetables::basic, 3>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.001, .begin = 0., .end = 1. }, // X
			{.precision = 0.01, .begin = 0., .end = 1. }, // WT Pos
			{.precision = 5.0, .begin = 1., .end = 20000. }, //Freq
		} 
	});

	std::cout << "Wavetables::sub\n";
	AlgorithmRangeTest<&Kaixo::Wavetables::sub, 2>::Run({
		.min = -1., .max = 1., // Output range
		.args
		{
			{.precision = 0.001, .begin = 0., .end = 1. }, // X
			{.precision = 0.01, .begin = 0., .end = 1. }, // WT Pos
		} 
	});

	std::cout << "CombineSingle\n";
	double input = 60;
	AlgorithmRangeTest<&Kaixo::Processor::CombineSingle, 3>::Run({
		.min = -1.8 * input * input, .max = 1.8 * input * input, // Output range
		.args
		{
			{.precision = input / 1000., .begin = -input, .end = input }, // in theory only -6..6, but filters 
			{.precision = input / 1000., .begin = -input, .end = input }, // may change that range.
			{.precision = 1, .begin = 0, .end = Kaixo::Processor::CombineMode::Size - 1 }, // may change that range.
		} 
	});
}

void SampleFlow()
{
	double phase = 0; // Phase is always 0-1

	phase = Kaixo::Shapers::mainPhaseShaper(
		phase     /* 0..1 */, 
		0         /* 0..1 */, 
		0         /* 0..1 */); 
	// Output range: 0..1

	phase = Kaixo::Shapers::powerCurve(
		phase    /*  0..1 */, 
		0        /* -1..1 */); 
	// Output range: 0..1

	double oscillator = Kaixo::Wavetables::basic(
		phase      /* 0..1 */, 
		0          /* 0..1 */,
		1          /* 1..20k */);
	// Output range: -1..1

	oscillator = Kaixo::Shapers::mainWaveShaper(
		oscillator /* -1..1 */, 
		0          /*  0..1 */, 
		0          /*  0..1 */); 
	// Output range:  -1..1

	oscillator = Kaixo::Shapers::simpleshaper(
		oscillator /* -1..1 */, 
		0          /*  0..1 */); 
	// Output range:  -1..1

	oscillator = oscillator + 1 /* -1..1 */; // DC Offset
	// Output range:  -2..2

	oscillator = Kaixo::Shapers::fold(
		oscillator /* -32..32 */, // Input is multiplied by value 1..16
		0          /*  -1..1  */);
	// Output range:   -1..1

	oscillator += 1 /* unspecified range */; // Noise + filtering, filter could add unspecified level if unstable.
	// Output range: unspecified 

	oscillator = Kaixo::Shapers::drive(
		oscillator /* unspecified */, // Does not matter, as the lookup input is constrained
		0          /* 1..4 */,
		0          /* 0..1 */);
	// Output range: -1..1 // Always limited, even if drive turned off, so always -1..1

	oscillator += 1 /* unspecified */; // Filter is applied, again unspecified range.

	double combines1 = oscillator * 4 /* + 2 * combiner */; // Each input of a combiner can only receive 
	double combines2 = oscillator * 4 /* + 2 * combiner */; // 4x oscillator + 2x combiner max.

	double combiner = Kaixo::Processor::CombineSingle(combines1, combines2, 0 /* mode */);
	// Output range: unspecified

	combiner = Kaixo::Shapers::fold(
		combiner /* unspecified */,
		0        /* -1..1 */);
	// Output range: unspecified // Fold outputs -1..1, but may be disabled

	combiner = Kaixo::Shapers::drive(
		combiner /* unspecified */, // Does not matter in this case, lookup table input is constrained
		0        /* 1..4 */,
		0        /* 0..1 */);
	// Output range: -1..1 // Always limited, even when disabled.

	combiner += 1 /* unspecified */; // Filter again.
}

#include "Utils/Utils.hpp"

int main()
{
	TestAlgorithmRanges();

	constexpr auto val1 = Kaixo::Constexpr::pow(2, 2.);

	return 0;
}