#pragma once

typedef struct {
    float x, y, z;
} Vector;

typedef struct {
    float x, y, z;
} QAngle;

void SyncFloatingPointControlWord(void);

void CreateOverlayPortalImage(Vector blue_pos,
                              QAngle blue_ang,
                              Vector orange_pos,
                              QAngle orange_ang,
                              int order,
                              const char* file_name,
                              int y_res,
                              int from_blue);
