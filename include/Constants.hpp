#pragma once

struct ParamInfo
{
    const char* name;
    double reset;
    bool two = false;
    bool constrain = true;
    bool smooth = true;
    uint32_t step = 0;
    ParameterInfo::ParameterFlags flags = ParameterInfo::kCanAutomate;
};

enum Params
{
    Volume1,      Volume2,      Volume3,      Volume4,
    Pitch1,       Pitch2,       Pitch3,       Pitch4,
    Detune1,      Detune2,      Detune3,      Detune4,
    WTPos1,       WTPos2,       WTPos3,       WTPos4,
    Sync1,        Sync2,        Sync3,        Sync4,
    PulseW1,      PulseW2,      PulseW3,      PulseW4,
    Phase1,       Phase2,       Phase3,       Phase4,
    Shaper1,      Shaper2,      Shaper3,      Shaper4,
    Shaper21,     Shaper22,     Shaper23,     Shaper24,
    Pan1,         Pan2,         Pan3,         Pan4,
    Freq1,        Freq2,        Freq3,        Freq4,
    Reso1,        Reso2,        Reso3,        Reso4,
    Noise1,       Noise2,       Noise3,       Noise4,

    GainX, GainY, GainZ,
    ModeX, ModeY, ModeZ,
    FreqX, FreqY, FreqZ,
    ResoX, ResoY, ResoZ,

    SubOct, SubGain, SubOvertone,

    LFORate1,   LFORate2,   LFORate3,   LFORate4,   LFORate5,
    LFOLvl1,    LFOLvl2,    LFOLvl3,    LFOLvl4,    LFOLvl5,
    LFOPhase1,  LFOPhase2,  LFOPhase3,  LFOPhase4,  LFOPhase5,
    LFOPos1,    LFOPos2,    LFOPos3,    LFOPos4,    LFOPos5,
    LFOShaper1, LFOShaper2, LFOShaper3, LFOShaper4, LFOShaper5,

    Env1A,   Env2A,   Env3A,   Env4A,   Env5A,
    Env1AC,  Env2AC,  Env3AC,  Env4AC,  Env5AC,
    Env1AL,  Env2AL,  Env3AL,  Env4AL,  Env5AL,
    Env1D,   Env2D,   Env3D,   Env4D,   Env5D,
    Env1DC,  Env2DC,  Env3DC,  Env4DC,  Env5DC,
    Env1DL,  Env2DL,  Env3DL,  Env4DL,  Env5DL,
    Env1S,   Env2S,   Env3S,   Env4S,   Env5S,
    Env1R,   Env2R,   Env3R,   Env4R,   Env5R,
    Env1RC,  Env2RC,  Env3RC,  Env4RC,  Env5RC,

    Transpose, Bend, Glide,
    FreqKey, FreqVel, OscVel,
    
    ModCount,

    Clipping, Oversample, Retrigger,

    FilterX, FilterY, FilterZ,
    DCX,     DCY,     DCZ,

    DestA, DestB, DestC, DestD, DestX, DestY,

    Filter1,      Filter2,      Filter3,      Filter4,
    RandomPhase1, RandomPhase2, RandomPhase3, RandomPhase4,

    LFO1M1,  LFO2M1,  LFO3M1,  LFO4M1,  LFO5M1,
    LFO1M1A, LFO2M1A, LFO3M1A, LFO4M1A, LFO5M1A,
    LFO1M2,  LFO2M2,  LFO3M2,  LFO4M2,  LFO5M2,
    LFO1M2A, LFO2M2A, LFO3M2A, LFO4M2A, LFO5M2A,
    LFO1M3,  LFO2M3,  LFO3M3,  LFO4M3,  LFO5M3,
    LFO1M3A, LFO2M3A, LFO3M3A, LFO4M3A, LFO5M3A,
    LFO1M4,  LFO2M4,  LFO3M4,  LFO4M4,  LFO5M4,
    LFO1M4A, LFO2M4A, LFO3M4A, LFO4M4A, LFO5M4A,
    LFO1M5,  LFO2M5,  LFO3M5,  LFO4M5,  LFO5M5,
    LFO1M5A, LFO2M5A, LFO3M5A, LFO4M5A, LFO5M5A,

    Env1M1,  Env2M1,  Env3M1,  Env4M1,  Env5M1,
    Env1M1A, Env2M1A, Env3M1A, Env4M1A, Env5M1A,
    Env1M2,  Env2M2,  Env3M2,  Env4M2,  Env5M2,
    Env1M2A, Env2M2A, Env3M2A, Env4M2A, Env5M2A,
    Env1M3,  Env2M3,  Env3M3,  Env4M3,  Env5M3,
    Env1M3A, Env2M3A, Env3M3A, Env4M3A, Env5M3A,
    Env1M4,  Env2M4,  Env3M4,  Env4M4,  Env5M4,
    Env1M4A, Env2M4A, Env3M4A, Env4M4A, Env5M4A,
    Env1M5,  Env2M5,  Env3M5,  Env4M5,  Env5M5,
    Env1M5A, Env2M5A, Env3M5A, Env4M5A, Env5M5A,

