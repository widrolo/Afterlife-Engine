set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Release>:NDEBUG;RC>
)

set(WENGINE_OUTPUT_NAME_RELEASE "$<$<CONFIG:Release>:Afterlife_Engine-RC>")