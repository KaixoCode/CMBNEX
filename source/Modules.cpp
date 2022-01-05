#include "Modules.hpp"

namespace Kaixo
{
    namespace Shapers
    {
        double noShaper(double x, double amt) { return x; }

        double shaper1(double x, double amt)
        {
            const double cos1 = std::cos(159 * x);
            const double sin1 = std::sin(59 + 132 * amt * x - 7.8);
            const double res = (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1);
            return 1.2 * constrain(res, -1, 1);
        }

        double shaper2(double x, double amt)
        {
            const double cos1 = std::cos((amt * 518 + 22) * x * x);
            const double sin1 = std::sin(10 * x * x);
            const double m1 = 0.64 * x;
            const double fp = 9 * m1 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1, 1);
        }

        double shaper3(double x, double amt)
        {
            const double x1 = x * 5;
            const double cos1 = std::cos((42 + amt * 2) * x);
            const double cos2 = std::cos((23 + amt * 2) * x);
            const double sin1 = std::sin((74 + amt * 2) * x1 * x1 * x1);
            const double sin2 = std::sin((80 + 40 * amt) * x);
            const double res = (0.1 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1, 1);
        }

        double shaper5(double x, double amt)
        {
            const double cos1 = std::cos((amt * 5 + 22) * x * x);
            const double sin1 = std::sin(312 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1, 1);
        }

        double shaper4(double x, double amt)
        {
            constexpr double steps = 4;
            constexpr double (*funcs[(int)steps + 1])(double, double){
                noShaper, shaper2, shaper3, shaper5, shaper1
            };

            for (int i = 1; i < steps + 1; i++) if (amt <= i / steps)
            {
                double r = (amt - (i - 1) / steps) * steps;
                const double s1 = funcs[i - 1](x, 1 - r);
                const double s2 = funcs[i](x, r);
                return (s2 * r + s1 * (1 - r)) * 0.5 + 0.5 * x;
            }

            return 0;
        }

        double shaper24(double x, double amt)
        {
            constexpr double steps = 4;
            constexpr double (*funcs[(int)steps + 1])(double, double){
                noShaper, shaper2, shaper3, shaper5, shaper1
            };

            for (int i = 1; i < steps + 1; i++) if (amt <= i / steps)
            {
                double r = (amt - (i - 1) / steps) * steps;
                const double s1 = funcs[i - 1](x, 1 - r);
                const double s2 = funcs[i](x, r);
                return (s2 * r + s1 * (1 - r)) * 0.5 + 0.5 * x;
            }

            return 0;
        }

