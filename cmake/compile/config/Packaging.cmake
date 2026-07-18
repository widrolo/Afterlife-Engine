set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Packaging>:NDEBUG;PACKAGE>
)

set(WENGINE_OUTPUT_NAME_PACKAGING "$<$<CONFIG:Packaging>:Afterlife_Engine-Package>")