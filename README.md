# Halcyon

An ambient synth for the Teenage Engineering OP–XY, built with the unofficial
[Open-XY SDK](https://github.com/etcetc000/open-xy). Soft, near-pure tones that
bloom in like tape played backwards, drift like an old reel and gather quiet
octaves above and below, after the sound of Aphex Twin's *Rhubarb* and Brian
Eno's *An Ending (Ascent)*. Everything is synthesised in code: no samples.

## How it plays

- **Press a key:** the note blooms in. A pure sine, and with Warmth a soft saw
  through a gentle low-pass, as on the CS-80 and the Oberheim Matrix pads of
  those records.
- **Hold:** the tape wanders, each voice slightly apart so chords rub softly
  against themselves, and the saw's filter breathes. With Air, an octave above
  arrives a moment later, then an octave below.
- **Release:** set a long amp release on the OP–XY and send the track to the
  reverb (large Size, Modulation up, a darker tone) or to Grainfield.

## Controls

Press any encoder to switch pages.

| Page 1 | At 0 | Turning up |
| --- | --- | --- |
| **Warmth** | Pure sine | A soft saw blends in through a low-pass that follows the note; the filter opens a little as it rises |
| **Swell** | Starts at once | A reverse-tape bloom from 10 ms to about 5 s; the filter opens with it |
| **Drift** | Still | Tape that wanders (up to about ±30 cents, evenly across the knob), voices apart, the filter breathing, and very slow built-in sines on the other knobs (below) |
| **Air** | Off | The octave above arrives after the note, then the octave below; Swell sets the wait |

| Page 2 | At 0 | Turning up |
| --- | --- | --- |
| **Hiss** | Clean | Soft tape hiss and air that swell with each note |
| **Chord** | One note | A pure fifth joins every note, then a ninth |
| **Ensemble** | One voice | A second copy, 4 to 14 cents apart and slowly sweeping, like an old string machine |
| **Age** | New tape | Gentle saturation, a rounded top and a slight wobble in level, fading in from about 15 |

Drift also moves six of the other knobs with very slow sines, each at its own
speed, so they never line up. They keep running from note to note, like a real
LFO, and each voice sits a little apart in the cycle.
The amounts are at full Drift and shrink with it; at Drift 0 everything is still,
and a knob at 0 stays at 0.

| Knob | Amount | One cycle |
| --- | --- | --- |
| Warmth | ±10% | 19 s |
| Air | ±6% | 17 s |
| Hiss | ±3% | 23 s |
| Chord | ±10% | 29 s |
| Ensemble | ±12% | 26 s |
| Age | ±4% | 31 s |

All eight can be LFO destinations. Presets: Rhubarb, Ascent, Pure, Fog, Lunar.

## The screen

Fog lake, in greyscale: a low sun behind bands of mist over still water. Each
note drops a ripple whose rings spread and fade; the late octaves' rings follow,
fainter. Warmth warms the sun, Drift sways the mist and stirs the reflection,
Swell slows the rings. Hiss thickens the mist, Chord adds rings, Ensemble
doubles the reflection and Age scatters dust on the water. When nothing plays,
the lake is still.

## Build

With the Open-XY SDK (tested with 0.0.2 on OP–XY OS 1.2.1):

```sh
python sdk.py test --machine path/to/halcyon
python sdk.py kit --personal --stock-os <your stock os.ldr> --machine path/to/halcyon --out <folder>
```

Firmware is always built from your own OP–XY's OS; no OS is distributed here.

## Notes

- Output is mono, as for every Open-XY synth: the space comes from the OP–XY's effects.
- Up to 16 voices, so long releases overlap without cutting off.
- Unofficial and experimental, like all Open-XY machines. Not affiliated with
  or endorsed by Teenage Engineering, Aphex Twin or Brian Eno.

## Licence

MIT, see [LICENSE](LICENSE). By Colors In Space.
