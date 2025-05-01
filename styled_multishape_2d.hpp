#pragma once

#include "multishape_2d.hpp"

class styled_multishape_2d : public multishape_2d {
 private:
  drw_file& drw;
 protected:
  void drawPoint(float x, float y, const glm::vec4& c);
  void drawPoint(float x, float y, uint32_t color_index, float alpha);
  uint32_t addSector(float x, float y, float xRad, float yRad, float fromAngle,
                     float toAngle, float angleInc, const glm::vec4& c);
  void addRectVertices(float x, float y, float w, float h, const glm::vec4& c);
  void addTriVertices(float x1, float y1, float x2, float y2, float x3, float y3, const glm::vec4& c);
  uint32_t addBezierVertices(std::vector<glm::vec2> points, float inc, const glm::vec4& c);
  uint32_t addQuadBezierVertices(float p1x, float p1y, float p2x, float p2y,
                             float p3x, float p3y, float inc, const glm::vec4& c);
  uint32_t addCubicBezierVertices(float p1x, float p1y, float p2x, float p2y, float p3x,
                         float p3y, float p4x, float p4y, float inc, const glm::vec4& c);
  uint32_t addCubicSplineVertices(std::vector<glm::vec2> points, float inc, const glm::vec4& c);

  void fillInFrontCircle(float x, float y, float rad, uint32_t sectors, const glm::vec4& c);
  void fillInFrontEllipse(float x, float y, float xRad, float yRad, uint32_t sectors, const glm::vec4& c);
  void fillInFrontTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const glm::vec4& c);
  void fillInFrontRectangle(float x, float y, float w, float h, const glm::vec4& c);
  void sAddInFrontClosedSectorIndices(uint32_t centerIndex, uint32_t indexCount);
  void sAddInFrontTriIndices();
  void sAddInFrontQuadIndices();
  void addColor(const glm::vec4& rgba) {
    vertices.push_back(rgba.r);
    vertices.push_back(rgba.g);
    vertices.push_back(rgba.b);
    vertices.push_back(rgba.a);
  }
  void addColor(float r, float g, float b, float a) {
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
    vertices.push_back(a);
  }

  // Solid Primitives
  void fillRectangle(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void fillSquare(float x, float y, float size, uint32_t color_index, float alpha);
  void fillRoundRect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color_index, float alpha);
  void fillCircle(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void fillEllipse(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void fillPolygon(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void fillPolyline(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void fillGrid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows, uint32_t bg_color_index, float bg_alpha, uint32_t fg_color_index, float fg_alpha);
  void fillCircleMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void fillTriangleMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void fillSquareMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void fillHexagonMarker(float x, float y, float size, uint32_t color_index, float alpha);

  // Line Primitives
  void drawRectangle(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void drawSquare(float x, float y, float size, uint32_t color_index, float alpha);
  void drawRoundRect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color_index, float alpha);
  void drawCircle(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void drawEllipse(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void drawLine(float x1, float y1, float x2, float y2, uint32_t color_index, float alpha);
  void drawPolyline(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void drawPolylineMarker(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);
  void drawPolygon(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void drawPolygonMarker(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);    
  void drawGrid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows, uint32_t color_index, float alpha);
  void drawCircleMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void drawTriangleMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void drawSquareMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void drawHexagonMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void drawCrossMarker(float x, float y, float size, uint32_t color_index, float alpha);
  void drawMarkers(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t color_index, float alpha);
  void drawBezier(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void drawQuadraticBezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc, uint32_t color_index, float alpha);
  void drawCubicBezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc, uint32_t color_index, float alpha);    
  void drawCubicSpline(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void drawCubicSplineMarker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);    
  void drawPCHIP(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);

  // Point Primitives
  void rectanglePoints(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void squarePoints(float x, float y, float size, uint32_t color_index, float alpha);
  void roundRectPoints(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void trianglePoints(float x1, float y1, float x2, float y2, float x3,
                      float y3, uint32_t color_index, float alpha);
  void circlePoints(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void ellipsePoints(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void bezierPoints(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void quadraticBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc, uint32_t color_index, float alpha);
  void cubicBezierPoints(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc, uint32_t color_index, float alpha);
  void cubicSplinePoints(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  
  void addStyledPoint(float x, float y, const glm::vec4& rgba) {
    addPoint(x, y);
    addColor(rgba);
  }

 public:
  void init() override;
  styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index, shader_id sid = shader_id::PER_VERTEX, uint32_t vertCount = 1024,
               uint32_t solidIndCount = 1024, uint32_t lineIndCount = 1024,
               uint32_t pointIndCount = 1024, uint32_t elemPerVert = 6)
      : drw(drw), multishape_2d(line_width, 0, transform_index, 1.f, sid, vertCount, solidIndCount, lineIndCount,
                   pointIndCount, elemPerVert)
                   {
                    set_start_instr(multishape::full_instruction(instruction::START_STYLED_MULTISHAPE_2D, {}, {}, {}, {line_width}, {transform_index, (uint32_t) to_underlying(sid)}));
                    end_instr = instruction::END_STYLED_MULTISHAPE_2D;
                   }
  ~styled_multishape_2d();

  std::unordered_map<instruction, std::function<void(dispatch_inputs)>> get_dispatch_table() override {
    return dispatch_table;
  };

  void add_draw_point(float x, float y, uint32_t color_index, float alpha);
  void add_fill_rectangle(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void add_fill_square(float x, float y, float size, uint32_t color_index, float alpha);
  void add_fill_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void add_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color_index, float alpha);
  void add_fill_circle(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_fill_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_fill_polygon(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void add_fill_polyline(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void add_fill_grid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows, uint32_t bg_color_index, float bg_alpha, uint32_t fg_color_index, float fg_alpha);
  void add_fill_circle_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_fill_triangle_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_fill_square_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_fill_hexagon_marker(float x, float y, float size, uint32_t color_index, float alpha);

  void add_draw_rectangle(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void add_draw_square(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_round_rect(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void add_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color_index, float alpha);
  void add_draw_circle(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_draw_ellipse(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_draw_line(float x1, float y1, float x2, float y2, uint32_t color_index, float alpha);
  void add_draw_polyline(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void add_draw_polyline_marker(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);
  void add_draw_polygon(std::vector<glm::vec2> points, uint32_t color_index, float alpha);
  void add_draw_polygon_marker(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);
  void add_draw_grid(float x0, float y0, float w, float h, uint32_t numCols, uint32_t numRows, uint32_t color_index, float alpha);
  void add_draw_circle_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_triangle_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_square_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_hexagon_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_cross_marker(float x, float y, float size, uint32_t color_index, float alpha);
  void add_draw_markers(std::vector<glm::vec2> points, Marker marker, float markerSize, uint32_t color_index, float alpha);
  void add_draw_bezier(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void add_draw_quadratic_bezier(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc, uint32_t color_index, float alpha);
  void add_draw_cubic_bezier(float p1x, float p1y, float p2x, float p2y,
                       float p3x, float p3y, float p4x, float p4y,
                       float inc, uint32_t color_index, float alpha);
  void add_draw_cubic_spline(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void add_draw_cubic_spline_marker(std::vector<glm::vec2> points, float inc, Marker marker, float markerSize, uint32_t line_color_index, float line_alpha, uint32_t marker_color_index, float marker_alpha);
  void add_draw_PCHIP(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);

  void add_rectangle_points(float x, float y, float w, float h, uint32_t color_index, float alpha);
  void add_square_points(float x, float y, float size, uint32_t color_index, float alpha);
  void add_round_rect_points(float x, float y, float w, float h, float rx, float ry, uint32_t sectors, uint32_t color_index, float alpha);
  void add_triangle_points(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color_index, float alpha);
  void add_circle_points(float x, float y, float rad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_ellipse_points(float x, float y, float xRad, float yRad, uint32_t sectors, uint32_t color_index, float alpha);
  void add_bezier_points(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);
  void add_quadratic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float inc, uint32_t color_index, float alpha);
  void add_cubic_bezier_points(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y, float inc, uint32_t color_index, float alpha);
  void add_cubic_spline_points(std::vector<glm::vec2> points, float inc, uint32_t color_index, float alpha);

  private:
  static const std::unordered_map<instruction, std::function<void(dispatch_inputs)>> dispatch_table;
};
