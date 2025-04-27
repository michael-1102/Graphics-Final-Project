#pragma once

#include <vector>
#include "shape.hpp"
#include "parasitic_vector.hpp"
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

  struct dispatch_inputs {
    multishape* shape;
    parasitic_vector<uint16_t> &instructions;
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

  void lAddIndices(uint32_t indexCount);
  void lAddClosedIndices(uint32_t indexCount);
  uint32_t getPointIndex() const { return vertices.size() / elemPerVert; }
  void sAddSectorIndices(uint32_t centerIndex, uint32_t indexCount);
  void sAddClosedSectorIndices(uint32_t centerIndex, uint32_t indexCount);
};
