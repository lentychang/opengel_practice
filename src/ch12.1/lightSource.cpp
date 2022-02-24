#include "common/shader.h"
#include "previous_code.cpp"
#include <cmath>
#include <glm/fwd.hpp>

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
  // Prepare Shader data
  Shader shader{VERTEX_SRC.c_str(), FRAGMENT_SRC.c_str()};
  shader.use();
  shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

  unsigned int VAO;
  { // prepare Vertex data
    VAO = create_vao();
    shader.use();
  }

  glm::mat4 view, projection;
  { // Prepare view/ projection matrix
    view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
  }

  Shader light_cube_shader{VERTEX_SRC.c_str(), LIGHT_FRAGMENT_SRC.c_str()};

  glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
  auto light_model = glm::mat4(1.0f);
  light_model = glm::translate(light_model, lightPos);
  light_model = glm::scale(light_model, glm::vec3(0.2f));
  light_cube_shader.setMat4f("model", light_model);
  light_cube_shader.setMat4f("view", view);
  light_cube_shader.setMat4f("projection", projection);

  // Model positions
  glm::vec3 cubePositions[] = {glm::vec3(1.0f, 1.0f, 1.0f)};

  auto lightVAO = create_light_VAO(VBO);

  // Init loop variable
  float theta = 0;
  float rotation_step = static_cast<float>(M_PI) / 180.0f / 10.0f;

  float count = 0.0f;
  float time_start = glfwGetTime();
  float time_end;
  float time_sum = 0.0f;

  std::cout << "press [Esc] to close the window" << std::endl;
  while (!glfwWindowShouldClose(window)) {
    // [process input]
    processInput(window);

    // [render]
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // This has to be run before rendering, it will clean the z Depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, theta, glm::vec3(0.5f, 1.0f, 0.0f));

    // Pass value to shader
    shader.setMat4f("model", model);
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Draw Light cube
    light_cube_shader.use();
    light_cube_shader.setMat4f("model", light_model);
    light_cube_shader.setMat4f("view", view);
    light_cube_shader.setMat4f("projection", projection);
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Check and call events and swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
    { // Increasing loop variable
      theta += rotation_step;
      theta = std::fmod(theta, static_cast<float>(M_PI) * 2.0f);

      time_end = glfwGetTime();
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
