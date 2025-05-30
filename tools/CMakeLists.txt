set(CMAKE_CXX_STANDARD 20)

add_executable(mock_sender mock_sender.cpp
        Range.h)

target_link_libraries(mock_sender PRIVATE
        MinimalSocket
)
