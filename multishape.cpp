#include "multishape.hpp"

multishape::~multishape() {}

void multishape::save(std::vector<uint8_t>& instrs, std::vector<uint32_t>& uint32s, std::vector<float>& floats, std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords) const {
  for (auto instr : instructions) {
    instrs.push_back(to_underlying(instr.instr));
    x_coords.insert(std::end(x_coords), std::begin(instr.x_coords), std::end(instr.x_coords));
    y_coords.insert(std::end(y_coords), std::begin(instr.y_coords), std::end(instr.y_coords));
    z_coords.insert(std::end(z_coords), std::begin(instr.z_coords), std::end(instr.z_coords));
    uint32s.insert(std::end(uint32s), std::begin(instr.misc_ints), std::end(instr.misc_ints));
    floats.insert(std::end(floats), std::begin(instr.misc_floats), std::end(instr.misc_floats));
  }

  if (end_instr != instruction::NO_INSTRUCTION) {
    instrs.push_back(to_underlying(end_instr));
  }
}

void multishape::update() {
  if (vbo != 0) updatePoints();
  if (sbo != 0) updateSolidIndices();
  if (lbo != 0) updateLineIndices();
  if (pbo != 0) updatePointIndices();
  if (sbo_front != 0) updateSolidsInFrontIndices();
}

void multishape::updatePoints() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],
               GL_STATIC_DRAW);
}

void multishape::updateSolidIndices() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * solidIndices.size(),
               &solidIndices[0], GL_STATIC_DRAW);
}

void multishape::updateLineIndices() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * lineIndices.size(),
               &lineIndices[0], GL_STATIC_DRAW);
}

void multishape::updatePointIndices() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pointIndices.size(),
               &pointIndices[0], GL_STATIC_DRAW);
}

void multishape::updateSolidsInFrontIndices() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo_front);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * solidsInFrontIndices.size(),
               &solidsInFrontIndices[0], GL_STATIC_DRAW);
}

void multishape::lAddIndices(uint32_t indexCount) {
  uint32_t startIndex = getPointIndex() - indexCount;
  for (uint32_t curIndex = startIndex;
       curIndex < startIndex + indexCount - 1; curIndex++) {
    lineIndices.push_back(curIndex);
    lineIndices.push_back(curIndex + 1);
  }
}

void multishape::lAddClosedIndices(uint32_t indexCount) {
  lAddIndices(indexCount);
  uint32_t pointIndex = getPointIndex();
  // close ellipse
  lineIndices.push_back(pointIndex - 1);
  lineIndices.push_back(pointIndex - indexCount);
}

void multishape::sAddSectorIndices(uint32_t centerIndex, uint32_t indexCount) {
  uint32_t startIndex = getPointIndex() - indexCount;
  for (uint32_t curIndex = startIndex; curIndex < startIndex + indexCount - 1; curIndex++) {
    solidIndices.push_back(centerIndex);
    solidIndices.push_back(curIndex);
    solidIndices.push_back(curIndex + 1);
  }
}

// used when sector angle is full 360, finishes sector by reconnecting to beginning
void multishape::sAddClosedSectorIndices(uint32_t centerIndex, uint32_t indexCount) {
  sAddSectorIndices(centerIndex, indexCount);
  uint32_t pointIndex = getPointIndex();
  solidIndices.push_back(centerIndex);
  solidIndices.push_back(pointIndex - 1);
  solidIndices.push_back(pointIndex - indexCount);
}

void multishape::dump() {
  fprintf(stderr, "\nVertices: %lu\n", vertices.size() / elemPerVert);

  for (int i = 0; i < vertices.size(); i += elemPerVert) {
    for (int j = 0; j < elemPerVert; j++) fprintf(stderr, "%12f\t", vertices[i + j]);
    fprintf(stderr, "\n");
  }

  if (solidIndices.size() != 0) {
    fprintf(stderr, "\nSolid Indices: %lu\n", solidIndices.size() / 3);
    for (int i = 0; i < solidIndices.size(); i += 3) {
      fprintf(stderr, "%6d %6d %6d\n", solidIndices[i], solidIndices[i + 1],
              solidIndices[i + 2]);
    }
  }

  if (lineIndices.size() != 0) {
    fprintf(stderr, "\nLine Indices: %lu\n", lineIndices.size() / 2);
    for (int i = 0; i < lineIndices.size(); i += 2) {
      fprintf(stderr, "%6d %6d\n", lineIndices[i], lineIndices[i + 1]);
    }
  }

  if (pointIndices.size() != 0) {
    fprintf(stderr, "\nPoint Indices: %lu\n", pointIndices.size());
    for (int i = 0; i < pointIndices.size(); i++) {
      fprintf(stderr, "%6d\n", pointIndices[i]);
    }
  }

  if (solidsInFrontIndices.size() != 0) {
    fprintf(stderr, "\nSolids In Front Indices: %lu\n", solidsInFrontIndices.size() / 3);
    for (int i = 0; i < solidsInFrontIndices.size(); i+= 3) {
      fprintf(stderr, "%6d %6d %6d\n", solidsInFrontIndices[i], solidsInFrontIndices[i + 1],
        solidsInFrontIndices[i + 2]);
    }
  }
}

void multishape::load(drw_file& drw, parasitic_vector<uint8_t> &instructions, parasitic_vector<float>& x_coords, parasitic_vector<float>& y_coords, parasitic_vector<float>& z_coords,
  parasitic_vector<float>& floats, parasitic_vector<uint32_t>& uint32s, uint32_t& instr_index, uint32_t& current_uint32, uint32_t& current_x_coord,
  uint32_t& current_y_coord, uint32_t& current_z_coord, uint32_t& current_float) {

  instr_index++;
  instruction current_instr = static_cast<instruction>(instructions[instr_index]);
  while (current_instr != end_instr) {
      get_dispatch_table()[current_instr](multishape::dispatch_inputs{this, instructions, x_coords, y_coords, z_coords,
                                                                      floats, uint32s, current_uint32,
                                                                      current_x_coord, current_y_coord,
                                                                      current_z_coord, current_float});
      instr_index++;
      current_instr = static_cast<instruction>(instructions[instr_index]);
    }

}