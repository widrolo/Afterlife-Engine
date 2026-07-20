set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Release>:NDEBUG;RC>
)

set(EXECUTABLE_OUTPUT_NAME_PACKAGING "$<$<CONFIG:Release>:Afterlife_Engine-RC>")