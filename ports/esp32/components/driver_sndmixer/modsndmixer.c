#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/objarray.h"

#include "sndmixer.h"

#define ASSERT_STARTED() {if (!sndmixer_started) { \
                            mp_raise_ValueError(msg_error_not_started); \
                            return mp_const_none; \
                          }}

#define MP_OBJ_IS_CLOSURE(o) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_closure))
#define MP_OBJ_IS_METH(o) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_bound_method))

bool sndmixer_started = 0;
int sndmixer_channels = 0;

const char *msg_error_not_started = "sndmixer task not started!";

static uint8_t *mp_obj_to_u8_ptr(mp_obj_t obj, size_t *len) {
  if (MP_OBJ_IS_TYPE(obj, &mp_type_bytes)) {
    return (uint8_t *)mp_obj_str_get_data(obj, len);
  } else if(MP_OBJ_IS_TYPE(obj, &mp_type_bytearray)) {
    mp_obj_array_t *array = MP_OBJ_TO_PTR(obj);
    *len = array->len;
    return array->items;
  }
  mp_raise_ValueError("Expected a bytes or bytearray object.");
  return NULL;
}

static mp_obj_t modsndmixer_begin(mp_uint_t n_args, const mp_obj_t *args) {
  int channels = (n_args>0) ? mp_obj_get_int(args[0]) : 1;
  int stereo   = (n_args>1) ? mp_obj_is_true(args[1]) : false;
  if (!sndmixer_started) {
    sndmixer_init(channels, stereo);
    sndmixer_started  = 1;
    sndmixer_channels = channels;
  } else {
    printf(
        "WARNING: The sndmixer task has already been started! The number of available channels "
        "(%d) can not be changed.\n",
        sndmixer_channels);
  }
  return mp_obj_new_int(sndmixer_channels);
}

static mp_obj_t modsndmixer_play(mp_obj_t _id) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = mp_obj_get_int(_id);
  sndmixer_play(id);
  return mp_const_none;
}

static mp_obj_t modsndmixer_pause(mp_obj_t _id) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = mp_obj_get_int(_id);
  sndmixer_pause(id);
  return mp_const_none;
}

static mp_obj_t modsndmixer_stop(mp_obj_t _id) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = mp_obj_get_int(_id);
  sndmixer_stop(id);
  return mp_const_none;
}

static mp_obj_t modsndmixer_pause_all() {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  sndmixer_pause_all();
  return mp_const_none;
}

static mp_obj_t modsndmixer_resume_all() {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  sndmixer_resume_all();
  return mp_const_none;
}

static mp_obj_t modsndmixer_loop(mp_obj_t _id, mp_obj_t _loop) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id   = mp_obj_get_int(_id);
  int loop = mp_obj_get_int(_loop);
  sndmixer_set_loop(id, loop);
  return mp_const_none;
}

static mp_obj_t modsndmixer_volume(mp_obj_t _id, mp_obj_t _volume) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id     = mp_obj_get_int(_id);
  int volume = mp_obj_get_int(_volume);
  sndmixer_set_volume(id, volume);
  return mp_const_none;
}

/* Audio sources */

static mp_obj_t modsndmixer_wav(mp_obj_t _data) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  mp_uint_t len;
  if (!MP_OBJ_IS_TYPE(_data, &mp_type_bytes)) {
    mp_raise_ValueError("Expected a bytestring like object.");
    return mp_const_none;
  }
  uint8_t *data = (uint8_t *)mp_obj_str_get_data(_data, &len);

  int id = sndmixer_queue_wav(data, data + len - 1, 1);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_wav_stream(mp_obj_t _stream) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = sndmixer_queue_wav_stream(mp_stream_posix_read, mp_stream_posix_lseek, _stream);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_mod(mp_obj_t _data) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  mp_uint_t len;
  if (!MP_OBJ_IS_TYPE(_data, &mp_type_bytes)) {
    mp_raise_ValueError("Expected a bytestring like object.");
    return mp_const_none;
  }
  uint8_t *data = (uint8_t *)mp_obj_str_get_data(_data, &len);

  int id = sndmixer_queue_mod(data, data + len - 1);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_mp3(mp_obj_t _data) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  size_t len;
  uint8_t *data = mp_obj_to_u8_ptr(_data, &len);
  int id = sndmixer_queue_mp3(data, data + len - 1);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_mp3_stream(mp_obj_t _stream) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = sndmixer_queue_mp3_stream(mp_stream_posix_read, mp_stream_posix_lseek, _stream);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_opus(mp_obj_t _data) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  mp_uint_t len;
  uint8_t *data = mp_obj_to_u8_ptr(_data, &len);
  int id = sndmixer_queue_opus(data, data + len);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_opus_stream(mp_obj_t _stream) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = sndmixer_queue_opus_stream(mp_stream_posix_read, _stream);
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_synth() {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id = sndmixer_queue_synth();
//  sndmixer_play(id);
  return mp_obj_new_int(id);
}

static mp_obj_t modsndmixer_freq(mp_obj_t _id, mp_obj_t _freq) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id   = mp_obj_get_int(_id);
  int freq = mp_obj_get_int(_freq);
  sndmixer_freq(id, freq);
  return mp_const_none;
}

