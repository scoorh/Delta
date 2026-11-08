file(GLOB_RECURSE SHADER_FILES
        "${SOURCE_DIR}/*.comp"
        "${SOURCE_DIR}/*.vert"
        "${SOURCE_DIR}/*.frag"
)

file(MAKE_DIRECTORY "${DEST_DIR}")

find_program(GLSLC glslc
        HINTS "$ENV{VULKAN_SDK}/Bin"
)

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(OUT_FILE "${DEST_DIR}/${SHADER_NAME}.spv")

    message(STATUS "Compiling shader: ${SHADER_NAME}")

    execute_process(
            COMMAND "${GLSLC}" --target-env=vulkan1.3 "${SHADER}" -o "${OUT_FILE}"
            RESULT_VARIABLE RESULT
    )

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to compile shader: ${SHADER}")
    endif()

endforeach ()