// Halcyon's own sound checks, compiled for the host and run by
// python sdk.py test --machine <folder>.
#include "../engine.cpp"
#include <cstdio>

namespace {
const opxy::i32 peak = 536870912;   // definition.json: outputPeakQ31
const opxy::i32 hi = 0x7fffffff, mid = 0x40000000, lo = 0;

struct Result { long long largest, early, late; bool bounded; };

// Plays one note; `early` sums |samples| over the first 20 ms, `late` over the last 64 blocks.
Result play(const opxy::i32* controls, opxy::u32 increment, unsigned blocks) {
    static halcyon::Voice voice;
    static halcyon::Shared shared;
    opxy::i32 parameters[8] = {controls[0], controls[1], controls[2], controls[3], controls[4], controls[5], controls[6], controls[7]};
    opxy::u32 pitch[opxy::block_frames];
    for (unsigned i = 0; i < opxy::block_frames; ++i) pitch[i] = increment;
    opxy::i32 output[opxy::block_frames];
    halcyon::initialize(voice, shared, 3);
    halcyon::start(voice, shared, 3);
    Result r = {0, 0, 0, true};
    for (unsigned block = 0; block < blocks; ++block) {
        halcyon::render(voice, shared, 3, output, parameters, pitch);
        for (unsigned i = 0; i < opxy::block_frames; ++i) {
            const long long s = output[i];
            if (s < -peak || s > peak) r.bounded = false;
            const long long a = s < 0 ? -s : s;
            if (a > r.largest) r.largest = a;
            if (block < 14) r.early += a >> 16;
            if (block + 64 >= blocks) r.late += a >> 16;
        }
    }
    return r;
}

// Correlation of the last second with a sine at `cycles` times the note: how much of that partial there is.
long long partial(const opxy::i32* controls, opxy::u32 increment, opxy::u32 times_two) {
    static halcyon::Voice voice;
    static halcyon::Shared shared;
    opxy::i32 parameters[8] = {controls[0], controls[1], controls[2], controls[3], controls[4], controls[5], controls[6], controls[7]};
    opxy::u32 pitch[opxy::block_frames];
    for (unsigned i = 0; i < opxy::block_frames; ++i) pitch[i] = increment;
    opxy::i32 output[opxy::block_frames];
    halcyon::initialize(voice, shared, 5);
    halcyon::start(voice, shared, 5);
    long long re = 0, im = 0;
    opxy::u32 probe = 0;
    const opxy::u32 step = increment / 2 * times_two;
    for (unsigned block = 0; block < 4 * 689; ++block) {
        halcyon::render(voice, shared, 5, output, parameters, pitch);
        for (unsigned i = 0; i < opxy::block_frames; ++i) {
            if (block >= 3 * 689) {
                re += static_cast<long long>(output[i] >> 16) * halcyon::table_sine(probe) >> 15;
                im += static_cast<long long>(output[i] >> 16) * halcyon::table_sine(probe + 0x40000000u) >> 15;
            }
            probe += step;
        }
    }
    if (re < 0) re = -re;
    if (im < 0) im = -im;
    return re > im ? re + im / 2 : im + re / 2;
}
}

int main() {
    const opxy::u32 notes[4] = {1594582u, 9989633u, 42852281u, 600000000u};  // ~16 Hz, 103 Hz, 440 Hz, ~6.2 kHz
    bool ok = true;
    // Every corner of the eight controls (off or full), and every control at
    // half with the others at full, at low to high notes, held for six
    // seconds so the late octaves and the slowest bloom have arrived.
    for (unsigned a = 0; a < 256 + 8; ++a) {
        opxy::i32 controls[8];
        for (unsigned k = 0; k < 8; ++k) controls[k] = a < 256 ? (a >> k & 1u ? hi : lo) : (a - 256 == k ? mid : hi);
        for (unsigned n = 0; n < 4; ++n) {
            const Result r = play(controls, notes[n], 6 * 689);
            if (!r.bounded) { std::printf("FAIL above the declared peak (case %u, note %u)\n", a, n); ok = false; }
            else if (r.late == 0) { std::printf("FAIL silent (case %u, note %u)\n", a, n); ok = false; }
        }
    }
    // A slow Swell blooms in: the first 20 ms are far quieter than the held note.
    const opxy::i32 slow[8] = {lo, hi, lo, lo, lo, lo, lo, lo};
    const Result s = play(slow, 42852281u, 8 * 689);
    if (!(s.early * 50 < s.late)) { std::puts("FAIL slow Swell does not bloom in"); ok = false; }
    // At rest the tone is clearly audible.
    const opxy::i32 plain[8] = {lo, lo, lo, lo, lo, lo, lo, lo};
    if (play(plain, 42852281u, 689).largest < peak / 2) { std::puts("FAIL too quiet"); ok = false; }
    // The saw reinforces the sine's fundamental rather than cancelling it.
    const opxy::i32 warm[8] = {hi, lo, lo, lo, lo, lo, lo, lo};
    const long long tone = partial(plain, 9989633u, 2);
    if (partial(warm, 9989633u, 2) * 2 < tone) { std::puts("FAIL Warmth cancels the fundamental"); ok = false; }
    // Air brings in the octave above and then the octave below.
    const opxy::i32 airy[8] = {lo, lo, lo, hi, lo, lo, lo, lo};
    if (partial(airy, 9989633u, 4) * 8 < tone) { std::puts("FAIL no octave above"); ok = false; }
    if (partial(airy, 9989633u, 1) * 8 < tone) { std::puts("FAIL no octave below"); ok = false; }
    if (partial(plain, 9989633u, 4) * 50 > tone) { std::puts("FAIL octave above present with Air off"); ok = false; }
    // Chord adds the fifth (3x the half-note probe) and the ninth (4.5x).
    const opxy::i32 chorded[8] = {lo, lo, lo, lo, lo, hi, lo, lo};
    if (partial(chorded, 9989633u, 3) * 8 < tone) { std::puts("FAIL no fifth"); ok = false; }
    if (partial(plain, 9989633u, 3) * 50 > tone) { std::puts("FAIL fifth present with Chord off"); ok = false; }
    // Ensemble keeps the note's pitch: its fundamental stays strong on average.
    const opxy::i32 doubled[8] = {lo, lo, lo, lo, lo, lo, hi, lo};
    if (play(doubled, 42852281u, 4 * 689).late * 3 < play(plain, 42852281u, 4 * 689).late) { std::puts("FAIL Ensemble cancels the note"); ok = false; }
    // Hiss is audible but stays below the tone.
    const opxy::i32 hissy[8] = {lo, lo, lo, lo, hi, lo, lo, lo};
    const long long with_hiss = play(hissy, 42852281u, 2 * 689).late, without = play(plain, 42852281u, 2 * 689).late;
    if (with_hiss == without) { std::puts("FAIL no hiss"); ok = false; }
    // Age saturates: a pure sine gains its third harmonic.
    const opxy::i32 aged[8] = {lo, lo, lo, lo, lo, lo, lo, hi};
    if (partial(aged, 9989633u, 6) * 40 < tone) { std::puts("FAIL Age adds no harmonics"); ok = false; }
    return ok ? 0 : 1;
}
