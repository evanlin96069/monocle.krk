#include <kuroko/util.h>
#include <kuroko/io.h>
#include <kuroko/object.h>

#include "monocle.h"

KRK_Function(create_image)
{
    KrkValue blue_pos;
    KrkValue blue_ang;
    KrkValue orange_pos;
    KrkValue orange_ang;
    int order;
    const char* file_name;
    int y_res;
    int from_blue;

    if (!krk_parseArgs("VVVVizip",
            (const char*[]){"blue_pos", "blue_ang", "orange_pos", "orange_ang", "order", "file_name", "y_res", "from_blue"},
                       &blue_pos, &blue_ang, &orange_pos, &orange_ang, &order, &file_name, &y_res, &from_blue))
        return NONE_VAL();

    if (!IS_TUPLE(blue_pos) || AS_TUPLE(blue_pos)->values.count != 3 || !IS_TUPLE(blue_ang) ||
        AS_TUPLE(blue_ang)->values.count != 3 || !IS_TUPLE(orange_pos) || AS_TUPLE(orange_pos)->values.count != 3 ||
        !IS_TUPLE(orange_ang) || AS_TUPLE(orange_ang)->values.count != 3)
        return krk_runtimeError(vm.exceptions->argumentError, "Requires 4 3-tuples");

    for (int i = 0; i < 3; i++) {
        if (!IS_FLOATING(AS_TUPLE(blue_pos)->values.values[i]) || !IS_FLOATING(AS_TUPLE(blue_ang)->values.values[i]) ||
            !IS_FLOATING(AS_TUPLE(orange_pos)->values.values[i]) ||
            !IS_FLOATING(AS_TUPLE(orange_ang)->values.values[i]))
            return krk_runtimeError(vm.exceptions->argumentError, "Requires 4 3-tuples");
    }

    CreateOverlayPortalImage(
        (Vector){
            AS_FLOATING((AS_TUPLE(blue_pos)->values.values[0])),
            AS_FLOATING((AS_TUPLE(blue_pos)->values.values[1])),
            AS_FLOATING((AS_TUPLE(blue_pos)->values.values[2])),
        },
        (QAngle){
            AS_FLOATING((AS_TUPLE(blue_ang)->values.values[0])),
            AS_FLOATING((AS_TUPLE(blue_ang)->values.values[1])),
            AS_FLOATING((AS_TUPLE(blue_ang)->values.values[2])),
        },
        (Vector){
            AS_FLOATING((AS_TUPLE(orange_pos)->values.values[0])),
            AS_FLOATING((AS_TUPLE(orange_pos)->values.values[1])),
            AS_FLOATING((AS_TUPLE(orange_pos)->values.values[2])),
        },
        (QAngle){
            AS_FLOATING((AS_TUPLE(orange_ang)->values.values[0])),
            AS_FLOATING((AS_TUPLE(orange_ang)->values.values[1])),
            AS_FLOATING((AS_TUPLE(orange_ang)->values.values[2])),
        },
        order,
        file_name,
        y_res,
        from_blue);

    return NONE_VAL();
}

KRK_Module(monocle)
{
    SyncFloatingPointControlWord();

    KRK_DOC(module, "@brief monocle module.");

    BIND_FUNC(module, create_image);
}
