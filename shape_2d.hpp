#pragma once

#include <vector>
#include <glm/glm.hpp>

// 2D shape that is extruded into 3D shape
// position of shape is determined by extrusion

class shape_2d {
  public:
    enum type {
      LINE,
      POLYLINE,
      POLYGON,
      RECTANGLE,
      SQUARE,
      TRIANGLE,
      ELLIPSE,
      CIRCLE,
      ROUND_RECTANGLE,
    };
    uint32_t get_num_vertices() { return vertices.size(); }
    uint32_t get_num_pipe_indices() { return pipe_indices.size(); }
    uint32_t get_num_solid_end_indices() { return solid_end_indices.size(); }
    uint32_t get_num_line_end_indices() { return line_end_indices.size(); }
    type get_id() { return id; }

    glm::vec2 get_vertex(uint32_t i) { return vertices[i]; }
    uint32_t get_pipe_index(uint32_t i) { return pipe_indices[i]; }
    uint32_t get_solid_end_index(uint32_t i) { return solid_end_indices[i]; }
    uint32_t get_line_end_index(uint32_t i) { return line_end_indices[i]; }

    std::vector<float> get_x_coords() { return x_coords; }
    std::vector<float> get_y_coords() { return y_coords; }
    std::vector<float> get_floats() { return floats; }
    std::vector<uint32_t> get_ints() { return ints; }

   private:
    std::vector<glm::vec2> vertices;
    std::vector<uint32_t> pipe_indices; // indices that are repeated along extrusion
    std::vector<uint32_t> solid_end_indices; // indices for the ends of the extruded shape (solid)
    std::vector<uint32_t> line_end_indices; // indices for the ends of the extruded shape (wireframe)
    type id; // identifier so we can determine what shape this is


   protected:
    std::vector<float> x_coords;
    std::vector<float> y_coords;
    std::vector<float> floats;
    std::vector<uint32_t> ints;

    void set_vertices(std::vector<glm::vec2> points) { vertices = points; }
    void set_pipe_indices(std::vector<uint32_t> indices) { pipe_indices = indices; }
    void set_line_end_indices(std::vector<uint32_t> indices) { line_end_indices = indices; }
    void set_solid_end_indices(std::vector<uint32_t> indices) { solid_end_indices = indices; }
    void set_id(type i) { id = i; }

    void vertices_push(std::vector<glm::vec2> points) {
      for (auto p : points) vertices.push_back(p);
    }
    void vertices_push(glm::vec2 point) { vertices.push_back(point); }
    void vertices_push(float x, float y) { vertices.push_back(glm::vec2(x, y)); }

    void solid_end_indices_push(uint32_t ind) { solid_end_indices.push_back(ind); }
    void line_end_indices_push(uint32_t ind) { line_end_indices.push_back(ind); }
    void pipe_indices_push(uint32_t ind) { pipe_indices.push_back(ind); }

    void add_sector(float xc, float yc, float rad_x, float rad_y, float from_angle, float to_angle,
                        float angle_inc) {
      for (float i = from_angle; i <= to_angle; i += angle_inc) {
        vertices_push(float(xc + rad_x * cos(-i * M_PI/180.0)), float(yc + rad_y * sin(-i * M_PI/180.0)));
      }
    }
};

class line : public shape_2d {
  public:
    line(float x, float y) { // first point is 0, 0
      set_vertices({glm::vec2(0, 0),
                    glm::vec2(x, y)});

      set_pipe_indices({0, 1});
      set_line_end_indices({});
      set_solid_end_indices({});

      set_id(type::LINE);

      x_coords = {x};
      y_coords = {y};
    }
};

class polyline : public shape_2d {
  public:
    polyline(std::vector<glm::vec2> points) { // first point is 0, 0
      vertices_push(0, 0);
      vertices_push(points);
      for (uint32_t i = 0; i < points.size() + 1; i++) {
        pipe_indices_push(i);
        x_coords.push_back(points[i].x);
        y_coords.push_back(points[i].y);
      }

      set_line_end_indices({});
      set_solid_end_indices({});
      set_id(type::POLYLINE);
    }
    std::vector<float> get_x_coords() { return x_coords; }
    std::vector<float> get_y_coords() { return y_coords; }
    std::vector<float> get_floats() { return {}; }
    std::vector<uint32_t> get_ints() { return {}; }
};

