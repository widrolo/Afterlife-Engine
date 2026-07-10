set_property(TARGET Widrolo_Engine_3D APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Release>:NDEBUG;RC>
)

set(WENGINE_OUTPUT_NAME_RELEASE "$<$<CONFIG:Release>:Widrolo_Engine_3D-RC>")