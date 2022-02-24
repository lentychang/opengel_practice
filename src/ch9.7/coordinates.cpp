#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "common/shader.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>
#include <vector>

const std::string PROJ_DIR = "/home/lenty/scripts/cpp/opengl/";

const std::string VERTEX_SRC = PROJ_DIR + "src/ch9.7vertex.sd";

const std::string FRAGMENT_SRC = PROJ_DIR + "src/ch9.7/fragment.sd";

const std::string TEXTURE_PATH_FLOOR = PROJ_DIR + "assets/floor.jpg";
const std::string TEXTURE_PATH_WALL = PROJ_DIR + "assets/wall.jpg";

std::vector<unsigned int> vbos{};
std::vector<unsigned int> vaos{};
std::vector<unsigned int> ebos{};

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void init_glfw() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow *create_window() {
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  return window;
}

void load_glad() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(-1);
  }
}

unsigned int create_vao() {
  float vertices[] = {
      // positions        // texture st
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right

  };

  unsigned int VBO, VAO;

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // layout num, type,xxx  ,stride, offset
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  vaos.push_back(VAO);
  vbos.push_back(VBO);
  return VAO;
}

unsigned int create_ebo(int vao) {
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 2, // first Triangle
      2, 3, 0  // second Triangle
  };

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(vao);
  // remember: do NOT unbind the EBO while a VAO is active as the bound element
  // buffer object IS stored in the VAO; keep the EBO bound.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.

  return EBO;
}

void clean_buffer() {
  while (!vaos.empty()) {
    glDeleteVertexArrays(1, &vaos.back());
    vaos.pop_back();
  }

  while (!vbos.empty()) {
    glDeleteBuffers(1, &vbos.back());
    vbos.pop_back();
  }
  while (!ebos.empty()) {
    glDeleteBuffers(1, &ebos.back());
    ebos.pop_back();
  }
}

unsigned int create_texture(const char *texture_path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on currently bound texture)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int image_w, image_h, image_nCh;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *image_data =
      stbi_load(texture_path, &image_w, &image_h, &image_nCh, 0);

  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  return texture;
}

int main() {

  init_glfw();
  auto window = create_window();
  load_glad();

  Shader shader{VERTEX_SRC.c_str(), FRAGMENT_SRC.c_str()};

  // prepare data
  auto VAO = create_vao();
  auto EBO = create_ebo(VAO);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Prepare Texture data
  unsigned int texture_floor = create_texture(TEXTURE_PATH_FLOOR.c_str());
  unsigned int texture_wall = create_texture(TEXTURE_PATH_WALL.c_str());

  // draw our first triangle
  shader.use();
  // either set it manually like so:
  shader.setInt("texture1", 0);
  // or set it via the texture class
  shader.setInt("texture2", 1);
  shader.setInt("texture2_percent", 1.0f);

  std::cout << "press [Esc] to close the window" << std::endl;
  float theta = 0;
  {
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;
  }

  // rotate our model (rectangle plane with picture) with x-Axis , and rotate it
  // by 55 degrees
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 view = glm::mat4(1.0f);
  // note that we’re translating the scene in the reverse direction

  // Move Camera from (0,0,0) to (0,0,3) which equals to move all scene downward
  // (0,0,-3)
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection;
  projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  float count = 0.0f;
  float time_start = glfwGetTime();
  float time_sum = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // process input
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.2, -0.2, 0.0));
    trans = glm::rotate(trans, theta, glm::vec3(0.0, 0.0, 1.0));

    int modelLoc = glGetUniformLocation(shader.getProgramId(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(shader.getProgramId(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projectionLoc =
        glGetUniformLocation(shader.getProgramId(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0); // activate texture unit first
    glBindTexture(GL_TEXTURE_2D, texture_floor);

    glActiveTexture(GL_TEXTURE1); // activate texture unit first
    glBindTexture(GL_TEXTURE_2D, texture_wall);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0); // no need to unbind it every time

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------

    // Check and call events and swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
    theta += static_cast<float>(M_PI) / 180.0f;
    theta = std::fmod(theta, static_cast<float>(M_PI) * 2.0f);

    // std::this_thread::sleep_for(std::chrono::milliseconds(400));
    float time_end = glfwGetTime();
    time_sum += time_end - time_start;
    time_start = time_end;
    ++count;
    if (count > 100) {
      std::cout << "FPS: " << count / time_sum << std::endl;
      time_sum = 0.0f;
      count = 0.0f;
    }
  }

  clean_buffer();
  // Close
  glfwTerminate();

  return 0;
}
