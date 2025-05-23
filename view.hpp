#pragma once
#include <cstdint>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

struct view {
  float x;
  float y;
  float width;
  float height;

  glm::mat4 matrix;

  view(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {
    //TODO: matrix becomes incorrect when width/height is larger than overall width/height
    if (width > height)
      matrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f, height/width, 1.f)) * glm::ortho(x, x + width, y + height, y);
    else
      matrix = glm::scale(glm::mat4(1.f), glm::vec3(width/height, 1.f, 1.f)) * glm::ortho(x, x + width, y + height, y);
  }

  view() : x(0), y(0), width(0), height(0) {
    matrix = glm::mat4(1.f);
  }

  void save(std::vector<float>& floats) const {
    floats.push_back(x);
    floats.push_back(y);
    floats.push_back(width);
    floats.push_back(height);
  }
};