#pragma once

typedef struct {
    float x, y, z;
} Vector;

typedef Vector QAngle;

enum PlacementOrder {

    /*
    * Teleport matrices are calculated in UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix.
    * Color here is the "primary" matrix and the other is the inverse.
    */
    _BLUE_UPTM,
    _ORANGE_UPTM,

    // Teleport matrices are calculated in UpdateLinkMatrix; both matrices are calculated the same way.
    _ULM,

    /*
    * Both portals are open, one of them was moved (either with portal gun or newlocation).
    * callstack:
    * blue NewLocation -> UpdatePortalLinkage -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix
    *                                         -> MoveTo -> UpdateLinkMatrix
    *                                                                       -> orange UpdateLinkMatrix
    *                  -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix ***
    */
    ORANGE_OPEN_BLUE_NEW_LOCATION = _BLUE_UPTM,
    BLUE_OPEN_ORANGE_NEW_LOCATION = _ORANGE_UPTM,

    /*
    * Both portals are open and you shot one or used newlocation, but its position and angles didn't change.
    * callstack:
    * blue NewLocation -> UpdatePortalLinkage -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix
    *                  -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix ***
    */
    ORANGE_OPEN_BLUE_NEW_LOCATION_NO_MOVE = _BLUE_UPTM,
    BLUE_OPEN_ORANGE_NEW_LOCATION_NO_MOVE = _ORANGE_UPTM,

    /*
    * Orange exists (closed & activated), blue is not activated and you shot blue somewhere.
    * callstack:
    * blue NewLocation -> UpdatePortalLinkage -> orange UpdatePortalLinkage -> UpdatePortalTransformationMatrix
    *                                                                       -> AttachTo -> UpdateLinkMatrix
    *                                                                                                       -> blue UpdateLinkMatrix
    *                                         -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix
    *                                         -> MoveTo -> UpdateLinkMatrix
    *                                                                       -> orange UpdateLinkMatrix
    *                  -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix ***
    * 
    * Note: during the call to blue's UpdateLinkMatrix, the simulator's position hasn't been updated yet.
    */
    ORANGE_WAS_CLOSED_BLUE_MOVED = _BLUE_UPTM,
    BLUE_WAS_CLOSED_ORANGE_MOVED = _ORANGE_UPTM,

    /*
    * Orange is closed and blue did exist but was just placed.
    * callstack:
    * blue NewLocation -> UpdatePortalLinkage -> orange UpdatePortalLinkage -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix
    *                                         -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix
    *                                         -> MoveTo -> UpdateLinkMatrix
    *                                                                       -> orange UpdateLinkMatrix
    *                  -> UpdatePortalTeleportMatrix -> UpdatePortalTransformationMatrix ***
    */
    ORANGE_WAS_CLOSED_BLUE_CREATED = _BLUE_UPTM,
    BLUE_WAS_CLOSED_ORANGE_CREATED = _ORANGE_UPTM,

    /*
    * Orange exists (closed & activated), blue is not activated and it opened.
    * The callstack is the same as the created case except there's an additional input fired on the opened
    * portal which calls UpdatePortalTransformationMatrix a couple times.
    * *callstack might not be the same if the portal existed already? whatever, doesn't change the result.
    */
    ORANGE_WAS_CLOSED_BLUE_OPENED = ORANGE_WAS_CLOSED_BLUE_CREATED,
    BLUE_WAS_CLOSED_ORANGE_OPENED = BLUE_WAS_CLOSED_ORANGE_CREATED,

    AFTER_LOAD = _ULM,
};

typedef struct {
    float m[4][4];
} VMatrix;

void SyncFloatingPointControlWord(void);

extern void CreateOverlayPortalImage(Vector blue_pos,
                                     QAngle blue_ang,
                                     Vector orange_pos,
                                     QAngle orange_ang,
                                     int order,
                                     const char* file_name,
                                     int y_res,
                                     int from_blue);

extern void CreateOverlayPortalImageMatrices(Vector blue_pos,
                                             QAngle blue_ang,
                                             Vector orange_pos,
                                             QAngle orange_ang,
                                             VMatrix b_to_o,
                                             VMatrix o_to_b,
                                             const char* file_name,
                                             int y_res,
                                             int from_blue);