        double simpleshaper(double x, double amt)
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
    }

    namespace Wavetables
    {
        double sine(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return std::sin(phase * std::numbers::pi_v<double> *2);
        };

        double saw(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return phase * 2 - 1;
        };

        double square(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return std::floor(phase + 0.5) * 2. - 1.;
        };

        double triangle(double phase, double wtpos)
        {
            //assert(phase >= 0 && phase <= 1);
            return 4 * std::abs(0.5 - phase) - 1;
        }

        double basic(double phase, double wtpos)
        {
            double p = phase;
            if (wtpos < 0.33)
            {
                double r = wtpos * 3;
                return triangle(p, wtpos) * r + sine(p, wtpos) * (1 - r);
            }
            else if (wtpos < 0.66)
            {
                double r = (wtpos - 0.33) * 3;
                return saw(p, wtpos) * r + triangle(p, wtpos) * (1 - r);
            }
            else
            {
                double r = (wtpos - 0.66) * 3;
                return square(p, wtpos) * r + saw(p, wtpos) * (1 - r);
            }
        }
    
        double sub(double phase, double wtpos)
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

    double f(double x, double a)
    {
        constexpr auto b = 0.5;
        if (a >= 0)
        {
            const auto pba = std::pow(b, a);
            return (std::pow(b * x + b, a) - pba) / (1 - pba);
        }
        else
        {
            const auto pba = std::pow(b, -a);
            return 1 - (std::pow(b * (1 - x) + b, -a) - pba) / (1 - pba);
        }
    }

    double ADSR::Offset(double p)
    {
        double _ac = settings.attackCurve < 0 ? (settings.attackCurve * 8 - 1) : settings.attackCurve * 8 + 1;
        double _dc = settings.decayCurve < 0 ? (settings.decayCurve * 8 - 1) : settings.decayCurve * 8 + 1;
        double _rc = settings.releaseCurve < 0 ? (settings.releaseCurve * 8 - 1) : settings.releaseCurve * 8 + 1;
        return p < 0 ? 0
            : p < settings.attack ? f(p / settings.attack, _ac) * (settings.decayLevel - settings.attackLevel) + settings.attackLevel
            : p < settings.attack + settings.decay ? f((p - settings.attack) / settings.decay, _dc) * (settings.sustain - settings.decayLevel) + settings.decayLevel
            : p == settings.attack + settings.decay ? settings.sustain
            : p < settings.attack + settings.decay + settings.release ? m_Down - m_Down * f((p - settings.attack - settings.decay) / settings.release, _rc)
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

    double Oscillator::OffsetOnce(double phaseoffset)
    {
        double _s = 0;
        double _pw = settings.pw * 2 - 1;
        if (_pw > 0)
        {
            double _ph = Shapers::shaper4(phase, settings.shaper);
            double _d = std::max(0.000001, 1 - _pw);
            double _p1 = _ph / _d;
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(
                Shapers::shaper24(settings.wavetable(std::fmod((_ph + phaseoffset) * settings.sync / _d, 1.), settings.wtpos)
                    , settings.shaper2), settings.shaper3);
        }
        else
        {
            double _ph = Shapers::shaper4(phase, settings.shaper);
            double _d = std::max(0.000001, 1 + _pw);
            double _p1 = (1 - _ph) / _d;
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(
                Shapers::shaper24(settings.wavetable(std::fmod((_ph + _pw + phaseoffset) * settings.sync / _d, 1.), settings.wtpos)
                    , settings.shaper2), settings.shaper3);
        }

        double delta = settings.frequency / (SAMPLE_RATE * settings.oversample);
        phase = std::fmod(1 + phase + delta, 1);
        return _s;
    }

    double Oscillator::Offset(double phaseoffset)
    {
        // No oversampling
        if (settings.oversample == 1)
            return OffsetOnce(phaseoffset);

        double _avg = 0;
        m_Params.sampleRate = SAMPLE_RATE * settings.oversample;
        m_Params.f0 = SAMPLE_RATE * 0.4;
        m_Params.Q = 1;
        m_Params.type = FilterType::LowPass;
        m_Params.RecalculateParameters();

        for (int i = 0; i < settings.oversample; i++)
        {
            double _s = OffsetOnce(phaseoffset);
            for (auto& i : m_Filter)
                _s = i.Apply(_s, m_Params);
            _avg += _s;
        }
        _avg /= settings.oversample;
        return constrain(_avg, -1, 1);
    }

    double Oscillator::Apply(double s, size_t)
    {
        return sample + s;
    }

    // Chorus

    void Chorus::Channels(int c)
    {
        m_Buffers.reserve(c);
        while (m_Buffers.size() < c)
        {
            auto& a = m_Buffers.emplace_back();
            while (a.size() < BUFFER_SIZE)
                a.emplace_back(0);
        }
    }

    double Chorus::Apply(double sin, size_t c)
    {
        Channels(c + 1);

        if (c == 0)
        {
            m_Position = (m_Position + 1) % BUFFER_SIZE;
            settings.oscillator.Generate(c);
        }

        m_Delay1t = ((settings.delay1 + settings.oscillator.Offset(settings.stereo ? (c % 2) * 0.5 : 0) * settings.amount) / 1000.0) * SAMPLE_RATE;
        m_Delay2t = ((settings.delay2 + settings.oscillator.Offset(settings.stereo ? (c % 2) * 0.5 : 0) * settings.amount) / 1000.0) * SAMPLE_RATE;

        m_Delay1t = (std::max(m_Delay1t, 1)) % BUFFER_SIZE;
        m_Delay2t = (std::max(m_Delay2t, 1)) % BUFFER_SIZE;

        auto& _buffer = m_Buffers[c];

        if (settings.enableDelay2)
        {
            int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;
            int i2 = (m_Position - m_Delay2t + BUFFER_SIZE) % BUFFER_SIZE;

            float del1s = _buffer[i1];
            float del2s = _buffer[i2];

            float now = (del1s + del2s) / 2.0;

            _buffer[m_Position] = sin + settings.polarity * now * settings.feedback;

            return sin * (1.0 - settings.mix) + now * settings.mix;
        }
        else
        {
            int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;

            float del1s = _buffer[i1];

            float now = del1s;

            _buffer[m_Position] = sin + settings.polarity * now * settings.feedback;

            return sin * (1.0 - settings.mix) + now * settings.mix;
        }
    };

    // LPF

    void LPF::Generate(size_t)
    {
        m_Params.sampleRate = SAMPLE_RATE;
        m_Params.type = FilterType::LowPass;
        m_Params.f0 = settings.frequency;
        m_Params.Q = settings.resonance;
        m_Params.RecalculateParameters();
    }

    double LPF::Apply(double s, size_t c)
    {
        return m_Filter.Apply(s, c) * settings.mix + s * (1 - settings.mix);
    }

    // Delay

    void Delay::Channels(int c)
    {
        BUFFER_SIZE = SAMPLE_RATE * 10;
        m_Equalizers.reserve(c);
        while (m_Equalizers.size() < c)
            m_Equalizers.emplace_back(m_Parameters.Parameters());

        m_Buffers.reserve(c);
        while (m_Buffers.size() < c)
        {
            auto& a = m_Buffers.emplace_back();
            while (a.size() < BUFFER_SIZE)
                a.emplace_back(0);
        }
    }

    void Delay::Generate(size_t c)
    {
        Channels(c + 1);
        m_Parameters.RecalculateParameters();
    }

    double Delay::Apply(double sin, size_t c)
    {
        float in = sin * db2lin(settings.gain);
        if (c == 0)
        {
            m_Oscillator.settings.frequency = settings.mod.rate;
            m_Position = (m_Position + 1) % BUFFER_SIZE;
            m_Oscillator.Generate(c);
        }

        int s = settings.stereo ? ((c % 2) * 0.5) : 0;
        int delayt = ((settings.delay + settings.delay * m_Oscillator * settings.mod.amount * 0.01 * 0.9) / 1000.0) * SAMPLE_RATE;
        delayt = (std::max(delayt, 1)) % BUFFER_SIZE;

        auto& _buffer = m_Buffers[c];
        int i1 = (int)(m_Position - (settings.stereo ? (delayt + delayt * (c % 2) * 0.5) : delayt) + 3 * BUFFER_SIZE) % BUFFER_SIZE;

        float del1s = _buffer[i1];

        float now = settings.filter ? m_Equalizers[c].Apply(del1s) : del1s;

        int next = settings.stereo ? ((int)(m_Position - (c % 2) * delayt * 0.5 - 1 + 3 * BUFFER_SIZE) % BUFFER_SIZE) : m_Position;
        _buffer[m_Position] = in;
        _buffer[next] += now * settings.feedback;

        return sin * (1.0 - settings.mix) + now * settings.mix;
    }
}