#include "multishape_2d.hpp"
#include "drw_file.hpp"


multishape_2d::~multishape_2d() {}

void multishape_2d::init() {
  // Create VAO,
  //  a container to have all shapes and their attributes
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  // Create VBO for vertices
  // Create an object in the VAO to store all the vertex values
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0],
               GL_STATIC_DRAW);
  // Desctribe how information is recieved in shaders
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Create SBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as a solid(filled)
  glGenBuffers(1, &sbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * solidIndices.size(),
               &solidIndices[0], GL_STATIC_DRAW);
  // Create LBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as lines(wireframe)
  glGenBuffers(1, &lbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * lineIndices.size(),
               &lineIndices[0], GL_STATIC_DRAW);

  // Create PBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as points.
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pointIndices.size(),
               &pointIndices[0], GL_STATIC_DRAW);

}

void multishape_2d::render(drw_file& drw, view& view) {
  shader& s = drw.get_shader(sid);
  s.use();
  s.setVec4("color", drw.get_color(color_index));
  s.setMat4("scale", drw.get_scale());
  s.setMat4("view", view.matrix);
  s.setMat4("transform", drw.get_transform(transform_index));

  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glLineWidth(line_width);

  // Draw Solids
  if (solidIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
    glDrawElements(GL_TRIANGLES, solidIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Draw Lines
  if (lineIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
    glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Draw Points
  if (pointIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
    glDrawElements(GL_POINTS, pointIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Draw Solids In Front
  if (solidsInFrontIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo_front);
    glDrawElements(GL_TRIANGLES, solidsInFrontIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Unbind
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glBindVertexArray(0);
}

void multishape_2d::sAddTriIndices() {
  uint32_t startIndex = getPointIndex() - 3;
  solidIndices.push_back(startIndex);
  solidIndices.push_back(startIndex + 1);
  solidIndices.push_back(startIndex + 2);
}

void multishape_2d::sAddQuadIndices() {
  uint32_t startIndex = getPointIndex() - 4;
  solidIndices.push_back(startIndex);
  solidIndices.push_back(startIndex + 1);
  solidIndices.push_back(startIndex + 2);

  solidIndices.push_back(startIndex + 2);
  solidIndices.push_back(startIndex + 3);
  solidIndices.push_back(startIndex);
}


void multishape_2d::lAddTriIndices() {
  uint32_t startIndex = getPointIndex() - 3;
  lineIndices.push_back(startIndex);
  lineIndices.push_back(startIndex + 1);
  lineIndices.push_back(startIndex + 1);
  lineIndices.push_back(startIndex + 2);
  lineIndices.push_back(startIndex + 2);
  lineIndices.push_back(startIndex);
}

void multishape_2d::lAddQuadIndices() {
  uint32_t startIndex = getPointIndex() - 4;
  lineIndices.push_back(startIndex);
  lineIndices.push_back(startIndex + 1);
  lineIndices.push_back(startIndex + 1);
  lineIndices.push_back(startIndex + 2);
  lineIndices.push_back(startIndex + 2);
  lineIndices.push_back(startIndex + 3);
  lineIndices.push_back(startIndex + 3);
  lineIndices.push_back(startIndex);
}


void multishape_2d::pAddTriIndices() {
  uint32_t startIndex = getPointIndex() - 3;

  pointIndices.push_back(startIndex);
  pointIndices.push_back(startIndex + 1);
  pointIndices.push_back(startIndex + 2);
}

void multishape_2d::pAddQuadIndices() {
  uint32_t startIndex = getPointIndex() - 4;

  pointIndices.push_back(startIndex);
  pointIndices.push_back(startIndex + 1);
  pointIndices.push_back(startIndex + 2);
  pointIndices.push_back(startIndex + 3);
}

void multishape_2d::pAddIndices(uint32_t indexCount) {
  uint32_t startIndex = getPointIndex() - indexCount;
  for (uint32_t curIndex = startIndex; curIndex < startIndex + indexCount;
       curIndex++) {
    pointIndices.push_back(curIndex);
  }
}

/*
        Draw a sector of an ellipse aligned to the xy axes
        centered on (xc, yc) with radius xRad in the x-direction, yRad in the y
        fromAngle to toAngle stepping by angleInc

  RETURNS: the number of points that make up the sector and
  have been added as (x,y) pairs to the vertices vector

        all angles specified in degrees
 */
uint32_t multishape_2d::addSector(float xc, float yc, float xRad, float yRad,
                                 float fromAngle, float toAngle,
                                 float angleInc) {
  uint32_t points_added = 0;
  for (float i = fromAngle; i <= toAngle; i += angleInc) {
    addPoint(float(xc + xRad * cos(-i * M_PI/180.0)), float(yc + yRad * sin(-i * M_PI/180.0)));
    points_added++;
  }
  return points_added;
}

// addXVertices functions used in cases where solid, line, and/or point primitives share most of their code
void multishape_2d::addRectVertices(float x, float y, float w, float h) {
  addPoint(x, y);
  addPoint(x, y + h);  // goes in counter-clockwise order
  addPoint(x + w, y + h);
  addPoint(x + w, y);
}

void multishape_2d::addTriVertices(float x1, float y1, float x2, float y2,
  float x3, float y3) {
  addPoint(x1, y1);
  addPoint(x2, y2);
  addPoint(x3, y3);
}

uint32_t multishape_2d::addBezierVertices(std::vector<glm::vec2> points, float inc) {
  int numPoints = points.size();
  if (numPoints <= 0) return 0;
  else if (numPoints == 1) {
    drawPoint(points[0].x, points[0].y);
    return 0;
  }
  uint32_t points_added = 0;
  float x, y, t;
  for (t = 0; t < 1; t += inc) {
    x = 0;
    y = 0;
    int pascalCoeff = 1;
    for (int i = 0; i < numPoints; i++) {
      x += pascalCoeff * pow((1.0 - t), numPoints - i - 1) * pow(t, i) *
           points[i].x;
      y += pascalCoeff * pow((1.0 - t), numPoints-i-1) * pow(t, i) * points[i].y;
      pascalCoeff *= numPoints - 1 - i;
      pascalCoeff /= i + 1;
    }
    addPoint(x, y); 
    points_added++;
  }
  addPoint(points[numPoints-1].x, points[numPoints-1].y);
  points_added++;
  return points_added;
}

uint32_t multishape_2d::addQuadBezierVertices(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc) {
  uint32_t points_added = 0;
  float t;
  for (t = 0; t < 1; t += inc) {
    addPoint(
      pow((1.0-t), 2)*p1x + 2*(1-t)*t*p2x + pow(t, 2)*p3x, 
      pow((1.0-t), 2)*p1y + 2*(1-t)*t*p2y + pow(t, 2)*p3y
    ); 
    points_added++;
  }
  addPoint(p3x, p3y);
  points_added++;
  return points_added;
}

uint32_t multishape_2d::addCubicBezierVertices(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc) {
  uint32_t points_added = 0;
  float t;
  for (t = 0; t < 1; t += inc) {
    //TODO: use Horner's form ((at+b)t+c)t + d to be more efficient?
    addPoint(
      pow((1.0-t), 3)*p1x + 3*pow((1-t), 2)*t*p2x + 3*(1-t)*pow(t, 2)*p3x + pow(t, 3)*p4x, 
      pow((1.0-t), 3)*p1y + 3*pow((1-t), 2)*t*p2y + 3*(1-t)*pow(t, 2)*p3y + pow(t, 3)*p4y 
    ); 
    points_added++;
  }
  addPoint(p4x, p4y);
  points_added++;
  return points_added;
}

uint32_t multishape_2d::addCubicSplineVertices(std::vector<glm::vec2> points, float inc) {
  int numPoints = points.size();
  if (numPoints <= 0) return 0;
  else if (numPoints == 1) {
    drawPoint(points[0].x, points[0].y);
    return 0;
  }
  int numSegments = numPoints - 1;
  uint32_t points_added = 0;
  std::vector<float> cPrime(numPoints), dxPrime(numPoints), dyPrime(numPoints), Dx(numPoints), Dy(numPoints);
  cPrime[0] = 0.5;
  dxPrime[0] = 1.5 * (points[1].x - points[0].x);
  dyPrime[0] = 1.5 * (points[1].y - points[0].y);
  for (int i = 1; i < numPoints - 1; i++) {
    cPrime[i] = 1 / (4 - cPrime[i - 1]);
    dxPrime[i] = (3*(points[i + 1].x - points[i - 1].x) - dxPrime[i - 1]) / (4 - cPrime[i - 1]);
    dyPrime[i] = (3*(points[i + 1].y - points[i - 1].y) - dyPrime[i - 1]) / (4 - cPrime[i - 1]);
  }
  dxPrime[numSegments] = (3*(points[numSegments].x - points[numSegments - 1].x) - dxPrime[numSegments - 1]) / (2 - cPrime[numSegments - 1]);
  dyPrime[numSegments] = (3*(points[numSegments].y - points[numSegments - 1].y) - dyPrime[numSegments - 1]) / (2 - cPrime[numSegments - 1]);

  Dx[numPoints - 1] = dxPrime[numPoints - 1];
  Dy[numPoints - 1] = dyPrime[numPoints - 1];

  for (int i = numPoints - 2; i >= 0; i--) {
    Dx[i] = dxPrime[i] - cPrime[i] * Dx[i + 1];
    Dy[i] = dyPrime[i] - cPrime[i] * Dy[i + 1];
  }

  std::vector<float> cx(numSegments), cy(numSegments), dx(numSegments), dy(numSegments);
  for (int i = 0; i < numSegments; i++) {
    cx[i] = 3 * (points[i + 1].x - points[i].x) - 2 * Dx[i] - Dx[i + 1];
    cy[i] = 3 * (points[i + 1].y - points[i].y) - 2 * Dy[i] - Dy[i + 1];

    dx[i] = 2 * (points[i].x - points[i + 1].x) + Dx[i] + Dx[i + 1];
    dy[i] = 2 * (points[i].y - points[i + 1].y) + Dy[i] + Dy[i + 1];

    float t;
    for (t = 0; t < 1; t += inc) {
      addPoint(points[i].x + t * Dx[i] + t * t * cx[i] + t * t * t * dx[i],
               points[i].y + t * Dy[i] + t * t * cy[i] + t * t * t * dy[i]);
      points_added++;
    }
  }
  addPoint(points[numSegments].x, points[numSegments].y);
  points_added++;
  return points_added;
}

// Solid Primitives
void multishape_2d::fillSquare(float x, float y, float size) {
  fillRectangle(x, y, size, size);
}

void multishape_2d::fillRectangle(float x, float y, float w, float h) {
  addRectVertices(x, y, w, h);
  sAddQuadIndices();
}

void multishape_2d::fillRoundRect(float x, float y, float w, float h, float rx,
                                 float ry, uint32_t sectors) {
  float angle_inc = 90 / sectors;
  uint32_t centerIndex = getPointIndex();
  addPoint(x + w / 2, y + h / 2);

  uint32_t toAdd = addSector(x + rx, y + ry, rx, ry, 90, 180, angle_inc);
  sAddSectorIndices(centerIndex, toAdd);

  solidIndices.push_back(centerIndex);
  solidIndices.push_back(getPointIndex() - 1);
  solidIndices.push_back(getPointIndex());

  toAdd = addSector(x + rx, y + h - ry, rx, ry, 180, 270, angle_inc);
  sAddSectorIndices(centerIndex, toAdd);

  solidIndices.push_back(centerIndex);
  solidIndices.push_back(getPointIndex() - 1);
  solidIndices.push_back(getPointIndex());

  toAdd = addSector(x + w - rx, y + h - ry, rx, ry, 270, 360, angle_inc);
  sAddSectorIndices(centerIndex, toAdd);

  solidIndices.push_back(centerIndex);
  solidIndices.push_back(getPointIndex() - 1);
  solidIndices.push_back(getPointIndex());

  toAdd = addSector(x + w - rx, y + ry, rx, ry, 0, 90, angle_inc);
  sAddSectorIndices(centerIndex, toAdd);

  solidIndices.push_back(centerIndex);
  solidIndices.push_back(getPointIndex() - 1);
  solidIndices.push_back(centerIndex + 1);
}

void multishape_2d::fillTriangle(float x1, float y1, float x2, float y2,
                                float x3, float y3) {
  addTriVertices(x1, y1, x2, y2, x3, y3);
  sAddTriIndices();
}

void multishape_2d::fillCircle(float x, float y, float rad, uint32_t sectors) {
  fillEllipse(x, y, rad, rad, sectors);
}

void multishape_2d::fillEllipse(float x, float y, float xRad, float yRad,
                               uint32_t sectors) {
  float angleInc = 360 / sectors;
  uint32_t centerIndex = getPointIndex();
  addPoint(x, y);

  uint32_t toAdd = addSector(x, y, xRad, yRad, 0, 360 - angleInc, angleInc);
  sAddClosedSectorIndices(centerIndex, toAdd);
}

void multishape_2d::fillPolygon(std::vector<glm::vec2> points) {
  //TODO: filled in polygon
}

// Markers for Graphs
void multishape_2d::fillCircleMarker(float x, float y, float size) {
  fillCircle(x, y, size, 20);
}

void multishape_2d::fillTriangleMarker(float x, float y, float size) {
  float y1 = y - (size * 0.5);
  float xOffset = size * 0.5 * sqrt(3);
  fillTriangle(x, y + size, x - xOffset, y1, x + xOffset, y1);
}

void multishape_2d::fillSquareMarker(float x, float y, float size) {
  float d = size * sqrt(2);
  fillRectangle(x - d/2, y - d/2, d, d);
}

void multishape_2d::fillHexagonMarker(float x, float y, float size) {
  fillEllipse(x, y, size, size, 6);
}

// Line Primitives
void multishape_2d::drawRectangle(float x, float y, float w, float h) {
  addRectVertices(x, y, w, h);
  lAddQuadIndices();
}

void multishape_2d::drawSquare(float x, float y, float size) {
  drawRectangle(x, y, size, size);
}

void multishape_2d::drawRoundRect(float x, float y, float w, float h, float rx,
                                 float ry, uint32_t sectors) {
  float angle_inc = 90 / sectors;
  uint32_t ind = getPointIndex();

  uint32_t toAdd = addSector(x + rx, y + ry, rx, ry, 90, 180, angle_inc);
  lAddIndices(toAdd);

  lineIndices.push_back(getPointIndex() - 1);
  lineIndices.push_back(getPointIndex());

  toAdd = addSector(x + rx, y + h - ry, rx, ry, 180, 270, angle_inc);
  lAddIndices(toAdd);

  lineIndices.push_back(getPointIndex() - 1);
  lineIndices.push_back(getPointIndex());

  toAdd = addSector(x + w - rx, y + h - ry, rx, ry, 270, 360, angle_inc);
  lAddIndices(toAdd);

  lineIndices.push_back(getPointIndex() - 1);
  lineIndices.push_back(getPointIndex());

  toAdd = addSector(x + w - rx, y + ry, rx, ry, 0, 90, angle_inc);
  lAddIndices(toAdd);

  lineIndices.push_back(getPointIndex() - 1);
  lineIndices.push_back(ind);
}

void multishape_2d::drawTriangle(float x1, float y1, float x2, float y2,
                                float x3, float y3) {
  addTriVertices(x1, y1, x2, y2, x3, y3);
  lAddTriIndices();
}

void multishape_2d::drawCircle(float x, float y, float rad, uint32_t sectors) {
  drawEllipse(x, y, rad, rad, sectors);
}

void multishape_2d::drawEllipse(float x, float y, float xRad, float yRad,
                               uint32_t sectors) {
  float angleInc = 360 / sectors;
  uint32_t ind = getPointIndex();
  uint32_t toAdd = addSector(x, y, xRad, yRad, 0, 360 - angleInc, angleInc);
  lAddClosedIndices(toAdd);
}

void multishape_2d::drawLine(float x1, float y1, float x2, float y2) {
  uint32_t ind = getPointIndex();

  addPoint(x1, y1);
  addPoint(x2, y2);
  lineIndices.push_back(ind++);
  lineIndices.push_back(ind);
}

void multishape_2d::drawPolyline(std::vector<glm::vec2> points) {
  int numPoints = points.size();
  if (numPoints <= 0) return;
  else if (numPoints == 1) {
    drawPoint(points[0].x, points[0].y);
    return;
  }
  for (int i = 0; i < numPoints - 1; i++) {
    drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
  }
}

void multishape_2d::drawPolylineMarker(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  int numPoints = points.size();
  if (numPoints <= 0) return;
  if (numPoints > 1) 
    for (int i = 0; i < numPoints - 1; i++) {
      drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
    }
  drawMarkers(points, marker, markerSize);
}

void multishape_2d::drawPolygon(std::vector<glm::vec2> points) {
  int numPoints = points.size();
  drawPolyline(points);
  if (numPoints > 2) {
    drawLine(points[0].x, points[0].y, points[numPoints - 1].x, points[numPoints - 1].y);
  }
}

void multishape_2d::drawPolygonMarker(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  int numPoints = points.size();
  drawPolylineMarker(points, marker, markerSize);
  if (numPoints > 2) {
    drawLine(points[0].x, points[0].y, points[numPoints - 1].x, points[numPoints - 1].y);
  }
}

void multishape_2d::drawGrid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows) {
  // draw vertical lines
  float x = x0;
  const float dx = (w / numCols);
  for (uint32_t i = numCols + 1; i > 0; i--, x += dx)
    drawLine(x, y0, x, y0 + h);
  // draw horizontal lines
  float y = y0;
  const float dy = (h / numRows);
  for (uint32_t i = numRows + 1; i > 0; i--, y += dy)
    drawLine(x0, y, x0 + w, y);
}

// Markers for Graphs
void multishape_2d::drawCircleMarker(float x, float y, float size) {
  drawCircle(x, y, size, 20);
}

void multishape_2d::drawTriangleMarker(float x, float y, float size) {
  float y1 = y - (size * 0.5);
  float xOffset = size * 0.5 * sqrt(3);
  drawTriangle(x, y + size, x - xOffset, y1, x + xOffset, y1);
}

void multishape_2d::drawSquareMarker(float x, float y, float size) {
  float d = size * sqrt(2);
  drawRectangle(x - d/2, y - d/2, d, d);
}

void multishape_2d::drawHexagonMarker(float x, float y, float size) {
  drawEllipse(x, y, size, size, 6);
}

void multishape_2d::drawCrossMarker(float x, float y, float size) {
  drawLine(x, y + size, x, y - size);
  drawLine(x - size, y, x + size, y);
}

void multishape_2d::drawMarkers(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  void (multishape_2d::*markerFunc)(float, float, float) = NULL;
  switch(marker) {
    case Marker::FILL_CIRCLE:
      markerFunc = &multishape_2d::fillCircleMarker;
      break;
    case Marker::FILL_TRIANGLE:
      markerFunc = &multishape_2d::fillTriangleMarker;
      break;
    case Marker::FILL_SQUARE:
      markerFunc = &multishape_2d::fillSquareMarker;
      break;
    case Marker::FILL_HEXAGON:
      markerFunc = &multishape_2d::fillHexagonMarker;
      break;
    case Marker::DRAW_CIRCLE:
      markerFunc = &multishape_2d::drawCircleMarker;
      break;
    case Marker::DRAW_TRIANGLE:
      markerFunc = &multishape_2d::drawTriangleMarker;
      break;
    case Marker::DRAW_SQUARE:
      markerFunc = &multishape_2d::drawSquareMarker;
      break;
    case Marker::DRAW_HEXAGON:
      markerFunc = &multishape_2d::drawHexagonMarker;
      break;
    case Marker::DRAW_CROSS:
    default:
      markerFunc = &multishape_2d::drawCrossMarker;
      break;
  }
  for (auto point : points) {
    (*this.*markerFunc)(point.x, point.y, markerSize);
  } 
}

void multishape_2d::drawBezier(std::vector<glm::vec2> points, float inc) {
  lAddIndices(addBezierVertices(points, inc));
}

void multishape_2d::drawQuadraticBezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc) {
  lAddIndices(addQuadBezierVertices(p1x, p1y, p2x, p2y, p3x, p3y, inc));
}

void multishape_2d::drawCubicBezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc) {
  lAddIndices(addCubicBezierVertices(p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y, inc));
}

void multishape_2d::drawCubicSpline(std::vector<glm::vec2> points, float inc) {
  lAddIndices(addCubicSplineVertices(points, inc));
}

void multishape_2d::drawCubicSplineMarker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize) {
  lAddIndices(addCubicSplineVertices(points, inc));
  drawMarkers(points, marker, markerSize);
}

void multishape_2d::drawPCHIP(std::vector<glm::vec2> points, float inc) {

}

// Point Primitives
void multishape_2d::rectanglePoints(float x, float y, float w, float h) {
  addRectVertices(x, y, w, h);
  pAddQuadIndices();
}

void multishape_2d::squarePoints(float x, float y, float size) {
  rectanglePoints(x, y, size, size);
}

void multishape_2d::roundRectPoints(float x, float y, float w, float h, float rx,
                                   float ry, uint32_t sectors) {
  float angle_inc = 90 / sectors;
  uint32_t toAdd = addSector(x + rx, y + ry, rx, ry, 90, 180, sectors);
  pAddIndices(toAdd);

  toAdd = addSector(x + rx, y + h - ry, rx, ry, 180, 270, sectors);
  pAddIndices(toAdd);

  toAdd = addSector(x + w - rx, y + h - ry, rx, ry, 270, 360, sectors);
  pAddIndices(toAdd);

  toAdd = addSector(x + w - rx, y + ry, rx, ry, 0, 90, sectors);
  pAddIndices(toAdd);
}

void multishape_2d::trianglePoints(float x1, float y1, float x2, float y2,
                                  float x3, float y3) {
  addTriVertices(x1, y1, x2, y2, x3, y3);
  pAddTriIndices();
}

void multishape_2d::circlePoints(float x, float y, float rad, uint32_t sectors) {
  ellipsePoints(x, y, rad, rad, sectors);
}

void multishape_2d::ellipsePoints(float x, float y, float xRad, float yRad,
                                 uint32_t sectors) {
  float angleInc = 360 / sectors;
  uint32_t toAdd = addSector(x, y, xRad, yRad, 0, 360 - angleInc, angleInc);
  pAddIndices(toAdd);
}

void multishape_2d::drawPoint(float x, float y) {
  uint32_t ind = getPointIndex();
  addPoint(x, y);
  pointIndices.push_back(ind);
}

void multishape_2d::bezierPoints(std::vector<glm::vec2> points, float inc) {
  pAddIndices(addBezierVertices(points, inc));
}

void multishape_2d::quadraticBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc) {
  pAddIndices(addQuadBezierVertices(p1x, p1y, p2x, p2y, p3x, p3y, inc));
}

void multishape_2d::cubicBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc) {
  pAddIndices(addCubicBezierVertices(p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y, inc));
}

