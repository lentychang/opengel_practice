#include <string>

class Texture2D {

public:
  Texture2D() = default;
  Texture2D(const std::string &path);
  ~Texture2D(){};

  int32_t getWidth() const { return mWidth; }
  int32_t getHeight() const { return mHeight; }

  void bind() const;
  void unbind() const;

private:
  std::string mPath;
  int32_t mWidth;
  int32_t mHeight;
  uint32_t mTextureID;
};