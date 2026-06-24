//
// Created by Greg Costello on 6/15/26.
//

#ifndef GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H
#define GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H

#include "HrtfAudioStream/hrtf_audio_stream.hpp"

#include "godot_cpp/classes/audio_stream_player3d.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>

namespace godot {
    class HrtfAudioStreamPlayer : public AudioStreamPlayer3D {
    GDCLASS(HrtfAudioStreamPlayer, AudioStreamPlayer3D);

    private:
        // Going to use the current camera. Will eventually allow for custom camera as an exposed attribute
        AzimuthElevation getDirection();
    protected:
        static void _bind_methods();

    public:
        void _process(double delta) override;
        void _ready() override;
    };

}

#endif //GODOT_SPATIAL_AUDIO_HRTF_AUDIO_STREAM_PLAYER_H
