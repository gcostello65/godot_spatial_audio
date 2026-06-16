#ifndef HRTF_AUDIO_STREAM_H
#define HRTF_AUDIO_STREAM_H

#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "HrtfAudioStreamPlayback/hrtf_audio_stream_playback.h"

namespace godot {

    class HrtfAudioStream : public AudioStream {
        GDCLASS(HrtfAudioStream, AudioStream)
        Ref<AudioStream> sourceStream;
    public:
        Ref<AudioStream> get_source_stream();
        void set_source_stream(Ref<AudioStream> sourceStream);

    private:

    protected:
        static void _bind_methods();

    public:
        HrtfAudioStream();
        ~HrtfAudioStream();

        Ref<AudioStreamPlayback> _instantiate_playback() const override;
        String _get_stream_name() const override;
        bool _is_monophonic() const override;
    };

}

#endif // HRTF_AUDIO_STREAM_H
