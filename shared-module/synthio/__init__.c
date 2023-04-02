#include "shared-module/synthio/__init__.h"

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

STATIC int count_active_channels(synthio_midi_span_t *span) {
    int result = 0;
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (span->note[i] != SYNTHIO_SILENCE) {
            result += 1;
        }
    }
    return result;
}


void synthio_synth_synthesize(synthio_synth_t *synth, uint8_t **buffer, uint32_t *buffer_length) {
    uint16_t dur = MIN(SYNTHIO_MAX_DUR, synth->span.dur);
    synth->span.dur -= dur;
    memset(synth->buffer, 0, synth->buffer_length);

    int32_t sample_rate = synth->sample_rate;
    int active_channels = count_active_channels(&synth->span);
    const int16_t *waveform = synth->waveform;
    uint32_t waveform_length = synth->waveform_length;
    int16_t *out_buffer = synth->buffer;
    if (active_channels) {
        int16_t loudness = 0x3fff / (1 + active_channels);
        for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
            if (synth->span.note[chan] == SYNTHIO_SILENCE) {
                synth->accum[chan] = 0;
                continue;
            }
            uint8_t octave = synth->span.note[chan] / 12;
            uint16_t base_freq = notes[synth->span.note[chan] % 12];
            uint32_t accum = synth->accum[chan];
#define SHIFT (16)
            // rate = base_freq * waveform_length
            // den = sample_rate * 2 ^ (10 - octave)
            // den = sample_rate * 2 ^ 10 / 2^octave
            // dds_rate = 2^SHIFT * rate / den
            // dds_rate = 2^(SHIFT-10+octave) * base_freq * waveform_length / sample_rate
            uint32_t dds_rate = (sample_rate / 2 + ((uint64_t)(base_freq * waveform_length) << (SHIFT - 10 + octave))) / sample_rate;

            for (uint16_t i = 0; i < dur; i++) {
                accum += dds_rate;
                if (accum > waveform_length << SHIFT) {
                    accum -= waveform_length << SHIFT;
                }
                int16_t idx = accum >> SHIFT;
                out_buffer[i] += (waveform[idx] * loudness) / 65536;
            }
            synth->accum[chan] = accum;
        }
    }

    *buffer_length = dur * SYNTHIO_BYTES_PER_SAMPLE;
    *buffer = (uint8_t *)synth->buffer;
}

bool synthio_synth_deinited(synthio_synth_t *synth) {
    return synth->buffer == NULL;
}

void synthio_synth_deinit(synthio_synth_t *synth) {
    m_del(uint8_t, synth->buffer, synth->buffer_length);
    synth->buffer = NULL;
}

void synthio_synth_init(synthio_synth_t *synth, uint16_t max_dur) {
    synth->buffer_length = MIN(SYNTHIO_MAX_DUR, max_dur) * SYNTHIO_BYTES_PER_SAMPLE;
    synth->buffer = m_malloc(synth->buffer_length, false);
}

void synthio_synth_get_buffer_structure(synthio_synth_t *synth, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = true;
    *samples_signed = true;
    *max_buffer_length = synth->buffer_length;
    *spacing = 1;
}
