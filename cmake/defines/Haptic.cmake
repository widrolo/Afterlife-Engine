# Use this option for any PC build.
set(HAPTIC_SDL_VAL 1)
# Use this for XBox. Do not use this flag for Windows builds!
set(HAPTIC_XINPUT_VAL 2)
# Use this for PlayStation.
set(HAPTIC_PLAYSTATION_VAL 3)
# Use this for Nintendo.
set(HAPTIC_NINTENDO_VAL 4)

target_compile_definitions(Afterlife_Engine PRIVATE
        HAPTIC_SDL=${HAPTIC_SDL_VAL}
        HAPTIC_XINPUT=${HAPTIC_XINPUT_VAL}
        HAPTIC_PLAYSTATION=${HAPTIC_PLAYSTATION_VAL}
        HAPTIC_NINTENDO=${HAPTIC_NINTENDO_VAL}
)

target_compile_definitions(Afterlife_Engine PRIVATE HAPTIC_BACKEND=${HAPTIC_SDL_VAL})