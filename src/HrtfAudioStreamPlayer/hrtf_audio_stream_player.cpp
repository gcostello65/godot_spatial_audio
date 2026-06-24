//
// Created by Greg Costello on 6/15/26.
//

#include "hrtf_audio_stream_player.hpp"

using namespace godot;

void HrtfAudioStreamPlayer::_ready() {
    set_panning_strength(0.0f);
}

void HrtfAudioStreamPlayer::_bind_methods() {

}

AzimuthElevation HrtfAudioStreamPlayer::getDirection() {
    AzimuthElevation result{};

    // Part of Node class
    Viewport *viewport = get_viewport();
    if (!viewport) {
        UtilityFunctions::print("No viewport");
        return result;
    }
    // Needed to pull from the godot-cpp header
    Camera3D *camera = viewport->get_camera_3d();
    if (!camera) {
        UtilityFunctions::print("No camera");
        return result;
    }

    // Commenting out this code since it is not needed for poc, letting this go for now, can play around when we go to prod
//    Node3D *listener_node = camera;

//    AudioListener3D *listener = viewport->get_audio_listener_3d();
//    if (listener) {
//        UtilityFunctions::print("Camera is the listener");
//        listener_node = listener;
//    }

    // The position of the AudioStreamPlayer3D
    // The origin part here references the origin of the local coordinates
    const Vector3 global_pos = get_global_transform().get_origin();

    /* The position of the AUdioStreamPlayer3D relative to the listener
     * Since we only have one camera for poc, we can just use the camera local variable. If we have more cameras in the future,
     * we will need to iterate through them like the audio_stream_player_3d.cpp line 388 does */
    const Vector3 local_pos = camera->get_global_transform().orthonormalized().affine_inverse().xform(global_pos);

    // Note to me: no need to calculate the directional vector: that is taken care of by the .xform(global_pos) above
    const real_t horizontal = Math::sqrt(
            local_pos.x * local_pos.x +
            local_pos.z * local_pos.z
    );

    result.azimuth = Math::rad_to_deg(
            Math::atan2(local_pos.x, -local_pos.z)
    );

    /* Note to me: the reason this is local_pos.y and horizontal for atan is the horizontal is the distance
     * in the x-z plane (which is the horizontal plane w.r.t the camera coordinates). Therefor, atan of y and horizontal distance
     * gives phi, the elevation, it is the triangle formed with the vector in the x-z plane and the y height. Camera forward is -z
     */
    result.elevation = Math::rad_to_deg(
            Math::atan2(local_pos.y, horizontal)
    );

    result.distance = local_pos.length();
    return result;
}

void HrtfAudioStreamPlayer::_process(double delta) {
    AzimuthElevation dir = getDirection();

    HrtfAudioStream *hrtf_stream = Object::cast_to<HrtfAudioStream>(get_stream().ptr());

    AzimuthElevation direction = {dir.azimuth,
                                  dir.elevation,
                                  dir.distance};
    if (hrtf_stream) {
        hrtf_stream->set_direction(direction);
    }

    UtilityFunctions::print("Azimuth: ", dir.azimuth);

    UtilityFunctions::print("Elevation: ", dir.elevation);
}
