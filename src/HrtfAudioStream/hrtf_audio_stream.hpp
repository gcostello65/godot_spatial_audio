#ifndef HRTF_AUDIO_STREAM_H
#define HRTF_AUDIO_STREAM_H

#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "HrtfAudioStreamPlayback/hrtf_audio_stream_playback.h"
#include "HrtfUtils/azimuth_elevation.h"

namespace godot {

    class HrtfAudioStream : public AudioStream {
        GDCLASS(HrtfAudioStream, AudioStream)

    private:
        static constexpr const char *DEFAULT_HRIR_PATH =
                "/Users/gregcostello/Documents/dev/c++/godot_spatial_audio/src/HrtfUtils/subject_003_hrir_f32.bin";

        Ref<AudioStream> source_stream;
        String hrir_path = DEFAULT_HRIR_PATH;
        mutable Ref<HrtfAudioStreamPlayback> active_playback;
        AzimuthElevation direction = {};

    protected:
        static void _bind_methods();

    public:
        HrtfAudioStream();
        ~HrtfAudioStream();

        Ref<AudioStream> get_source_stream() const;
        void set_source_stream(const Ref<AudioStream> &p_source_stream);

        void set_hrir_path(const String &p_path);
        String get_hrir_path() const;

        Ref<AudioStreamPlayback> _instantiate_playback() const override;
        String _get_stream_name() const override;
        bool _is_monophonic() const override;

        void set_direction(const AzimuthElevation &p_direction);
        AzimuthElevation get_direction() const;
    };

}

#endif // HRTF_AUDIO_STREAM_H