static mp_obj_t modsndmixer_waveform(mp_obj_t _id, mp_obj_t _waveform) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id       = mp_obj_get_int(_id);
  int waveform = mp_obj_get_int(_waveform);
  sndmixer_waveform(id, waveform);
  return mp_const_none;
}

static mp_obj_t modsndmixer_on_finished(mp_obj_t _id, mp_obj_t _handle) {
  if (!sndmixer_started) {
    mp_raise_ValueError(msg_error_not_started);
    return mp_const_none;
  }
  int id       = mp_obj_get_int(_id);
  if(! MP_OBJ_IS_FUN(_handle) && (! MP_OBJ_IS_METH(_handle)) && (! MP_OBJ_IS_CLOSURE(_handle)))
  {
    mp_raise_ValueError("Callback function expected!");
    return mp_const_none;
  }
  sndmixer_set_callback(id, mp_sched_schedule, _handle);
  return mp_const_none;
}

static mp_obj_t modsndmixer_beat_sync_start(mp_obj_t _bpm) {
  ASSERT_STARTED();
  int bpm       = mp_obj_get_int(_bpm);

  sndmixer_beat_sync_start(bpm);
  return mp_const_none;
}

static mp_obj_t modsndmixer_beat_sync_stop() {
  ASSERT_STARTED();
  sndmixer_beat_sync_stop();
  return mp_const_none;
}

static mp_obj_t modsndmixer_start_at_next(mp_obj_t _id, mp_obj_t _play_at_next) {
  ASSERT_STARTED();
  int id            = mp_obj_get_int(_id);
  int play_at_next  = mp_obj_get_int(_play_at_next);

  sndmixer_start_at_next(id, play_at_next);
  return mp_const_none;
}
/* --- */
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modsndmixer_begin_obj, 0, 2, modsndmixer_begin);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_play_obj, modsndmixer_play);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_pause_obj, modsndmixer_pause);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_stop_obj, modsndmixer_stop);
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_pause_all_obj, modsndmixer_pause_all);
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_resume_all_obj, modsndmixer_resume_all);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_loop_obj, modsndmixer_loop);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_volume_obj, modsndmixer_volume);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_wav_obj, modsndmixer_wav);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_wav_stream_obj, modsndmixer_wav_stream);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_mod_obj, modsndmixer_mod);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_mp3_obj, modsndmixer_mp3);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_mp3_stream_obj, modsndmixer_mp3_stream);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_opus_obj, modsndmixer_opus);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_opus_stream_obj, modsndmixer_opus_stream);
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_synth_obj, modsndmixer_synth);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_freq_obj, modsndmixer_freq);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_waveform_obj, modsndmixer_waveform);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_on_finished_obj, modsndmixer_on_finished);
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_beat_sync_start_obj, modsndmixer_beat_sync_start);
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_beat_sync_stop_obj, modsndmixer_beat_sync_stop);
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_start_at_next_obj, modsndmixer_start_at_next);

static const mp_rom_map_elem_t sndmixer_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_begin), MP_ROM_PTR(&modsndmixer_begin_obj)},
    {MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&modsndmixer_play_obj)},
    {MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&modsndmixer_pause_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&modsndmixer_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_pause_all), MP_ROM_PTR(&modsndmixer_pause_all_obj)},
    {MP_ROM_QSTR(MP_QSTR_resume_all), MP_ROM_PTR(&modsndmixer_resume_all_obj)},
    {MP_ROM_QSTR(MP_QSTR_loop), MP_ROM_PTR(&modsndmixer_loop_obj)},
    {MP_ROM_QSTR(MP_QSTR_volume), MP_ROM_PTR(&modsndmixer_volume_obj)},
    {MP_ROM_QSTR(MP_QSTR_wav), MP_ROM_PTR(&modsndmixer_wav_obj)},
    {MP_ROM_QSTR(MP_QSTR_wav_stream), MP_ROM_PTR(&modsndmixer_wav_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_mod), MP_ROM_PTR(&modsndmixer_mod_obj)},
    {MP_ROM_QSTR(MP_QSTR_mp3), MP_ROM_PTR(&modsndmixer_mp3_obj)},
    {MP_ROM_QSTR(MP_QSTR_mp3_stream), MP_ROM_PTR(&modsndmixer_mp3_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_opus), MP_ROM_PTR(&modsndmixer_opus_obj)},
    {MP_ROM_QSTR(MP_QSTR_opus_stream), MP_ROM_PTR(&modsndmixer_opus_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_synth), MP_ROM_PTR(&modsndmixer_synth_obj)},
    {MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&modsndmixer_freq_obj)},
    {MP_ROM_QSTR(MP_QSTR_waveform), MP_ROM_PTR(&modsndmixer_waveform_obj)},
    {MP_ROM_QSTR(MP_QSTR_on_finished), MP_ROM_PTR(&modsndmixer_on_finished_obj)},
    {MP_ROM_QSTR(MP_QSTR_beat_sync_start), MP_ROM_PTR(&modsndmixer_beat_sync_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_beat_sync_stop), MP_ROM_PTR(&modsndmixer_beat_sync_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_start_at_next), MP_ROM_PTR(&modsndmixer_start_at_next_obj)}
};

static MP_DEFINE_CONST_DICT(sndmixer_module_globals, sndmixer_module_globals_table);

const mp_obj_module_t sndmixer_module = {
    .base    = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&sndmixer_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_sndmixer, sndmixer_module);
