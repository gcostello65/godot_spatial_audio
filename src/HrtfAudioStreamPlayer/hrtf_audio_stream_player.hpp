//
// Created by Greg Costello on 6/15/26.
//

#ifndef GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H
#define GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H

#include "HrtfAudioStream/hrtf_audio_stream.hpp"

#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>

namespace godot {
    class HrtfAudioStreamPlayer : public AudioStreamPlayer3D {
    GDCLASS(HrtfAudioStreamPlayer, AudioStreamPlayer3D);

    private:
        static constexpr float GODOT_PANNING_DISABLED = 0.0f;

        AzimuthElevation get_direction_from_current_camera();

    protected:
        static void _bind_methods();

    public:
        void _process(double delta) override;
        void _ready() override;
    };

}

#endif //GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H
