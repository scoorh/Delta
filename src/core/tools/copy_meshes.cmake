file(GLOB_RECURSE MESH_FILES
        "${SOURCE_DIR}/*.mesh"
)
file(GLOB_RECURSE NESTED_MESH_FILES
        "${SOURCE_DIR}/*/*.mesh"
)

list(APPEND MESH_FILES ${NESTED_MESH_FILES})

file(MAKE_DIRECTORY "${DEST_DIR}")

foreach(MESH_FILE IN LISTS MESH_FILES)
    get_filename_component(MESH_NAME "${MESH_FILE}" NAME)
    set(DEST_FILE "${DEST_DIR}/${MESH_NAME}")

    execute_process(
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${MESH_FILE}"
            "${DEST_FILE}"
            COMMAND_ERROR_IS_FATAL ANY
    )
endforeach()