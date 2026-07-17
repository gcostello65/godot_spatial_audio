//
// Created by Greg Costello on 6/25/26.
//

#include "LoadCipic.h"

#include <algorithm>
#include <cmath>
#include <fstream>

namespace {
float calculate_fraction(float value, float lower, float upper) {
    if (upper <= lower) {
        return 0.0f;
    }

    return std::clamp((value - lower) / (upper - lower), 0.0f, 1.0f);
}
}

LoadCipic::IndexRange LoadCipic::calculate_azimuth_indices(float angle_degrees) {
    const auto upper = std::lower_bound(AZIMUTH_DEGREES.begin(), AZIMUTH_DEGREES.end(), angle_degrees);

    if (upper == AZIMUTH_DEGREES.begin()) {
        return {0, 0};
    }

    if (upper == AZIMUTH_DEGREES.end()) {
        constexpr int last_index = AZIMUTH_COUNT - 1;
        return {last_index, last_index};
    }

    const int upper_index = static_cast<int>(upper - AZIMUTH_DEGREES.begin());
    return {upper_index - 1, upper_index};
}

LoadCipic::IndexRange LoadCipic::calculate_elevation_indices(float angle_degrees) {
    const float clamped_angle = std::clamp(angle_degrees, MIN_ELEVATION_DEGREES, MAX_ELEVATION_DEGREES);
    const int lower_index = static_cast<int>(
            std::floor((clamped_angle - MIN_ELEVATION_DEGREES) / ELEVATION_STEP_DEGREES)
    );

    if (lower_index >= ELEVATION_COUNT - 1) {
        constexpr int last_index = ELEVATION_COUNT - 1;
        return {last_index, last_index};
    }

    return {lower_index, lower_index + 1};
}

bool LoadCipic::is_loaded() const {
    return loaded;
}

const float *LoadCipic::get_hrir(int ear, int az_idx, int el_idx) const {
    if (!loaded) {
        godot::UtilityFunctions::push_error("get_hrir failed: CIPIC data not loaded.");
        return nullptr;
    }

    if (hrir_data.empty()) {
        godot::UtilityFunctions::push_error("get_hrir failed: hrir_data is empty.");
        return nullptr;
    }

    if (ear < 0 || ear >= EAR_COUNT) {
        godot::UtilityFunctions::push_error("get_hrir failed: bad ear index.");
        return nullptr;
    }

    if (az_idx < 0 || az_idx >= AZIMUTH_COUNT) {
        godot::UtilityFunctions::push_error("get_hrir failed: bad azimuth index.");
        return nullptr;
    }

    if (el_idx < 0 || el_idx >= ELEVATION_COUNT) {
        godot::UtilityFunctions::push_error("get_hrir failed: bad elevation index.");
        return nullptr;
    }

    const int index = hrir_index(ear, az_idx, el_idx, 0);
    if (index < 0 || index + TAP_COUNT - 1 >= static_cast<int>(hrir_data.size())) {
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

godot::Error LoadCipic::load_hrir_file(const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file) {
        godot::UtilityFunctions::push_error("Could not open HRIR file: ", path.c_str());
        loaded = false;
        hrir_data.clear();
        return godot::ERR_FILE_CANT_OPEN;
    }

    const std::streamsize size_bytes = file.tellg();

    if (size_bytes <= 0) {
        godot::UtilityFunctions::push_error("HRIR file is empty: ", path.c_str());
        loaded = false;
        hrir_data.clear();
        return godot::ERR_FILE_CORRUPT;
    }

    if (size_bytes % sizeof(float) != 0) {
        godot::UtilityFunctions::push_error("HRIR file size is not a multiple of float size: ", path.c_str());
        loaded = false;
        hrir_data.clear();
        return godot::ERR_FILE_CORRUPT;
    }

    const size_t float_count = static_cast<size_t>(size_bytes) / sizeof(float);
    constexpr size_t expected_count = EAR_COUNT * AZIMUTH_COUNT * ELEVATION_COUNT * TAP_COUNT;

    if (float_count != expected_count) {
        godot::UtilityFunctions::push_error(
                "Unexpected HRIR float count. Got ",
                static_cast<int64_t>(float_count),
                ", expected ",
                static_cast<int64_t>(expected_count)
        );
        loaded = false;
        hrir_data.clear();
        return godot::ERR_FILE_CORRUPT;
    }

    hrir_data.resize(float_count);
    file.seekg(0, std::ios::beg);

    if (!file.read(reinterpret_cast<char *>(hrir_data.data()), size_bytes)) {
        godot::UtilityFunctions::push_error("Could not read HRIR file: ", path.c_str());
        loaded = false;
        hrir_data.clear();
        return godot::ERR_FILE_CANT_READ;
    }

    loaded = true;
    godot::UtilityFunctions::print(
            "Loaded HRIR file: ",
            path.c_str(),
            " floats=",
            static_cast<int64_t>(hrir_data.size())
    );

    return godot::OK;
}

void LoadCipic::interpolate_hrir(const float *bottomLeft, const float *bottomRight, const float *topLeft,
                                 const float *topRight, float azimuth, float elevation, int size,
                                 std::vector<float> &finalFilter) {
    const IndexRange azimuth_indices = calculate_azimuth_indices(azimuth);
    const IndexRange elevation_indices = calculate_elevation_indices(elevation);
    const float azimuth_fraction = calculate_fraction(
            azimuth,
            AZIMUTH_DEGREES[azimuth_indices.lower],
            AZIMUTH_DEGREES[azimuth_indices.upper]
    );
    const float clamped_elevation = std::clamp(elevation, MIN_ELEVATION_DEGREES, MAX_ELEVATION_DEGREES);
    const float lower_elevation = MIN_ELEVATION_DEGREES + ELEVATION_STEP_DEGREES * elevation_indices.lower;
    const float upper_elevation = MIN_ELEVATION_DEGREES + ELEVATION_STEP_DEGREES * elevation_indices.upper;
    const float elevation_fraction = calculate_fraction(clamped_elevation, lower_elevation, upper_elevation);

    for (int i = 0; i < size; i++) {
        const float bottom = bottomLeft[i] + (bottomRight[i] - bottomLeft[i]) * azimuth_fraction;
        const float top = topLeft[i] + (topRight[i] - topLeft[i]) * azimuth_fraction;
        finalFilter[i] = bottom + (top - bottom) * elevation_fraction;
    }
}
