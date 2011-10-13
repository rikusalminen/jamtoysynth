include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jamtoysynth
    main.c
    synth.c
    audio_output.c
    )
