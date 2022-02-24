#include "glad/glad.h"
#include "common/shader.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

constexpr const char *VERTEX_SRC =
    "/home/lenty/scripts/cpp/opengl/src/ch6.3_ex4/vertex.sd";

constexpr const char *FRAGMENT_SRC =
    "/home/lenty/scripts/cpp/opengl/src/ch6.3_ex4/fragement.sd";

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
      // positions // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top

  };

  unsigned int VBO, VAO;

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // layout num, type,xxx  ,stride, offset
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
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
               // 1, 2, 3  // second Triangle
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

int main() {

  init_glfw();

  auto window = create_window();

  load_glad();

  Shader shader{VERTEX_SRC, FRAGMENT_SRC};

  // prepare data
  auto VAO = create_vao();
  auto EBO = create_ebo(VAO);

  // Render loop
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  std::cout << "press [Esc] to close the window" << std::endl;
  float theta = 0;
  while (!glfwWindowShouldClose(window)) {
    // process input
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    shader.use();
    shader.setFloat("displacement_x", 0.5*std::sin(theta));
    
    
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
    theta+= M_PI/180.0;
  }

  clean_buffer();
  // Close
  glfwTerminate();

  return 0;
}
