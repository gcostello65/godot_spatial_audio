//
// Created by Greg Costello on 6/4/26.
//
#include "hrtf_audio_stream.hpp"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

void HrtfAudioStream::_bind_methods() {
    ClassDB::bind_method(
            D_METHOD("set_source_stream", "stream"),
            &HrtfAudioStream::set_source_stream);
    ClassDB::bind_method(
            D_METHOD("get_source_stream"),
            &HrtfAudioStream::get_source_stream);
    ADD_PROPERTY(
            PropertyInfo(
                    Variant::OBJECT,
                    "source_stream",
                    PROPERTY_HINT_RESOURCE_TYPE,
                    "AudioStream"
            ),
            "set_source_stream",
            "get_source_stream");
}

HrtfAudioStream::HrtfAudioStream() {
}

HrtfAudioStream::~HrtfAudioStream() {
    // Add your cleanup here.
}

Ref<AudioStreamPlayback> HrtfAudioStream::_instantiate_playback() const {
    // Set the source playback to provide samples when fetched from the HrtfAudioStreamPlayback
    Ref<HrtfAudioStreamPlayback> hrtfPlayback;
    hrtfPlayback.instantiate();

    hrtfPlayback->set_inner_playback(sourceStream->instantiate_playback());
    return hrtfPlayback;
}

String HrtfAudioStream::_get_stream_name() const {
    return "HrtfAudioStream";
}

bool HrtfAudioStream::_is_monophonic() const {
    return false;
}

Ref<AudioStream> HrtfAudioStream::get_source_stream() {
    return sourceStream;
}

// Maybe make this arg p_sourceStream to coincide with the godot pattern?
void HrtfAudioStream::set_source_stream(Ref<AudioStream> sourceStream) {
    this->sourceStream = sourceStream;
}

void HrtfAudioStream::set_direction(AzimuthElevation &direction) {
    this->direction = direction;
}

AzimuthElevation HrtfAudioStream::get_direction() {
    return direction;
}
