#pragma once

#include "multishape.hpp"

enum Marker {
  DRAW_CIRCLE,
  DRAW_TRIANGLE,
  DRAW_SQUARE,
  DRAW_HEXAGON,
  DRAW_CROSS,
  FILL_CIRCLE,
  FILL_TRIANGLE,
  FILL_SQUARE,
  FILL_HEXAGON
};

class multishape_2d : public multishape { 
 protected:
  uint32_t color_index;
  float alpha;
  // Utility
  void addRectVertices(float x, float y, float w, float h);
  void addTriVertices(float x1, float y1, float x2, float y2, float x3, float y3);
  uint32_t addBezierVertices(std::vector<glm::vec2> points, float inc);
  uint32_t addQuadBezierVertices(float p1x, float p1y, float p2x, float p2y,
                             float p3x, float p3y, float inc);
  uint32_t addCubicBezierVertices(float p1x, float p1y, float p2x, float p2y, float p3x,
                         float p3y, float p4x, float p4y, float inc);
  uint32_t addCubicSplineVertices(std::vector<glm::vec2> points, float inc);

  void sAddTriIndices();
  void sAddQuadIndices();

  void lAddTriIndices();
  void lAddQuadIndices();

  void pAddTriIndices();
  void pAddQuadIndices();
  void pAddIndices(uint32_t indexCount);

  uint32_t addSector(float x, float y, float xRad, float yRad, float fromAngle,
                     float toAngle, float angleInc);

  // Solid Primitives
  void fillRectangle(float x, float y, float w, float h);
  void fillSquare(float x, float y, float size);
  void fillRoundRect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
  void fillCircle(float x, float y, float rad, uint32_t sectors);
  void fillEllipse(float x, float y, float xRad, float yRad, uint32_t sectors);
  void fillPolygon(std::vector<glm::vec2> points);
  void fillCircleMarker(float x, float y, float size);
  void fillTriangleMarker(float x, float y, float size);
  void fillSquareMarker(float x, float y, float size);
  void fillHexagonMarker(float x, float y, float size);

  // Line Primitives
  void drawRectangle(float x, float y, float w, float h);
  void drawSquare(float x, float y, float size);
  void drawRoundRect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
  void drawCircle(float x, float y, float rad, uint32_t sectors);
  void drawEllipse(float x, float y, float xRad, float yRad, uint32_t sectors);
  void drawLine(float x1, float y1, float x2, float y2);
  void drawPolyline(std::vector<glm::vec2> points);
  void drawPolylineMarker(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void drawPolygon(std::vector<glm::vec2> points);
  void drawPolygonMarker(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void drawGrid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows);
  void drawCircleMarker(float x, float y, float size);
  void drawTriangleMarker(float x, float y, float size);
  void drawSquareMarker(float x, float y, float size);
  void drawHexagonMarker(float x, float y, float size);
  void drawCrossMarker(float x, float y, float size);
  void drawMarkers(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void drawBezier(std::vector<glm::vec2> points, float inc);
  void drawQuadraticBezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc);
  void drawCubicBezier(float p1x, float p1y, float p2x, float p2y,
                       float p3x, float p3y, float p4x, float p4y,
                       float inc);
  void drawCubicSpline(std::vector<glm::vec2> points, float inc);
  void drawCubicSplineMarker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize);
  void drawPCHIP(std::vector<glm::vec2> points, float inc);

