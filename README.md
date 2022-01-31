
# CMBNEX
[![paypal](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/kaixo1/10)

![image](https://kaixo.me/assets/CMBNEX.png)

If you run into any crashes/bugs, or you have feedback/questions, contact me over on Discord: `@Kaixo#0001`.

## What is CMBNEX?
CMBNEX Is a 4 oscillator synth that revolves around waveshaping and combining sound using several algorithms. 

### Oscillators and Waveshaping
The main oscillators all contain a variety of parameters to make complex and unique wave shapes. The Waveshaper
section contains 9 unique waveshaping algorithms. SHP-X will transform the phase, and SHP-Y will transform the level.
With Morph at 0% the SHP-X and SHP-Y both morph between 4 softer waveshapers, but by turning up the Morph it will
interpolate with 5 sharper waveshapers. At 100% Morph only the sharp waveshapers are present.
The result will then be sent to the specified outputs, which can either be a Combiner, or the main output ('o'). 

### Combiners
A Combiner has 2 inputs, marked with roman numerals. Those inputs will be combined using several algorithms. 
Each algorithm has a separate mix slider, so you have fine control over the mix of each of them. 
The results of each algorithm is then summed up and normalized before it will be fed into the wavefolder, drive, and filter.
Since some of the combiner algorithms can create incredibly strange waveforms, the result is always clipped, and there is 
an option to remove any DC offset. 

### Modulation
On top of all this there are 5 envelopes, 5 LFOs, and 5 macros. Each parameter has 4 slots to be modulated by any of those.
Besides those modulation sources, there's also Key, Velocity, Random, and Mod (mod wheel) sources. The random source will generate a random
value every time a note is pressed.

### Polyphonic and Monophonic
There is an option to turn off polyphonic mode. When in monophonic mode by default the gain envelope will retrigger, meaning it
will jump back to the start of the envelope when a new note is pressed. You can turn this off by toggling the Retrigger button off.
When in polyphonic mode, each voice will be generated on a separate CPU core, you can turn this off by toggling the Threads button off
in case this causes any issues.

### Presets
When you press the preset name in the top bar you can select a preset file (.cmbnex). You can save a preset by clicking the Save button.
The Init button will reset the entire plugin back to the default preset, be careful, this is irreversable!
