set_property(TARGET Afterlife_Engine APPEND PROPERTY
        COMPILE_DEFINITIONS $<$<CONFIG:Testing>:DEBUG;TESTING>
)

target_compile_options(Afterlife_Engine PRIVATE
        $<$<CONFIG:Testing>:$<$<CXX_COMPILER_ID:MSVC>:/Zi /Od>>
        $<$<CONFIG:Testing>:$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-g -O0>>
)

set(EXECUTABLE_OUTPUT_NAME_TESTING "$<$<CONFIG:Testing>:Afterlife_Engine-Testing>")