    PitchBend,

    Size
};

constexpr bool Divider(int x)
{
    return x == 0 || x == 1 || x == 2 || x == 3 
        || x == 52 
        || x == 53 
        || x == 54 
        || x == 64 
        || x == 67 
        || x == 68 
        || x == 69 
        || x == 70 
        || x == 71 
        || x == 92 || x == 93 || x == 94 || x == 95 || x == 96 
        || x == 137;
}

constexpr int ParamOrder[]
{
    -1, 0, 4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, // Osc 1
    -1, 1, 5,  9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, // Osc 2
    -1, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, // Osc 3
    -1, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, // Osc 4

    -1, 52, 55, 58, 61, // Combine X
    -1, 53, 56, 59, 62, // Combine Y
    -1, 54, 57, 60, 63, // Combine Z

    -1, 64, 65, 66, // Sub Osc

    -1, 67, 72, 77, 82, 87, // LFO 1
    -1, 68, 73, 78, 83, 88, // LFO 2
    -1, 69, 74, 79, 84, 89, // LFO 3
    -1, 70, 75, 80, 85, 90, // LFO 4
    -1, 71, 76, 81, 86, 91, // LFO 5

    -1, 92,  97, 102, 107, 112, 117, 122, 127, 132, // Env 1
    -1, 93,  98, 103, 108, 113, 118, 123, 128, 133, // Env 2
    -1, 94,  99, 104, 109, 114, 119, 124, 129, 134, // Env 3
    -1, 95, 100, 105, 110, 115, 120, 125, 130, 135, // Env 4
    -1, 96, 101, 106, 111, 116, 121, 126, 131, 136, // Env 5

    -1, 137, 138, 139, 140, 141, 142 // Midi
};

constexpr auto ParamOrderSize = std::size(ParamOrder);

