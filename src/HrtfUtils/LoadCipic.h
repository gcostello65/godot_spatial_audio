//
// Created by Greg Costello on 6/25/26.
//


#ifndef GODOT_SPATIAL_AUDIO_LOADCIPIC_H
#define GODOT_SPATIAL_AUDIO_LOADCIPIC_H

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <array>
#include <string>
#include <vector>

class LoadCipic {
public:
    enum Ear {
        LEFT_EAR = 0,
        RIGHT_EAR = 1,
    };

    struct IndexRange {
        int lower = 0;
        int upper = 0;
    };

    static constexpr int EAR_COUNT = 2;
    static constexpr int AZIMUTH_COUNT = 25;
    static constexpr int ELEVATION_COUNT = 50;
    static constexpr int TAP_COUNT = 200;
    static constexpr float MIN_ELEVATION_DEGREES = -45.0f;
    static constexpr float MAX_ELEVATION_DEGREES = 230.625f;
    static constexpr float ELEVATION_STEP_DEGREES = 5.625f;
    static constexpr std::array<float, AZIMUTH_COUNT> AZIMUTH_DEGREES = {
            -80.0f, -65.0f, -55.0f, -45.0f, -40.0f,
            -35.0f, -30.0f, -25.0f, -20.0f, -15.0f,
            -10.0f, -5.0f, 0.0f, 5.0f, 10.0f,
            15.0f, 20.0f, 25.0f, 30.0f, 35.0f,
            40.0f, 45.0f, 55.0f, 65.0f, 80.0f
    };

    static void interpolate_hrir(const float *bottomLeft, const float *bottomRight, const float *topLeft,
                                 const float *topRight, float azimuth, float elevation, int size,
                                 std::vector<float> &finalFilter);

    static int hrir_index(int ear, int az_idx, int el_idx, int tap) {
        return (((ear * AZIMUTH_COUNT + az_idx) * ELEVATION_COUNT + el_idx) * TAP_COUNT + tap);
    }

    static IndexRange calculate_azimuth_indices(float angle_degrees);
    static IndexRange calculate_elevation_indices(float angle_degrees);

    bool is_loaded() const;
    const float *get_hrir(int ear, int az_idx, int el_idx) const;
    godot::Error load_hrir_file(const std::string &path);

private:
    std::vector<float> hrir_data;
    bool loaded = false;
};


#endif //GODOT_SPATIAL_AUDIO_LOADCIPIC_H