void multishape_2d::cubicSplinePoints(std::vector<glm::vec2> points, float inc) {
  pAddIndices(addCubicSplineVertices(points, inc));
}

void multishape_2d::add_draw_point(float x, float y) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_POINT, {x}, {y}, {}, {}, {}));
  drawPoint(x, y);
}
void multishape_2d::add_fill_rectangle(float x, float y, float w, float h) {
  instructions.push_back(shape::full_instruction(instruction::FILL_RECTANGLE, {x}, {y}, {}, {w, h}, {}));
  fillRectangle(x, y, w, h);
}
void multishape_2d::add_fill_square(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_SQUARE, {x}, {y}, {}, {size}, {}));
  fillSquare(x, y, size);
}
void multishape_2d::add_fill_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::FILL_ROUND_RECT, {x}, {y}, {}, {w, h, rx, ry}, {sectors}));
  fillRoundRect(x, y, w, h, rx, ry, sectors);
}
void multishape_2d::add_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
  instructions.push_back(shape::full_instruction(instruction::FILL_TRIANGLE, {x1, x2, x3}, {y1, y2, y3}, {}, {}, {}));
  fillTriangle(x1, y1, x2, y2, x3, y3);
}
void multishape_2d::add_fill_circle(float x, float y, float rad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::FILL_CIRCLE, {x}, {y}, {}, {rad}, {sectors}));
  fillCircle(x, y, rad, sectors);
}
void multishape_2d::add_fill_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::FILL_ELLIPSE, {x}, {y}, {}, {xRad, yRad}, {sectors}));
  fillEllipse(x, y, xRad, yRad, sectors);
}
void multishape_2d::add_fill_polygon(std::vector<glm::vec2> points) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::FILL_POLYGON, x_coords, y_coords, {}, {}, {(uint32_t) points.size()}));
  fillPolygon(points);
}
void multishape_2d::add_fill_circle_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_CIRCLE_MARKER, {x}, {y}, {}, {size}, {}));
  fillCircleMarker(x, y, size);
}
void multishape_2d::add_fill_triangle_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_TRIANGLE_MARKER, {x}, {y}, {}, {size}, {}));
  fillTriangleMarker(x, y, size);
}
void multishape_2d::add_fill_square_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_SQUARE_MARKER, {x}, {y}, {}, {size}, {}));
  fillSquareMarker(x, y, size);
}
void multishape_2d::add_fill_hexagon_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_HEXAGON_MARKER, {x}, {y}, {}, {size}, {}));
  fillHexagonMarker(x, y, size);
}

