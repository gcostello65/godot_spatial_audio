//
// Created by Greg Costello on 6/15/26.
//

#ifndef GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
#define GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H

#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"

namespace godot {
    class HrtfAudioStreamPlayback : public AudioStreamPlayback {
    GDCLASS(HrtfAudioStreamPlayback, AudioStreamPlayback);

    private:
        Ref<AudioStreamPlayback> innerPlayback;

    protected:
        static void _bind_methods();

    public:
        void set_inner_playback(Ref<AudioStreamPlayback> playback);
        void _start(double p_from_pos) override;
        void _stop() override;
        void _seek(double p_position) override;
        int32_t _mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) override;
        bool _is_playing() const override;
        int32_t _get_loop_count() const override;
        double _get_playback_position() const override;
    };

}

#endif //GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYBACK_H
