#pragma once

constexpr const char* UPDATE_MODULATION = "update modulation";
constexpr const char* UPDATE_MODULATION_PARAM = "update modulation param";
constexpr const char* UPDATE_MODULATION_VALUE = "update modulation value";

#ifndef CMBNEX_TESTS
constexpr CColor Background = CColor{ 15, 15, 15, 255 };
constexpr CColor MainGreen =  CColor{ 0, 179, 98, 255 };
constexpr CColor MainBack =  CColor{ 25, 25, 25, 255 };
constexpr CColor MainBackL =  CColor{ 40, 40, 40, 255 };
constexpr CColor MainBackD =  CColor{  5,  5,  5, 255 };
constexpr CColor DarkBackD =  CColor{ 5, 5, 5, 255 };
constexpr CColor DarkBack =  CColor{ 15, 15, 15, 255 };
constexpr CColor DarkBackH =  CColor{ 23, 23, 23, 255 };
constexpr CColor ItemBack =  CColor{ 15, 15, 15, 255 };
constexpr CColor KnobBack =  CColor{ 45, 45, 45, 255 };
constexpr CColor KnobBackL =  CColor{ 55, 55, 55, 255 };
constexpr CColor KnobBackDark =  CColor{ 30, 30, 30, 255 };
constexpr CColor Border =  CColor{ 30, 30, 30, 255 };
constexpr CColor MainText = CColor{ 200, 200, 200, 255 };
constexpr CColor OffText = CColor{ 100, 100, 100, 255 };
constexpr CColor BorderHover = CColor{ 70, 70, 70, 255 };
#endif 

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
    Macro1, Macro2, Macro3, Macro4, Macro5,

    Volume1,      Volume2,      Volume3,      Volume4,
    Pitch1,       Pitch2,       Pitch3,       Pitch4,
    Detune1,      Detune2,      Detune3,      Detune4,
    Pan1,         Pan2,         Pan3,         Pan4,

    WTPos1,       WTPos2,       WTPos3,       WTPos4,
    Sync1,        Sync2,        Sync3,        Sync4,
    PulseW1,      PulseW2,      PulseW3,      PulseW4,
    Phase1,       Phase2,       Phase3,       Phase4,
    Bend1,        Bend2,        Bend3,        Bend4,
    DCOff1,       DCOff2,       DCOff3,       DCOff4,

    Shaper1,      Shaper2,      Shaper3,      Shaper4,
    ShaperMix1,   ShaperMix2,   ShaperMix3,   ShaperMix4,
    Shaper21,     Shaper22,     Shaper23,     Shaper24,
    Shaper2Mix1,  Shaper2Mix2,  Shaper2Mix3,  Shaper2Mix4,
    ShaperMorph1, ShaperMorph2, ShaperMorph3, ShaperMorph4,

    Noise1,       Noise2,       Noise3,       Noise4,
    Color1,       Color2,       Color3,       Color4,

    Fold1,        Fold2,        Fold3,        Fold4,
    Bias1,        Bias2,        Bias3,        Bias4,

    DriveGain1,   DriveGain2,   DriveGain3,   DriveGain4,
    DriveAmt1,    DriveAmt2,    DriveAmt3,    DriveAmt4,

    Freq1,        Freq2,        Freq3,        Freq4,
    Reso1,        Reso2,        Reso3,        Reso4,

    GainX,      GainY,      GainZ,
    PreGainX,   PreGainY,   PreGainZ,
    FreqX,      FreqY,      FreqZ,
    ResoX,      ResoY,      ResoZ,
    MixX,       MixY,       MixZ,
    FoldX,      FoldY,      FoldZ,
    BiasX,      BiasY,      BiasZ,
    DriveGainX, DriveGainY, DriveGainZ,
    DriveAmtX,  DriveAmtY,  DriveAmtZ,
    AddMixX,    AddMixY,    AddMixZ,
    MinMixX,    MinMixY,    MinMixZ,
    MultMixX,   MultMixY,   MultMixZ,
    PongMixX,   PongMixY,   PongMixZ,
    MaxMixX,    MaxMixY,    MaxMixZ,
    ModMixX,    ModMixY,    ModMixZ,
    AndMixX,    AndMixY,    AndMixZ,
    InlvMixX,   InlvMixY,   InlvMixZ,
    OrMixX,     OrMixY,     OrMixZ,
    XOrMixX,    XOrMixY,    XOrMixZ,

    SubGain, SubOvertone,

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
    //Panning, 
    GlobalGain,
    Time,
    
    ModCount,

    Enable1,      Enable2,      Enable3,      Enable4,
    ENBShaper1,   ENBShaper2,   ENBShaper3,   ENBShaper4,
    ENBNoise1,    ENBNoise2,    ENBNoise3,    ENBNoise4,
    ENBFold1,     ENBFold2,     ENBFold3,     ENBFold4,
    ENBDrive1,    ENBDrive2,    ENBDrive3,    ENBDrive4,
    ENBFilter1,   ENBFilter2,   ENBFilter3,   ENBFilter4,
    Filter1,      Filter2,      Filter3,      Filter4,
    RandomPhase1, RandomPhase2, RandomPhase3, RandomPhase4,

    SubOct,

    //Clipping, 
    Oversample, Retrigger,

    FilterX,    FilterY,    FilterZ,
    DCX,        DCY,        DCZ,
    ENBFoldX,   ENBFoldY,   ENBFoldZ,
    ENBDriveX,  ENBDriveY,  ENBDriveZ,
    ENBFilterX, ENBFilterY, ENBFilterZ,

    DestA, DestB, DestC, DestD, DestX, DestY,

    LFOSync1, LFOSync2, LFOSync3, LFOSync4, LFOSync5,
    LFORetr1, LFORetr2, LFORetr3, LFORetr4, LFORetr5,

    PitchBend, Voices, Threading, ModWheel,

    Size
};

