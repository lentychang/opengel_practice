#include "previous_code.cpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/fwd.hpp>

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float FOV = 45;
bool left_key_pressed = false;
bool firstMouse = true;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static float lastX = 400, lastY = 300;
  static float yaw = -90.0f, pitch = 0.0f;

  float x = static_cast<float>(xpos);
  float y = static_cast<float>(ypos);

  static constexpr float sensitivity = 0.1f;
  if (left_key_pressed) {
    if (firstMouse) {
      lastX = x;
      lastY = y;
      firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset =
        static_cast<float>(lastY - ypos); // reversed: y ranges bottom to top
    lastX = x;
    lastY = y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
  }
}

void on_mouse_click(GLFWwindow *window, int button, int action, int mods) {
  if (button != GLFW_MOUSE_BUTTON_LEFT)
    return;

  if (GLFW_PRESS == action)
    left_key_pressed = true;
  else if (GLFW_RELEASE == action) {
    left_key_pressed = false;
    firstMouse = true;
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  static constexpr float fov_max = 45.0;
  static constexpr float fov_min = 1.0;
  FOV -= static_cast<float>(yoffset);
  if (FOV < fov_min)
    FOV = fov_min;
  if (FOV > fov_max)
    FOV = fov_max;
}

int main() {

  GLFWwindow *window;
  { // init
    init_glfw();
    window = create_window();
    load_glad();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, on_mouse_click);
  glfwSetScrollCallback(window, scroll_callback);

  Shader shader{VERTEX_SRC.c_str(), FRAGMENT_SRC.c_str()};
  unsigned int texture_floor, texture_wall;
  unsigned int VAO;
  { // prepare data
    VAO = create_vao();

    // Prepare Texture data
    texture_floor = create_texture(TEXTURE_PATH_FLOOR.c_str());
    texture_wall = create_texture(TEXTURE_PATH_WALL.c_str());

    // draw our first triangle
    shader.use();
    // either set it manually like so:
    shader.setInt("texture1", 0);
    // or set it via the texture class
    shader.setInt("texture2", 1);
    shader.setInt("texture2_percent", 1.0f);
  }

  // Model positions
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // Init loop variable
  float theta = 0;
  float rotation_step = static_cast<float>(M_PI) / 180.0f / 10.0f;

  float count = 0.0f;
  float time_start = glfwGetTime();
  float time_end;
  float time_sum = 0.0f;

  std::cout << "press [Esc] to close the window" << std::endl;

  float deltaTime = 0.0f; // Time between current frame and last frame
  float lastFrame = 0.0f; // Time of last frame

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // [process input]
    processInput(window);
    processKeyEvent(window, cameraPos, cameraFront, cameraUp, deltaTime);

    // [render]
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // This has to be run before rendering, it will clean the z Depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_floor);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_wall);

    glm::mat4 view, projection;

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    projection =
        glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, 100.0f);

    for (size_t i = 0; i < 10; ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);

      if (std::fmod(i, 3) == 2) // ex3
        model = glm::rotate(model, theta, glm::vec3(0.5f, 1.0f, 0.0f));

      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

      // Pass value to shader
      int modelLoc = glGetUniformLocation(shader.getProgramId(), "model");
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      int viewLoc = glGetUniformLocation(shader.getProgramId(), "view");
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      int projectionLoc =
          glGetUniformLocation(shader.getProgramId(), "projection");
      glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                         glm::value_ptr(projection));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // glBindVertexArray(0); // no need to unbind it every time

    // Check and call events and swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
    { // Increasing loop variable
      theta += rotation_step;
      theta = std::fmod(theta, static_cast<float>(M_PI) * 2.0f);

      time_end = static_cast<float>(glfwGetTime());
      time_sum += time_end - time_start;
      time_start = time_end;
      ++count;
      if (count > 100) {
        std::cout << "FPS: " << count / time_sum << std::endl;
        time_sum = 0.0f;
        count = 0.0f;
      }
    }
  }

  clean_buffer();
  // Close
  glfwTerminate();

  return 0;
}
