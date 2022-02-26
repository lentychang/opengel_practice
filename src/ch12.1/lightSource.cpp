#include "common/Camera.h"
#include "common/shader.h"
#include "previous_code.cpp"
#include <cmath>
#include <glm/fwd.hpp>

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void on_mouse_click(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool left_key_pressed = false;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

unsigned int lightVAO;
const std::string LIGHT_FRAGMENT_SRC =
    PROJ_DIR + "src/" + SUB_DIR + "/light_fragment.sd";

unsigned int create_light_VAO(unsigned int vbo) {
  unsigned int lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  // we only need to bind to the VBO, the container’s VBO’s data
  // already contains the data.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // set the vertex attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  return lightVAO;
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

  // Prepare Shader data
  Shader shader{VERTEX_SRC.c_str(), FRAGMENT_SRC.c_str()};
  shader.use();

  unsigned int VAO;
  { // prepare Vertex data
    VAO = create_vao();
    shader.use();
  }

  Shader light_cube_shader{VERTEX_SRC.c_str(), LIGHT_FRAGMENT_SRC.c_str()};

  glm::mat4 view, projection;
  view =
      glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

  projection = glm::perspective(glm::radians(camera.Zoom),
                                static_cast<float>(SCR_WIDTH) /
                                    static_cast<float>(SCR_HEIGHT),
                                0.1f, 100.0f);

  glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
  auto light_model = glm::mat4(1.0f);
  light_model = glm::translate(light_model, lightPos);
  light_model = glm::scale(light_model, glm::vec3(0.2f));

  // Model positions
  glm::vec3 cubePositions[] = {glm::vec3(1.0f, 1.0f, 1.0f)};

  auto lightVAO = create_light_VAO(vbos[0]);

  // Init loop variable
  float theta = 0;
  float rotation_step = static_cast<float>(M_PI) / 180.0f / 10.0f;

  float count = 0.0f;
  float time_sum = 0.0f;

  glm::vec3 lightColor{1.0f, 1.0f, 1.0f};

  std::cout << "press [Esc] to close the window" << std::endl;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // [process input]
    processInput(window);
    processKeyEvent(window, camera, deltaTime);

    // [render]
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // This has to be run before rendering, it will clean the z Depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = camera.GetViewMatrix();

    projection = glm::perspective(glm::radians(camera.Zoom),
                                  static_cast<float>(SCR_WIDTH) /
                                      static_cast<float>(SCR_HEIGHT),
                                  0.1f, 100.0f);

    { // Draw object
      shader.use();
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::rotate(model, theta, glm::vec3(0.5f, 1.0f, 0.0f));

      // Pass value to shader
      shader.setMat4f("model", model);
      shader.setMat4f("view", view);
      shader.setMat4f("projection", projection);
      shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
      shader.setVec3("lightColor", lightColor);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      shader.disable();
    }

    { // Draw Light cube
      light_cube_shader.use();
      light_cube_shader.setMat4f("model", light_model);
      light_cube_shader.setMat4f("view", view);
      light_cube_shader.setMat4f("projection", projection);
      light_cube_shader.setVec3("lightColor", lightColor);
      glBindVertexArray(lightVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      light_cube_shader.disable();
    }
    // Check and call events and swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    { // Increasing loop variable
      theta += rotation_step;
      theta = std::fmod(theta, static_cast<float>(M_PI) * 2.0f);

      time_sum += deltaTime;      
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

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static float lastX = 400.0f, lastY = 300.0f;
  static float yaw = -90.0f, pitch = 0.0f;
  static constexpr float sensitivity = 0.1f;

  static constexpr float max_pitch = 89.0f;

  float x = static_cast<float>(xpos);
  float y = static_cast<float>(ypos);

  if (left_key_pressed) {
    if (firstMouse) {
      lastX = x;
      lastY = y;
      firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed: y ranges bottom to top
    lastX = x;
    lastY = y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > max_pitch)
      pitch = max_pitch;
    if (pitch < -max_pitch)
      pitch = -max_pitch;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.Front = glm::normalize(direction);
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
  camera.Zoom -= static_cast<float>(yoffset);
  if (camera.Zoom < 1.0f)
    camera.Zoom = 1.0f;
  if (camera.Zoom > 45.0f)
    camera.Zoom = 45.0f;
}
