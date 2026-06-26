//
// Created by Greg Costello on 6/25/26.
//


#ifndef GODOT_SPATIAL_AUDIO_LOADCIPIC_H
#define GODOT_SPATIAL_AUDIO_LOADCIPIC_H

#include <vector>
#include <fstream>
#include <stdexcept>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

class LoadCipic {
    static constexpr int EAR_COUNT = 2;
    static constexpr int AZ_COUNT = 25;
    static constexpr int EL_COUNT = 50;
    static constexpr int TAP_COUNT = 200;

private:
    std::vector<float> hrir_data;
    bool loaded;
public:
    // Go through this code and see how it is done, just copied from chat for now
    static int hrir_index(int ear, int az_idx, int el_idx, int tap) {
        return (((ear * AZ_COUNT + az_idx) * EL_COUNT + el_idx) * TAP_COUNT + tap);
    }

    static constexpr int HRTF_AZ_COUNT = 25;

    static constexpr int HRTF_EL_COUNT = 50;

    static constexpr int HRTF_TAPS = 200;

    const float *get_hrir(int ear, int az_idx, int el_idx) const {

        if (!loaded) {

            godot::UtilityFunctions::push_error("get_hrir failed: CIPIC data not loaded.");

            return nullptr;

        }

        if (hrir_data.empty()) {

            godot::UtilityFunctions::push_error("get_hrir failed: hrir_data is empty.");

            return nullptr;

        }

        if (ear < 0 || ear >= 2) {

            godot::UtilityFunctions::push_error("get_hrir failed: bad ear index.");

            return nullptr;

        }

        if (az_idx < 0 || az_idx >= 25) {

            godot::UtilityFunctions::push_error("get_hrir failed: bad azimuth index.");

            return nullptr;

        }

        if (el_idx < 0 || el_idx >= 50) {

            godot::UtilityFunctions::push_error("get_hrir failed: bad elevation index.");

            return nullptr;

        }

        const int index = (((ear * 25 + az_idx) * 50 + el_idx) * 200);

        if (index < 0 || index + 199 >= static_cast<int>(hrir_data.size())) {

            godot::UtilityFunctions::push_error(

                    "get_hrir failed: computed index out of range. index=",

                    index,

                    " size=",

                    static_cast<int64_t>(hrir_data.size())

            );

            return nullptr;

        }

        return &hrir_data[index];

    }

    godot::Error load_hrir_file(std::string path);
};


#endif //GODOT_SPATIAL_AUDIO_LOADCIPIC_H
