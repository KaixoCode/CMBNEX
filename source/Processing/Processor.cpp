#include "Processing/Processor.hpp"

#define SIMD_TYPE 256
#if SIMD_TYPE == 256

#define SIMD_T __m256
#define SIMD_Ti __m256i
#define set1_ps _mm256_set1_ps
#define load_ps _mm256_load_ps
#define loadu_ps _mm256_loadu_ps
#define store_ps _mm256_store_ps
#define storeu_ps _mm256_storeu_ps
#define max_ps _mm256_max_ps
#define min_ps _mm256_min_ps
#define mul_ps _mm256_mul_ps
#define add_ps _mm256_add_ps
#define and_ps _mm256_and_ps
#define div_ps _mm256_div_ps
#define xor_ps _mm256_xor_ps
#define or_ps _mm256_or_ps
#define sub_ps _mm256_sub_ps
#define cmp_ps _mm256_cmp_ps
#define log2_ps _mm256_log2_ps
#define floor_ps _mm256_floor_ps
#define scalef_ps _mm256_scalef_ps
#define cvtps_epi32 _mm256_cvtps_epi32
#define add_epi32 _mm256_add_epi32
#define sub_epi32 _mm256_sub_epi32
#define mullo_epi32 _mm256_mullo_epi32
#define i32gather_ps(a, b, c) _mm256_i32gather_ps(b, a, c)
#define set1_epi32 _mm256_set1_epi32
#define castps_si _mm256_castps_si256
#define castsi_ps _mm256_castsi256_ps

#elif SIMD_TYPE == 512

#define SIMD_T __m512
#define SIMD_Ti __m512i
#define set1_ps _mm512_set1_ps
#define load_ps _mm512_load_ps
#define loadu_ps _mm512_loadu_ps
#define store_ps _mm512_store_ps
#define storeu_ps _mm512_storeu_ps
#define max_ps _mm512_max_ps
#define min_ps _mm512_min_ps
#define mul_ps _mm512_mul_ps
#define add_ps _mm512_add_ps
#define div_ps _mm512_div_ps
#define and_ps _mm512_and_ps
#define xor_ps _mm512_xor_ps
#define or_ps _mm512_or_ps
#define sub_ps _mm512_sub_ps
#define cmp_ps _mm512_cmp_ps_mask
#define log2_ps _mm512_log2_ps
#define floor_ps _mm512_floor_ps
#define scalef_ps _mm512_scalef_ps
#define cvtps_epi32 _mm512_cvtps_epi32
#define add_epi32 _mm512_add_epi32
#define sub_epi32 _mm512_sub_epi32
#define mullo_epi32 _mm512_mullo_epi32
#define i32gather_ps _mm512_i32gather_ps
#define set1_epi32 _mm512_set1_epi32
#define castps_si _mm512_castps_si512
#define castsi_ps _mm512_castsi512_ps

#endif

struct SIMD;

struct SIMDi
{
    inline SIMDi()
        : value{}
    {}

    inline SIMDi(int v)
        : value(set1_epi32(v))
    {}

    inline  SIMDi(const SIMD_Ti& v)
        : value(v)
    {}

    SIMD_Ti value;
};

struct SIMD
{
    inline SIMD()
        : value{}
    {}

    explicit inline SIMD(bool v)
        : value(v ? castsi_ps(set1_epi32(UINT_MAX)) : set1_ps(0))
    {}

    inline SIMD(float* v)
        : value(loadu_ps(v))
    {}

    inline SIMD(float v)
        : value(set1_ps(v))
    {}

    inline SIMD(const SIMD_T& v)
        : value(v)
    {}

    SIMD_T value;

    inline SIMD floor() const { return floor_ps(value); }
    inline SIMD log2() const { return log2_ps(value); }
    
    inline SIMDi toInt() const { return cvtps_epi32(value); }
    
    inline SIMD operator~() { return xor_ps(value, castsi_ps(set1_epi32(UINT_MAX))); }
};

static inline SIMD operator+(const SIMD& a, const SIMD& b) { return add_ps(a.value, b.value); }
static inline SIMD operator-(const SIMD& a, const SIMD& b) { return sub_ps(a.value, b.value); }
static inline SIMD operator*(const SIMD& a, const SIMD& b) { return mul_ps(a.value, b.value); }
static inline SIMD operator/(const SIMD& a, const SIMD& b) { return div_ps(a.value, b.value); }

static inline SIMD operator&(const SIMD& a, const SIMD& b) { return and_ps(a.value, b.value); }
static inline SIMD operator|(const SIMD& a, const SIMD& b) { return or_ps(a.value, b.value); }
       
static inline SIMD operator<(const SIMD& a, const SIMD& b) { return cmp_ps(a.value, b.value, _CMP_LT_OS); }
static inline SIMD operator>(const SIMD& a, const SIMD& b) { return cmp_ps(a.value, b.value, _CMP_GT_OS); }
       
static inline SIMDi operator+(const SIMDi& a, const SIMDi& b) { return add_epi32(a.value, b.value); }
static inline SIMDi operator-(const SIMDi& a, const SIMDi& b) { return sub_epi32(a.value, b.value); }
static inline SIMDi operator*(const SIMDi& a, const SIMDi& b) { return mullo_epi32(a.value, b.value); }
       
static inline SIMD simdmin(const SIMD& a, const SIMD& b) { return min_ps(a.value, b.value); }
static inline SIMD simdmax(const SIMD& a, const SIMD& b) { return max_ps(a.value, b.value); }
static inline SIMD simdconstrain(const SIMD& a, const SIMD& b, const SIMD& c) { return simdmin(simdmax(a, b), c); }

static inline auto interpolate(SIMD& num, SIMDi& a, SIMD& r)
{
    SIMD _start = num.floor(); // Calculate index 1
    r = num - _start; // Calculate how far along it is to the next index
    a = _start.toInt();
};

static inline auto simdmyfmod1(const SIMD& num)
{
    SIMD _start = num.floor();
    return num - _start;
};

