#pragma once
#include "common.h"
#include <numeric>
#include <string>
#include <glm/matrix.hpp>
#include <vector>

class Shader {
public:
  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);
  // use/activate the shader
  void use();
  void disable();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec3(const std::string &name,float x,float y, float z) const;
  void setVec3(const std::string &name, glm::vec3 const& vec3) const;
  void setMat4f(const std::string &name,glm::mat4 const& mat) const;
  unsigned int getProgramId();
  ~Shader();

private:
  // the program ID
  unsigned int mId{std::numeric_limits<unsigned int>::max()};

  void read_shader_file(const char *vertexPath, const char *fragmentPath,
                        std::string *vertexCode, std::string *fragCode);
  unsigned int compile_shader(GLenum shaderType, const char *shaderCode,
                              std::string const &shader_name);
  unsigned int create_program(unsigned int vertexShader,
                              unsigned int fragementShader);
};
