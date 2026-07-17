//
// Created by Greg Costello on 6/4/26.
//
#include "hrtf_audio_stream.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include <string>

using namespace godot;

void HrtfAudioStream::_bind_methods() {
    ClassDB::bind_method(
            D_METHOD("set_source_stream", "stream"),
            &HrtfAudioStream::set_source_stream);
    ClassDB::bind_method(
            D_METHOD("get_source_stream"),
            &HrtfAudioStream::get_source_stream);

    ClassDB::bind_method(
            D_METHOD("set_hrir_path", "path"),
            &HrtfAudioStream::set_hrir_path);
    ClassDB::bind_method(
            D_METHOD("get_hrir_path"),
            &HrtfAudioStream::get_hrir_path);

    ADD_PROPERTY(
            PropertyInfo(
                    Variant::OBJECT,
                    "source_stream",
                    PROPERTY_HINT_RESOURCE_TYPE,
                    "AudioStream"
            ),
            "set_source_stream",
            "get_source_stream");
    ADD_PROPERTY(
            PropertyInfo(
                    Variant::STRING,
                    "hrir_path",
                    PROPERTY_HINT_FILE,
                    "*.bin"
            ),
            "set_hrir_path",
            "get_hrir_path");
}

HrtfAudioStream::HrtfAudioStream() {
}

HrtfAudioStream::~HrtfAudioStream() {
}

void HrtfAudioStream::set_hrir_path(const String &p_path) {
    hrir_path = p_path;
}

String HrtfAudioStream::get_hrir_path() const {
    return hrir_path;
}

Ref<AudioStreamPlayback> HrtfAudioStream::_instantiate_playback() const {
    if (source_stream.is_null()) {
        UtilityFunctions::push_error("HrtfAudioStream requires a source_stream before playback can be instantiated.");
        return Ref<AudioStreamPlayback>();
    }

    Ref<HrtfAudioStreamPlayback> hrtfPlayback;
    hrtfPlayback.instantiate();
    active_playback = hrtfPlayback;

    hrtfPlayback->set_inner_playback(source_stream->instantiate_playback());
    hrtfPlayback->load_hrir_file(std::string(hrir_path.utf8().get_data()));
    hrtfPlayback->set_direction(direction);

    return hrtfPlayback;
}

String HrtfAudioStream::_get_stream_name() const {
    return "HrtfAudioStream";
}

bool HrtfAudioStream::_is_monophonic() const {
    return false;
}

Ref<AudioStream> HrtfAudioStream::get_source_stream() const {
    return source_stream;
}

void HrtfAudioStream::set_source_stream(const Ref<AudioStream> &p_source_stream) {
    source_stream = p_source_stream;
}

void HrtfAudioStream::set_direction(const AzimuthElevation &p_direction) {
    direction = p_direction;

    if (active_playback.is_valid()) {
        active_playback->set_direction(direction);
    }
}

AzimuthElevation HrtfAudioStream::get_direction() const {
    return direction;
}
