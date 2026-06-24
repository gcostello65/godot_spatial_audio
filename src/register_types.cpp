//
// Created by Greg Costello on 6/4/26.
//

#include "register_types.hpp"

#include "HrtfAudioStream/hrtf_audio_stream.hpp"
#include "HrtfAudioStreamPlayer/hrtf_audio_stream_player.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_hrtfaudiostream_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    GDREGISTER_CLASS(HrtfAudioStream);
    GDREGISTER_CLASS(HrtfAudioStreamPlayback);
    GDREGISTER_CLASS(HrtfAudioStreamPlayer);
}

void uninitialize_hrtfaudiostream_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT hrtfaudiostream_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                        const GDExtensionClassLibraryPtr p_library,
                                                        GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_hrtfaudiostream_module);
    init_obj.register_terminator(uninitialize_hrtfaudiostream_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
