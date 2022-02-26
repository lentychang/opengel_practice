#include "Texture2D.h"
#include <cstdint>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

Texture2D::Texture2D(const std::string &path) : mPath(path) {
  int image_w, image_h, image_nCh;

  std::cout << "Print load image: " << path << std::endl;
  stbi_set_flip_vertically_on_load(true);
  auto *imageData = stbi_load(path.c_str(), &image_w, &image_h, &image_nCh, 0);

  mWidth = image_w;
  mHeight = image_h;
  glGenTextures(1, &mTextureID);
  glBindTexture(GL_TEXTURE_2D, mTextureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imageData);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind() const { glBindTexture(GL_TEXTURE_2D, mTextureID); }

void Texture2D::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
