#pragma once
#include <cstdint>
#include "shader.hpp"
#include "instruction.hpp"
#include "block_loader.hpp"
#include "view.hpp"

class drw_file;

class shape {
  public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render(drw_file& drw, view& view) = 0;
    virtual void save(std::vector<uint16_t>& instructions, std::vector<uint32_t>& uint32s, std::vector<float>& floats, std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords) const = 0;
    virtual void load(drw_file& drw, parasitic_vector<uint16_t> &instructions, parasitic_vector<float>& x_coords, parasitic_vector<float>& y_coords, parasitic_vector<float>& z_coords,
                      parasitic_vector<float>& floats, parasitic_vector<uint32_t>& uint32s, uint32_t& instr_index, uint32_t& current_uint32, uint32_t& current_x_coord,
                      uint32_t& current_y_coord, uint32_t& current_z_coord, uint32_t& current_float) = 0;

  protected:
    uint32_t vao, vbo, sbo, lbo, pbo, sbo_front;
};