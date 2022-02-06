#pragma once
#include "pch.hpp"
#include "Utils/Utils.hpp"

namespace Kaixo
{
    enum class FilterType
    {
        Off, LowPass, HighPass, BandPass, Notch, AllPass, PeakingEQ, LowShelf, HighShelf, ITEMS
    };

    // Biquad filter parameters.
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

        void RecalculateParameters(bool direct = false)
        {
            // If no changes, no update;
            if (Qb == Q && f0b == f0 && sb == sampleRate && tb == type && dbgb == dbgain)
                return;

            Qb = Q;
            if (direct)
            {
                f0b = f0;
            } 
            else 
            {
                if (f0 - f0b > 5) f0b += 5;
                else if (f0 - f0b < 5) f0b -= 5;
                else f0b = f0;
            }
            sb = sampleRate;
            tb = type;
            dbgb = dbgain;

            w0 = 6.28318530718 * (constrain(f0b, 10., sampleRate / 2.1) / sampleRate);
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

    // Simple biquad filter apply, needs BiquadParameters
    class BiquadFilter
    {
    public:
        using Params = BiquadParameters;
        double Apply(double s, Params& p)
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

    class EllipticParameters
    {
        void ellipticIntegralK(double k, double& K, double& Kp)
        {
            constexpr int M = 4;

            K = std::numbers::pi_v<double> / 2;
            auto lastK = k;

            for (int i = 0; i < M; ++i)
            {
                lastK = std::pow(lastK / (1 + std::sqrt(1 - std::pow(lastK, 2.0))), 2.0);
                K *= 1 + lastK;
            }

            Kp = std::numbers::pi_v<double> / 2;
            auto last = std::sqrt(1 - k * k);

            for (int i = 0; i < M; ++i)
            {
                last = std::pow(last / (1.0 + std::sqrt(1.0 - std::pow(last, 2.0))), 2.0);
                Kp *= 1 + last;
            }
        }
        
        std::complex<double> asne(std::complex<double> w, double k) noexcept
        {
            constexpr int M = 4;

            double ke[M + 1];
            double* kei = ke;
            *kei = k;

            for (int i = 0; i < M; ++i)
            {
                auto next = std::pow(*kei / (1.0 + std::sqrt(1.0 - std::pow(*kei, 2.0))), 2.0);
                *++kei = next;
            }

            std::complex<double> last = w;

            for (int i = 1; i <= M; ++i)
                last = 2.0 * last / ((1.0 + ke[i]) * (1.0 + std::sqrt(1.0 - std::pow(ke[i - 1] * last, 2.0))));

            return 2.0 / std::numbers::pi_v<double> * std::asin(last);
        }

        std::complex<double> sne(std::complex<double> u, double k) noexcept
        {
            constexpr int M = 4;

            double ke[M + 1];
            double* kei = ke;
            *kei = k;

            for (int i = 0; i < M; ++i)
            {
                auto next = std::pow(*kei / (1 + std::sqrt(1 - std::pow(*kei, 2.0))), 2.0);
                *++kei = next;
            }

            // NB: the spurious cast to double here is a workaround for a very odd link-time failure
            std::complex<double> last = std::sin(u * (double)(std::numbers::pi_v<double> / 2));

            for (int i = M - 1; i >= 0; --i)
                last = (1.0 + ke[i + 1]) / (1.0 / last + ke[i + 1] * last);

            return last;
        }

        std::complex<double> cde(std::complex<double> u, double k) noexcept
        {
            constexpr int M = 4;

            double ke[M + 1];
            double* kei = ke;
            *kei = k;

            for (int i = 0; i < M; ++i)
            {
                auto next = std::pow(*kei / (1.0 + std::sqrt(1.0 - std::pow(*kei, 2.0))), 2.0);
                *++kei = next;
            }

            // NB: the spurious cast to double here is a workaround for a very odd link-time failure
            std::complex<double> last = std::cos(u * (double)(std::numbers::pi_v<double> / 2));

            for (int i = M - 1; i >= 0; --i)
                last = (1.0 + ke[i + 1]) / (1.0 / last + ke[i + 1] * last);

            return last;
        }

        float passbandAmplitudedB = -1;
        float stopbandAmplitudedB = -80;
        float normalisedTransitionWidth = 0.001;

        float pf0 = 0;
        float psampleRate = 0;
    public:
        float f0 = 20000;
        float sampleRate = 44100;
        FilterType type = FilterType::LowPass;

        struct Coeficients
        {
            int order = 1;
            double b[4];
            double a[4];

            double state[4];
        };

        std::vector<Coeficients> coeficients;

        void RecalculateParameters()
        {
            if (!(pf0 != f0 || psampleRate != sampleRate))
                return;

            pf0 = f0;
            psampleRate = sampleRate;
            assert(0 < sampleRate);
            assert(0 < f0 && f0 <= sampleRate * 0.5);
            assert(0 < normalisedTransitionWidth && normalisedTransitionWidth <= 0.5);
            assert(-20 < passbandAmplitudedB && passbandAmplitudedB < 0);
            assert(-300 < stopbandAmplitudedB && stopbandAmplitudedB < -20);

            auto normalisedFrequency = f0 / sampleRate;

            auto fp = normalisedFrequency - normalisedTransitionWidth / 2;
            assert(0.0 < fp && fp < 0.5);

            auto fs = normalisedFrequency + normalisedTransitionWidth / 2;
            assert(0.0 < fs && fs < 0.5);

            double Ap = passbandAmplitudedB;
            double As = stopbandAmplitudedB;
            auto Gp = db2lin(Ap);
            auto Gs = db2lin(As);
            auto epsp = std::sqrt(1.0 / (Gp * Gp) - 1.0);
            auto epss = std::sqrt(1.0 / (Gs * Gs) - 1.0);

            auto omegap = std::tan(std::numbers::pi_v<double> * fp);
            auto omegas = std::tan(std::numbers::pi_v<double> * fs);
            constexpr auto halfPi = std::numbers::pi_v<double> / 2;

            auto k = omegap / omegas;
            auto k1 = epsp / epss;

            int N;

            double K, Kp, K1, K1p;

            ellipticIntegralK(k, K, Kp);
            ellipticIntegralK(k1, K1, K1p);

            N = std::round(std::ceil((K1p * K) / (K1 * Kp)));
            
            const int r = N % 2;
            const int L = (N - r) / 2;
            const double H0 = std::pow(Gp, 1.0 - r);

            std::vector<std::complex<double>> pa, za;
            pa.reserve(L);
            za.reserve(L);
            std::complex<double> j(0, 1);

            auto v0 = -j * (asne(j / epsp, k1) / (double)N);

            if (r == 1)
                pa.push_back(omegap * j * sne(j * v0, k));

            for (int i = 1; i <= L; ++i)
            {
                auto ui = (2 * i - 1.0) / (double)N;
                auto zetai = cde(ui, k);

                pa.push_back(omegap * j * cde(ui - j * v0, k));
                za.push_back(omegap * j / (k * zetai));
            }
            
            std::vector<std::complex<double>> p, z, g;
            p.reserve(L + 1);
            z.reserve(L + 1);
            g.reserve(L + 1);

            if (r == 1)
            {
                p.push_back((1.0 + pa[0]) / (1.0 - pa[0]));
                g.push_back(0.5 * (1.0 - p[0]));
            }

            for (int i = 0; i < L; ++i)
            {
                p.push_back((1.0 + pa[i + r]) / (1.0 - pa[i + r]));
                z.push_back(za.size() == 0 ? -1.0 : (1.0 + za[i]) / (1.0 - za[i]));
                g.push_back((1.0 - p[i + r]) / (1.0 - z[i]));
            }

            coeficients.clear();

            if (r == 1)
            {
                auto b0 = static_cast<double> (H0 * std::real(g[0]));
                auto b1 = b0;
                auto a1 = static_cast<double> (-std::real(p[0]));

                coeficients.push_back({ .order = 1, .b = { b0, b1 }, .a = { 1.0, a1 } });
            }

            for (int i = 0; i < L; ++i)
            {
                auto gain = std::pow(std::abs(g[i + r]), 2.0);

                auto b0 = static_cast<double> (gain);
                auto b1 = static_cast<double> (std::real(-z[i] - std::conj(z[i])) * gain);
                auto b2 = static_cast<double> (std::real(z[i] * std::conj(z[i])) * gain);
                                      
                auto a1 = static_cast<double> (std::real(-p[i + r] - std::conj(p[i + r])));
                auto a2 = static_cast<double> (std::real(p[i + r] * std::conj(p[i + r])));

                coeficients.push_back({ .order = 2, .b = { b0, b1, b2 }, .a = { 1.0, a1, a2 } });
            }
        }
    };

    class EllipticFilter
    {
    public:
        using Params = EllipticParameters;

        double Apply(double s, Params& p)
        {
            double res = s;
            for (auto& c : p.coeficients)
            {
                auto output = (c.b[0] * res) + c.state[0];

                for (size_t j = 0; j < c.order - 1; ++j)
                    c.state[j] = (c.b[j + 1] * res) - (c.a[j + 1] * output) + c.state[j + 1];

                c.state[c.order - 1] = (c.b[c.order] * res) - (c.a[c.order] * output);
                res = output;
            }

            return res;
        }
    };

    // Simple stereo equalizer, applies a distinct filter on each channel.
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

        void Reset()
        {
            for (auto& i : m_Filters)
                i.Reset();
        }

        P& m_Params;
        F m_Filters[N];
    };
}