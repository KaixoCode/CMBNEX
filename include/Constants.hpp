#pragma once

constexpr CColor MainMain =  CColor{ 0, 179, 98, 255 };
constexpr CColor MainOsc =   CColor{ 0, 179, 98, 255 };
constexpr CColor MainEnv = CColor{ 0, 179, 98, 255 };
constexpr CColor MainMacro = CColor{ 0, 179, 98, 255 };
constexpr CColor MainLFO = CColor{ 0, 179, 98, 255 };
//constexpr CColor MainMain = CColor{ 0, 179, 98, 255 };
//constexpr CColor MainOsc = CColor{ 212, 72, 72, 255 };
//constexpr CColor MainEnv = CColor{ 255, 216, 43, 255 };
//constexpr CColor MainMacro = CColor{ 212, 72, 72, 255 };
//constexpr CColor MainLFO = CColor{ 72, 77, 212, 255 };
constexpr CColor MainText = CColor{ 200, 200, 200, 255 };
constexpr CColor OffText = CColor{ 128, 128, 128, 255 };


struct ParamInfo
{
    const char* name;
    double reset;
    bool two = false;
    bool constrain = true;
    bool smooth = true;
    uint32_t step = 0;
    int flags = ParameterInfo::kCanAutomate;
};

enum Params
{
    Enable1,      Enable2,      Enable3,      Enable4,
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
    Panning, Time, OscVel,
    
    ModCount,

    Clipping, Oversample, Retrigger,

    FilterX, FilterY, FilterZ,
    DCX,     DCY,     DCZ,

    DestA, DestB, DestC, DestD, DestX, DestY,

    Filter1,      Filter2,      Filter3,      Filter4,
    RandomPhase1, RandomPhase2, RandomPhase3, RandomPhase4,

    LFOSync1,   LFOSync2,   LFOSync3,   LFOSync4,   LFOSync5,

    PitchBend,

    Size
};

constexpr size_t ModAmt = 4;

enum class ModSources 
{
    None, 
    LFO1, LFO2, LFO3, LFO4, LFO5, 
    Env1, Env2, Env3, Env4, Env5, 
    Amount
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
    { "A Enable", 1 },         { "B Enable", 1 },         { "C Enable", 1 },         { "D Enable", 1 },
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

    { "LFO 1 Rate", 0, 0, 0, 1, 0 },   { "LFO 2 Rate", 0, 0, 0, 1, 0 },    { "LFO 3 Rate", 0, 0, 0, 1, 0 },   { "LFO 4 Rate", 0, 0, 0, 1, 0 },   { "LFO 5 Rate", 0, 0, 0, 1, 0 },
    { "LFO 1 Amount", 1 },  { "LFO 2 Amount", 1 },  { "LFO 3 Amount", 1 },  { "LFO 4 Amount", 1 },  { "LFO 5 Amount", 1 },
    { "LFO 1 Pos",  0 },    { "LFO 2 Pos",  0 },    { "LFO 3 Pos",  0 },    { "LFO 4 Pos",  0 },    { "LFO 5 Pos",  0 },
    { "LFO 1 Offset", 0 },  { "LFO 2 Offset", 0 },  { "LFO 3 Offset", 0 },  { "LFO 4 Offset", 0 },  { "LFO 5 Offset", 0 },
    { "LFO 1 Shaper", .5 }, { "LFO 2 Shaper", .5 }, { "LFO 3 Shaper", .5 }, { "LFO 4 Shaper", .5 }, { "LFO 5 Shaper", .5 },

    { "Gain A Time",  0 },     { "Env 1 A Time", 0 },       { "Env 2 A Time",  0 },      { "Env 3 A Time",  0 },      { "Env 4 A Time",  0 },
    { "Gain A Slope", .5 },    { "Env 1 A Slope", .5 },     { "Env 2 A Slope", .5 },     { "Env 3 A Slope", .5 },     { "Env 4 A Slope", .5 },
    { "Gain A Value",  0 },    { "Env 1 A Value",  0 },     { "Env 2 A Value",  0 },     { "Env 3 A Value",  0 },     { "Env 4 A Value",  0 },
    { "Gain D Time", 0.49300 },{ "Env 1 D Time", 0.49300 }, { "Env 2 D Time", 0.49300 }, { "Env 3 D Time", 0.49300 }, { "Env 4 D Time", 0.49300 },
    { "Gain D Slope",   0 },   { "Env 1 D Slope",   0 },    { "Env 2 D Slope",   0 },    { "Env 3 D Slope",   0 },    { "Env 4 D Slope",   0 },
    { "Gain D Value",   1 },   { "Env 1 D Value",   1 },    { "Env 2 D Value",   1 },    { "Env 3 D Value",   1 },    { "Env 4 D Value",   1 },
    { "Gain Sustain", .5 },    { "Env 1 Sustain", .5 },     { "Env 2 Sustain", .5 },     { "Env 3 Sustain", .5 },     { "Env 4 Sustain", .5 },
    { "Gain R Time", 0.49300 },{ "Env 1 R Time", 0.49300 }, { "Env 2 R Time", 0.49300 }, { "Env 3 R Time", 0.49300 }, { "Env 4 R Time", 0.49300 },
    { "Gain R Slope", 0 },     { "Env 1 R Slope", 0 },      { "Env 2 R Slope", 0 },      { "Env 3 R Slope", 0 },      { "Env 4 R Slope", 0 },

    { "Transpose", 0.5, true, false, true }, { "Bend", 0.0416666 }, { "Glide", 0, false, true, false },
    { "Pan", 0.5 }, { "Time", 0.5 }, { "Osc<Vel", 0 },

    { "ModCount", 0, false, true, false, 0, ParameterInfo::kIsHidden },

    { "Clipping", 1 }, { "Oversample", 1 }, { "Retrigger", 1 },

    { "Filter X", 0 }, { "Filter Y", 0 }, { "Filter Z", 0 },
    { "DC X", 0 },     { "DC Y", 0 },     { "DC Z", 0 }, 
    
    { "Destination A", 1. / 128., 0, 1, 0 }, { "Destination B",  2. / 128., 0, 1, 0 }, { "Destination C",  4. / 128., 0, 1, 0 },
    { "Destination D",  8. / 128., 0, 1, 0 }, { "Destination X",  16. / 128., 0, 1, 0 }, { "Destination Y",  32. / 128., 0, 1, 0 },

    { "Filter A", 0 },      { "Filter B", 0 },      { "Filter C", 0 },      { "Filter D", 0 },
    { "Random A", 0 },      { "Random B", 0 },      { "Random C", 0 },      { "Random D", 0 },
    
    { "LFO 1 Sync", 0 }, { "LFO 2 Sync", 0 }, { "LFO 3 Sync", 0 }, { "LFO 4 Sync", 0 }, { "LFO 5 Sync", 0 },

    { "Pitch Bend", 0.5, false, true, true, 0, ParameterInfo::kIsHidden },
};

constexpr static const char* TimesString[]{
    "4", "3", "2", "1", "1/2", "1/3", "1/4", "1/6", "1/8", "1/12", "1/16", "1/24", "1/32", "1/64",
};

constexpr static double TimesValue[]{
    4.0, 3.0, 2.0, 1.0, 1./2., 1./3., 1./4., 1./6., 1./8., 1./12., 1./16., 1./24., 1./32., 1./64.,
};

constexpr static size_t TimesAmount = std::size(TimesValue);