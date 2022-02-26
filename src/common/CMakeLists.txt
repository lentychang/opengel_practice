add_library(shader shader.cpp)
target_link_libraries(shader PUBLIC glfw GL ${CMAKE_DL_LIBS})

add_library(camera Camera.cpp)
target_link_libraries(camera PUBLIC glfw GL ${CMAKE_DL_LIBS})
add_library(Texture2D Texture2D.cpp)
target_link_libraries(camera PUBLIC glfw GL ${CMAKE_DL_LIBS})
