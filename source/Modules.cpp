#include "Processing/Modules.hpp"

namespace Kaixo
{
    namespace Shapers
    {
        inline double noShaper(double x, double amt) { return x * amt; }

        inline double shaper1(double x, double amt)
        {
            const double cos1 = std::cos(159 * x);
            const double sin1 = std::sin(59 + 132 * amt * x - 7.8);
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1);
            return constrain(res, -1., 1.) * amt;
            //return shaper1l.get(x, amt);
        }

        inline double shaper2(double x, double amt)
        {
            const double cos1 = std::cos((amt * 518 + 22) * x * x);
            const double sin1 = std::sin(10 * x * x);
            const double m1 = 0.64 * x;
            const double fp = 9 * m1 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
            //return shaper2l.get(x, amt);
        }

        inline double shaper3(double x, double amt)
        {
            const double x1 = x * 5;
            const double cos1 = std::cos((42 + amt * 2) * x);
            const double cos2 = std::cos((23 + amt * 2) * x);
            const double sin1 = std::sin((74 + amt * 2) * x1 * x1 * x1);
            const double sin2 = std::sin((80 + 40 * amt) * x);
            const double res = (0.1 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.) * amt;
            //return shaper3l.get(x, amt);
        }

        inline double shaper8(double x, double amt)
        {
            const double x1 = x * 8;
            const double cos1 = std::sin((3 + amt * 2) * x);
            const double cos2 = std::cos((6 + amt * 9) * x);
            const double sin1 = std::sin((2 + amt * 2) * x1 * x1 * x1 * x1);
            const double sin2 = std::cos((4 + 3 * amt) * x);
            const double res = (0.9 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.) * amt;
            //return shaper8l.get(x, amt);
        }

        inline double shaper9(double x, double amt)
        {
            const double cos1 = std::cos((amt * 3 + 22) * x * 129);
            const double sin1 = std::sin(3 * x * x * x);
            const double m1 = 0.64 * x;
            const double fp = 3 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
            //return shaper9l.get(x, amt);
        }

