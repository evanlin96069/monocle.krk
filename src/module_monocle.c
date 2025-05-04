#include <kuroko/util.h>
#include <kuroko/io.h>
#include <kuroko/object.h>

#include "monocle.h"

typedef struct {
    Vector pos;
    QAngle ang;
    int has_matrix;
    VMatrix mat;
} PortalInfo;

int GetVector(KrkValue value, Vector* out)
{
    KrkValue vx = krk_valueGetAttribute(value, "x");
    if (IS_NONE(vx))
        return 0;
    KrkValue vy = krk_valueGetAttribute(value, "y");
    if (IS_NONE(vx))
        return 0;
    KrkValue vz = krk_valueGetAttribute(value, "z");
    if (IS_NONE(vz))
        return 0;
    double x, y, z;
    if (!krk_valueAsNumber(vx, &x))
        return 0;
    if (!krk_valueAsNumber(vy, &y))
        return 0;
    if (!krk_valueAsNumber(vz, &z))
        return 0;
    out->x = (float)x;
    out->y = (float)y;
    out->z = (float)z;
    return 1;
}

int GetVMatrix(KrkValue value, VMatrix* out)
{
    KrkValue vl = krk_valueGetAttribute(value, "data");
    if (IS_NONE(vl))
        return 0;
    if (!IS_list(vl))
        return 0;
    KrkValueArray* list = AS_LIST(vl);
    if (list->count != 16)
        return 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            KrkValue v = list->values[i * 4 + j];
            double n;
            if (!krk_valueAsNumber(v, &n))
                return 0;
            out->m[i][j] = (float)n;
        }
    }
    return 1;
}

int GetPortalInfo(KrkValue value, PortalInfo* portal)
{
    KrkValue vpos = krk_valueGetAttribute(value, "pos");
    if (IS_NONE(vpos))
        return 0;
    KrkValue vang = krk_valueGetAttribute(value, "ang");
    if (IS_NONE(vang))
        return 0;
    KrkValue vmat = krk_valueGetAttribute(value, "matrix_this_to_linked");
    if (!IS_NONE(krk_currentThread.currentException))
        return 0;
    if (!GetVector(vpos, &portal->pos))
        return 0;
    if (!GetVector(vang, &portal->ang))
        return 0;
    if (IS_NONE(vmat)) {
        portal->has_matrix = 0;
        return 1;
    }
    portal->has_matrix = 1;
    return GetVMatrix(vmat, &portal->mat);
}

KRK_Function(create_image)
{
    KrkValue blue_portal;
    KrkValue orange_portal;
    const char* file_name;
    int y_res;
    int from_blue;
    KrkValue vorder = NONE_VAL();

    if (!krk_parseArgs("VVzip|V",
                       (const char*[]){
                           "blue_portal",
                           "orange_portal",
                           "file_name",
                           "y_res",
                           "from_blue"
                           "order",
                       },
                       &blue_portal,
                       &orange_portal,
                       &file_name,
                       &y_res,
                       &from_blue,
                       &vorder)) {
        return NONE_VAL();
    }

    PortalInfo blue, orange;
    if (!GetPortalInfo(blue_portal, &blue)) {
        return krk_runtimeError(vm.exceptions->valueError, "bad blue portal");
    }
    if (!GetPortalInfo(orange_portal, &orange)) {
        return krk_runtimeError(vm.exceptions->valueError, "bad orange portal");
    }

    if (IS_NONE(vorder)) {
        if (blue.has_matrix && orange.has_matrix) {
            CreateOverlayPortalImageMatrices(blue.pos,
                                             blue.ang,
                                             orange.pos,
                                             orange.ang,
                                             blue.mat,
                                             orange.mat,
                                             file_name,
                                             y_res,
                                             from_blue);
            return NONE_VAL();
        }
        return krk_runtimeError(vm.exceptions->valueError,
                                "required placement order if portal matrices not provided");
    }
    if (!IS_INTEGER(vorder)) {
        return krk_runtimeError(vm.exceptions->typeError,
                                "placement order must be an integer");
    }

    int order = AS_INTEGER(vorder);
    if (order != _BLUE_UPTM && order != _ORANGE_UPTM && order != _ULM) {
        return krk_runtimeError(vm.exceptions->valueError,
                                "invalid placement order");
    }
    CreateOverlayPortalImage(blue.pos, blue.ang, orange.pos, orange.ang, order, file_name, y_res, from_blue);
    return NONE_VAL();
}

KRK_Module(monocle)
{
    KRK_DOC(module, "@brief Reproducing the Vertical Angle Glitch (VAG) bug in Portal.");

    KRK_DOC(BIND_FUNC(module, create_image),
            "@brief Create overlay portal image.\n"
            "@arguments blue_portal, orange_portal, file_name, y_res, from_blue, order=None.\n\n"
            "If order not provided, blue_portal and orange_portal must have matrix_this_to_linked.");

#define DO_INT(name) krk_attachNamedValue(&module->fields, #name, INTEGER_VAL(name))
    DO_INT(ORANGE_OPEN_BLUE_NEW_LOCATION);
    DO_INT(BLUE_OPEN_ORANGE_NEW_LOCATION);

    DO_INT(ORANGE_OPEN_BLUE_NEW_LOCATION_NO_MOVE);
    DO_INT(BLUE_OPEN_ORANGE_NEW_LOCATION_NO_MOVE);

    DO_INT(ORANGE_WAS_CLOSED_BLUE_MOVED);
    DO_INT(BLUE_WAS_CLOSED_ORANGE_MOVED);

    DO_INT(ORANGE_WAS_CLOSED_BLUE_CREATED);
    DO_INT(BLUE_WAS_CLOSED_ORANGE_CREATED);

    DO_INT(ORANGE_WAS_CLOSED_BLUE_OPENED);
    DO_INT(BLUE_WAS_CLOSED_ORANGE_OPENED);

    DO_INT(AFTER_LOAD);
}
