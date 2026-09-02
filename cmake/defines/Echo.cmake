set(ECHO_MINIAUDIO_VAL 1)

target_compile_definitions(Afterlife_Engine PRIVATE
        ECHO_MINIAUDIO=${ECHO_MINIAUDIO_VAL}
)

target_compile_definitions(Afterlife_Engine PRIVATE ECHO_BACKEND=${ECHO_MINIAUDIO})