        inline double shaper7(double x, double amt)
        {
            const double cos1 = std::cos(19 * x * x * x);
            const double sin1 = std::sin(1 + 3 * amt * x * x - 7.8);
            const double sin2 = std::sin(x * x);
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1 * sin2);
            return constrain(res, -1., 1.) * amt;
            //return shaper7l.get(x, amt);
        }

        inline double shaper6(double x, double amt)
        {
            const double cos1 = std::sin((amt * 3 + 22) * x * x);
            const double sin1 = std::cos(54 * x * x * amt);
            const double m1 = 0.64 * x;
            const double m3 = 0.58 * x * cos1;
            const double fp = 3 * m1 * m1 * m3;
            const double sp = x * x * x * fp * m1;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
            //return shaper6l.get(x, amt);
        }

        inline double shaper5(double x, double amt)
        {
            const double cos1 = std::cos((amt * 5 + 22) * x * x);
            const double sin1 = std::sin(312 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
            //return shaper5l.get(x, amt);
        }

        inline double shaper0(double x, double amt)
        {
            const double cos1 = std::cos((amt * 3 + 4) * x * x * x);
            const double sin1 = std::sin(5 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
            //return shaper0l.get(x, amt);
        }

        struct Table7
        {
            constexpr static size_t precisiona = 5000;
            constexpr static size_t precisionb = 1000;
            constexpr Table7(auto&& l)
            {
                for (int a = 0; a < precisiona + 1; a++)
                    for (int b = 0; b < precisionb + 1; b++)
                        table[a + b * (precisiona + 1)] = l(a / (precisiona / 2.) - 1., b / (double)precisionb);
            }

            inline double get(double x, double amt) const
            {
                double v = (x * 0.5 + 0.5) * precisiona;
                int a = (int)(v);
                double r = v - a;
                return (1 - r) * table[a + (precisiona + 1) * (int)(amt * precisionb)]
                    + r * table[a + 1 + (precisiona + 1) * (int)(amt * precisionb)];
            }

            double table[(precisiona + 1) * (precisionb + 1)];
        };

        const static Table7 shaperl1 = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static std::pair<double(*)(double, double), double(*)(double, double)> funcs[(int)steps + 2]{
                { shaper7,  shaper3 },
                { shaper8,  shaper2 },
                { noShaper, shaper9 },
                { shaper0,  shaper5 },
                { shaper6,  shaper1 },
                { shaper6,  shaper1 },
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1.first(x, 1 - r) * (1 - r);
            const double s2 = func2.first(x, r) * r;
            return s1 + s2;
        };        
        
        const static Table7 shaperl2 = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static std::pair<double(*)(double, double), double(*)(double, double)> funcs[(int)steps + 2]{
                { shaper7,  shaper3 },
                { shaper8,  shaper2 },
                { noShaper, shaper9 },
                { shaper0,  shaper5 },
                { shaper6,  shaper1 },
                { shaper6,  shaper1 },
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1.second(x, 1 - r) * (1 - r);
            const double s4 = func2.second(x, r) * r;
            return s3 + s4;
        };

        const static Table7 shaperl41 = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static std::pair<double(*)(double, double), double(*)(double, double)> funcs[(int)steps + 2]{
                { shaper7,  shaper3 },
                { shaper8,  shaper2 },
                { noShaper, shaper9 },
                { shaper0,  shaper5 },
                { shaper6,  shaper1 },
                { shaper6,  shaper1 },
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1.first(x, 1 - r) * (1 - r);
            const double s2 = func2.first(x, r) * r;
            return constrain(s1 + s2, 0., 1.);
        };

        const static Table7 shaperl42 = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static std::pair<double(*)(double, double), double(*)(double, double)> funcs[(int)steps + 2]{
                { shaper7,  shaper3 },
                { shaper8,  shaper2 },
                { noShaper, shaper9 },
                { shaper0,  shaper5 },
                { shaper6,  shaper1 },
                { shaper6,  shaper1 },
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1.second(x, 1 - r) * (1 - r);
            const double s4 = func2.second(x, r) * r;
            return constrain(s3 + s4, 0., 1.);
        };

        inline double shaper4(double x, double amt, double morph)
        {
            return (1 - morph) * shaperl41.get(x, amt) + morph * shaperl42.get(x, amt);
        }

        inline double shaper24(double x, double amt, double morph)
        {
            return (1 - morph) * shaperl1.get(x, amt) + morph * shaperl2.get(x, amt);
        }

        inline double simpleshaper(double x, double amt)
        {
            if (amt < 0.5)
            {
                if (x < 0)
                {
                    const double p2 = std::pow(-x, 1. / 5.);
                    double r = amt * 2;
                    return x * r + -p2 * (1 - r);
                }
                const double p2 = std::pow(x, 1. / 5.);
                double r = amt * 2;
                return x * r + p2 * (1 - r);
            }
            else
            {
                const double p1 = x * x * x * x * x;
                double r = (amt - 0.5) * 2;
                return p1 * r + x * (1 - r);
            }
        }

        struct Table4
        {
            constexpr static size_t precision = 1000;
            constexpr Table4(auto&& l)
            {
                for (int a = 0; a < precision + 1; a++)
                    for (int b = 0; b < precision + 1; b++)
                        table[a + b * (precision + 1)] = l(a / (precision / 2.) - 1., (b / (precision / 2.)) - 1.);
            }

            inline double get(double x, double amt) const
            {
                double v = (x + 1) * (precision / 2.);
                int a = (int)(v);
                double r = v - a;
                return (1 - r) * table[a + (precision + 1) * (int)((amt * 0.5 + 0.5) * precision)]
                    + r * table[a + 1 + (precision + 1) * (int)((amt * 0.5 + 0.5) * precision)];
            }

            float table[(precision + 1) * (precision + 1)];
        };

        Table4 foldt = [](double x, double bias) {
            constexpr static double b = 4;
            x += bias;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        };

        double fold(double x, double bias)
        {
            //x = constrain(x, -4., 4.);
            //return foldt.get(x, bias);
            constexpr static double b = 4;
            x += bias;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        }

        struct Table5
        {
            constexpr static int precision = 100000;
            constexpr static int start = -10;
            constexpr static int end = 10;
            constexpr Table5(auto&& l)
            {
                for (int i = 0; i < precision; i++)
                    table[i] = l((i / (double)precision) * (end - start) + start);
            }

            inline double get(double val) const
            {
                if (val < start) return -1;
                if (val > end) return 1;
                return table[(int)(precision * (val - start) / (end - start))];
            }

            double table[precision];
        };

        Table5 drivet = [](double x) {
            const double _abs = std::max(std::abs(x), 0.000001);
            const double _pow = (x / _abs) * (1 - std::exp((-x * x) / _abs));
            return _pow;
        };

        double drive(double x, double gain, double amt)
        {
            const double _gain = gain * x;
            return drivet.get(_gain) * amt + (constrain(_gain, -1., 1.)) * (1 - amt);
        }
    }

    namespace Wavetables
    {
        inline double sine(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return std::sin(phase * std::numbers::pi_v<double> *2);
        };

        inline double saw(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return phase * 2 - 1;
        };

        inline double square(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return std::floor(phase + 0.5) * 2. - 1.;
        };

        inline double triangle(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return 4 * std::abs(0.5 - phase) - 1;
        }
        struct Table6
        {
            constexpr static size_t precision = 1000;
            constexpr Table6(auto&& l)
            {
                for (int a = 0; a < precision + 1; a++)
                    for (int b = 0; b < precision + 1; b++)
                        table[a + b * (precision + 1)] = l(a / (double)precision, b / (double)precision);
            }

            inline double get(double x, double amt) const
            {
                double v = x * precision;
                int a = (int)(v);
                double r = v - a;
                return (1 - r) * table[a + (precision + 1) * (int)(amt * precision)]
                    + r * table[a + 1 + (precision + 1) * (int)(amt * precision)];
            }

            float table[(precision + 1) * (precision + 1)];
        };

        Table6 basict = [](double phase, double wtpos) {
            double p = phase;
            if (wtpos < 0.33)
            {
                const double r = wtpos * 3;
                return triangle(p, wtpos) * r + sine(p, wtpos) * (1 - r);
            }
            else if (wtpos < 0.66)
            {
                const double r = (wtpos - 0.33) * 3;
                return saw(p, wtpos) * r + triangle(p, wtpos) * (1 - r);
            }
            else
            {
                const double r = (wtpos - 0.66) * 2.9;
                return square(p, wtpos) * r + saw(p, wtpos) * (1 - r);
            }
        };

        inline double basic(double phase, double wtpos)
        {
            return basict.get(phase, wtpos);
        }
    
        inline double sub(double phase, double wtpos)
        {
            return Shapers::simpleshaper(sine(phase, wtpos), -wtpos * 0.5 + 0.5);
        }
    }


    // ADSR

    void ADSR::Generate(size_t c)
    {
        if (c != 0) return;
        if (m_Phase >= 0 && (m_Phase < settings.attack + settings.decay || !m_Gate))
            m_Phase += settings.timeMult / (double)SAMPLE_RATE;

        else if (m_Gate)
            m_Phase = settings.attack + settings.decay;

        if (m_Phase > settings.attack + settings.decay + settings.release) m_Phase = -1;

        sample = Offset(m_Phase);
    }

    inline double fastPow(double a, double b) {
        return std::pow(a, b);
        union {
            double d;
            int x[2];
        } u = { a };
        u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
        u.x[0] = 0;
        return u.d;
    }

    struct Table2
    {
        constexpr static size_t precision = 1000;
        constexpr Table2(auto&& l)
        {
            for (int a = 0; a < precision + 1; a++)
                for (int b = 0; b < precision + 1; b++)
                    table[a + b * (precision + 1)] = l(a / (double)precision, (b / (double)precision) * 2 - 1);
        }

        inline double get(double x, double amt) const
        {
            double v = x * precision;
            int a = (int)(v);
            double r = v - a;
            return (1 - r) * table[a + (precision + 1) * (int)((amt * 0.5 + 0.5) * precision)]
                + r * table[a + 1 + (precision + 1) * (int)((amt * 0.5 + 0.5) * precision)];
        }

        float table[(precision + 1) * (precision + 1)];
    };

    const static Table2 ftable = [](double x, double curve)
    {
        constexpr double MULT = 16;
        const double a = curve < 0 ? (curve * MULT - 1) : curve * MULT + 1;
        constexpr static auto b = 0.5;
        if (a >= 0)
        {
            const auto pba = std::pow(b, a); //fastPow(b, a);
            return (std::pow(b * x + b, a) - pba) / (1 - pba);
        }
        else
        {
            const auto pba = std::pow(b, -a); //fastPow(b, -a);
            return 1 - (std::pow(b * (1 - x) + b, -a) - pba) / (1 - pba);
        }
    };

    inline double f(double x, double curve)
    {
        return ftable.get(x, curve);
    }

    double ADSR::Offset(double p)
    {
        return p < 0 ? 0
            : p < settings.attack ? f(p / settings.attack, settings.attackCurve) * (settings.decayLevel - settings.attackLevel) + settings.attackLevel
            : p < settings.attack + settings.decay ? f((p - settings.attack) / settings.decay, settings.decayCurve) * (settings.sustain - settings.decayLevel) + settings.decayLevel
            : p == settings.attack + settings.decay ? settings.sustain
            : p < settings.attack + settings.decay + settings.release ? m_Down - m_Down * f((p - settings.attack - settings.decay) / settings.release, settings.releaseCurve)
            : 0;
    }

    void ADSR::Trigger()
    {
        m_Down = settings.sustain;
        if (m_Gate && settings.legato)
            m_Phase = std::min(m_Phase, settings.attack + settings.decay);
        else
            m_Phase = 0;
    }

    void ADSR::Gate(bool g)
    {
        if (m_Gate && !g)
        {
            m_Phase = settings.attack + settings.decay;
            m_Down = sample;
        }
        else if (!m_Gate && g)
        { 
            Trigger();
        }
        else if (!settings.legato) 
            m_Phase = 0;

        m_Gate = g; 
    }

    // Oscillator

    void Oscillator::Generate(size_t c)
    {
        if (c != 0)
            return;

        sample = Offset(0);
    }

    double Oscillator::OffsetOnceClean(double phaseoffset)
    {
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Wavetables::sub(myfmod1(phase + phaseoffset + 100000), settings.wtpos);
    }

    double Oscillator::OffsetOnceLFO(double phaseoffset)
    {
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Shapers::simpleshaper(Wavetables::basic(myfmod1(phase + phaseoffset + 100000), settings.wtpos), settings.shaper3);
    }

    double Oscillator::OffsetOnce(double phaseoffset)
    {
        double _s = 0;
        const double _pw = settings.pw * 2 - 1;
        if (_pw > 0)
        {
            const double _ph = Shapers::shaper4(phase, settings.shaper, settings.shaperMorph) * settings.shaperMix + phase * (1 - settings.shaperMix);
            const double _d = std::max(0.000001, 1 - _pw);
            const double _p1 = _ph / _d;
            const double _phase = myfmod1((_ph) / _d + phaseoffset + 1000000);
            const double _dphase = myfmod1(f(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);
            const double _wt = Wavetables::basic(_dphase, settings.wtpos);
            const double _s1 = Shapers::shaper24(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + _wt * (1 - settings.shaper2Mix);
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(_s1, settings.shaper3);
        }
        else
        {
            const double _ph = Shapers::shaper4(phase, settings.shaper, settings.shaperMorph) * settings.shaperMix + phase * (1 - settings.shaperMix);
            const double _d = std::max(0.000001, 1 + _pw); 
            const double _p1 = (1 - _ph) / _d;
            const double _phase = myfmod1((_ph + _pw) / _d + phaseoffset + 1000000);
            const double _dphase = myfmod1(f(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);
            const double _wt = Wavetables::basic(_dphase, settings.wtpos);
            const double _s1 = Shapers::shaper24(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + _wt * (1 - settings.shaper2Mix);
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(_s1, settings.shaper3);
        }

        phase = phase + settings.frequency / SAMPLE_RATE;
        if (phase > 1) phase -= 1;

        return _s;
    }

    double Oscillator::Offset(double phaseoffset)
    {
        return OffsetOnce(phaseoffset);
    }

    double Oscillator::Apply(double s, size_t)
    {
        return sample + s;
    }
}