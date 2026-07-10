set_property(TARGET Widrolo_Engine_3D APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Packaging>:NDEBUG;PACKAGE>
)

set(WENGINE_OUTPUT_NAME_PACKAGING "$<$<CONFIG:Packaging>:Widrolo_Engine_3D-Package>")