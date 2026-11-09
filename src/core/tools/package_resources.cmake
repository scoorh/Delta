file(GLOB_RECURSE RESOURCE_PACKAGE_FILES
        "${SOURCE_DIR}/*_package.json"
)

file(MAKE_DIRECTORY ${DEST_DIR})


foreach(RESOURCE_PACKAGE ${RESOURCE_PACKAGE_FILES})

    message(STATUS "Packaging resources: ${RESOURCE_PACKAGE}")

    message(STATUS ${PYTHON_SCRIPT})

    execute_process(
            COMMAND ${PYTHON_EXECUTABLE} "${PYTHON_SCRIPT}"
            "${RESOURCE_PACKAGE}"
            "${DEST_DIR}"
            "${COMPILED_RESOURCES_DIR}"
            "${METADATA_CODE_DIR}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE PROCESS_RESULT
    )

    if(NOT PROCESS_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to package resources: ${RESOURCE_PACKAGE}")
    endif()


endforeach ()