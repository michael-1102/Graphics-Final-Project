#pragma once
#include <cstdint>
#include "shader.hpp"
#include "instruction.hpp"
#include "view.hpp"

class drw_file;

class shape {
  public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render(drw_file& drw, view& view) = 0;
    virtual void save(std::vector<instruction>& instructions, std::vector<uint32_t> uint32s, std::vector<float> floats, std::vector<float> x_coords, std::vector<float> y_coords, std::vector<float> z_coords) = 0;

  protected:
    uint32_t vao, vbo, sbo, lbo, pbo, sbo_front;
};