#include "previous_code.cpp"
#include <cmath>
#include <glm/fwd.hpp>

int main() {

  GLFWwindow *window;
  { // init
    init_glfw();
    window = create_window();
    load_glad();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
  }

  Shader shader{VERTEX_SRC.c_str(), FRAGMENT_SRC.c_str()};
  unsigned int texture_floor, texture_wall;
  unsigned int VAO;
  { // prepare data
    VAO = create_vao();
    auto EBO = create_ebo(VAO);

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

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

  float deltaTime = 0.0f; // Time between current frame and last frame
  float lastFrame = 0.0f; // Time of last frame

  while (!glfwWindowShouldClose(window)) {
    // [process input]
    processInput(window);
    processKeyEvent(window, cameraPos, cameraFront, cameraUp);

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

    const float radius = 10.0f;

    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;

    glm::mat4 view, projection;

    view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0),
                       glm::vec3(0.0, 1.0, 0.0));

    projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

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
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
  }

  clean_buffer();
  // Close
  glfwTerminate();

  return 0;
}
