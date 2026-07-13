//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_playback.h"

using namespace godot;
std::vector<float> left_hrir(200);
std::vector<float> right_hrir(200);

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

float HrtfAudioStreamPlayback::convolve_200(const float *hrir) const {

    float acc = 0.0f;

    for (int tap = 0; tap < HRTF_TAPS; tap++) {
        acc += delay_line[tap] * hrir[tap];
    }
    return acc;
}

void HrtfAudioStreamPlayback::push_delay_sample(float sample) {

    for (int i = HRTF_TAPS - 1; i > 0; i--) {
        delay_line[i] = delay_line[i - 1];
    }

    delay_line[0] = sample;
}

static constexpr std::array<float, 25> CIPIC_AZIMUTH_LUT = {
        -80.0f, -65.0f, -55.0f, -45.0f, -40.0f,
        -35.0f, -30.0f, -25.0f, -20.0f, -15.0f,
        -10.0f,  -5.0f,   0.0f,   5.0f,  10.0f,
        15.0f,  20.0f,  25.0f,  30.0f,  35.0f,
        40.0f,  45.0f,  55.0f,  65.0f,  80.0f
};

static const float low_ele = -45.0;
static const float high_ele = 230.625;

// no lut needed for the elevations since all are at constant angle increments. Just need to clamp and then divide by 5.625

std::tuple<int, int> calculate_az_index(float angle) {
    auto lower_bound = std::lower_bound(CIPIC_AZIMUTH_LUT.begin(), CIPIC_AZIMUTH_LUT.end(), angle);
    if (lower_bound == CIPIC_AZIMUTH_LUT.begin()) {
        return {0,0};
    }

    if (lower_bound == CIPIC_AZIMUTH_LUT.end()) {
        int end_index = CIPIC_AZIMUTH_LUT.size() - 1;
        return {end_index, end_index};
    }

    // The way iterator arithmetic works, this is how to get the index of the angle. Subtract the beginning iterator from the returned one.
    // Look up to see if this is the industry standard way, maybe there is a better LUT version
    // Since we have handled edge cases above, this is a simple subtract
    int upper_index = static_cast<int>(lower_bound - CIPIC_AZIMUTH_LUT.begin());
    int lower_index = upper_index - 1;

    return {lower_index, upper_index};

}

std::tuple<int, int>  calculate_el_index(float angle) {
    // Since these values are in 5.625 degree increments in the CIPIC dataset, we can just do the index multiplication easily with a clamp
    float clamp_angle = std::clamp(angle, low_ele, high_ele);

    // From the pdf, elevations = -45 + 5.625*(0:49)
    // So we can do the inverse index = elevations + 45 / 5.625
    int lower_index = static_cast<int>(floor((clamp_angle + 45.0f) / 5.625f));

    int upper_index;
    // Magic number alert: 50 is the number of angles in the elevation, so just checking to see if we are at the end
    // Again, this is hardcoded for now and will be generalized later
    if (lower_index == 49) {
        upper_index = lower_index;
    } else {
        upper_index = lower_index + 1;
    }

    return {lower_index, upper_index};
}

int32_t HrtfAudioStreamPlayback::_mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) {
    if (!innerPlayback.is_valid()) {
        return 0;
    }

    PackedVector2Array mixed_frames = innerPlayback->mix_audio(p_rate_scale, p_frames);
    const int32_t frames_to_copy = MIN(static_cast<int32_t>(mixed_frames.size()), p_frames);

    static int sample_counter = 0;
    constexpr int samples_per_step = 48000;

    // Pseudo moving the sound, will be replaced by the actual interpolation
    for (int32_t i = 0; i < frames_to_copy; i++) {
        const Vector2 frame = mixed_frames[i];
        const float mono = frame.x;
        const int step = (sample_counter / samples_per_step) % 4;
        sample_counter++;

        // Going to code this based on the CIPIC dataset and then make more general later when I have a poc
        const std::tuple<int, int> az_idx = calculate_az_index(direction.azimuth);
        const std::tuple<int, int> el_idx = calculate_el_index(direction.elevation);

        // Gathering the 4 hrirs for left and right in order to interpolate
        const float *left_hrir_lower_left = loadCipic.get_hrir(HRTF_LEFT, std::get<0>(az_idx), std::get<0>(el_idx));
        const float *left_hrir_lower_right = loadCipic.get_hrir(HRTF_LEFT, std::get<0>(az_idx), std::get<1>(el_idx));
        const float *left_hrir_upper_left = loadCipic.get_hrir(HRTF_LEFT, std::get<1>(az_idx), std::get<0>(el_idx));
        const float *left_hrir_upper_right = loadCipic.get_hrir(HRTF_LEFT, std::get<1>(az_idx), std::get<1>(el_idx));

        const float *right_hrir_lower_left = loadCipic.get_hrir(HRTF_RIGHT, std::get<0>(az_idx), std::get<0>(el_idx));
        const float *right_hrir_lower_right = loadCipic.get_hrir(HRTF_RIGHT, std::get<0>(az_idx), std::get<1>(el_idx));
        const float *right_hrir_upper_left = loadCipic.get_hrir(HRTF_RIGHT, std::get<1>(az_idx), std::get<0>(el_idx));
        const float *right_hrir_upper_right = loadCipic.get_hrir(HRTF_RIGHT, std::get<1>(az_idx), std::get<1>(el_idx));

        // Eventually add the null checks back in, but let's just push through with an assumption that the hrir are there for now.
//        if (left_hrir == nullptr || right_hrir == nullptr) {
//            p_buffer[i].left = mono;
//            p_buffer[i].right = mono;
//            continue;
//        }

        push_delay_sample(mono);

        LoadCipic::interpolate_hrir(left_hrir_lower_left, left_hrir_lower_right,
                                                             left_hrir_upper_left, left_hrir_upper_right,
                                                             direction.azimuth, direction.elevation, 200, left_hrir);
        LoadCipic::interpolate_hrir(right_hrir_lower_left, right_hrir_lower_right,
                                                              right_hrir_upper_left, right_hrir_upper_right,
                                                              direction.azimuth, direction.elevation, 200, right_hrir);

        const float left = convolve_200(left_hrir.data());
        const float right = convolve_200(right_hrir.data());

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

void HrtfAudioStreamPlayback::set_direction(AzimuthElevation &incoming_direction) {
    this->direction = incoming_direction;
}
