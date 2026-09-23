set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Packaging>:NDEBUG;PACKAGE>
)

if(NOT MSVC)
    target_compile_options(Afterlife_Engine PRIVATE
            $<$<CONFIG:Packaging>:-ffunction-sections>
            $<$<CONFIG:Packaging>:-fdata-sections>
    )

    target_link_options(Afterlife_Engine PRIVATE
            $<$<CONFIG:Packaging>:-Wl,--gc-sections>
            $<$<CONFIG:Packaging>:-s>
    )
endif()

set(EXECUTABLE_OUTPUT_NAME_RELEASE "$<$<CONFIG:Packaging>:Afterlife_Engine-Package>")