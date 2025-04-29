cmake_minimum_required(VERSION 3.16)
project(vkrk_monocle C CXX ASM_MASM)
set(CMAKE_CXX_STANDARD 23)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall /wd5045 /wd4820")
endif()

add_library(vkuroko_import STATIC IMPORTED GLOBAL)

set_target_properties(vkuroko_import PROPERTIES
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/vkuroko.lib"
)

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/vkuroko.lib"
    COMMAND lib /DEF:${CMAKE_SOURCE_DIR}/def/vkuroko.def /OUT:${CMAKE_BINARY_DIR}/vkuroko.lib /MACHINE:X86
    DEPENDS "${CMAKE_SOURCE_DIR}/def/vkuroko.def"
    COMMENT "Generating vkuroko.lib from vkuroko.def"
)

add_custom_target(generate_vkuroko_import_lib ALL
    DEPENDS "${CMAKE_BINARY_DIR}/vkuroko.lib"
)

file(GLOB SRC_FILES
    src/*.c
    src/*.cpp
    )

file(GLOB SRC_ASM_FILES
	src/*.asm
)
set_source_files_properties(${SRC_ASM_FILES} PROPERTIES LANGUAGE ASM_MASM)

add_library(module_monocle SHARED ${SRC_FILES} ${SRC_ASM_FILES})

target_include_directories(module_monocle PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(module_monocle PRIVATE KRK_BUILDING_MODULE KRK_DISABLE_THREADS)

add_dependencies(module_monocle generate_vkuroko_import_lib)

target_link_libraries(module_monocle PRIVATE vkuroko_import)

set_target_properties(module_monocle PROPERTIES
    OUTPUT_NAME "monocle"
    SUFFIX ".dll"
)