void multishape_2d::add_draw_line(float x1, float y1, float x2, float y2) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_LINE, {x1, x2}, {y1, y2}, {}, {}, {}));
  drawLine(x1, y1, x2, y2);
}

void multishape_2d::add_draw_rectangle(float x, float y, float w, float h) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_RECTANGLE, {x}, {y}, {}, {w, h}, {}));
  drawRectangle(x, y, w, h);
}
void multishape_2d::add_draw_square(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_SQUARE, {x}, {y}, {}, {size}, {}));
  drawSquare(x, y, size);
}
void multishape_2d::add_draw_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_ROUND_RECT, {x}, {y}, {}, {w, h, rx, ry}, {sectors}));
  drawRoundRect(x, y, w, h, rx, ry, sectors);
}
void multishape_2d::add_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_TRIANGLE, {x1, x2, x3}, {y1, y2, y3}, {}, {}, {}));
  drawTriangle(x1, y1, x2, y2, x3, y3);
}
void multishape_2d::add_draw_circle(float x, float y, float rad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CIRCLE, {x}, {y}, {}, {rad}, {sectors}));
  drawCircle(x, y, rad, sectors);
}
void multishape_2d::add_draw_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_ELLIPSE, {x}, {y}, {}, {xRad, yRad}, {sectors}));
  drawEllipse(x, y, xRad, yRad, sectors);
}

