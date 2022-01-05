#pragma once
#include "pch.hpp"

namespace Kaixo
{
#define constrain(x, y, z) (x < y ? y : x > z ? z : x)

    enum class FilterType
    {
        Off, LowPass, HighPass, BandPass, Notch, AllPass, PeakingEQ, LowShelf, HighShelf, ITEMS
    };

    template<typename T>
    class Filter
    {
    public:
        using Params = T;
        virtual double Apply(double s, Params& p) = 0;
    };

    class FilterParameters
    {
    public:
        virtual void RecalculateParameters() = 0;
    };

    template<size_t M>
    class IIRFilterParameters : public FilterParameters
    {
    public:

        // Coefficients
        double A[M + 1];
        double B[M + 1];
    };

    template<size_t M>
    class FIRFilterParameters : public FilterParameters
    {
    public:

        // Coefficients
        double H[M];
    };

    class BiquadParameters
    {
        double Qb = 0, f0b = 0, sb = 0, dbgb = 0;
        FilterType tb = FilterType::Off;
    public:
        union { double Q, BW, S = 1; };
        double f0 = 22000, dbgain = 0;
        double sampleRate = 48000;
        bool off = false;
        FilterType type = FilterType::Off;

        void RecalculateParameters()
        {
            // If no changes, no update;
            if (Qb == Q && f0b == f0 && sb == sampleRate && tb == type && dbgb == dbgain)
                return;

            Qb = Q;
            f0b = f0;
            sb = sampleRate;
            tb = type;
            dbgb = dbgain;

            w0 = 6.28318530718 * (constrain(f0, 10, sampleRate / 2.1) / sampleRate);
            cosw0 = std::cos(w0), sinw0 = std::sin(w0);

            switch (type) {
            case FilterType::Off:
            {
                b0 = 1, a0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
            } break;
            case FilterType::LowPass:
            {
                off = f0 >= (sampleRate / 2 - 100);
                alpha = sinw0 / (2.0 * Q);
                //alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
                b0 = (1.0 - cosw0) / 2.0, b1 = 1.0 - cosw0, b2 = b0;
                a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
            } break;
            case FilterType::HighPass:
            {
                off = f0 <= 21;
                alpha = sinw0 / (2.0 * Q);
                b0 = (1.0 + cosw0) / 2.0, b1 = -(1.0 + cosw0), b2 = b0;
                a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
            } break;
            case FilterType::BandPass:
            {
                alpha = sinw0 * std::sinh((log2 / 2.0) * (1 / BW) * (w0 / sinw0));
                b0 = sinw0 / 2.0, b1 = 0.0, b2 = -b0;
                a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
            } break;
            case FilterType::Notch:
            {
                alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
                b0 = 1, b1 = -2.0 * cosw0, b2 = 1.0;
                a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
            } break;
            case FilterType::AllPass:
            {
                alpha = sinw0 / (2.0 * Q);
                b0 = 1.0 - alpha, b1 = -2.0 * cosw0, b2 = 1.0 + alpha;
                a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
            } break;
            case FilterType::PeakingEQ:
            {
                A = std::pow(10, dbgain / 40.0);
                alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
                b0 = 1.0 + alpha * A, b1 = -2.0 * cosw0, b2 = 1.0 - alpha * A;
                a0 = 1.0 + alpha / A, a1 = -2.0 * cosw0, a2 = 1.0 - alpha / A;
            } break;
            case FilterType::LowShelf:
            {
                A = std::pow(10, dbgain / 40.0);
                double t = std::max((A + 1.0 / A) * (1.0 / S - 1.0) + 2, 0.0);
                alpha = (sinw0 / 2.0) * std::sqrt(t);
                double sqrtAa = std::sqrt(A) * alpha;
                b0 = A * ((A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtAa);
                b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosw0);
                b2 = A * ((A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtAa);
                a0 = (A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtAa;
                a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosw0);
                a2 = (A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtAa;
            } break;
            case FilterType::HighShelf:
            {
                A = std::pow(10, dbgain / 40.0);
                double t = std::max((A + 1.0 / A) * (1.0 / S - 1.0) + 2, 0.0);
                alpha = (sinw0 / 2.0) * std::sqrt(t);
                double sqrtAa = std::sqrt(A) * alpha;
                b0 = A * ((A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtAa);
                b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosw0);
                b2 = A * ((A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtAa);
                a0 = (A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtAa;
                a1 = 2.0 * ((A - 1.0) - (A + 1) * cosw0);
                a2 = (A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtAa;
            }
            }
            b0a0 = b0 / a0, b1a0 = b1 / a0, b2a0 = b2 / a0, a1a0 = a1 / a0, a2a0 = a2 / a0;
        }

        constexpr static double log2 = 0.30102999566;
        double b0 = 1, b1 = 0, b2 = 0, a0 = 1, a1 = 0, a2 = 0;
        double b0a0 = 0, b1a0 = 0, b2a0 = 0, a1a0 = 0, a2a0 = 0;
        double w0 = 0, cosw0 = 0, sinw0 = 0, A = 0, alpha = 0;
    };

    template<typename P = BiquadParameters>
    class BiquadFilter : public Filter<P>
    {
    public:
        double Apply(double s, P& p) override
        {
            x[0] = s;
            y[0] = p.b0a0 * x[0] + p.b1a0 * x[1] + p.b2a0 * x[2] - p.a1a0 * y[1] - p.a2a0 * y[2];

            for (int i = sizeof(y) / sizeof(double) - 2; i >= 0; i--)
                y[i + 1] = y[i];

            for (int i = sizeof(x) / sizeof(double) - 2; i >= 0; i--)
                x[i + 1] = x[i];

            return y[0];
        }

    private:
        double y[3]{ 0, 0, 0 }, x[3]{ 0, 0, 0 };
    };
    
    template<size_t M>
    class KaiserBesselParameters : public FIRFilterParameters<M>
    {
        double fab = 0, fbb = 0, sb = 0, attb = 0;
    public:
        FilterType type = FilterType::LowPass;

        double sampleRate = 48000;
        void RecalculateParameters()
        {
            if (fab == Fa && fbb == Fb && sb == sampleRate && attb == attenuation)
                return;

            fab = Fa;
            fbb = Fb;
            sb = sampleRate;
            attb = attenuation;

            // Calculate the impulse response
            A[0] = 2 * (Fb - Fa) / sampleRate;
            int _np = (M - 1) / 2;
            for (int j = 1; j <= _np; j++)
                A[j] = (std::sin(j * 6.28318530718 * Fb / sampleRate) - std::sin(j * 6.28318530718 * Fa / sampleRate)) / (j * 3.14159265359);

            // Calculate alpha
            double _alpha;
            if (attenuation < 21)
                _alpha = 0;

            else if (attenuation > 50)
                _alpha = 0.1102 * (attenuation - 8.7);

            else
                _alpha = 0.5842 * std::pow((attenuation - 21), 0.4) + 0.07886 * (attenuation - 21);

            // Window the ideal response with the Kaiser-Bessel window
            double _i0alpha = I0(_alpha);
            for (int j = 0; j <= _np; j++)
                this->H[_np + j] = A[j] * I0(_alpha * std::sqrt(1.0 - ((double)j * (double)j / (_np * _np)))) / _i0alpha;

            // It is mirrored so other half is same
            for (int j = 0; j < _np; j++)
                this->H[j] = this->H[M - 1 - j];
        }

        // This function calculates the zeroth order Bessel function
        double I0(double x)
        {
            double d = 0, ds = 1, s = 1;
            do
            {
                d += 2;
                ds *= x * x / (d * d);
                s += ds;
            } while (ds > s * 1e-6);
            return s;
        }

        double Fa = 0, Fb = 7200;	// Frequencies a and b
        double attenuation = 48;	// Attenuation
        double A[M];				// Ideal impulse response
    };

    template<size_t M, typename P = KaiserBesselParameters<M>>
    class FIRFilter : public Filter<P>
    {
    public:
        FIRFilter() { std::fill(std::begin(x), std::end(x), 0); }

        float Apply(float s, P& p) override
        {
            x[0] = s;

            double y = 0;
            for (int i = 0; i < M; i++)
                y += p.H[i] * x[i];

            for (int i = sizeof(x) / sizeof(double) - 2; i >= 0; i--)
                x[i + 1] = x[i];

            return y;
        }

    private:
        double x[M];
    };

    template<size_t N, class F, class P = F::Params>
    class ChannelEqualizer
    {
    public:
        ChannelEqualizer(std::vector<P>& a)
            : m_Params(a), m_Filters()
        {}

        double Apply(double s)
        {
            for (int i = 0; i < N; i++)
                if (m_Params[i].type != FilterType::Off)
                    s = m_Filters[i].Apply(s, m_Params[i]);

            return s;
        }

        std::vector<P>& m_Params;
        F m_Filters[N];
    };


    template<size_t N, class F, class P = F::Params>
    class StereoEqualizer
    {
    public:
        StereoEqualizer(P& a)
            : m_Params(a), m_Filters()
        {}

        double Apply(double s, int c)
        {
            int channel = c % N;
            if (m_Params.type != FilterType::Off)
                s = m_Filters[channel].Apply(s, m_Params);

            return s;
        }

        P& m_Params;
        F m_Filters[N];
    };

    // Simple low/high pass band filter
    struct SimpleFilterParameters
    {
        double freq = 440, width = 1;

        SimpleFilterParameters()
        {
            m_Params.emplace_back();
            m_Params.emplace_back();
        }

        void RecalculateParameters()
        {
            m_Params[0].type = FilterType::HighPass;
            double from = FromFreq(freq);
            double a = from - std::pow(width, 2) + 0.001;
            m_Params[0].f0 = a < 0 ? -ToFreq(-a) : ToFreq(a);
            m_Params[0].Q = 0.6;
            m_Params[0].RecalculateParameters();
            m_Params[1].type = FilterType::LowPass;
            a = from + std::pow(width, 2) - 0.001;
            m_Params[1].f0 = a < 0 ? -ToFreq(-a) : ToFreq(a);
            m_Params[1].Q = 0.6;
            m_Params[1].RecalculateParameters();
        }

        double ToFreq(double x)
        {
            if (x <= 0)
                return 0;

            return std::pow(m_Log, (x * (m_Log22000 - m_Log10)) + m_Log10);
        }
        double FromFreq(double freq)
        {
            static const auto mylog = [](double v, double b) { return std::log(v) / b; };

            auto log = mylog(freq, m_Logg);

            auto norm1 = (log - m_Log10) / (m_Log22000 - m_Log10);
            return norm1;
        }

        std::vector<BiquadParameters>& Parameters() { return m_Params; }

    private:
        double m_Log = 10;
        double m_Logg = std::log(m_Log);
        double m_Log10 = std::log(10) / m_Logg;
        double m_Log22000 = std::log(22000) / m_Logg;
        std::vector<BiquadParameters> m_Params;
    };
}