#pragma once

#include <vector>
#include "shape.hpp"
#include "block_loader.hpp"
#include <iostream>
#include <functional>

class multishape : public shape {
 public:
  multishape(float line_width, uint32_t transform_index, shader_id sid = shader_id::FIXED_COLOR, uint32_t vertCount = 1024,
             uint32_t solidIndCount = 1024, uint32_t lineIndCount = 1024,
             uint32_t pointIndCount = 1024, uint32_t elemPerVert = 2)
      : line_width(line_width), elemPerVert(elemPerVert), transform_index(transform_index), sid(sid), shape() {
    vertices.reserve(vertCount * elemPerVert);
    solidIndices.reserve(solidIndCount);
    lineIndices.reserve(lineIndCount);
    pointIndices.reserve(pointIndCount);
  }
  ~multishape();
  void update() override;
  void updatePoints();
  void updateSolidIndices();
  void updateLineIndices();
  void updatePointIndices();
  void updateSolidsInFrontIndices();

  void addPoint(float x, float y) {
    vertices.push_back(x);
    vertices.push_back(y);
  }
  void dump();
  void save(std::vector<uint8_t>& instructions, std::vector<uint32_t>& uint32s, std::vector<float>& floats, std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords) const override;
  void load(drw_file& drw, parasitic_vector<uint8_t> &instructions, parasitic_vector<float>& x_coords, parasitic_vector<float>& y_coords, parasitic_vector<float>& z_coords,
            parasitic_vector<float>& floats, parasitic_vector<uint32_t>& uint32s, uint32_t& instr_index, uint32_t& current_uint32, uint32_t& current_x_coord,
            uint32_t& current_y_coord, uint32_t& current_z_coord, uint32_t& current_float);


  struct dispatch_inputs {
    multishape* shape;
    parasitic_vector<uint8_t> &instructions;
    parasitic_vector<float> &x_coords;
    parasitic_vector<float> &y_coords;
    parasitic_vector<float> &z_coords;
    parasitic_vector<float> &floats;
    parasitic_vector<uint32_t> &uint32s;
    uint32_t &current_uint32;
    uint32_t &current_x_coord;
    uint32_t &current_y_coord;
    uint32_t &current_z_coord;
    uint32_t &current_float;
  };
  
  virtual std::unordered_map<instruction, std::function<void(dispatch_inputs)>> get_dispatch_table() = 0;

  struct full_instruction {
    instruction instr;
    std::vector<float> x_coords;
    std::vector<float> y_coords;
    std::vector<float> z_coords;
    std::vector<float> misc_floats;
    std::vector<uint32_t> misc_ints;

    full_instruction() : instr(instruction::NO_INSTRUCTION) {}

    full_instruction(instruction instr, std::vector<float> x_coords, std::vector<float> y_coords, std::vector<float> z_coords, std::vector<float> misc_floats, std::vector<uint32_t> misc_ints) 
                  : instr(instr), x_coords(x_coords), y_coords(y_coords), z_coords(z_coords), misc_floats(misc_floats), misc_ints(misc_ints) {}
  };

  inline std::vector<full_instruction> get_full_instructions() const {
    return instructions;
  }

  void set_start_instr(full_instruction start_instr) {
    if (instructions.size() <= 0) instructions.push_back(start_instr);
    else instructions[0] = start_instr;
  }

  inline instruction get_end_instr() const { return end_instr; }

  protected:
  std::vector<float> vertices;
  std::vector<uint32_t> solidIndices;
  std::vector<uint32_t> lineIndices;
  std::vector<uint32_t> pointIndices;
  std::vector<uint32_t> solidsInFrontIndices;
  shader_id sid;
  uint32_t transform_index;
  const uint32_t elemPerVert;
  float line_width;

  std::vector<full_instruction> instructions;
  instruction end_instr;

  void lAddIndices(uint32_t indexCount);
  void lAddClosedIndices(uint32_t indexCount);
  uint32_t getPointIndex() const { return vertices.size() / elemPerVert; }
  void sAddSectorIndices(uint32_t centerIndex, uint32_t indexCount);
  void sAddClosedSectorIndices(uint32_t centerIndex, uint32_t indexCount);
};