static inline auto lookup3di(const SIMD& x, const SIMD& y, const SIMD& z, const std::array<float, 4097 * 9 * 1025 + 1>& table)
{
    SIMD _x = (x + 1) * 2048;
    SIMDi _xr;
    SIMD _xrem;
    interpolate(_x, _xr, _xrem);

    SIMD _y = y * 8;
    SIMDi _yr;
    SIMD _yrem;
    interpolate(_y, _yr, _yrem);

    SIMDi _z = simdconstrain(z * (1024 / 3.f), 0.f, 1024.f).toInt();
    //SIMD _z = simdconstrain(z * (1024 / 3.f), 0.f, 1024.f);
    //SIMDi _zr;
    //SIMD _zrem;
    //interpolate(_z, _zr, _zrem);

    // Calculate indices to interpolate
    // Get data at all indices, and interpolate xaxis using xrem
    SIMDi _zi = _z * ((4096 + 1) * (8 + 1));
    SIMDi _yi1 = _yr * (4096 + 1) + _zi;
    SIMDi _yi2 = (_yr + 1) * (4096 + 1) + _zi;
    SIMD _xa11 = i32gather_ps((_xr +     _yi1).value, table.data(), sizeof(float)) * (1 - _xrem)
               + i32gather_ps((_xr + 1 + _yi1).value, table.data(), sizeof(float)) * _xrem;
    SIMD _xa12 = i32gather_ps((_xr +     _yi2).value, table.data(), sizeof(float)) * (1 - _xrem)
               + i32gather_ps((_xr + 1 + _yi2).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _xa11 = i32gather_ps((_xr + _yr * (4096 + 1) + _zr * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * (1 - _xrem)
    //    + i32gather_ps((_xr + 1 + _yr * (4096 + 1) + _zr * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _xa12 = i32gather_ps((_xr + (_yr + 1) * (4096 + 1) + _zr * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * (1 - _xrem)
    //    + i32gather_ps((_xr + 1 + (_yr + 1) * (4096 + 1) + _zr * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _xa21 = i32gather_ps((_xr + _yr * (4096 + 1) + (_zr + 1) * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * (1 - _xrem)
    //    + i32gather_ps((_xr + 1 + _yr * (4096 + 1) + (_zr + 1) * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _xa22 = i32gather_ps((_xr + (_yr + 1) * (4096 + 1) + (_zr + 1) * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * (1 - _xrem)
    //    + i32gather_ps((_xr + 1 + (_yr + 1) * (4096 + 1) + (_zr + 1) * ((4096 + 1) * (8 + 1))).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _ya1 = _xa11 * (1 - _yrem) + _xa12 * _yrem;
    //SIMD _ya2 = _xa21 * (1 - _yrem) + _xa22 * _yrem;

    //return _ya1 * (1 - _zrem) + _ya2 * _zrem;
    return _xa11 * (1 - _yrem) + _xa12 * _yrem;
};

static inline auto lookup3di2(const SIMD& x, const SIMD& y, const SIMD& z, const std::array<float, 2049 * 4 * 33 + 1>& table)
{
    SIMD _x = x * 2048;
    SIMDi _xr;
    SIMD _xrem;
    interpolate(_x, _xr, _xrem);

    SIMD _y = y * 3;
    SIMDi _yr;
    SIMD _yrem;
    interpolate(_y, _yr, _yrem);

    SIMDi _z = z.toInt();

    // Calculate indices to interpolate
    // Get data at all indices, and interpolate xaxis using xrem
    SIMDi _zi = _z * ((2048 + 1) * (3 + 1));
    SIMDi _yi1 = _yr * (2048 + 1) + _zi;
    SIMDi _yi2 = (_yr + 1) * (2048 + 1) + _zi;
    SIMD _xa11 = i32gather_ps((_xr +     _yi1).value, table.data(), sizeof(float)) * (1 - _xrem)
               + i32gather_ps((_xr + 1 + _yi1).value, table.data(), sizeof(float)) * _xrem;
    SIMD _xa12 = i32gather_ps((_xr +     _yi2).value, table.data(), sizeof(float)) * (1 - _xrem)
               + i32gather_ps((_xr + 1 + _yi2).value, table.data(), sizeof(float)) * _xrem;
    return _xa11 * (1 - _yrem) + _xa12 * _yrem;
};

static inline auto lookup2di2(const SIMD& x, const SIMD& y, const std::array<float, 100001 * 2 + 1>& table)
{
    SIMDi _x = ((x + 10) * 5000).toInt();
    //SIMD _x = (x + 10) * 5000;
    //SIMDi _xr;
    //SIMD _xrem;
    //interpolate(_x, _xr, _xrem);

    SIMD _y = y;
    SIMDi _yr;
    SIMD _yrem;
    interpolate(_y, _yr, _yrem);

    // Calculate indices to interpolate
    // Get data at all indices, and interpolate xaxis using xrem
    //SIMDi _yi1 = _yr * (100000 + 1);
    //SIMDi _yi2 = (_yr + 1) * (100000 + 1);
    //SIMD _xa11 = i32gather_ps((_xr +     _yi1).value, table.data(), sizeof(float)) * (1 - _xrem)
    //           + i32gather_ps((_xr + 1 + _yi1).value, table.data(), sizeof(float)) * _xrem;
    //SIMD _xa12 = i32gather_ps((_xr +     _yi2).value, table.data(), sizeof(float)) * (1 - _xrem)
    //           + i32gather_ps((_xr + 1 + _yi2).value, table.data(), sizeof(float)) * _xrem;
    SIMDi _yi1 = _yr * (100000 + 1);
    SIMDi _yi2 = (_yr + 1) * (100000 + 1);
    return i32gather_ps((_x + _yi1).value, table.data(), sizeof(float)) * (1 - _yrem)
         + i32gather_ps((_x + _yi2).value, table.data(), sizeof(float)) * _yrem;
};

static inline auto lookup2di1(const SIMD& x, const SIMD& y, const std::array<float, 1001 * 1001 + 1>& table)
{
    SIMD _x = x * 1000;
    SIMDi _xr;
    SIMD _xrem;
    interpolate(_x, _xr, _xrem);

    SIMDi _y = (y * 500.f + 500.f).toInt() * (1000 + 1);

    // Calculate indices to interpolate
    // Get data at all indices, and interpolate xaxis using xrem
    return i32gather_ps((_xr +     _y).value, table.data(), sizeof(float)) * (1 - _xrem)
         + i32gather_ps((_xr + 1 + _y).value, table.data(), sizeof(float)) * _xrem;
};

static inline auto lookup1di1(const SIMD& x, const std::array<float, 100001 + 1>& table)
{
    SIMDi _x = ((x + 5) * 10000).toInt();
    //SIMD _x = (x + 5) * 10000;
    //SIMDi _xr;
    //SIMD _xrem;
    //interpolate(_x, _xr, _xrem);

    // Calculate indices to interpolate
    // Get data at all indices, and interpolate xaxis using xrem
    //return i32gather_ps(_xr.value, table.data(), sizeof(float)) * (1 - _xrem)
    //    + i32gather_ps((_xr + 1).value, table.data(), sizeof(float))* _xrem;
    return i32gather_ps(_x.value, table.data(), sizeof(float));
};

using namespace Steinberg;

namespace Kaixo
{
    Processor::~Processor()
    {
        doModulationSync = false;
        modulationThread.join();
    }

    void Processor::TriggerVoice(int voice, int pitch, double velocity, bool legato)
    {
        // Get between 0 and 300 random numbers, this desyncs the voices, as they
        // all start with the same seed for the xorshift algorithm, this should make it more random per voice.
        for (int i = 0; i < std::rand() % 300; i++)
            voices[voice].myrandom();

        voices[voice].bendOffset = (params.goals[Params::PitchBend] * 2 - 1) * 24 * voices[voice].modulated[Params::Bend];

        voices[voice].rand = voices[voice].myrandom() * 0.5 + 0.5;
        voices[voice].velocity = velocity;
        voices[voice].frequency = legato ? voices[lastPressedVoice].frequency : pitch;
        voices[voice].pressedOld = legato ? voices[lastPressedVoice].frequency : pitch;
        voices[voice].pressed = pitch;
        lastPressedVoice = voice;

        if (params.goals[Params::Retrigger] > 0.5 || !voices[voice].env[0].Gate())
        {
            for (int i = 0; i < Oscillators; i++)
            {
                int _unisonMode = std::round(params.goals[Params::UnisonType1 + i] * 2);
                int _unison = std::round(params.goals[Params::UnisonCount1 + i] * 7) + 1;
                switch (_unisonMode)
                {
                case 0:
                case 1:
                {
                    double _phase = params.goals[Params::RandomPhase1 + i] > 0.5 ? voices[voice].myrandom() * 0.5 + 0.5 : 0;
                    for (int j = 0; j < Unison; j++)
                    {
                        double _mult = params.goals[Params::RandomPhase1 + i] > 0.5 ? 1.3 + 7 * (voices[voice].myrandom() * 0.5 + 0.5) : 5.3;
                        voices[voice].osc[i * Unison + j].phase = myfmod1(_phase + _mult * std::pow(j / (double)Unison, 2));
                    }
                    break;
                }
                case 2:
                {
                    double _phase = params.goals[Params::RandomPhase1 + i] > 0.5 ? voices[voice].myrandom() * 0.5 + 0.5 : 0;
                    for (int j = 0; j < Unison; j++)
                        voices[voice].osc[i * Unison + j].phase = _phase;
                    break;
                }
                }
            }
            for (int i = 0; i < LFOs; i++)
                voices[voice].lfo[i].phase = 0;
            voices[voice].sub.phase = 0;
            voices[voice].samplesPress = projectTimeSamples;
        }

        for (int i = 0; i < Envelopes; i++)
        {
            voices[voice].env[i].settings.legato = params.goals[Params::Retrigger] < 0.5;
            voices[voice].env[i].Gate(true);
        }
    }

    void Processor::NotePress(Vst::Event& event)
    {
        if (params.goals[Params::Voices] < 0.5)
        {
            TriggerVoice(0, event.noteOn.pitch, event.noteOn.velocity, !m_MonoNotePresses.empty());
            m_MonoNotePresses.push_back(event.noteOn.pitch);
            return;
        }

        // Release the longest held note
        if (m_Available.size() == 0)
        {
            int longestheld = m_Pressed.back();
            m_Pressed.pop_back();

            // Set note to -1 and emplace to available.
            m_Notes[longestheld] = -1;
            m_Available.emplace(m_Available.begin(), longestheld);
        }

        // Get an available voice
        if (!m_Available.empty())
        {
            int voice = m_Available.back();
            m_Available.pop_back();

            // Emplace it to pressed voices queue
            m_Pressed.emplace(m_Pressed.begin(), voice);

            // Set voice to note
            m_Notes[voice] = event.noteOn.pitch;
            {
                TriggerVoice(voice, event.noteOn.pitch, event.noteOn.velocity, m_Pressed.size() > 1);
            }
        }
    }

    void Processor::ReleaseVoice(int voice, int pitch, double velocity)
    {
        for (int i = 0; i < Envelopes; i++)
            voices[voice].env[i].Gate(false);
    }

    void Processor::NoteRelease(Vst::Event& event)
    {
        // If monophonic
        if (params.goals[Params::Voices] < 0.5)
        {
            // If we released a note that isn't currently producing sound (because monophonic)
            // we will remove it from the notepresses
            if (voices[0].pressed != event.noteOff.pitch)
            {
                m_MonoNotePresses.remove(event.noteOff.pitch);
            }
            else
            {
                // Otherwise we'll remove the released note from the stack
                if (!m_MonoNotePresses.empty())
                    m_MonoNotePresses.pop_back();

                // And if notes remain on the stack, we'll trigger that one again.
                if (!m_MonoNotePresses.empty())
                    TriggerVoice(0, m_MonoNotePresses.back(), voices[0].velocity, true);

                // Otherwise release the voice
                else
                    ReleaseVoice(0, event.noteOff.pitch, event.noteOff.velocity);
            }

            // Clear polyphonic voices
            for (int i = 0; i < Voices; i++)
                if (m_Notes[i] != -1)
                    m_Available.push_back(i), m_Notes[i] = -1;

            return;
        }
        else
        {
            // If not monophonic, empty note stack
            if (!m_MonoNotePresses.empty())
            {
                ReleaseVoice(0, m_MonoNotePresses.back(), voices[0].velocity);
                m_MonoNotePresses.clear();
            }
        }

        // Find the note in the pressed notes per voice
        while (true)
        {
            auto it = std::find(m_Notes.begin(), m_Notes.end(), event.noteOff.pitch);
            if (it != m_Notes.end())
            {
                // If it was pressed, get the voice index
                int voice = std::distance(m_Notes.begin(), it);

                // Set note to -1 and emplace to available.
                ReleaseVoice(voice, event.noteOff.pitch, event.noteOff.velocity);
                m_Notes[voice] = -1;
                m_Available.emplace(m_Available.begin(), voice);

                // Erase it from the pressed queue
                auto it2 = std::find(m_Pressed.begin(), m_Pressed.end(), voice);
                if (it2 != m_Pressed.end())
                    m_Pressed.erase(it2);
            }
            else break;
        }
    }

    void Processor::Event(Vst::Event& event)
    {
        switch (event.type)
        {
        case Event::kNoteOnEvent:
            //if (params.goals[Params::PitchBend] > 1 || params.goals[Params::PitchBend] < 0) params.goals[Params::PitchBend] = 0.5;

            //params.goals[Params::PitchBend] = constrain(params.goals[Params::PitchBend]);

            NotePress(event);
            break;
        case Event::kNoteOffEvent:

            NoteRelease(event);
            break;
        }
    }

    void Processor::ModSync()
    {   // Gets ran in thread, sends messages for modulation to controller.
        while (doModulationSync)
        {   // Loop over mod parameters
            for (int m = 0; m < Params::ModCount; m++)
            {   // Allocate a message
                if (auto message = Steinberg::owned(allocateMessage()))
                {   // Set id and correct content, then send
                    message->setMessageID(UPDATE_MODULATION);
                    message->getAttributes()->setInt(UPDATE_MODULATION_PARAM, m);
                    message->getAttributes()->setFloat(UPDATE_MODULATION_VALUE, voices[lastPressedVoice].modulated[m]);
                    sendMessage(message);
                }
            }

            // Approximately 60 times a second.
            std::this_thread::sleep_for(std::chrono::milliseconds{ 30 });
        }
    }

    double Processor::GenerateSample(size_t channel, ProcessData& data, Voice& voice, double ratio, int osi)
    {   // If the gain envelope is done, no more sound, so return 0.
        if (voice.env[0].Done()) return 0;

        if (channel == 0)
        {
            // Sub oscillator
            voice.sub.sample = voice.sub.OffsetClean(0);
        }

        // Oscillator panning and sending to destination
        double _cs[Combines * 2 + 1]{ 0, 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < Oscillators; i++)
        {
            double _vs = voice.oscs[channel][i][osi];

            // Pan
            if (voice.modulated[Params::Pan1 + i] != 0.5)
                _vs *= std::min((channel == 1 ? 2 * voice.modulated[Params::Pan1 + i] : 2 - 2 * voice.modulated[Params::Pan1 + i]), 1.); // Panning

            size_t _dests = params.goals[Params::DestA + i] * 128.;
            for (int j = 0; j < Combines * 2 + 1; j++)
                if ((_dests >> j) & 1U) _cs[j] += _vs;
        }

        // Result, start with sub, rest will be added later.
        double _res = voice.modulated[Params::SubGain] * voice.sub.sample; // Start with sub

        // All the combiners
        for (int i = 0; i < Combines; i++)
        {
            constexpr static size_t _offsets[]{ 5, 3, 0 };
            constexpr static size_t _mult[]{ 32, 8, 0 };

            // Do combine, apply pre-gain
            double _v = Combine(
                _cs[i * 2] * voice.modulated[Params::PreGainX + i] * 2,
                _cs[i * 2 + 1] * voice.modulated[Params::PreGainX + i] * 2,
                i, voice);

            // Fold
            if (params.goals[Params::ENBFoldX + i] > 0.5)
                _v = Shapers::fold(_v * (voice.modulated[Params::FoldX + i] * 15 + 1), voice.modulated[Params::BiasX + i] * 2 - 1);

            // Drive
            if (params.goals[Params::ENBDriveX + i] > 0.5)
                _v = Shapers::drive(_v, voice.modulated[Params::DriveGainX + i] * 3 + 1, voice.modulated[Params::DriveAmtX + i]);
            else // Always clip, even if drive disabled, to prevent unstable filter
                _v = constrain(_v, -16., 16.);

            // Volume
            _v = _v * voice.modulated[Params::GainX + i] * 2;

            // Filter
            if (params.goals[Params::ENBFilterX + i] > 0.5)
                _v = voice.cfilter[i].Apply(_v, channel);

            // DCoffset
            _v = params.goals[Params::DCX + i] > 0.5 ? voice.dcoff[i].Apply(_v, channel) : _v;

            // Apply AAF if oversampling
            //if (params.goals[Params::Oversample] > 0)
            //    _v = voice.aaf[6 + i + channel * Combines].Apply(_v, voice.aafp[6 + i + channel * Combines]);

            if (i == Combines - 1) // if combiner 'Z', add to output and break.
            {
                _res += _v;
                break;
            }

            // Send combiner output to destinations
            const size_t _dests = params.goals[Params::DestX + i] * _mult[i];
            for (int j = 0; j < _offsets[i]; j++)
                if ((_dests >> j) & 1U) _cs[j + (i + 1) * 2] += _v;
        }

        // Add the signals that were sent directly to output.
        _res += _cs[Combines * 2];

        // Clip sample, apply gain envelope and return.
        return voice.env[0].Apply(_res, channel) * voice.modulated[Params::GlobalGain] * 2;
    }

    void Processor::Generate(ProcessData& data, size_t s)
    {
        processData = &data; // Store the process data, so it's accessible everywhere.
        projectTimeSamples = data.processContext->projectTimeSamples; // Store project time samples

        // Prepare buffers
        for (auto& i : voices)
            i.buffer.prepare(s);


        // If monophonic, only generate 1st voice.
        if (params.goals[Params::Voices] < 0.5)
        {
            GenerateVoice(voices[0]);

            if (!monophonic)
            {
                for (int i = 1; i < Voices; i++)
                    voices[i].Reset(); // Reset all other (inactive) voices
                monophonic = true;
            }
        }

        // If no threading, do everything on main thread
        else if (params.goals[Params::Threading] < 0.5)
        {
            monophonic = false;
            for (int i = 0; i < Voices; i++)
                GenerateVoice(voices[i]);
        }
        else
        {
            monophonic = false;

            // Divide voices among worker threads and main thread
            std::future<void> futures[Voices];
            bool onMain[Voices]{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
            int forDone = 0;
            for (int i = 0; i < Voices; i++)
            {
                if (swapBuffer.amount < 52) onMain[i] = true; // If buffer < 52, do work on main thread always
                else if (!voices[i].env[0].Done() && forDone < 1) onMain[i] = true, forDone++; // Always do 1 voice on main thread
                else if (voices[i].env[0].Done()) onMain[i] = true; // If no generating, do on main thread
                else futures[i] = voiceThreadPool.push([this, i]() { GenerateVoice(voices[i]); }); // Otherwise send work to threadpool
            }

            // Generate everything that was selected for main thread right here.
            for (int i = 0; i < Voices; i++)
                if (onMain[i])
                    GenerateVoice(voices[i]);

            // Wait on threadpool.
            for (int i = 0; i < Voices; i++)
                if (!onMain[i])
                    futures[i].wait();
        }

        // Finally, sum all voices
        for (auto& i : voices)
            for (int j = 0; j < s; j++)
                swapBuffer.left[j] += i.buffer.left[j],
                swapBuffer.right[j] += i.buffer.right[j];
    }


    inline double Processor::Combine(double a, double b, int index, Voice& voice)
    {
        const uint64_t _a = std::bit_cast<uint64_t>(a);
        const uint64_t _b = std::bit_cast<uint64_t>(b);
        const uint64_t _as = a * 4294967296;
        const uint64_t _bs = b * 4294967296;

        // Calculate all the combines
        const double sum =
            voice.modulated[Params::MinMixX + index] * 1.4 * (std::min(a, b))
            + voice.modulated[Params::MaxMixX + index] * 1.3 * (std::max(a, b))
            + voice.modulated[Params::AndMixX + index] * 1.5 * (std::bit_cast<double>(_a & _b))
            + voice.modulated[Params::XOrMixX + index] * 1.7 * ((((uint64_t)(a * 4294967296) ^ (uint64_t)(b * 4294967296)) % 4294967296) / 4294967296. - 0.5)
            + voice.modulated[Params::OrMixX + index] * 0.3 * std::bit_cast<double>(_a | _b)
            + voice.modulated[Params::MultMixX + index] * a * b * 1.8
            + voice.modulated[Params::PongMixX + index] * (1.2 * (a > 0 ? a : b < 0 ? b : 0))
            + voice.modulated[Params::ModMixX + index] * 1.4 * (std::bit_cast<double>(_a % (_b == 0 ? 1 : _b)))
            + voice.modulated[Params::InlvMixX + index] * 0.7 * (std::bit_cast<double>((_a & 0x5555555555555555) | (_b & 0xAAAAAAAAAAAAAAAA)))
            + voice.modulated[Params::AddMixX + index] * (a + b);

        // Return the combined signals, taking into account the mix value for this combiner.
        return voice.modulated[Params::MixX + index] * sum + (1 - voice.modulated[Params::MixX + index]) * (a + b);
    }

    double Processor::CombineSingle(double a, double b, int mode)
    {
        // Calculate a couple helper values
        const uint64_t _a = std::bit_cast<uint64_t>(a);
        const uint64_t _b = std::bit_cast<uint64_t>(b);
        const uint64_t _as = a * 4294967296;
        const uint64_t _bs = b * 4294967296;
        switch (mode) { // Switch to the combine mode and calculate result
        case AND:  return 1.5 * (std::bit_cast<double>(_a & _b));
        case OR: { double _ab = 0.3 * (std::bit_cast<double>(_a | _b)); if (std::isnan(_ab)) _ab = 0; return _ab; } // Special case for 'OR' because it can potentially generate NANs
        case XOR:  return 1.7 * (((_as ^ _bs) % 4294967296) / 4294967296. - 0.5);
        case PONG: return 1.2 * (a > 0 ? a : b < 0 ? b : 0); break;
        case INLV: return 0.7 * (std::bit_cast<double>((_a & 0x5555555555555555) | (_b & 0xAAAAAAAAAAAAAAAA)));
        case MIN:  return 1.4 * (std::min(a, b));
        case MAX:  return 1.3 * (std::max(a, b));
        case MOD:  return 1.4 * (std::bit_cast<double>(_a % (_b == 0 ? 1 : _b)));
        case MULT: return 1.8 * (a * b);
        case ADD:  return (a + b);
        }
    }

    void Processor::CalculateModulation(Voice& voice, double ratio)
    {   // If ratio == 1, move goals, because it's more efficient.
        if (ratio == 1) std::memcpy(voice.modulated, params.goals, Params::ModCount * sizeof(double));

        for (int m = 0; m < Params::ModCount; m++)
        {
            // only copy value if ration != 1, because we already did that.
            if (ratio != 1) voice.modulated[m] = params[{ m, ratio }];

            // If this param doesn't have modulation, continue. (Extra state, but makes it more efficient)
            if (!hasmod[m]) continue;

            // Go through all mods of this parameter
            bool edited = false; // Keep track if it was edited
            for (int i = 0; i < ModAmt; i++)
            {   // Calculate index
                int index = m * ModAmt + i;
                if (!modgoals[index]) continue; // If no goal, continue

                // Get the source
                int source = (modgoals[index] * (int)ModSources::Amount + 0.1);
                edited = true; // Since we had a goal, we're gonna edit.

                double amount = modamount[index] * 2 - 1; // Calculate mod amount
                if (source == (int)ModSources::Ran)
                {   // Random * amount
                    voice.modulated[m] += (voice.rand * amount);
                }
                else if (source == (int)ModSources::Mod)
                {   // Modwheel * amount
                    voice.modulated[m] += (params[{ Params::ModWheel, ratio }] * amount);
                }
                else if (source == (int)ModSources::Vel)
                {   // Velocity * amount
                    voice.modulated[m] += (voice.velocity * amount);
                }
                else if (source == (int)ModSources::Key)
                {   // Key also takes into account bend and transpose
                    voice.modulated[m] += (voice.key * amount);
                }
                else if (source >= (int)ModSources::Mac1)
                {   // Macro, first calculate macro index, then multiply amount
                    int mindex = (source - (int)ModSources::Mac1);
                    voice.modulated[m] += (voice.modulated[Params::Macro1 + mindex] * amount);
                }
                else if (source >= (int)ModSources::Env1)
                {   // Envelopes, calculate index, then multiply amount
                    int mindex = (source - (int)ModSources::Env1);
                    voice.modulated[m] += (voice.env[mindex].sample * amount);
                }
                else if (source >= (int)ModSources::LFO1)
                {   // LFO, calculate index, multiply amount
                    int mindex = (source - (int)ModSources::LFO1);
                    voice.modulated[m] += (voice.lfo[mindex].sample * amount);
                }
            }

            // If we need to constrain, and have edited, constrain the value.
            if (edited && ParamNames[m].constrain)
                voice.modulated[m] = constrain(voice.modulated[m], 0., 1.);
        }
    }

    void Processor::UpdateComponentParameters(Voice& voice, double ratio)
    {   // Get BPM from process context if valid, otherwise just use 128
        double bpm = (processData->processContext->state & ProcessContext::kTempoValid) ? processData->processContext->tempo : 128;

        // Calculate bend offset and time mult.
        const double _bendRatio = 50. / processData->processContext->sampleRate;
        voice.bendOffset = voice.bendOffset * (1 - _bendRatio) + _bendRatio * ((params[{ Params::PitchBend, ratio }] * 2 - 1) * 24 * voice.modulated[Params::Bend]);
        const double _bendOffset = voice.bendOffset + voice.modulated[Params::Transpose] * 96 - 48;
        const double _timeMult = voice.modulated[Params::Time] < 0.5 ? (voice.modulated[Params::Time] + 0.5) : ((voice.modulated[Params::Time] - 0.5) * 2 + 1);

        // Frequency glide
        voice.deltaf = _timeMult * std::abs((voice.pressed - voice.pressedOld) / (0.001 + voice.modulated[Params::Glide] *
            voice.modulated[Params::Glide] * voice.modulated[Params::Glide] * 10 * processData->processContext->sampleRate));
        if (voice.frequency > voice.pressed)
            if (voice.frequency - voice.deltaf < voice.pressed) voice.frequency = voice.pressed;
            else voice.frequency -= voice.deltaf;

        if (voice.frequency < voice.pressed)
            if (voice.frequency + voice.deltaf > voice.pressed) voice.frequency = voice.pressed;
            else voice.frequency += voice.deltaf;

        voice.key = (voice.frequency + _bendOffset) / 127.;

        for (int i = 0; i < Combines; i++)
        {   // Combines parameters
            voice.dcoffp[i].f0 = 35;
            voice.dcoffp[i].type = FilterType::HighPass;
            voice.dcoffp[i].Q = 1;
            voice.dcoffp[i].sampleRate = voice.samplerate;
            voice.dcoffp[i].RecalculateParameters(ratio == 1);

            auto _ft = std::floor(params.goals[Params::FilterX + i] * 3); // Filter parameters
            voice.cfilterp[i].sampleRate = voice.samplerate;
            voice.cfilterp[i].f0 = voice.modulated[Params::FreqX + i] * voice.modulated[Params::FreqX + i] * (22000 - 30) + 30;
            voice.cfilterp[i].Q = voice.modulated[Params::ResoX + i] * 16 + 1;
            voice.cfilterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
            voice.cfilterp[i].RecalculateParameters(ratio == 1);
        }

        for (int i = 0; i < Envelopes; i++)
        {   // Adjust Envelope parameters and generate
            voice.env[i].SAMPLE_RATE = processData->processContext->sampleRate;
            voice.env[i].settings.attack = voice.modulated[Params::Env1A + i] * voice.modulated[Params::Env1A + i] * voice.modulated[Params::Env1A + i] * 5;
            voice.env[i].settings.attackCurve = voice.modulated[Params::Env1AC + i] * 2 - 1;
            voice.env[i].settings.attackLevel = voice.modulated[Params::Env1AL + i];
            voice.env[i].settings.decay = voice.modulated[Params::Env1D + i] * voice.modulated[Params::Env1D + i] * voice.modulated[Params::Env1D + i] * 5;
            voice.env[i].settings.decayCurve = voice.modulated[Params::Env1DC + i] * 2 - 1;
            voice.env[i].settings.decayLevel = voice.modulated[Params::Env1DL + i];
            voice.env[i].settings.sustain = voice.modulated[Params::Env1S + i];
            voice.env[i].settings.release = voice.modulated[Params::Env1R + i] * voice.modulated[Params::Env1R + i] * voice.modulated[Params::Env1R + i] * 5;
            voice.env[i].settings.releaseCurve = voice.modulated[Params::Env1RC + i] * 2 - 1;
            voice.env[i].settings.timeMult = _timeMult;
            voice.env[i].Generate(0);
        }

        for (int i = 0; i < LFOs; i++)
        {   // Adjust lfo parameters 
            voice.lfo[i].SAMPLE_RATE = processData->processContext->sampleRate;

            if (params.goals[Params::LFOSync1 + i] > 0.5) // If bpm synced lfo
            {
                size_t _type = std::floor(voice.modulated[Params::LFORate1 + i] * (TimesAmount - 1));

                double _ps = (60. / bpm) * TimesValue[_type] * 4; // Seconds per oscillation
                double _f = 1 / _ps; // Frequency of the oscillations
                double _spp = _ps * processData->processContext->sampleRate; // Samples per oscillation
                double _phase = std::fmod((params.goals[Params::LFORetr1 + i] > 0.5 ? projectTimeSamples - voice.samplesPress : projectTimeSamples) + swapBuffer.amount * ratio, _spp) / _spp;

                // If playing, sync phase to samples played
                if (processData->processContext->state & ProcessContext::kPlaying)
                    voice.lfo[i].phase = _phase;
                voice.lfo[i].settings.frequency = _f;
            }
            else
            {
                double _f = _timeMult * voice.modulated[Params::LFORate1 + i] * voice.modulated[Params::LFORate1 + i] * 29.9 + 0.1; // Frequency of the oscillations
                double _ps = 1 / _f; // Seconds per oscillation
                double _spp = _ps * processData->processContext->sampleRate; // Samples per oscillation
                double _phase = std::fmod((params.goals[Params::LFORetr1 + i] > 0.5 ? projectTimeSamples - voice.samplesPress : projectTimeSamples) + swapBuffer.amount * ratio, _spp) / _spp;

                // If playing, sync phase to samples played
                if (processData->processContext->state & ProcessContext::kPlaying)
                    voice.lfo[i].phase = _phase;
                voice.lfo[i].settings.frequency = _f;
            }

            voice.lfo[i].settings.wtpos = voice.modulated[Params::LFOPos1 + i];
            voice.lfo[i].settings.shaper3 = voice.modulated[Params::LFOShaper1 + i];
            voice.lfo[i].sample = voice.lfo[i].OffsetSimple(voice.modulated[Params::LFOPhase1 + i]) * (voice.modulated[Params::LFOLvl1 + i] * 2 - 1);
        }

        for (int i = 0; i < Oscillators; i++)
        {
            // Oscillator parameters
            auto _ft = std::floor(params.goals[Params::Filter1 + i] * 3); // Filter parameters
            voice.filterp[i].sampleRate = voice.samplerate;
            voice.filterp[i].f0 = voice.modulated[Params::Freq1 + i] * voice.modulated[Params::Freq1 + i] * (22000 - 30) + 30;
            voice.filterp[i].Q = voice.modulated[Params::Reso1 + i] * 16 + 1;
            voice.filterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
            voice.filterp[i].RecalculateParameters(ratio == 1);

            int _unison = std::round(params.goals[Params::UnisonCount1 + i] * 7) + 1;
            voice.unison[i] = _unison;
            int _unisonMode = std::round(params.goals[Params::UnisonType1 + i] * 2);
            for (int j = 0; j < Unison; j++)
            {
                double foff = _unison == 1 ? 0 : 2 * voice.modulated[Params::UnisonDetun1 + i] * (j / (double)(_unison - 1) - 0.5);

                if (_unisonMode == 1) foff = (voice.myrandom() - 0.1) * voice.modulated[Params::UnisonDetun1 + i] * 8;

                voice.osc[i * Unison + j].SAMPLE_RATE = voice.samplerate;
                voice.osc[i * Unison + j].settings.frequency = noteToFreq(foff + voice.frequency + _bendOffset
                    + voice.modulated[Params::Detune1 + i] * 4 - 2 + voice.modulated[Params::Pitch1 + i] * 48 - 24);
                voice.osc[i * Unison + j].settings.wtpos = voice.modulated[Params::WTPos1 + i];
                voice.osc[i * Unison + j].settings.sync = voice.modulated[Params::Sync1 + i];
                voice.osc[i * Unison + j].settings.pw = voice.modulated[Params::PulseW1 + i];
                voice.osc[i * Unison + j].settings.bend = voice.modulated[Params::Bend1 + i];
                voice.osc[i * Unison + j].settings.shaper = voice.modulated[Params::Shaper1 + i];
                voice.osc[i * Unison + j].settings.shaperMix = voice.modulated[Params::ShaperMix1 + i] * (params.goals[Params::ENBShaper1 + i] > 0.5);
                voice.osc[i * Unison + j].settings.shaper2 = voice.modulated[Params::Shaper21 + i];
                voice.osc[i * Unison + j].settings.shaper2Mix = voice.modulated[Params::Shaper2Mix1 + i] * (params.goals[Params::ENBShaper1 + i] > 0.5);
                voice.osc[i * Unison + j].settings.shaperMorph = voice.modulated[Params::ShaperMorph1 + i];
                voice.osc[i * Unison + j].settings.panning = voice.unison[i] == 1 ? 0.5 : (j / (double)(voice.unison[i] - 1) - 0.5) * voice.modulated[Params::UnisonWidth1 + i] + 0.5;
            }
        }

        { // Sub oscillator
            int _octave = std::round(params.goals[Params::SubOct] * 4 - 2);
            voice.sub.SAMPLE_RATE = voice.samplerate;
            voice.sub.settings.frequency = noteToFreq(voice.frequency + _octave * 12 + _bendOffset);
            voice.sub.settings.wtpos = voice.modulated[Params::SubOvertone];
        }
    }

    inline void Processor::GenerateOscillator(Voice& voice, int os)
    {
        
#ifdef USE_SIMD
        std::fill_n(voice.memory._ovsinA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._phasoA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._dcoffA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._enbflA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._fldgaA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._fldbiA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._enbdrA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._drvgaA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._drvshA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._pulswA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._bendaA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._syncaA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._shap1A, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._shap2A, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._morphA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._shmx1A, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._shmx2A, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._phaseA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._wtposA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._freqcA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._gainsA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._panniA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._makeuA, Oscillators * Unison * 8, 0);
        std::fill_n(voice.memory._destL, Oscillators * Unison * 8, nullptr);
        std::fill_n(voice.memory._destR, Oscillators * Unison * 8, nullptr);

        // Count amount of voices
        int _unisonVoices = 0;
        int _index = 0;
        for (int i = 0; i < Oscillators; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                voice.oscs[0][i][j] = 0;
                voice.oscs[1][i][j] = 0;
            }

            if (params.goals[Params::Enable1 + i] < 0.5)
                continue;

            auto makeup = 2 * 3. / (voice.unison[i] + 2.);

            std::fill_n(&voice.memory._makeuA[_index], voice.unison[i] * os, makeup);
            std::fill_n(&voice.memory._phasoA[_index], voice.unison[i] * os, voice.modulated[Params::Phase1 + i]);
            std::fill_n(&voice.memory._dcoffA[_index], voice.unison[i] * os, voice.modulated[Params::DCOff1 + i] * 2 - 1);
            std::fill_n(&voice.memory._enbflA[_index], voice.unison[i] * os, params.goals[Params::ENBFold1 + i]);
            std::fill_n(&voice.memory._fldgaA[_index], voice.unison[i] * os, voice.modulated[Params::Fold1 + i] * 15 + 1);
            std::fill_n(&voice.memory._fldbiA[_index], voice.unison[i] * os, voice.modulated[Params::Bias1 + i] * 2 - 1);
            std::fill_n(&voice.memory._enbdrA[_index], voice.unison[i] * os, params.goals[Params::ENBDrive1 + i]);
            std::fill_n(&voice.memory._drvgaA[_index], voice.unison[i] * os, voice.modulated[Params::DriveGain1 + i] * 3 + 1);
            std::fill_n(&voice.memory._drvshA[_index], voice.unison[i] * os, voice.modulated[Params::DriveAmt1 + i]);
            std::fill_n(&voice.memory._pulswA[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.pw * 2 - 1);
            std::fill_n(&voice.memory._bendaA[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.bend * 2 - 1);
            std::fill_n(&voice.memory._syncaA[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.sync * 7 + 1);
            std::fill_n(&voice.memory._shap1A[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.shaper);
            std::fill_n(&voice.memory._shap2A[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.shaper2);
            std::fill_n(&voice.memory._morphA[_index], voice.unison[i] * os, params.goals[Params::ShaperFreez1 + i] > 0.5 ? 3 * voice.osc[i * Unison].settings.shaperMorph * (30 / voice.osc[i * Unison].settings.frequency) : voice.osc[i * Unison].settings.shaperMorph);
            std::fill_n(&voice.memory._shmx1A[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.shaperMix);
            std::fill_n(&voice.memory._shmx2A[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.shaper2Mix);
            std::fill_n(&voice.memory._wtposA[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.wtpos);
            std::fill_n(&voice.memory._freqcA[_index], voice.unison[i] * os, voice.osc[i * Unison].settings.frequency);
            std::fill_n(&voice.memory._gainsA[_index], voice.unison[i] * os, voice.modulated[Params::Volume1 + i]);
            std::fill_n(&voice.memory._destL[_index], voice.unison[i] * os, &voice.oscs[0][i][0]);
            std::fill_n(&voice.memory._destR[_index], voice.unison[i] * os, &voice.oscs[1][i][0]);

            for (int j = 0; j < voice.unison[i]; j++)
            {
                const int _oi = i * Unison + j;
                auto& osc = voice.osc[_oi];
                for (int k = 0; k < os; k++)
                {
                    voice.memory._panniA[_index] = osc.settings.panning;
                    voice.memory._ovsinA[_index] = k;
                    voice.memory._phaseA[_index] = osc.phase;
                    _index++;

                    osc.phase = myfmod1(osc.phase + osc.settings.frequency / voice.samplerate);
                }
            }
            _unisonVoices += voice.unison[i] * os;
        }

        for (int i = 0; i < Oscillators * Unison * 8; i += 8)
        {
            SIMD _oscFreq = &voice.memory._freqcA[i];

            SIMD _morph = &voice.memory._morphA[i];
            SIMD _shap1 = &voice.memory._shap1A[i];
            SIMD _shap2 = &voice.memory._shap2A[i];
            SIMD _shmx1 = &voice.memory._shmx1A[i];
            SIMD _shmx2 = &voice.memory._shmx2A[i];
            SIMD _pulsw = &voice.memory._pulswA[i];
            SIMD _synca = &voice.memory._syncaA[i];
            SIMD _benda = &voice.memory._bendaA[i];

            SIMD _phaso = &voice.memory._phasoA[i];
            SIMD _dcoff = &voice.memory._dcoffA[i];
            SIMD _enbfl = &voice.memory._enbflA[i];
            SIMD _fldga = &voice.memory._fldgaA[i];
            SIMD _fldbi = &voice.memory._fldbiA[i];
            SIMD _enbdr = &voice.memory._enbdrA[i];
            SIMD _drvga = &voice.memory._drvgaA[i];
            SIMD _drvsh = &voice.memory._drvshA[i];
            SIMD _phasedata = &voice.memory._phaseA[i];

            SIMD _gain = &voice.memory._gainsA[i];

            SIMD _pw = _pulsw > 0.f;
            SIMD _d = simdmax(0.000001f,
                (_pw & (1 - _pulsw)) | // If pw < 0 : 1 - pw
                ((~_pw) & (1 + _pulsw))); //        else 1 + pw

            SIMD _freq = _oscFreq * _synca;
            SIMD _freqc = simdconstrain(_freq.log2() * 2, 0.f, 32.f);

            _phasedata = _phasedata * (1 - _shmx1) + _shmx1 * lookup3di(_phasedata, _shap1, _morph, Shapers::getMainPhaseShaper());

            SIMD _pwdone =
                (_pw & ((_phasedata / _d) > 1.f)) | // If pw < 0 : phase / _d > 1
                ((~_pw) & (((1 - _phasedata) / _d) > 1.f));  //  else (1 - phase) / _d > 1

            _phasedata = simdmyfmod1(
                (_pw & ((_phasedata / _d) + _phaso)) |
                ((~_pw) & (((_phasedata + _pulsw) / _d) + _phaso))
            );

            _phasedata = simdmyfmod1(lookup2di1(_phasedata, _benda, Shapers::getPowerCurve()) * _synca);

            // calculate the index of the frequency in the wavetable using log and scalar
            SIMD _wavetable = lookup3di2(_phasedata, &voice.memory._wtposA[i], _freqc, Wavetables::getBasicTable());

            _wavetable =
                (_pwdone & 0.f) | // If pulsewidth done, value 0, otherwise other stuff
                ((~_pwdone) & (_wavetable * (1 - _shmx2) + _shmx2 * lookup3di(_wavetable, _shap2, _morph, Shapers::getMainWaveShaper())));


            SIMD _result = (_wavetable + _dcoff);

            SIMD _cond = _enbfl > 0.5;

            _result = // Fold
                (_cond & lookup1di1(simdmyfmod1(0.25 * (_result * _fldga + _fldbi)) * 4.f, Shapers::getFold())) |
                ((~_cond) & _result);

            _cond = _enbdr > 0.5;
            _result = // Drive
                (_cond & lookup2di2(simdconstrain(_result * _drvga, -10.f, 10.f), _drvsh, Shapers::getDrive())) |
                ((~_cond) & simdconstrain(_result, -1.f, 1.f));

            _result = _result * _gain;

            // Get final values
            float _finalData[8];
            storeu_ps(_finalData, _result.value);

            // Get new phases
            for (int j = 0; j < 8; j++)
            {
                if (voice.memory._destL[i + j])
                {   // Assign new phase, and set generated audio to destination
                    (voice.memory._destL[i + j][voice.memory._ovsinA[i + j]]) += _finalData[j] * voice.memory._makeuA[i + j] * voice.memory._panniA[i + j];
                    (voice.memory._destR[i + j][voice.memory._ovsinA[i + j]]) += _finalData[j] * voice.memory._makeuA[i + j] * (1 - voice.memory._panniA[i + j]);
                }
            }

            if (i + 8 >= _unisonVoices) break;
        }

        for (int k = 0; k < os; k++)
        {
            for (int i = 0; i < Oscillators; i++)
            {
                if (params.goals[Params::ENBFilter1 + i] > 0.5)
                {
                    voice.oscs[0][i][k] = voice.filter[i].Apply(voice.oscs[0][i][k], 0); // Apply pre-combine filter
                    voice.oscs[1][i][k] = voice.filter[i].Apply(voice.oscs[1][i][k], 1); // Apply pre-combine filter
                }
            }
        }

#else
        for (int k = 0; k < os; k++)
        {
            for (int i = 0; i < Oscillators; i++)
            {
                int _unison = std::round(params.goals[Params::UnisonCount1 + i] * 7) + 1;

                voice.oscs[0][i][k] = 0;
                voice.oscs[1][i][k] = 0;
                //for (int j = 0; j < Unison; j++)
                for (int j = 0; j < _unison; j++)
                {
                    double _panning = _unison == 1 ? 0.5 : (j / (double)(_unison - 1) - 0.5) * voice.modulated[Params::UnisonWidth1 + i] + 0.5;

                    // Oscillator generate
                    if (params.goals[Params::Enable1 + i] > 0.5) // Generate oscillator sound
                        voice.osc[i * Unison + j].sample = voice.osc[i * Unison + j].Offset(myfmod1(voice.modulated[Params::Phase1 + i] + 5), params.goals[Params::ENBShaper1 + i] > 0.5, params.goals[Params::ShaperFreez1 + i] > 0.5);
                    else
                    {
                        voice.osc[i * Unison + j].sample = 0;
                        voice.oscs[0][i][k] = 0;
                        voice.oscs[1][i][k] = 0;
                        break;
                    }

                    double _v = voice.osc[i * Unison + j].sample + voice.modulated[Params::DCOff1 + i] * 2 - 1;

                    // Fold
                    if (params.goals[Params::ENBFold1 + i] > 0.5)
                        _v = Shapers::fold(_v * (voice.modulated[Params::Fold1 + i] * 15 + 1), voice.modulated[Params::Bias1 + i] * 2 - 1);

                    // Drive
                    if (params.goals[Params::ENBDrive1 + i] > 0.5)
                        _v = Shapers::drive(_v, voice.modulated[Params::DriveGain1 + i] * 3 + 1, voice.modulated[Params::DriveAmt1 + i]);

                    // Gain
                    _v *= voice.modulated[Params::Volume1 + i]; // Adjust for volume

                    // Apply AAF if oversampling
                    //if (params.goals[Params::Oversample] > 0)
                    //    voice.oscs[i] = voice.aaf[2 + i].Apply(voice.oscs[i], voice.aafp[2 + i]);

                    double _makeup = 3. / (_unison + 2.);

                    voice.oscs[0][i][k] += 2 * _makeup * _v * _panning;
                    voice.oscs[1][i][k] += 2 * _makeup * _v * (1 - _panning);
                }

                // Filter
                if (params.goals[Params::ENBFilter1 + i] > 0.5)
                {
                    voice.oscs[0][i][k] = voice.filter[i].Apply(voice.oscs[0][i][k], 0); // Apply pre-combine filter
                    voice.oscs[1][i][k] = voice.filter[i].Apply(voice.oscs[1][i][k], 1); // Apply pre-combine filter
                }
            }
        }
#endif
    }

    inline void Processor::GenerateVoice(Voice& voice)
    {   
        // Calculate oversample amount
        size_t _index = std::floor(params.goals[Params::Oversample] * 4);
        size_t _osa = _index == 0 ? 1 : _index == 1 ? 2 : _index == 2 ? 4 : 8;
        if (processData->processMode & ProcessModes::kOffline) _osa = 16;

        // Set oversampled sample rate in voice
        voice.samplerate = processData->processContext->sampleRate * _osa;

        // If envelope is done, no sound so return, but do calculate modulations
        if (voice.env[0].Done())
        {
            CalculateModulation(voice, 1);
            UpdateComponentParameters(voice, 1);
            return;
        }

        // Generate the necessary sample amount
        for (int index = 0; index < swapBuffer.amount; index++)
        {
            double ratio = index / (double)swapBuffer.amount;

            CalculateModulation(voice, ratio);
            UpdateComponentParameters(voice, ratio);

            // Generate Oscillator
            GenerateOscillator(voice, _osa);

            if (_osa == 1) // No oversampling
            {
                const double l = GenerateSample(0, *processData, voice, ratio, 0);
                const double r = GenerateSample(1, *processData, voice, ratio, 0);
                voice.buffer.left[index] += l;
                voice.buffer.right[index] += r;
            }
            else // Oversampling
            {
                for (auto& i : voice.aafp)
                {
                    i.sampleRate = voice.samplerate;
                    i.f0 = std::min(22000., voice.samplerate * 0.4);
                    i.RecalculateParameters();
                }

                double _suml = 0;
                double _sumr = 0;
                for (int k = 0; k < _osa; k++)
                {
                    double _l = GenerateSample(0, *processData, voice, ratio, k);
                    double _r = GenerateSample(1, *processData, voice, ratio, k);

                    _l = voice.aaf[0].Apply(_l, voice.aafp[0]);
                    _r = voice.aaf[1].Apply(_r, voice.aafp[1]);

                    _suml += _l;
                    _sumr += _r;
                }
                voice.buffer.left[index] += _suml / _osa;
                voice.buffer.right[index] += _sumr / _osa;
            }
        }
    }
}