void multishape_2d::add_rectangle_points(float x, float y, float w, float h) {
  instructions.push_back(shape::full_instruction(instruction::RECTANGLE_POINTS, {x}, {y}, {}, {w, h}, {}));
  rectanglePoints(x, y, w, h);
}
void multishape_2d::add_square_points(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::SQUARE_POINTS, {x}, {y}, {}, {size}, {}));
  squarePoints(x, y, size);
}
void multishape_2d::add_round_rect_points(float x, float y, float w, float h, float rx, float ry, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::ROUND_RECT_POINTS, {x}, {y}, {}, {w, h, rx, ry}, {sectors}));
  roundRectPoints(x, y, w, h, rx, ry, sectors);
}
void multishape_2d::add_triangle_points(float x1, float y1, float x2, float y2, float x3, float y3) {
  instructions.push_back(shape::full_instruction(instruction::TRIANGLE_POINTS, {x1, x2, x3}, {y1, y2, y3}, {}, {}, {}));
  trianglePoints(x1, y1, x2, y2, x3, y3);
}
void multishape_2d::add_circle_points(float x, float y, float rad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::CIRCLE_POINTS, {x}, {y}, {}, {rad}, {sectors}));
  circlePoints(x, y, rad, sectors);
}
void multishape_2d::add_ellipse_points(float x, float y, float xRad, float yRad, uint32_t sectors) {
  instructions.push_back(shape::full_instruction(instruction::ELLIPSE_POINTS, {x}, {y}, {}, {xRad, yRad}, {sectors}));
  ellipsePoints(x, y, xRad, yRad, sectors);
}

