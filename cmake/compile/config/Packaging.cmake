set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Packaging>:NDEBUG;PACKAGE>
)

set(EXECUTABLE_OUTPUT_NAME_RELEASE "$<$<CONFIG:Packaging>:Afterlife_Engine-Package>")