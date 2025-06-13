set(CHATBOT_SOURCES
    ChatbotEngine.cpp
    ChatbotModel.cpp
    ChatbotProcessor.cpp
)

add_library(VRMusicStudioChatbot STATIC ${CHATBOT_SOURCES})

target_include_directories(VRMusicStudioChatbot
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

target_link_libraries(VRMusicStudioChatbot
    PUBLIC
        VRMusicStudio::Core
        VRMusicStudio::Audio
        VRMusicStudio::AI
    PRIVATE
        spdlog::spdlog
)

target_compile_features(VRMusicStudioChatbot PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(VRMusicStudioChatbot PRIVATE /W4)
else()
    target_compile_options(VRMusicStudioChatbot PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS VRMusicStudioChatbot
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 