constexpr ParamInfo ParamNames[]
{ 
    { "A Gain", 1 },           { "B Gain", 0 },           { "C Gain", 0 },           { "D Gain", 0 },
    { "A Pitch", 0.5, 1, 0 },  { "B Pitch", 0.5, 1, 0 },  { "C Pitch", 0.5, 1, 0 },  { "D Pitch", 0.5, 1, 0 },
    { "A Detune", 0.5, 1, 0 }, { "B Detune", 0.5, 1, 0 }, { "C Detune", 0.5, 1, 0 }, { "D Detune", 0.5, 1, 0 },
    { "A Pos", 0 },            { "B Pos", 0 },            { "C Pos", 0 },            { "D Pos", 0 },
    { "A Sync", 0 },           { "B Sync", 0 },           { "C Sync", 0 },           { "D Sync", 0 },
    { "A PW", 0.5, 1 },        { "B PW", 0.5, 1 },        { "C PW", 0.5, 1 },        { "D PW", 0.5, 1 },
    { "A Phase", 0 },          { "B Phase", 0 },          { "C Phase", 0 },          { "D Phase", 0 },
    { "A SHP-X", 0 },          { "B SHP-X", 0 },          { "C SHP-X", 0 },          { "D SHP-X", 0 },
    { "A SHP-Y", 0 },          { "B SHP-Y", 0 },          { "C SHP-Y", 0 },          { "D SHP-Y", 0 },
    { "A Pan", 0.5, 1 },       { "B Pan", 0.5, 1 },       { "C Pan", 0.5, 1 },       { "D Pan", 0.5, 1 },
    { "A Freq", 1 },           { "B Freq", 1 },           { "C Freq", 1 },           { "D Freq", 1 },
    { "A Reso", 0 },           { "B Reso", 0 },           { "C Reso", 0 },           { "D Reso", 0 },
    { "A Noise", 0 },          { "B Noise", 0 },          { "C Noise", 0 },          { "D Noise", 0 },

    { "X Gain", 1 }, { "Y Gain", 1 }, { "Z Gain", 1 },
    { "X Mode", 0 }, { "Y Mode", 0 }, { "Z Mode", 0 },
    { "X Freq", 1 }, { "Y Freq", 1 }, { "Z Freq", 1 },
    { "X Reso", 0 }, { "Y Reso", 0 }, { "Z Reso", 0 },

    { "Octave", 0.5, false, true, true, 4 }, { "Sub Gain", 0 }, { "Overtone", 0 },

    { "LFO 1 Rate",  0 },   { "LFO 2 Rate", 0 },    { "LFO 3 Rate",  0 },   { "LFO 4 Rate",  0 },   { "LFO 5 Rate",  0 },
    { "LFO 1 Amount", 1 },  { "LFO 2 Amount", 1 },  { "LFO 3 Amount", 1 },  { "LFO 4 Amount", 1 },  { "LFO 5 Amount", 1 },
    { "LFO 1 Pos",  0 },    { "LFO 2 Pos",  0 },    { "LFO 3 Pos",  0 },    { "LFO 4 Pos",  0 },    { "LFO 5 Pos",  0 },
    { "LFO 1 Offset", 0 },  { "LFO 2 Offset", 0 },  { "LFO 3 Offset", 0 },  { "LFO 4 Offset", 0 },  { "LFO 5 Offset", 0 },
    { "LFO 1 Shaper", .5 }, { "LFO 2 Shaper", .5 }, { "LFO 3 Shaper", .5 }, { "LFO 4 Shaper", .5 }, { "LFO 5 Shaper", .5 },

    { "Gain A Time",  0 },     { "Env 1 A Time", 0 },       { "Env 2 A Time",  0 },      { "Env 3 A Time",  0 },      { "Env 4 A Time",  0 },
    { "Gain A Slope", .5 },    { "Env 1 A Slope", .5 },     { "Env 2 A Slope", .5 },     { "Env 3 A Slope", .5 },     { "Env 4 A Slope", .5 },
    { "Gain A Value",  0 },    { "Env 1 A Value",  0 },     { "Env 2 A Value",  0 },     { "Env 3 A Value",  0 },     { "Env 4 A Value",  0 },
    { "Gain D Time", 0.34615 },{ "Env 1 D Time", 0.34615 }, { "Env 2 D Time", 0.34615 }, { "Env 3 D Time", 0.34615 }, { "Env 4 D Time", 0.34615 },
    { "Gain D Slope",   0 },   { "Env 1 D Slope",   0 },    { "Env 2 D Slope",   0 },    { "Env 3 D Slope",   0 },    { "Env 4 D Slope",   0 },
    { "Gain D Value",   1 },   { "Env 1 D Value",   1 },    { "Env 2 D Value",   1 },    { "Env 3 D Value",   1 },    { "Env 4 D Value",   1 },
    { "Gain Sustain", .5 },    { "Env 1 Sustain", .5 },     { "Env 2 Sustain", .5 },     { "Env 3 Sustain", .5 },     { "Env 4 Sustain", .5 },
    { "Gain R Time", 0.34615 },{ "Env 1 R Time", 0.34615 }, { "Env 2 R Time", 0.34615 }, { "Env 3 R Time", 0.34615 }, { "Env 4 R Time", 0.34615 },
    { "Gain R Slope", 0 },     { "Env 1 R Slope", 0 },      { "Env 2 R Slope", 0 },      { "Env 3 R Slope", 0 },      { "Env 4 R Slope", 0 },

    { "Transpose", 0.5 }, { "Bend", 0.0416666 }, { "Glide", 0 },
    { "Freq<Key", 0 }, { "Freq<Vel", 0 }, { "Osc<Vel", 0 },

    { "ModCount", 0, false, true, false, 0, ParameterInfo::kIsHidden },

    { "Clipping", 1 }, { "Oversample", 1 }, { "Retrigger", 1 },

    { "Filter X", 0 }, { "Filter Y", 0 }, { "Filter Z", 0 },
    { "DC X", 0 },     { "DC Y", 0 },     { "DC Z", 0 }, 
    
    { "Destination A", 1. / 64., 0, 0, 0 }, { "Destination B",  2. / 64., 0, 0, 0 }, { "Destination C",  4. / 64., 0, 0, 0 },
    { "Destination D",  8. / 64., 0, 0, 0 }, { "Destination X",  16. / 64., 0, 0, 0 }, { "Destination Y",  32. / 64., 0, 0, 0 },

    { "Filter A", 0 },      { "Filter B", 0 },      { "Filter C", 0 },      { "Filter D", 0 },
    { "Random A", 0 },      { "Random B", 0 },      { "Random C", 0 },      { "Random D", 0 },

    { "LM1M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M1",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M2",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M3",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M4",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M5",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "LM1M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM2M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM3M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM4M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "LM5M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },

    { "EM1M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M1",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M1",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M1L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M2",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M2",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M2L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M3",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M3",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M3L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M4",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M4",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M4L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M5",  0, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M5",  0, false, true, false, 0, ParameterInfo::kIsHidden },
    { "EM1M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM2M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM3M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM4M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden }, { "EM5M5L", 0.5, false, true, false, 0, ParameterInfo::kIsHidden },

    { "Pitch Bend", 0.5, false, true, true, 0, ParameterInfo::kIsHidden },
};