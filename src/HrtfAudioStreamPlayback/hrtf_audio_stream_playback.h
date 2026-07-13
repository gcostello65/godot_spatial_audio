//
// Created by Greg Costello on 6/15/26.
//

#ifndef GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
#define GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H

#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "HrtfUtils/LoadCipic.h"
#include "HrtfUtils/azimuth_elevation.h"

namespace godot {
    class HrtfAudioStreamPlayback : public AudioStreamPlayback {
    GDCLASS(HrtfAudioStreamPlayback, AudioStreamPlayback);

    private:
        Ref<AudioStreamPlayback> innerPlayback;
        LoadCipic loadCipic;
        static constexpr int HRTF_LEFT = 0;

        static constexpr int HRTF_RIGHT = 1;

        static constexpr int HRTF_TAPS = 200;
        double circle_phase = 0.0;

        double circle_speed_hz = 0.25; // one full circle every 4 seconds

        static constexpr double TWO_PI = 6.28318530717958647692;

        float delay_line[HRTF_TAPS] = {};

        AzimuthElevation direction;
    protected:
        static void _bind_methods();

    public:
        void set_direction(AzimuthElevation& incoming_direction);

        void set_inner_playback(Ref<AudioStreamPlayback> playback);

        void _start(double p_from_pos) override;

        void _stop() override;

        void _seek(double p_position) override;

        int32_t _mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) override;

        bool _is_playing() const override;

        int32_t _get_loop_count() const override;

        double _get_playback_position() const override;

        void load_hrir_file(std::string path);

        void push_delay_sample(float sample);

        float convolve_200(const float *hrir) const;
    };

}

#endif //GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