  // Point Primitives
  void rectanglePoints(float x, float y, float w, float h);
  void squarePoints(float x, float y, float size);
  void roundRectPoints(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void trianglePoints(float x1, float y1, float x2, float y2, float x3,
                      float y3);
  void circlePoints(float x, float y, float rad, uint32_t sectors);
  void ellipsePoints(float x, float y, float xRad, float yRad, uint32_t sectors);
  void bezierPoints(std::vector<glm::vec2> points, float inc);
  void quadraticBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc);
  void cubicBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc);
  void cubicSplinePoints(std::vector<glm::vec2> points, float inc);

 public:
  void init() override;
  void drawPoint(float x, float y);
  multishape_2d(float line_width, uint32_t color_index, uint32_t transform_index, float alpha = 1.f, shader_id sid = shader_id::FIXED_COLOR, uint32_t vertCount = 1024,
               uint32_t solidIndCount = 1024, uint32_t lineIndCount = 1024,
               uint32_t pointIndCount = 1024, uint32_t elemPerVert = 2)
      : alpha(alpha), color_index(color_index), multishape(line_width, transform_index, sid, vertCount, solidIndCount, lineIndCount,
                   pointIndCount, elemPerVert) {
                    set_start_instr(shape::full_instruction(instruction::START_MULTISHAPE_2D, {}, {}, {}, {line_width, alpha}, {color_index, transform_index, (uint32_t) to_underlying(sid)}));
                    end_instr = instruction::END_MULTISHAPE_2D;
  }
  ~multishape_2d();

  std::unordered_map<instruction, std::function<void(dispatch_inputs)>> get_dispatch_table() override {
    return dispatch_table;
  };

  void render(drw_file& drw, view& view) override;

  void add_draw_point(float x, float y);
  void add_fill_rectangle(float x, float y, float w, float h);
  void add_fill_square(float x, float y, float size);
  void add_fill_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void add_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
  void add_fill_circle(float x, float y, float rad, uint32_t sectors);
  void add_fill_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors);
  void add_fill_polygon(std::vector<glm::vec2> points);
  void add_fill_circle_marker(float x, float y, float size);
  void add_fill_triangle_marker(float x, float y, float size);
  void add_fill_square_marker(float x, float y, float size);
  void add_fill_hexagon_marker(float x, float y, float size);

  void add_draw_rectangle(float x, float y, float w, float h);
  void add_draw_square(float x, float y, float size);
  void add_draw_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void add_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
  void add_draw_circle(float x, float y, float rad, uint32_t sectors);
  void add_draw_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors);
  void add_draw_line(float x1, float y1, float x2, float y2);
  void add_draw_polyline(std::vector<glm::vec2> points);
  void add_draw_polyline_marker(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void add_draw_polygon(std::vector<glm::vec2> points);
  void add_draw_polygon_marker(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void add_draw_grid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows);
  void add_draw_circle_marker(float x, float y, float size);
  void add_draw_triangle_marker(float x, float y, float size);
  void add_draw_square_marker(float x, float y, float size);
  void add_draw_hexagon_marker(float x, float y, float size);
  void add_draw_cross_marker(float x, float y, float size);
  void add_draw_markers(std::vector<glm::vec2> points, Marker marker, float markerSize);
  void add_draw_bezier(std::vector<glm::vec2> points, float inc);
  void add_draw_quadratic_bezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc);
  void add_draw_cubic_bezier(float p1x, float p1y, float p2x, float p2y,
                       float p3x, float p3y, float p4x, float p4y,
                       float inc);
  void add_draw_cubic_spline(std::vector<glm::vec2> points, float inc);
  void add_draw_cubic_spline_marker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize);
  void add_draw_PCHIP(std::vector<glm::vec2> points, float inc);

  void add_rectangle_points(float x, float y, float w, float h);
  void add_square_points(float x, float y, float size);
  void add_round_rect_points(float x, float y, float w, float h, float rx, float ry, uint32_t sectors);
  void add_triangle_points(float x1, float y1, float x2, float y2, float x3, float y3);
  void add_circle_points(float x, float y, float rad, uint32_t sectors);
  void add_ellipse_points(float x, float y, float xRad, float yRad, uint32_t sectors);
  void add_bezier_points(std::vector<glm::vec2> points, float inc);
  void add_quadratic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc);
  void add_cubic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc);
  void add_cubic_spline_points(std::vector<glm::vec2> points, float inc);

  private:
  static const std::unordered_map<instruction, std::function<void(dispatch_inputs)>> dispatch_table;
};
