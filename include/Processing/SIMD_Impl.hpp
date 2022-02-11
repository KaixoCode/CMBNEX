template<size_t Bits>
void GenerateOscillator(Voice& voice, int os)
{
    using SIMDType = SIMD<float, Bits>;
    using SIMDiType = SIMD<int, Bits>;

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

    for (int i = 0; i < Oscillators * Unison * 8; i += SIMDType::COUNT)
    {
        SIMDType _oscFreq = &voice.memory._freqcA[i];

        SIMDType _morph = &voice.memory._morphA[i];
        SIMDType _shap1 = &voice.memory._shap1A[i];
        SIMDType _shap2 = &voice.memory._shap2A[i];
        SIMDType _shmx1 = &voice.memory._shmx1A[i];
        SIMDType _shmx2 = &voice.memory._shmx2A[i];
        SIMDType _pulsw = &voice.memory._pulswA[i];
        SIMDType _synca = &voice.memory._syncaA[i];
        SIMDType _benda = &voice.memory._bendaA[i];

        SIMDType _wtpos = &voice.memory._wtposA[i];
        SIMDType _phaso = &voice.memory._phasoA[i];
        SIMDType _dcoff = &voice.memory._dcoffA[i];
        SIMDType _enbfl = &voice.memory._enbflA[i];
        SIMDType _fldga = &voice.memory._fldgaA[i];
        SIMDType _fldbi = &voice.memory._fldbiA[i];
        SIMDType _enbdr = &voice.memory._enbdrA[i];
        SIMDType _drvga = &voice.memory._drvgaA[i];
        SIMDType _drvsh = &voice.memory._drvshA[i];
        SIMDType _phasedata = &voice.memory._phaseA[i];

        SIMDType _gain = &voice.memory._gainsA[i];

        SIMDType _pw = _pulsw > 0.f;
        SIMDType _d = max(0.000001f,
            (_pw & (1 - _pulsw)) | // If pw < 0 : 1 - pw
            ((~_pw) & (1 + _pulsw))); //        else 1 + pw

        SIMDType _freq = _oscFreq * _synca;
        SIMDType _freqc = minmax(_freq.log2() * 2, 0.f, 32.f);

        _phasedata = _phasedata * (1 - _shmx1) + _shmx1 * lookup3di(_phasedata, _shap1, _morph, Shapers::getMainPhaseShaper());

        SIMDType _pwdone =
            (_pw & ((_phasedata / _d) > 1.f)) | // If pw < 0 : phase / _d > 1
            ((~_pw) & (((1 - _phasedata) / _d) > 1.f));  //  else (1 - phase) / _d > 1

        _phasedata = simdmyfmod1(
            (_pw & ((_phasedata / _d) + _phaso)) |
            ((~_pw) & (((_phasedata + _pulsw) / _d) + _phaso))
        );

        _phasedata = simdmyfmod1(lookup2di1(_phasedata, _benda, Shapers::getPowerCurve()) * _synca);

        // calculate the index of the frequency in the wavetable using log and scalar
        SIMDType _wavetable = lookup3di2(_phasedata, _wtpos, _freqc, Wavetables::getBasicTable());

        _wavetable =
            (_pwdone & 0.f) | // If pulsewidth done, value 0, otherwise other stuff
            ((~_pwdone) & (_wavetable * (1 - _shmx2) + _shmx2 * lookup3di(_wavetable, _shap2, _morph, Shapers::getMainWaveShaper())));


        SIMDType _result = (_wavetable + _dcoff);

        SIMDType _cond = _enbfl > 0.5;

        _result = // Fold
            (_cond & lookup1di1(simdmyfmod1(0.25 * (_result * _fldga + _fldbi)) * 4.f, Shapers::getFold())) |
            ((~_cond) & _result);

        _cond = _enbdr > 0.5;
        _result = // Drive
            (_cond & lookup2di2(minmax(_result * _drvga, -10.f, 10.f), _drvsh, Shapers::getDrive())) |
            ((~_cond) & minmax(_result, -1.f, 1.f));

        _result = _result * _gain;

        // Get final values
        float _finalData[SIMDType::COUNT];
        _result.get(_finalData);

        // Get new phases
        for (int j = 0; j < SIMDType::COUNT; j++)
        {
            if (voice.memory._destL[i + j])
            {   // Assign new phase, and set generated audio to destination
                (voice.memory._destL[i + j][voice.memory._ovsinA[i + j]]) += _finalData[j] * voice.memory._makeuA[i + j] * voice.memory._panniA[i + j];
                (voice.memory._destR[i + j][voice.memory._ovsinA[i + j]]) += _finalData[j] * voice.memory._makeuA[i + j] * (1 - voice.memory._panniA[i + j]);
            }
        }

        if (i + SIMDType::COUNT >= _unisonVoices) break;
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
}

template<>
void GenerateOscillator<0>(Voice& voice, int os)
{
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
}