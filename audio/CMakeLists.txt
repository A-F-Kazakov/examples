set(HEAD_NAME ${PROJECT_NAME}-audio)

set(TARGET_NAME ${HEAD_NAME}-generator)
add_executable(${TARGET_NAME} generator.c)
target_compile_definitions(${TARGET_NAME} PRIVATE $<$<PLATFORM_ID:Windows>:_USE_MATH_DEFINES>)

set(TARGET_NAME ${HEAD_NAME}-recorder)
add_executable(${TARGET_NAME} recorder.c)

#ffmpeg -y -f s16le -ac 2 -ar 48k -i _build/audio/sample.pcm out.wav
