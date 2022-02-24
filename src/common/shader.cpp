#include "shader.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;

  read_shader_file(vertexPath, fragmentPath, &vertexCode, &fragmentCode);
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  auto vertexShader = compile_shader(GL_VERTEX_SHADER, vShaderCode, "vertex");
  auto fragmentShader =
      compile_shader(GL_FRAGMENT_SHADER, fShaderCode, "fragement");

  mId = create_program(vertexShader, fragmentShader);
}

void Shader::read_shader_file(const char *vertexPath, const char *fragmentPath,
                              std::string *vertexCode, std::string *fragCode) {
  try {
    std::ifstream vShaderFile{vertexPath};
    std::ifstream fShaderFile{fragmentPath};

    std::stringstream vShaderStream, fShaderStream;
    // read file’s buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    *vertexCode = vShaderStream.str();
    *fragCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR: SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
}

unsigned int Shader::compile_shader(GLenum shaderType, const char *shaderCode,
                                    std::string const &shader_name) {
  int success;
  char infoLog[512];
  // vertex Shader
  auto shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);
  // print compile errors if any
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR Failed to compile SHADER('" << shader_name << "',"
              << shaderType << ")\n"
              << infoLog << std::endl;
  };
  return shader;
}

unsigned int Shader::create_program(unsigned int vertex,
                                    unsigned int fragment) {
  int success;
  char infoLog[512];
  unsigned int id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  // print linking errors if any
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  // delete shaders; they’re linked into our program and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return id;
}

void Shader::use() { glUseProgram(mId); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(mId, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(mId, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(mId, name.c_str()), value);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(mId, name.c_str()), x, y, z);
}

void Shader::setMat4f(const std::string &name,glm::mat4 const& mat) const{
  int var = glGetUniformLocation(mId, name.c_str());
      glUniformMatrix4fv(var, 1, GL_FALSE, glm::value_ptr(mat));
}

Shader::~Shader() {
  // if (mId != std::numeric_limits<unsigned int>::max()) {
  //   glDeleteProgram(mId);
  // }
}

unsigned int Shader::getProgramId() { return mId; }