void multishape_2d::add_draw_polygon(std::vector<glm::vec2> points) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_POLYGON, x_coords, y_coords, {}, {}, {(uint32_t) points.size()}));
  drawPolygon(points);
}
void multishape_2d::add_draw_PCHIP(std::vector<glm::vec2> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_PCHIP, x_coords, y_coords, {}, {inc}, {(uint32_t) points.size()}));
  drawPCHIP(points, inc);
}
void multishape_2d::add_draw_cubic_spline(std::vector<glm::vec2> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_CUBIC_SPLINE, x_coords, y_coords, {}, {inc}, {(uint32_t) points.size()}));
  drawCubicSpline(points, inc);
}
void multishape_2d::add_cubic_spline_points(std::vector<glm::vec2> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::CUBIC_SPLINE_POINTS, x_coords, y_coords, {}, {inc}, {(uint32_t) points.size()}));
  cubicSplinePoints(points, inc);
}
void multishape_2d::add_draw_bezier(std::vector<glm::vec2> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_BEZIER, x_coords, y_coords, {}, {inc}, {(uint32_t) points.size()}));
  drawBezier(points, inc);
}
void multishape_2d::add_bezier_points(std::vector<glm::vec2> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::BEZIER_POINTS, x_coords, y_coords, {}, {inc}, {(uint32_t) points.size()}));
  bezierPoints(points, inc);
}
void multishape_2d::add_draw_grid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_GRID, {x0}, {y0}, {}, {w, h}, {numCols, numRows}));
  drawGrid(x0, y0, w, h, numCols, numRows);
}
void multishape_2d::add_draw_polyline(std::vector<glm::vec2> points) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_POLYLINE, x_coords, y_coords, {}, {}, {(uint32_t) points.size()}));
  drawPolyline(points);
}
void multishape_2d::add_draw_polyline_marker(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_POLYLINE_MARKER, x_coords, y_coords, {}, {markerSize}, {(uint32_t) points.size(), to_underlying(marker)}));
  drawPolylineMarker(points, marker, markerSize);
}
void multishape_2d::add_draw_polygon_marker(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_POLYGON_MARKER, x_coords, y_coords, {}, {markerSize}, {(uint32_t) points.size(), to_underlying(marker)}));
  drawPolygonMarker(points, marker, markerSize);
}
void multishape_2d::add_draw_cubic_spline_marker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_CUBIC_SPLINE_MARKER, x_coords, y_coords, {}, {markerSize, inc}, {(uint32_t) points.size(), to_underlying(marker)}));
  drawCubicSplineMarker(points, inc, marker, markerSize);
}
void multishape_2d::add_draw_circle_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CIRCLE_MARKER, {x}, {y}, {}, {size}, {}));
  drawCircleMarker(x, y, size);
}
void multishape_2d::add_draw_quadratic_bezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_QUADRATIC_BEZIER, {p1x, p2x, p3x}, {p1y, p2y, p3y}, {}, {inc}, {}));
  drawQuadraticBezier(p1x, p1y, p2x, p2y, p3x, p3y, inc);
}
void multishape_2d::add_draw_cubic_bezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CUBIC_BEZIER, {p1x, p2x, p3x, p4x}, {p1y, p2y, p3y, p4y}, {}, {inc}, {}));
  drawCubicBezier(p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y, inc);
}
void multishape_2d::add_quadratic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc) {
  instructions.push_back(shape::full_instruction(instruction::QUADRATIC_BEZIER_POINTS, {p1x, p2x, p3x}, {p1y, p2y, p3y}, {}, {inc}, {}));
  quadraticBezierPoints(p1x, p1y, p2x, p2y, p3x, p3y, inc);
}
void multishape_2d::add_cubic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc) {
  instructions.push_back(shape::full_instruction(instruction::CUBIC_BEZIER_POINTS, {p1x, p2x, p3x, p4x}, {p1y, p2y, p3y, p4y}, {}, {inc}, {}));
  cubicBezierPoints(p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y, inc);
}
void multishape_2d::add_draw_cross_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CROSS_MARKER, {x}, {y}, {}, {size}, {}));
  drawCrossMarker(x, y, size);
}
void multishape_2d::add_draw_markers(std::vector<glm::vec2> points, Marker marker, float markerSize) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_MARKERS, x_coords, y_coords, {}, {markerSize}, {(uint32_t) points.size(), to_underlying(marker)}));
  drawMarkers(points, marker, markerSize);
}
void multishape_2d::add_draw_triangle_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_TRIANGLE_MARKER, {x}, {y}, {}, {size}, {}));
  drawTriangleMarker(x, y, size);
}
void multishape_2d::add_draw_square_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_SQUARE_MARKER, {x}, {y}, {}, {size}, {}));
  drawSquareMarker(x, y, size);
}
void multishape_2d::add_draw_hexagon_marker(float x, float y, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_HEXAGON_MARKER, {x}, {y}, {}, {size}, {}));
  drawHexagonMarker(x, y, size);
}

