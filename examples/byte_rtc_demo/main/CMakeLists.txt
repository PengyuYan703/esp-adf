set(COMPONENT_SRCS "main.c" "byte_rtc_request.c" "VolcRTCDemo.c" "AudioPipeline.c" "VolcRTCDemo.c")
set(COMPONENT_ADD_INCLUDEDIRS .)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wno-unused-variable")

register_component()