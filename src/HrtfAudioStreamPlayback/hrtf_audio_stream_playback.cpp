//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_playback.h"

using namespace godot;

void HrtfAudioStreamPlayback::_bind_methods() {
//    ClassDB::bind_method(
//            D_METHOD("load_hrir_file", "path"),
//            &HrtfAudioStreamPlayback::load_hrir_file
//    );
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

float HrtfAudioStreamPlayback::convolve_200(const float *hrir) const {

    float acc = 0.0f;

    for (int tap = 0; tap < HRTF_TAPS; tap++) {

        acc += delay_line[tap] * hrir[tap];
//        UtilityFunctionsdelay_line[tap]);
    }

    return acc;

}

void HrtfAudioStreamPlayback::push_delay_sample(float sample) {

    for (int i = HRTF_TAPS - 1; i > 0; i--) {

        delay_line[i] = delay_line[i - 1];

    }

    delay_line[0] = sample;

}

int32_t HrtfAudioStreamPlayback::_mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) {
////    if (!innerPlayback.is_valid()) {
////        return 0;
////    }
////
////    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);
////    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);
////
////    for (int32_t i = 0; i < frames_to_copy; i++) {
////        const Vector2 frame = mixed_frames[i];
////
////        p_buffer[i].left = frame.x;
////        p_buffer[i].right = frame.y;
////    }
////
////    return frames_to_copy;
//
//    if (!innerPlayback.is_valid()) {
//
//        for (int i = 0; i < p_frames; i++) {
//
//            p_buffer[i] = AudioFrame({0.0f, 0.0f});
//
//        }
//
//        return p_frames;
//
//    }
//
//    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);
//
//    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);
//
////    if (!loadCipic.loa()) {
////
////        // Fallback to pass-through so failed HRIR load doesn't kill audio.
////
////        for (int32_t i = 0; i < frames_to_copy; i++) {
////
////            const Vector2 frame = mixed_frames[i];
////
////            p_buffer[i].left = frame.x;
////
////            p_buffer[i].right = frame.y;
////
////        }
////
////        return frames_to_copy;
////
////    }
//
//    constexpr int az_idx = 12;
//
//    constexpr int el_idx = 40;
//
//    const float *left_hrir = loadCipic.get_hrir(HRTF_LEFT, az_idx, el_idx);
////    UtilityFunctions::print(left_hrir[0]);
//
//    const float *right_hrir = loadCipic.get_hrir(HRTF_RIGHT, az_idx, el_idx);
//
//    if (left_hrir == nullptr || right_hrir == nullptr) {
//
//        for (int32_t i = 0; i < frames_to_copy; i++) {
//
//            const Vector2 frame = mixed_frames[i];
//
//            p_buffer[i].left = frame.x;
//
//            p_buffer[i].right = frame.y;
////            UtilityFunctions::print(frame.x);
//
//        }
//
//        return frames_to_copy;
//
//    }
//
//    for (int32_t i = 0; i < frames_to_copy; i++) {
//
//        const Vector2 frame = mixed_frames[i];
//
//        const float mono = frame.x;
//
//        push_delay_sample(mono);
//
//        const float left = convolve_200(left_hrir);
//
//        const float right = convolve_200(right_hrir);
//
//        p_buffer[i].left = left;
//
//        p_buffer[i].right = right;
//
////        UtilityFunctions::print(left);
//    }
//
//    return frames_to_copy;

    if (!innerPlayback.is_valid()) {

        return 0;

    }

    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);

    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);


    // Crude fake orbit:

    // front -> right -> behind -> left -> repeat

    //

    // Assumes 48kHz-ish audio.

    // 48000 samples = about 1 second per position.

    static int sample_counter = 0;

    constexpr int samples_per_step = 48000;

    for (int32_t i = 0; i < frames_to_copy; i++) {

        const Vector2 frame = mixed_frames[i];

        const float mono = frame.x;

        const int step = (sample_counter / samples_per_step) % 4;

        sample_counter++;

        int az_idx = 12;

        int el_idx = 8;

        if (step == 0) {

            az_idx = 12;

            el_idx = 8;   // front

        } else if (step == 1) {

            az_idx = 24;

            el_idx = 8;   // right

        } else if (step == 2) {

            az_idx = 12;

            el_idx = 40;  // behind

        } else {

            az_idx = 0;

            el_idx = 8;   // left

        }

        const float *left_hrir = loadCipic.get_hrir(HRTF_LEFT, az_idx, el_idx);

        const float *right_hrir = loadCipic.get_hrir(HRTF_RIGHT, az_idx, el_idx);

        if (left_hrir == nullptr || right_hrir == nullptr) {

            p_buffer[i].left = mono;

            p_buffer[i].right = mono;

            continue;

        }

        push_delay_sample(mono);

        const float left = convolve_200(left_hrir);

        const float right = convolve_200(right_hrir);

        // Debug gain because HRIR convolution may be quiet.

        constexpr float debug_gain = 3.0f;

        p_buffer[i].left = left * debug_gain;

        p_buffer[i].right = right * debug_gain;

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

void HrtfAudioStreamPlayback::load_hrir_file(std::string path) {
    Error err = loadCipic.load_hrir_file(path);

    if (err != OK) {
        UtilityFunctions::push_error("Failed to load CIPIC HRIR data.");
        return;
    }
}
