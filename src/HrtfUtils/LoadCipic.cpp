//
// Created by Greg Costello on 6/25/26.
//

#include "LoadCipic.h"

//godot::Error LoadCipic::load_hrir_file(const godot::String &path) {
//    godot::Ref<godot::FileAccess> file = godot::FileAccess::open(path, godot::FileAccess::READ);
//
//    if (file.is_null()) {
//        godot::UtilityFunctions::push_error("Could not open HRIR file: ", path);
//        return godot::ERR_FILE_CANT_OPEN;
//    }
//
//    const uint64_t size_bytes = file->get_length();
//
//    if (size_bytes == 0) {
//        godot::UtilityFunctions::push_error("HRIR file is empty: ", path);
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    if (size_bytes % sizeof(float) != 0) {
//        godot::UtilityFunctions::push_error("HRIR file size is not a multiple of float size: ", path);
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    const uint64_t float_count = size_bytes / sizeof(float);
//
//    constexpr uint64_t expected_count = 2 * 25 * 50 * 200;
//
//    if (float_count != expected_count) {
//        godot::UtilityFunctions::push_error(
//                "Unexpected HRIR float count. Got ",
//                static_cast<int64_t>(float_count),
//                ", expected ",
//                static_cast<int64_t>(expected_count)
//        );
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    hrir_data.resize(float_count);
//
//    godot::PackedByteArray bytes = file->get_buffer(size_bytes);
//
//    if (bytes.size() != size_bytes) {
//        godot::UtilityFunctions::push_error("Could not read full HRIR file: ", path);
//        return godot::ERR_FILE_CANT_READ;
//    }
//
//    memcpy(hrir_data.data(), bytes.ptr(), size_bytes);
//
//    loaded = true;
//
//    godot::UtilityFunctions::print("Loaded HRIR file: ", path);
//    return godot::OK;
//}

godot::Error LoadCipic::load_hrir_file(std::string path) {
//    // Loading the hrir data from the cipic data set. The dataset was loaded from the cipic subject_3.mat file using python into flat binary
//    std::ifstream file(path, std::ios::binary | std::ios::ate);
//
//    if (!file) {
//        godot::UtilityFunctions::push_error("Could not open HRIR file: ", path.c_str());
//        return godot::ERR_FILE_CANT_OPEN;
//    }
//
//    // Look up streamsize and tellg and see what they are Greg
//    // So tellg returns a streampos? And then streamsize is number of bytes for the io buffer
//    const std::streamsize size_bytes = file.tellg();
//
//    // This goes back to the beginning of the stream
//    file.seekg(0, std::ios::beg);
//
//    if (size_bytes <= 0) {
//        godot::UtilityFunctions::push_error("HRIR file is empty: ", path.c_str());
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    if (size_bytes % sizeof(float) != 0) {
//        godot::UtilityFunctions::push_error("HRIR file size is not a multiple of float size: ", path.c_str());
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    hrir_data = static_cast<float *>(std::malloc(static_cast<size_t>(size_bytes)));
//
//    const size_t expected_size = EAR_COUNT * AZ_COUNT * EL_COUNT * TAP_COUNT;
//
//    if (size_bytes / sizeof(float) != expected_size) {
//        godot::UtilityFunctions::push_error(
//                "Unexpected HRIR float count. Got ",
//                static_cast<int64_t>(size_bytes),
//                ", expected ",
//                static_cast<int64_t>(expected_size)
//        );
//
//        return godot::ERR_FILE_CORRUPT;
//    }
//
//    // Huge memory leak Greg, make sure this gets fixed/
//
//    return godot::OK;

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

    constexpr size_t expected_count = EAR_COUNT * AZ_COUNT * EL_COUNT * TAP_COUNT;

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
