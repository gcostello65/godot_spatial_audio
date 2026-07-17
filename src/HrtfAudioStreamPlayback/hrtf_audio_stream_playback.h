//
// Created by Greg Costello on 6/15/26.
//

#ifndef GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
#define GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H

#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "HrtfUtils/LoadCipic.h"
#include "HrtfUtils/azimuth_elevation.h"

#include <array>
#include <string>
#include <vector>

namespace godot {
    class HrtfAudioStreamPlayback : public AudioStreamPlayback {
    GDCLASS(HrtfAudioStreamPlayback, AudioStreamPlayback);

    private:
        Ref<AudioStreamPlayback> innerPlayback;
        LoadCipic hrir_dataset;
        std::array<float, LoadCipic::TAP_COUNT> delay_line = {};
        std::vector<float> left_hrir;
        std::vector<float> right_hrir;
        AzimuthElevation direction = {};

        float render_hrtf_sample(float mono, int ear);
        void render_fallback_sample(AudioFrame &target_frame, float mono) const;

    protected:
        static void _bind_methods();

    public:
        HrtfAudioStreamPlayback();

        void set_direction(const AzimuthElevation &incoming_direction);

        void set_inner_playback(Ref<AudioStreamPlayback> playback);

        void _start(double p_from_pos) override;

        void _stop() override;

        void _seek(double p_position) override;

        int32_t _mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) override;

        bool _is_playing() const override;

        int32_t _get_loop_count() const override;

        double _get_playback_position() const override;

        void load_hrir_file(const std::string &path);

        void push_delay_sample(float sample);

        float convolve_hrir(const float *hrir) const;
    };

}

#endif //GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
