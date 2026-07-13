//
// Created by Greg Costello on 7/12/26.
//

#ifndef GODOT_SPATIAL_AUDIO_AZIMUTH_ELEVATION_H
#define GODOT_SPATIAL_AUDIO_AZIMUTH_ELEVATION_H
struct AzimuthElevation {
    // radian measurements for the hrtf lookup
    float azimuth;
    float elevation;
    float distance;
};
#endif //GODOT_SPATIAL_AUDIO_AZIMUTH_ELEVATION_H
