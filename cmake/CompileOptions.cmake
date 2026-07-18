set_target_properties(Afterlife_Engine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_SYSTEM_NAME}-x64"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_SYSTEM_NAME}-x64"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_SYSTEM_NAME}-x64"
)

if(WIN32)
    include(cmake/compile/platform/Windows.cmake)
else() # really should be elseif(linux), but it doesnt work...
    include(cmake/compile/platform/Linux.cmake)
endif()

include(cmake/compile/config/Debug.cmake)
include(cmake/compile/config/Release.cmake)
include(cmake/compile/config/Packaging.cmake)

set_target_properties(Afterlife_Engine PROPERTIES
        OUTPUT_NAME "${WIDROLO_OUTPUT_NAME_DEBUG}${WIDROLO_OUTPUT_NAME_RELEASE}${WIDROLO_OUTPUT_NAME_PACKAGING}"
)