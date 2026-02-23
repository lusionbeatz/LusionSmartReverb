cmake_minimum_required(VERSION 3.22)
project(LUSIONBEATZSMARTREVERB VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# ── Download JUCE 8.0.10 (exact version you used on Windows) ──
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.10
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── Plugin Definition ──
juce_add_plugin(LUSIONBEATZSMARTREVERB
    COMPANY_NAME                "LusionBeatz"
    PLUGIN_NAME                 "LUSION SMART REVERB"
    PLUGIN_MANUFACTURER_CODE    "LuBz"
    PLUGIN_CODE                 "Lsmr"
    FORMATS                     VST3 AU Standalone
    PRODUCT_NAME                "LUSIONBEATZSMARTREVERB"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             "Fx|Reverb"
    AU_MAIN_TYPE                kAudioUnitType_Effect
)

# ── Source Files ──
target_sources(LUSIONBEATZSMARTREVERB PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ReverbEngine.cpp
)

# ── Binary Resources (your logo image) ──
juce_add_binary_data(BinaryData SOURCES
    Source/lusionbeatz.png
)

# ── JUCE Modules ──
target_link_libraries(LUSIONBEATZSMARTREVERB PRIVATE
    BinaryData
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ── JUCE Compile Definitions ──
target_compile_definitions(LUSIONBEATZSMARTREVERB PUBLIC
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)
