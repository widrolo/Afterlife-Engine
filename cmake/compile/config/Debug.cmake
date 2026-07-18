set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUG>
)

set(WENGINE_OUTPUT_NAME_DEBUG "$<$<CONFIG:Debug>:Afterlife_Engine-Dev>")