#include "multishape.hpp"

multishape::~multishape() {}

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