const std::unordered_map<instruction, std::function<void(multishape::dispatch_inputs)>> multishape_2d::dispatch_table = std::unordered_map<instruction, std::function<void(dispatch_inputs)>> {
  {instruction::DRAW_POINT, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawPoint(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]);
    i.current_x_coord++;
    i.current_y_coord++;
  }},  
  {instruction::FILL_RECTANGLE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillRectangle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
  }},
  {instruction::FILL_SQUARE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillSquare(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::FILL_ROUND_RECT, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillRoundRect(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 4;
    i.current_uint32++;
  }},
  {instruction::FILL_TRIANGLE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillTriangle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2]);
    i.current_x_coord+=3;
    i.current_y_coord+=3;
  }},
  {instruction::FILL_CIRCLE, [](dispatch_inputs i) {
    ((multishape_2d*)i.shape)->fillCircle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
    i.current_uint32++;
  }},
  {instruction::FILL_ELLIPSE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillEllipse(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
    i.current_uint32++;
  }},
  {instruction::FILL_POLYGON, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->fillPolygon(points);
  }},
  {instruction::FILL_CIRCLE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillCircleMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::FILL_TRIANGLE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillTriangleMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::FILL_SQUARE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillSquareMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::FILL_HEXAGON_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->fillHexagonMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_RECTANGLE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawRectangle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
  }},
  {instruction::DRAW_SQUARE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawSquare(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_ROUND_RECT, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawRoundRect(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 4;
    i.current_uint32++;
  }},
  {instruction::DRAW_TRIANGLE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawTriangle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2]);
    i.current_x_coord+=3;
    i.current_y_coord+=3;
  }},
  {instruction::DRAW_CIRCLE, [](dispatch_inputs i) {
    ((multishape_2d*)i.shape)->drawCircle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
    i.current_uint32++;
  }},
  {instruction::DRAW_ELLIPSE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawEllipse(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
    i.current_uint32++;
  }},
  {instruction::DRAW_LINE, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawLine(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1]);
    i.current_x_coord += 2;
    i.current_y_coord += 2;
  }},
  {instruction::DRAW_POLYLINE, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawPolyline(points);
  }},
  {instruction::DRAW_POLYLINE_MARKER, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    Marker marker = static_cast<Marker>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawPolylineMarker(points, marker, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_POLYGON, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawPolygon(points);
  }},
  {instruction::DRAW_POLYGON_MARKER, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    Marker marker = static_cast<Marker>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawPolygonMarker(points, marker, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_GRID, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawGrid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32+1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_CIRCLE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawCircleMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_TRIANGLE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawTriangleMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_SQUARE_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawSquareMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_HEXAGON_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawHexagonMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_CROSS_MARKER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawCrossMarker(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_MARKERS, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    Marker marker = static_cast<Marker>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawMarkers(points, marker, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_BEZIER, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawBezier(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_QUADRATIC_BEZIER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawQuadraticBezier(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2], i.floats[i.current_float]);
    i.current_x_coord += 3;
    i.current_y_coord += 3;
    i.current_float++;
  }},
  {instruction::DRAW_CUBIC_BEZIER, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->drawCubicBezier(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2], i.x_coords[i.current_x_coord + 3], i.y_coords[i.current_y_coord + 3], i.floats[i.current_float]);
    i.current_x_coord += 4;
    i.current_y_coord += 4;
    i.current_float++;
  }},
  {instruction::DRAW_CUBIC_SPLINE, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawCubicSpline(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_CUBIC_SPLINE_MARKER, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    Marker marker = static_cast<Marker>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawCubicSplineMarker(points, i.floats[i.current_float], marker, i.floats[i.current_float + 1]);
    i.current_float += 2;
  }},
  {instruction::DRAW_PCHIP, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->drawPCHIP(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::RECTANGLE_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->rectanglePoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
  }},
  {instruction::SQUARE_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->squarePoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
  }},
  {instruction::ROUND_RECT_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->roundRectPoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 4;
    i.current_uint32++;
  }},
  {instruction::TRIANGLE_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->trianglePoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2]);
    i.current_x_coord+=3;
    i.current_y_coord+=3;
  }},
  {instruction::CIRCLE_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*)i.shape)->circlePoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float++;
    i.current_uint32++;
  }},
  {instruction::ELLIPSE_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->ellipsePoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_float += 2;
    i.current_uint32++;
  }},
  {instruction::BEZIER_POINTS, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->bezierPoints(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::QUADRATIC_BEZIER_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->quadraticBezierPoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2], i.floats[i.current_float]);
    i.current_x_coord += 3;
    i.current_y_coord += 3;
    i.current_float++;
  }},
  {instruction::CUBIC_BEZIER_POINTS, [](dispatch_inputs i) {
    ((multishape_2d*) i.shape)->cubicBezierPoints(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.x_coords[i.current_x_coord + 2], i.y_coords[i.current_y_coord + 2], i.x_coords[i.current_x_coord + 3], i.y_coords[i.current_y_coord + 3], i.floats[i.current_float]);
    i.current_x_coord += 4;
    i.current_y_coord += 4;
    i.current_float++;
  }},
  {instruction::CUBIC_SPLINE_POINTS, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec2> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
    }
    ((multishape_2d*) i.shape)->cubicSplinePoints(points, i.floats[i.current_float]);
    i.current_float++;
  }}
};
