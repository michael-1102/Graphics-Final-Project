#pragma once
#include <cstdint>
#include "drawing.hpp"
#include "shader.hpp"
#include "instruction.hpp"

class shape {
  public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render(view& view) = 0;
  protected:
    uint32_t vao, vbo, sbo, lbo, pbo, sbo_front;
};