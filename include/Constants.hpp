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

    ModCount,

    FilterX, FilterY, FilterZ,
    DCX,     DCY,     DCZ,

    DestA, DestB, DestC, DestD, DestX, DestY,

    Filter1,      Filter2,      Filter3,      Filter4,
    RandomPhase1, RandomPhase2, RandomPhase3, RandomPhase4,

    LFORate1,   LFORate2,   LFORate3,   LFORate4,   LFORate5,
    LFOLvl1,    LFOLvl2,    LFOLvl3,    LFOLvl4,    LFOLvl5,
    LFOPhase1,  LFOPhase2,  LFOPhase3,  LFOPhase4,  LFOPhase5,
    LFOPos1,    LFOPos2,    LFOPos3,    LFOPos4,    LFOPos5,
    LFOShaper1, LFOShaper2, LFOShaper3, LFOShaper4, LFOShaper5,

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

    Env1A,   Env2A,   Env3A,   Env4A,   Env5A,
    Env1AC,  Env2AC,  Env3AC,  Env4AC,  Env5AC,
    Env1AL,  Env2AL,  Env3AL,  Env4AL,  Env5AL,
    Env1D,   Env2D,   Env3D,   Env4D,   Env5D,
    Env1DC,  Env2DC,  Env3DC,  Env4DC,  Env5DC,
    Env1DL,  Env2DL,  Env3DL,  Env4DL,  Env5DL,
    Env1S,   Env2S,   Env3S,   Env4S,   Env5S,
    Env1R,   Env2R,   Env3R,   Env4R,   Env5R,
    Env1RC,  Env2RC,  Env3RC,  Env4RC,  Env5RC,

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

    Size
};

constexpr ParamInfo ParamNames[]
{ 
    { "Gain A", 1 },           { "Gain B", 0 },           { "Gain C", 0 },           { "Gain D", 0 },
    { "Pitch A", 0.5, 1, 0 },  { "Pitch B", 0.5, 1, 0 },  { "Pitch C", 0.5, 1, 0 },  { "Pitch D", 0.5, 1, 0 },
    { "Detune A", 0.5, 1, 0 }, { "Detune B", 0.5, 1, 0 }, { "Detune C", 0.5, 1, 0 }, { "Detune D", 0.5, 1, 0 },
    { "Pos A", 0 },            { "Pos B", 0 },            { "Pos C", 0 },            { "Pos D", 0 },
    { "Sync A", 0 },           { "Sync B", 0 },           { "Sync C", 0 },           { "Sync D", 0 },
    { "PW A", 0.5, 1 },        { "PW B", 0.5, 1 },        { "PW C", 0.5, 1 },        { "PW D", 0.5, 1 },
    { "Phase A", 0 },          { "Phase B", 0 },          { "Phase C", 0 },          { "Phase D", 0 },
    { "SHPR-X A", 0 },         { "SHPR-X B", 0 },         { "SHPR-X C", 0 },         { "SHPR-X D", 0 },
    { "SHPR-Y A", 0 },         { "SHPR-Y B", 0 },         { "SHPR-Y C", 0 },         { "SHPR-Y D", 0 },
    { "Pan A", 0.5, 1 },       { "Pan B", 0.5, 1 },       { "Pan C", 0.5, 1 },       { "Pan D", 0.5, 1 },
    { "Freq A", 1 },           { "Freq B", 1 },           { "Freq C", 1 },           { "Freq D", 1 },
    { "Reso A", 0 },           { "Reso B", 0 },           { "Reso C", 0 },           { "Reso D", 0 },
    { "Noise A", 0 },          { "Noise B", 0 },          { "Noise C", 0 },          { "Noise D", 0 },

    { "Gain X", 1 }, { "Gain Y", 1 }, { "Gain Z", 1 }, 
    { "Mode X", 0 }, { "Mode Y", 0 }, { "Mode Z", 0 },
    { "Freq X", 1 }, { "Freq Y", 1 }, { "Freq Z", 1 },
    { "Reso X", 0 }, { "Reso Y", 0 }, { "Reso Z", 0 },

    { "Sub Oct", 0.5, false, true, true, 4 }, { "Sub Gain", 0 }, { "Sub Overtone", 0 },

    { "ModCount", 0, false, true, false, 0, ParameterInfo::kIsHidden },

    { "Filter X", 0 }, { "Filter Y", 0 }, { "Filter Z", 0 },
    { "DC X", 0 },     { "DC Y", 0 },     { "DC Z", 0 }, 
    
    { "Destination A", 1. / 64., 0, 0, 0 }, { "Destination B",  2. / 64., 0, 0, 0 }, { "Destination C",  4. / 64., 0, 0, 0 },
    { "Destination D",  8. / 64., 0, 0, 0 }, { "Destination X",  16. / 64., 0, 0, 0 }, { "Destination Y",  32. / 64., 0, 0, 0 },

    { "Filter A", 0 },      { "Filter B", 0 },      { "Filter C", 0 },      { "Filter D", 0 },
    { "Random A", 0 },      { "Random B", 0 },      { "Random C", 0 },      { "Random D", 0 },

    { "Rate 1",  0 },   { "Rate 2", 0 },    { "Rate 3",  0 },   { "Rate 4",  0 },   { "Rate 5",  0 },
    { "Amount 1", 1 },  { "Amount 2", 1 },  { "Amount 3", 1 },  { "Amount 4", 1 },  { "Amount 5", 1 },
    { "Pos 1",  0 },    { "Pos 2",  0 },    { "Pos 3",  0 },    { "Pos 4",  0 },    { "Pos 5",  0 },
    { "Offset 1", 0 },  { "Offset 2", 0 },  { "Offset 3", 0 },  { "Offset 4", 0 },  { "Offset 5", 0 },
    { "Shaper 1", .5 }, { "Shaper 2", .5 }, { "Shaper 3", .5 }, { "Shaper 4", .5 }, { "Shaper 5", .5 },

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

    { "Attack 1",  0 },       { "Attack 2", 0 },        { "Attack 3",  0 },       { "Attack 4",  0 },        { "Attack 5",  0 },
    { "Attack Curve 1", .5 }, { "Attack Curve 2", .5 }, { "Attack Curve 3", .5 }, { "Attack Curve 4", .5 },  { "Attack Curve 5", .5 },
    { "Attack Level 1",  0 }, { "Attack Level 2",  0 }, { "Attack Level 3",  0 }, { "Attack Level 4",  0 },  { "Attack Level 5",  0 },  
    { "Decay 1", 0.34615 },   { "Decay 2", 0.34615 },   { "Decay 3", 0.34615 },   { "Decay 4", 0.34615 },    { "Decay 5", 0.34615 },
    { "Decay Curve 1",   0 }, { "Decay Curve 2",   0 }, { "Decay Curve 3",   0 }, { "Decay Curve 4",   0 },  { "Decay Curve 5",   0 },
    { "Decay Level 1",   1 }, { "Decay Level 2",   1 }, { "Decay Level 3",   1 }, { "Decay Level 4",   1 },  { "Decay Level 5",   1 },
    { "Sustain 1", .5 },      { "Sustain 2", .5 },      { "Sustain 3", .5 },      { "Sustain 4", .5 },       { "Sustain 5", .5 },
    { "Release 1", 0.34615 }, { "Release 2", 0.34615 }, { "Release 3", 0.34615 }, { "Release 4", 0.34615 },  { "Release 5", 0.34615 },
    { "Release Curve 1", 0 }, { "Release Curve 2", 0 }, { "Release Curve 3", 0 }, { "Release Curve 4", 0 },  { "Release Curve 5", 0 },
    
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
};