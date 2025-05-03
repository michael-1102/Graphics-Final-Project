#include "multishape_3d.hpp"
#include "lighting.hpp"
#include <array>

class lit_multishape_3d : public multishape_3d {
  protected:
   uint32_t material_index;
   //TODO: can only have up to 100 of any type of light, so we don't need 32 bits to store num_x_lights
   uint32_t num_point_lights;
   uint32_t num_spot_lights;
   uint32_t num_dir_lights;

   std::vector<uint32_t> point_light_indices;
   std::vector<uint32_t> spot_light_indices;
   std::vector<uint32_t> dir_light_indices;

   std::array<point_light, 100> point_lights; 
   std::array<spot_light, 100> spot_lights;
   std::array<dir_light, 100> dir_lights;

   // Utility
   void sAddRectFace(float topLeftX, float topLeftY, float topLeftZ,
                     float topRightX, float topRightY, float topRightZ,
                     float bottomLeftX, float bottomLeftY, float bottomLeftZ,
                     float bottomRightX, float bottomRightY, float bottomRightZ,
                     float normalX, float normalY, float normalZ);
   void sAddRectFace(float topLeftX, float topLeftY, float topLeftZ,
                     float topRightX, float topRightY, float topRightZ,
                     float bottomLeftX, float bottomLeftY, float bottomLeftZ,
                     float bottomRightX, float bottomRightY,
                     float bottomRightZ);
   void sAddExtrusionIndices(shape_2d shape, float stacks,
                             uint32_t total_num_vertices);

   void addSolidPoint(float x, float y, float z) {
     add3DPoint(x, y, z, 0.f, 0.f, 0.f);
     solidIndices.push_back(getPointIndex() - 1);
   }
    void addSolidPoint(float x, float y, float z, float xn, float yn, float zn) {
      add3DPoint(x, y, z, xn, yn, zn);
      solidIndices.push_back(getPointIndex() - 1);
    }
    void add3DPoint(float x, float y, float z) {
      add3DPoint(x, y, z, 0.f, 0.f, 0.f);
    }
    void add3DPoint(float x, float y, float z, float xn, float yn, float zn) {
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
      vertices.push_back(xn);
      vertices.push_back(yn);
      vertices.push_back(zn);
    }
    uint32_t addSector(float xc, float yc, float zc, float xn, float yn, float zn, float rad, float fromAngle, float toAngle, float angleInc);
    uint32_t addOuterSector(float x, float y, float z, float yn, float rad, float fromAngle, float toAngle, float angleInc);
    uint32_t addInnerSector(float xc, float yc, float zc, float yn, float rad, float fromAngle, float toAngle, float angleInc);
    uint32_t fillCircle(float x, float y, float z, float xn, float yn, float zn, float rad, float angleInc);
    void addEllipsoidVertices(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks);
    uint32_t addLinearExtrusionVertices(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
    uint32_t addSplineExtrusionVertices(shape_2d shape, std::vector<glm::vec3> points, float inc);

    void addTriNormals(uint32_t numSurfaces);
    void addRectNormals(uint32_t numSurfaces);
    void normalize(uint32_t numIndices);
    void drawNormal(float x, float y, float z, float xn, float yn, float zn) {
      drawLine(x, y, z, x + xn/4, y + yn/4, z + zn/4);
    }

    // Solid Primitives
  void fillRectPrism(float x, float y, float z, float width, float height, float length);
  void fillCube(float x, float y, float z, float size);
  void fillRectPyramid(float x, float y, float z, float width, float height, float length);
  void fillWedge(float x, float y, float z, float width, float height, float length);
  void fillEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks);
  void fillSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void fillTorus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks);
  void fillHelix(float x, float y, float z, float R, float r, float ang, float height, uint32_t sectors, uint32_t stacks);
  void fillFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks);
  void fillObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks);
  void fillObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks);
  void fillCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void fillCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks);
  void fillObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks);
  void fillTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks);
  void fillFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks);
  void fillLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void fillSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);

   public:
    lit_multishape_3d(float line_width, uint32_t camera_index, uint32_t material_index, std::vector<uint32_t> point_light_indices, std::vector<uint32_t> spot_light_indices, std::vector<uint32_t> dir_light_indices, uint32_t transform_index, shader_id sid = shader_id::MATERIAL_MULTILIGHT, uint32_t vertCount = 1024,
      uint32_t solidIndCount = 1024, uint32_t lineIndCount = 1024,
      uint32_t pointIndCount = 1024, uint32_t elemPerVert = 6)
      : material_index(material_index), point_light_indices(point_light_indices), spot_light_indices(spot_light_indices), dir_light_indices(dir_light_indices), multishape_3d(line_width, 0, camera_index, transform_index, 1.f, sid, vertCount, solidIndCount, lineIndCount,
        pointIndCount, elemPerVert) {
          num_point_lights = point_light_indices.size();
          num_spot_lights = spot_light_indices.size();
          num_dir_lights = dir_light_indices.size();

          std::vector<uint32_t> indices = {num_point_lights, num_spot_lights, num_dir_lights};
          for (uint32_t i = 0; i < num_point_lights; i++) {
            indices.push_back(point_light_indices[i]);
          }
          for (uint32_t i = 0; i < num_spot_lights; i++) {
            indices.push_back(spot_light_indices[i]);
          }
          for (uint32_t i = 0; i < num_dir_lights; i++) {
            indices.push_back(dir_light_indices[i]);
          }
          indices.push_back(camera_index);
          indices.push_back(material_index);
          indices.push_back(transform_index);
          indices.push_back(to_underlying<shader_id>(sid));
          set_start_instr(multishape::full_instruction(instruction::START_LIT_MULTISHAPE_3D, {}, {}, {}, {line_width}, indices));
          end_instr = instruction::END_LIT_MULTISHAPE_3D;
    }
    ~lit_multishape_3d();

    std::unordered_map<instruction, std::function<void(dispatch_inputs)>> get_dispatch_table() override {
      return dispatch_table;
    };

    void init() override;
    void render(drw_file& drw, view& view) override;

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
  void add_fill_helix(float x, float y, float z, float R, float r, float ang, float height, uint32_t sectors, uint32_t stacks);
  void add_fill_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks); 
  void add_fill_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks);
  void add_fill_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks);
  void add_fill_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc);
  
  void drawNormals();

  private:
  static const std::unordered_map<instruction, std::function<void(dispatch_inputs)>> dispatch_table;
};