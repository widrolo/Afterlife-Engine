set_property(TARGET Widrolo_Engine_3D APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUG>
)

set(WENGINE_OUTPUT_NAME_DEBUG "$<$<CONFIG:Debug>:Widrolo_Engine_3D-Dev>")