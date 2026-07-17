//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_playback.h"

using namespace godot;

namespace {
constexpr float DEFAULT_OUTPUT_GAIN = 3.0f;
}

HrtfAudioStreamPlayback::HrtfAudioStreamPlayback() :
        left_hrir(LoadCipic::TAP_COUNT),
        right_hrir(LoadCipic::TAP_COUNT) {
}

void HrtfAudioStreamPlayback::_bind_methods() {
}

void HrtfAudioStreamPlayback::set_inner_playback(const Ref<AudioStreamPlayback> playback) {
    innerPlayback = playback;
}

void HrtfAudioStreamPlayback::_start(double p_from_pos) {
    if (innerPlayback.is_valid()) {
        innerPlayback->start(p_from_pos);
    }
}

void HrtfAudioStreamPlayback::_stop() {
    if (innerPlayback.is_valid()) {
        innerPlayback->stop();
    }
}

void HrtfAudioStreamPlayback::_seek(double p_position) {
    if (innerPlayback.is_valid()) {
        innerPlayback->seek(p_position);
    }
}

float HrtfAudioStreamPlayback::convolve_hrir(const float *hrir) const {
    float acc = 0.0f;

    for (int tap = 0; tap < LoadCipic::TAP_COUNT; tap++) {
        acc += delay_line[tap] * hrir[tap];
    }

    return acc;
}

void HrtfAudioStreamPlayback::push_delay_sample(float sample) {
    for (int i = LoadCipic::TAP_COUNT - 1; i > 0; i--) {
        delay_line[i] = delay_line[i - 1];
    }

    delay_line[0] = sample;
}

float HrtfAudioStreamPlayback::render_hrtf_sample(float mono, int ear) {
    const LoadCipic::IndexRange azimuth_indices = LoadCipic::calculate_azimuth_indices(direction.azimuth);
    const LoadCipic::IndexRange elevation_indices = LoadCipic::calculate_elevation_indices(direction.elevation);

    const float *bottom_left = hrir_dataset.get_hrir(ear, azimuth_indices.lower, elevation_indices.lower);
    const float *bottom_right = hrir_dataset.get_hrir(ear, azimuth_indices.upper, elevation_indices.lower);
    const float *top_left = hrir_dataset.get_hrir(ear, azimuth_indices.lower, elevation_indices.upper);
    const float *top_right = hrir_dataset.get_hrir(ear, azimuth_indices.upper, elevation_indices.upper);

    if (!bottom_left || !bottom_right || !top_left || !top_right) {
        return mono;
    }

    std::vector<float> &target_hrir = (ear == LoadCipic::LEFT_EAR) ? left_hrir : right_hrir;
    LoadCipic::interpolate_hrir(
            bottom_left,
            bottom_right,
            top_left,
            top_right,
            direction.azimuth,
            direction.elevation,
            LoadCipic::TAP_COUNT,
            target_hrir
    );

    return convolve_hrir(target_hrir.data());
}

void HrtfAudioStreamPlayback::render_fallback_sample(AudioFrame &target_frame, float mono) const {
    target_frame.left = mono;
    target_frame.right = mono;
}

int32_t HrtfAudioStreamPlayback::_mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) {
    if (!innerPlayback.is_valid()) {
        return 0;
    }

    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);
    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);

    for (int32_t i = 0; i < frames_to_copy; i++) {
        const Vector2 frame = mixed_frames[i];
        const float mono = (frame.x + frame.y) * 0.5f;

        if (!hrir_dataset.is_loaded()) {
            render_fallback_sample(p_buffer[i], mono);
            continue;
        }

        push_delay_sample(mono);

        p_buffer[i].left = render_hrtf_sample(mono, LoadCipic::LEFT_EAR) * DEFAULT_OUTPUT_GAIN;
        p_buffer[i].right = render_hrtf_sample(mono, LoadCipic::RIGHT_EAR) * DEFAULT_OUTPUT_GAIN;
    }

    return frames_to_copy;
}

bool HrtfAudioStreamPlayback::_is_playing() const {
    return innerPlayback.is_valid() && innerPlayback->is_playing();
}

int32_t HrtfAudioStreamPlayback::_get_loop_count() const {
    if (innerPlayback.is_valid()) {
        return innerPlayback->get_loop_count();
    }

    return 0;
}

double HrtfAudioStreamPlayback::_get_playback_position() const {
    if (innerPlayback.is_valid()) {
        return innerPlayback->get_playback_position();
    }

    return 0.0;
}

void HrtfAudioStreamPlayback::load_hrir_file(const std::string &path) {
    Error err = hrir_dataset.load_hrir_file(path);

    if (err != OK) {
        UtilityFunctions::push_error("Failed to load CIPIC HRIR data.");
    }
}

void HrtfAudioStreamPlayback::set_direction(const AzimuthElevation &incoming_direction) {
    direction = incoming_direction;
}