class polygon : public polyline {
  public:
    polygon(std::vector<glm::vec2> points) : polyline(points) { // first point is 0, 0
      //TODO: solid end indices

      pipe_indices_push(0);
      set_id(type::POLYGON);
    }
};

class rectangle : public shape_2d {
  public:
    rectangle(float width, float height) { // bottom left is 0, 0
      set_vertices({glm::vec2(0, height), // top left (index 0)
                    glm::vec2(0, 0), // bottom left (index 1)
                    glm::vec2(width, 0), // bottom right (index 2)
                    glm::vec2(width, height)}); // top right (index 3)

      set_pipe_indices({0, 1, 2, 3, 0});
      set_line_end_indices({});
      set_solid_end_indices({0, 1, 2, 0, 2, 3});
      set_id(type::RECTANGLE);

      floats = {width, height};
    }
};

class square : public rectangle {
  public:
    square(float size) : rectangle(size, size) {
      set_id(type::SQUARE);

      floats = {size};
    }
};

class triangle : public shape_2d {
  public:
    triangle(float x1, float y1, float x2, float y2) { // first point is 0, 0
      set_vertices({glm::vec2(0, 0), // 0
                    glm::vec2(x1, y1), // 1
                    glm::vec2(x2, y2)}); // 2
      if (x1*y2 > y1*x2) { // if point order is counterclockwise
        set_pipe_indices({0, 1, 2, 0});
        set_solid_end_indices({0, 1, 2});
      } else {
        set_pipe_indices({0, 2, 1, 0});
        set_solid_end_indices({0, 2, 1});
      }
      set_line_end_indices({});
      set_id(type::TRIANGLE);

      x_coords = {x1, x2};
      y_coords = {y1, y2};
    }
};

// the center vertex of the ellipse and the rounded rectangle are also extruded but are only necessary at the ends
// possible way to fix this would be to store two different arrays of vertices? pipe_vertice and end_vertices
class ellipse : public shape_2d {
  public:
    ellipse(float rad_x, float rad_y, uint32_t sectors) { // centered around 0, 0
      vertices_push(0, 0);
      float angle_inc = 360 / sectors;
      add_sector(0, 0, rad_x, rad_y, 0, 360 - angle_inc, angle_inc);
      pipe_indices_push(1);
      pipe_indices_push(sectors);
      for (uint32_t i = 1; i < sectors; i++) {
        pipe_indices_push(sectors - i);

        line_end_indices_push(0);
        line_end_indices_push(i);

        solid_end_indices_push(0);
        solid_end_indices_push(i);
        solid_end_indices_push(i + 1);
      }

      line_end_indices_push(0);
      line_end_indices_push(sectors);

      solid_end_indices_push(0);
      solid_end_indices_push(sectors);
      solid_end_indices_push(1);

      set_id(type::ELLIPSE);

      floats = {rad_x, rad_y};
      ints = {sectors};
    }
};

class circle : public ellipse {
  public: 
    circle(float rad, uint32_t sectors) : ellipse(rad, rad, sectors) {
      set_id(type::CIRCLE);

      floats = {rad};
    }
};

class round_rectangle : public shape_2d { 
  public:
    round_rectangle(float width, float height, float rad_x, float rad_y, uint32_t sectors) { // bottom left is 0, 0
      vertices_push(width / 2, height / 2);
      float angle_inc = 90 / sectors;
      add_sector(rad_x, rad_y, rad_x, rad_y, 90, 180, angle_inc);

      add_sector(rad_x, height - rad_y, rad_x, rad_y, 180, 270, angle_inc);

      add_sector(width - rad_x, height - rad_y, rad_x, rad_y, 270, 360, angle_inc);

      add_sector(width - rad_x, rad_y, rad_x, rad_y, 0, 90, angle_inc);

      pipe_indices_push(1);
      uint32_t last_index = get_num_vertices() - 1;
      for (uint32_t i = 1; i < last_index; i++) {
        // add pipe indices in reverse order to make counter-clockwise
        pipe_indices_push(last_index - i);

        line_end_indices_push(0);
        line_end_indices_push(i);

        solid_end_indices_push(0);
        solid_end_indices_push(i);
        solid_end_indices_push(i + 1);
      }

      line_end_indices_push(0);
      line_end_indices_push(last_index);

      solid_end_indices_push(0);
      solid_end_indices_push(last_index);
      solid_end_indices_push(1);

      set_id(type::ROUND_RECTANGLE);

      floats = {width, height, rad_x, rad_y};
      ints = {sectors};
    }
};