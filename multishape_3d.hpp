#pragma once

#include "multishape.hpp"
#include "camera.hpp"
#include "shape_2d.hpp"

class multishape_3d : public multishape {
 protected:
  uint32_t camera_index;
  uint32_t color_index;
  float alpha;

  void add3DPoint(float x, float y, float z) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    for (int i = 3; i < elemPerVert; i++) {
      vertices.push_back(0);
    }   
  }

  // Utility
  uint32_t addSector(float xc, float yc, float zc, float rad, float fromAngle, float toAngle, float angleInc);
  void addRectPrismVertices(float x, float y, float z, float width, float height, float length);
  void addWedgeVertices(float x, float y, float z, float width, float height, float length);
  void addRectPyramidVertices(float x, float y, float z, float width, float height, float length);
  void addEllipsoidVertices(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks);
  void sAddRectFaceIndices(uint32_t bottomLeft, uint32_t bottomRight, uint32_t topLeft, uint32_t topRight, uint32_t offset);    
  void sAddTriFaceIndices(uint32_t vertex1, uint32_t vertex2, uint32_t vertex3, uint32_t offset);
  void lAddLineIndices(uint32_t i1, uint32_t i2, uint32_t offset);
  void addTorusVertices(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  uint32_t addCubicSplineVertices(std::vector<glm::vec3> points, float inc);
  uint32_t addLinearExtrusionVertices(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  uint32_t addSplineExtrusionVertices(shape_2d shape, std::vector<glm::vec3> points, float inc);
  void lAddExtrusionIndices(shape_2d shape, float stacks, uint32_t global_offset);
  void sAddExtrusionIndices(shape_2d shape, float stacks, uint32_t global_offset);
  uint32_t drawCircle(float x, float y, float z, float rad, float angleInc);
  uint32_t fillCircle(float x, float y, float z, float rad, float angleInc);

  static shape_2d get_shape_2d(shape_2d::type shape_type, dispatch_inputs i);

  void drawPoint(float x, float y, float z);
  void drawLine(float x1, float y1, float z1, float x2, float y2, float z2);
  // Solid Primitives
  void fillRectPrism(float x, float y, float z, float width, float height, float length);
  void fillCube(float x, float y, float z, float size);
  void fillRectPyramid(float x, float y, float z, float width, float height, float length);
  void fillWedge(float x, float y, float z, float width, float height, float length);
  void fillCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void fillCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void fillFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks);
  void fillFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks);
  void fillTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks);
  void fillObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks);
  void fillObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks);
  void fillObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks);
  void fillTorus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  void fillEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks); 
  void fillSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void fillLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void fillSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);

  // Wire Frame Primitives
  void drawRectPrism(float x, float y, float z, float width, float height, float length);
  void drawCube(float x, float y, float z, float size);
  void drawRectPyramid(float x, float y, float z, float width, float height, float length);
  void drawWedge(float x, float y, float z, float width, float height, float length);
  void drawCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void drawCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void drawFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks);
  void drawFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks);
  void drawTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks);
  void drawEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks);
  void drawSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void drawObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks);
  void drawObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks);
  void drawObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks);
  void drawTorus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  void drawLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void drawSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);
  void drawCubicSpline(std::vector<glm::vec3> points, float inc);

 public:
  void init() override;
  
  void add_draw_point(float x, float y, float z);
  void add_draw_line(float x1, float y1, float z1, float x2, float y2, float z2);
  void add_fill_rect_prism(float x, float y, float z, float width, float height, float length);
  void add_fill_cube(float x, float y, float z, float size);
  void add_fill_rect_pyramid(float x, float y, float z, float width, float height, float length);
  void add_fill_wedge(float x, float y, float z, float width, float height, float length);
  void add_fill_cylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_cone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_frustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_frustum_minus_frustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_tube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_oblique_frustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks);
  void add_fill_oblique_cylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks);
  void add_fill_oblique_cone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks);
  void add_fill_torus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  void add_fill_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks); 
  void add_fill_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void add_fill_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void add_fill_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);
  void add_draw_rect_prism(float x, float y, float z, float width, float height, float length);
  void add_draw_cube(float x, float y, float z, float size);
  void add_draw_rect_pyramid(float x, float y, float z, float width, float height, float length);
  void add_draw_wedge(float x, float y, float z, float width, float height, float length);
  void add_draw_cylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void add_draw_cone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void add_draw_frustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks);
  void add_draw_frustum_minus_frustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks);
  void add_draw_tube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks);
  void add_draw_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks);
  void add_draw_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void add_draw_oblique_frustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks);
  void add_draw_oblique_cylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks);
  void add_draw_oblique_cone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks);
  void add_draw_torus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  void add_draw_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void add_draw_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);
  void add_draw_cubic_spline(std::vector<glm::vec3> points, float inc);

  multishape_3d(float line_width, uint32_t color_index, uint32_t camera_index, uint32_t transform_index, float alpha = 1.f, shader_id sid = shader_id::FIXED_COLOR_CAMERA, uint32_t vertCount = 1024,
               uint32_t solidIndCount = 1024, uint32_t lineIndCount = 1024,
               uint32_t pointIndCount = 1024, uint32_t elemPerVert = 3)
      : alpha(alpha), color_index(color_index), camera_index(camera_index), multishape(line_width, transform_index, sid, vertCount, solidIndCount, lineIndCount, pointIndCount, elemPerVert) {
        set_start_instr(multishape::full_instruction(instruction::START_MULTISHAPE_3D, {}, {}, {}, {line_width, alpha}, {color_index, camera_index, transform_index, (uint32_t) to_underlying(sid)}));
        end_instr = instruction::END_MULTISHAPE_3D;
      }
  ~multishape_3d();

  std::unordered_map<instruction, std::function<void(dispatch_inputs)>> get_dispatch_table() override {
    return dispatch_table;
  };

  void render(drw_file& drw, view& view) override;


  private:
  static const std::unordered_map<instruction, std::function<void(dispatch_inputs)>> dispatch_table;
};