constexpr size_t ModAmt = 4;

enum class ModSources 
{
    None, 
    LFO1, LFO2, LFO3, LFO4, LFO5, // 1-5
    Env1, Env2, Env3, Env4, Env5, // 1-5
    Mac1, Mac2, Mac3, Mac4, Mac5, // a, b, c, d, e
    Key, Vel, Mod, Ran, // K, V, M, R
    Amount,
    //Osc1, Osc2, Osc3, Osc4, // A, B, C, D
    //Cmb1, Cmb2, Cmb3, // X, Y, Z
};

constexpr ParamInfo ParamNames[]
{
    { "Macro P", 0 }, { "Macro Q", 0 }, { "Macro R", 0 }, { "Macro S", 0 }, { "Macro T", 0 },

    { "A Gain", 1 },           { "B Gain", 0 },           { "C Gain", 0 },           { "D Gain", 0 },
    { "A Pitch", 0.5, 1, 0 },  { "B Pitch", 0.5, 1, 0 },  { "C Pitch", 0.5, 1, 0 },  { "D Pitch", 0.5, 1, 0 },
    { "A Detune", 0.5, 1, 0 }, { "B Detune", 0.5, 1, 0 }, { "C Detune", 0.5, 1, 0 }, { "D Detune", 0.5, 1, 0 },
    { "A Pan", 0.5, 1 },       { "B Pan", 0.5, 1 },       { "C Pan", 0.5, 1 },       { "D Pan", 0.5, 1 },

    { "A Pos", 0 },            { "B Pos", 0 },            { "C Pos", 0 },            { "D Pos", 0 },
    { "A Sync", 0 },           { "B Sync", 0 },           { "C Sync", 0 },           { "D Sync", 0 },
    { "A PW", 0.5, 1 },        { "B PW", 0.5, 1 },        { "C PW", 0.5, 1 },        { "D PW", 0.5, 1 },
    { "A Phase", 0, 0, 0 },    { "B Phase", 0, 0, 0  },   { "C Phase", 0, 0, 0  },   { "D Phase", 0, 0, 0 },
    { "A Bend", 0.5 },         { "B Bend", 0.5 },         { "C Bend", 0.5 },         { "D Bend", 0.5 },
    { "A DC Offset", 0.5 },    { "B DC Offset", 0.5 },    { "C DC Offset", 0.5 },    { "D DC Offset", 0.5 },

    { "A SHP-X", 0.5 },        { "B SHP-X", 0.5 },        { "C SHP-X", 0.5 },        { "D SHP-X", 0.5 },
    { "A SHP-X Mix", 0.5 },    { "B SHP-X Mix", 0.5 },    { "C SHP-X Mix", 0.5 },    { "D SHP-X Mix", 0.5 },
    { "A SHP-Y", 0.5 },        { "B SHP-Y", 0.5 },        { "C SHP-Y", 0.5 },        { "D SHP-Y", 0.5 },
    { "A SHP-Y Mix", 0.5 },    { "B SHP-Y Mix", 0.5 },    { "C SHP-Y Mix", 0.5 },    { "D SHP-Y Mix", 0.5 },
    { "A Shaper Morph", 0 },   { "B Shaper Morph", 0 },   { "C Shaper Morph", 0 },   { "D Shaper Morph", 0 },

    { "A Noise", 0 },          { "B Noise", 0 },          { "C Noise", 0 },          { "D Noise", 0 },
    { "A Color", 0.5 },        { "B Color", 0.5 },        { "C Color", 0.5 },        { "D Color", 0.5 },

    { "A Fold", 0 },           { "B Fold", 0 },           { "C Fold", 0 },           { "D Fold", 0 },
    { "A Bias", 0.5 },         { "B Bias", 0.5 },         { "C Bias", 0.5 },         { "D Bias", 0.5 },

    { "A Drive Gain", 0 },     { "B Drive Gain", 0 },     { "C Drive Gain", 0 },     { "D Drive Gain", 0 },
    { "A Drive Shape", 0 },    { "B Drive Shape", 0 },    { "C Drive Shape", 0 },    { "D Drive Shape", 0 },

    { "A Freq", 1 },           { "B Freq", 1 },           { "C Freq", 1 },           { "D Freq", 1 },
    { "A Reso", 0 },           { "B Reso", 0 },           { "C Reso", 0 },           { "D Reso", 0 },

    { "X Gain", 1 },     { "Y Gain", 1 },     { "Z Gain", 1 },
    { "X Pre-Gain", 1 }, { "Y Pre-Gain", 1 }, { "Z Pre-Gain", 1 },
    { "X Freq", 1 },     { "Y Freq", 1 },     { "Z Freq", 1 },
    { "X Reso", 0 },     { "Y Reso", 0 },     { "Z Reso", 0 },
    { "X Mix", 1 },      { "Y Mix", 1 },      { "Z Mix", 1 },
    { "X Fold", 0 },      { "Y Fold", 0 },      { "Z Fold", 0 },
    { "X Bias", 0.5 },      { "Y Bias", 0.5 },      { "Z Bias", 0.5 },
    { "X Drive Gain", 0 },      { "Y Drive Gain", 0 },      { "Z Drive Gain", 0 },
    { "X Drive Shape", 0 },      { "Y Drive Shape", 0 },      { "Z Drive Shape", 0 },
    { "X Add Mix", 1 },  { "Y Add Mix", 1 },  { "Z Add Mix", 1 },
    { "X Min Mix", 0 },  { "Y Min Mix", 0 },  { "Z Min Mix", 0 },
    { "X Mult Mix", 0 }, { "Y Mult Mix", 0 }, { "Z Mult Mix", 0 },
    { "X Pong Mix", 0 }, { "Y Pong Mix", 0 }, { "Z Pong Mix", 0 },
    { "X Max Mix", 0 },  { "Y Max Mix", 0 },  { "Z Max Mix", 0 },
    { "X Mod Mix", 0 },  { "Y Mod Mix", 0 },  { "Z Mod Mix", 0 },
    { "X And Mix", 0 },  { "Y And Mix", 0 },  { "Z And Mix", 0 },
    { "X Inlv Mix", 0 }, { "Y Inlv Mix", 0 }, { "Z Inlv Mix", 0 },
    { "X Or Mix", 0 },   { "Y Or Mix", 0 },   { "Z Or Mix", 0 },
    { "X XOr Mix", 0 },  { "Y XOr Mix", 0 },  { "Z XOr Mix", 0 },

    { "Sub Gain", 0 }, { "Overtone", 0 },

    { "LFO 1 Rate", 0, 0, 0, 1, 0 },   { "LFO 2 Rate", 0, 0, 0, 1, 0 },    { "LFO 3 Rate", 0, 0, 0, 1, 0 },   { "LFO 4 Rate", 0, 0, 0, 1, 0 },   { "LFO 5 Rate", 0, 0, 0, 1, 0 },
    { "LFO 1 Amount", 1 },  { "LFO 2 Amount", 1 },  { "LFO 3 Amount", 1 },  { "LFO 4 Amount", 1 },  { "LFO 5 Amount", 1 },
    { "LFO 1 Pos",  0 },    { "LFO 2 Pos",  0 },    { "LFO 3 Pos",  0 },    { "LFO 4 Pos",  0 },    { "LFO 5 Pos",  0 },
    { "LFO 1 Offset", 0 },  { "LFO 2 Offset", 0 },  { "LFO 3 Offset", 0 },  { "LFO 4 Offset", 0 },  { "LFO 5 Offset", 0 },
    { "LFO 1 Shaper", .5 }, { "LFO 2 Shaper", .5 }, { "LFO 3 Shaper", .5 }, { "LFO 4 Shaper", .5 }, { "LFO 5 Shaper", .5 },

    { "Gain A Time", 0.05849 },{ "Env 2 A Time", 0.05849 }, { "Env 3 A Time", 0.05849 }, { "Env 4 A Time", 0.05849 }, { "Env 5 A Time",  .05849 },
    { "Gain A Slope", .5 },    { "Env 2 A Slope", .5 },     { "Env 3 A Slope", .5 },     { "Env 4 A Slope", .5 },     { "Env 5 A Slope", .5 },
    { "Gain A Value",  0 },    { "Env 2 A Value",  0 },     { "Env 3 A Value",  0 },     { "Env 4 A Value",  0 },     { "Env 5 A Value",  0 },
    { "Gain D Time", 0.49300 },{ "Env 2 D Time", 0.49300 }, { "Env 3 D Time", 0.49300 }, { "Env 4 D Time", 0.49300 }, { "Env 5 D Time", 0.49300 },
    { "Gain D Slope", .255 },  { "Env 2 D Slope", .25 },    { "Env 3 D Slope",   .25 },  { "Env 4 D Slope",   .25 },  { "Env 5 D Slope",   .25 },
    { "Gain D Value",   1 },   { "Env 2 D Value",   1 },    { "Env 3 D Value",   1 },    { "Env 4 D Value",   1 },    { "Env 5 D Value",   1 },
    { "Gain Sustain", .5 },    { "Env 2 Sustain", .5 },     { "Env 3 Sustain", .5 },     { "Env 4 Sustain", .5 },     { "Env 5 Sustain", .5 },
    { "Gain R Time", 0.49300 },{ "Env 2 R Time", 0.49300 }, { "Env 3 R Time", 0.49300 }, { "Env 4 R Time", 0.49300 }, { "Env 5 R Time", 0.49300 },
    { "Gain R Slope", .25 },   { "Env 2 R Slope", .25 },    { "Env 3 R Slope", .25 },    { "Env 4 R Slope", .25 },    { "Env 5 R Slope", .25 },

    { "Transpose", 0.5, true, false, true }, { "Bend", 0.0416666 }, { "Glide", 0, false, true, false },
    //{ "Pan", 0.5 }, 
    { "Global Gain", 0.5 },
    { "Time", 0.5 },

    { "ModCount", 0, false, true, false, 0, ParameterInfo::kIsHidden },

    { "A Enable", 1 },            { "B Enable", 1 },            { "C Enable", 1 },            { "D Enable", 1 },
    { "A Enable Waveshaper", 0 }, { "B Enable Waveshaper", 0 }, { "C Enable Waveshaper", 0 }, { "D Enable Waveshaper", 0 },
    { "A Enable Noise", 0 },      { "B Enable Noise", 0 },      { "C Enable Noise", 0 },      { "D Enable Noise", 0 },
    { "A Enable Fold", 0 },       { "B Enable Fold", 0 },       { "C Enable Fold", 0 },       { "D Enable Fold", 0 },
    { "A Enable Drive", 0 },      { "B Enable Drive", 0 },      { "C Enable Drive", 0 },      { "D Enable Drive", 0 },
    { "A Enable Filter", 0 },     { "B Enable Filter", 0 },     { "C Enable Filter", 0 },     { "D Enable Filter", 0 },
    { "A Filter", 0 },            { "B Filter", 0 },            { "C Filter", 0 },            { "D Filter", 0 },
    { "A Random", 0 },            { "B Random", 0 },            { "C Random", 0 },            { "D Random", 0 },

    { "Octave", 0.5, false, true, true, 4 },

    //{ "Clipping", 1 }, 
    { "Oversample", 0.25 }, { "Retrigger", 1 },

    { "Filter X", 0 }, { "Filter Y", 0 }, { "Filter Z", 0 },
    { "DC X", 0 },     { "DC Y", 0 },     { "DC Z", 0 },
    { "Enable Fold X", 0 },     { "Enable Fold Y", 0 },     { "Enable Fold Z", 0 },
    { "Enable Drive X", 0 },     { "Enable Drive Y", 0 },     { "Enable Drive Z", 0 },
    { "Enable Filter X", 0 },     { "Enable Filter Y", 0 },     { "Enable Filter Z", 0 },
    
    { "Destination A", 1. / 128., 0, 1, 0 }, { "Destination B",  2. / 128., 0, 1, 0 }, { "Destination C",  4. / 128., 0, 1, 0 },
    { "Destination D",  8. / 128., 0, 1, 0 }, { "Destination X",  16. / 128., 0, 1, 0 }, { "Destination Y",  32. / 128., 0, 1, 0 },
    
    { "LFO 1 Sync", 0 }, { "LFO 2 Sync", 0 }, { "LFO 3 Sync", 0 }, { "LFO 4 Sync", 0 }, { "LFO 5 Sync", 0 },
    { "LFO 1 Retrigger", 1 }, { "LFO 2 Retrigger", 1 }, { "LFO 3 Retrigger", 1 }, { "LFO 4 Retrigger", 1 }, { "LFO 5 Retrigger", 1 },

    { "Pitch Bend", 0.5, false, true, true, 0, ParameterInfo::kIsHidden },
    { "Voices", 1, false, true, false, 0, ParameterInfo::kIsHidden },
    { "Threading", 1, false, true, false, 0, ParameterInfo::kIsHidden },
    { "Mod Wheel", 0.5, false, true, true, 0, ParameterInfo::kIsHidden },
};

constexpr static const char* TimesString[]{
    "4", "3", "2", "1", "1/2", "1/3", "1/4", "1/6", "1/8", "1/12", "1/16", "1/24", "1/32", "1/64",
};

constexpr static double TimesValue[]{
    4.0, 3.0, 2.0, 1.0, 1./2., 1./3., 1./4., 1./6., 1./8., 1./12., 1./16., 1./24., 1./32., 1./64.,
};

constexpr static size_t TimesAmount = std::size(TimesValue);