//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_playback.h"

using namespace godot;

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

int32_t HrtfAudioStreamPlayback::_mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) {
    if (!innerPlayback.is_valid()) {
        return 0;
    }

    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);
    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);

    for (int32_t i = 0; i < frames_to_copy; i++) {
        const Vector2 frame = mixed_frames[i];
        p_buffer[i].left = frame.x;
        p_buffer[i].right = frame.y;
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
