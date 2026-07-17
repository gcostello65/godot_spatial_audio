//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_player.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void HrtfAudioStreamPlayer::_ready() {
    set_panning_strength(GODOT_PANNING_DISABLED);
}

void HrtfAudioStreamPlayer::_bind_methods() {

}

AzimuthElevation HrtfAudioStreamPlayer::get_direction_from_current_camera() {
    AzimuthElevation result{};

    Viewport *viewport = get_viewport();
    if (!viewport) {
        UtilityFunctions::print("No viewport");
        return result;
    }

    Camera3D *camera = viewport->get_camera_3d();
    if (!camera) {
        UtilityFunctions::print("No camera");
        return result;
    }

    const Vector3 global_pos = get_global_transform().get_origin();
    const Vector3 local_pos = camera->get_global_transform().orthonormalized().affine_inverse().xform(global_pos);
    const real_t horizontal = Math::sqrt(
            local_pos.x * local_pos.x +
            local_pos.z * local_pos.z
    );

    result.azimuth = Math::rad_to_deg(
            Math::atan2(local_pos.x, -local_pos.z)
    );

    result.elevation = Math::rad_to_deg(
            Math::atan2(local_pos.y, horizontal)
    );

    result.distance = local_pos.length();
    return result;
}

void HrtfAudioStreamPlayer::_process(double delta) {
    (void)delta;

    AzimuthElevation direction = get_direction_from_current_camera();

    HrtfAudioStream *hrtf_stream = Object::cast_to<HrtfAudioStream>(get_stream().ptr());
    if (hrtf_stream) {
        hrtf_stream->set_direction(direction);
    }
}
