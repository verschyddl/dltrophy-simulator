
add_executable(mock_sender mock_sender.cpp)

target_link_libraries(mock_sender PRIVATE
        MinimalSocket
)
