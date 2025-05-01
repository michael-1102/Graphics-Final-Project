#pragma once

#include <vector>
#include "drawing.hpp"
#include "lighting.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "antlr4-runtime.h"
#include "libs/XMLLexer.h"
#include "libs/XMLParser.h"
#include "libs/XMLParserBaseVisitor.h"

#define DEFAULT_STROKE_WIDTH 1.f
#define DEFAULT_OPACITY 1.f
#define DEFAULT_COLOR_INDEX 0
#define DEFAULT_TRANSFORM_INDEX 0

#define NUM_SECTORS 100

using namespace antlr4;


class drw_file {
  private:
    struct group_attributes {
      uint32_t stroke_width;

      float fill_opacity;
      bool has_fill = true;
      bool has_stroke = false;
      float stroke_opacity;
      uint32_t fill_color_index;
      uint32_t stroke_color_index;

      uint32_t transform_index;

      group_attributes(uint32_t stroke_width = DEFAULT_STROKE_WIDTH, float fill_opacity = DEFAULT_OPACITY, 
                       float stroke_opacity = DEFAULT_OPACITY, uint32_t fill_color_index = DEFAULT_COLOR_INDEX, uint32_t stroke_color_index = DEFAULT_COLOR_INDEX, uint32_t transform_index = DEFAULT_TRANSFORM_INDEX) :
                       transform_index(transform_index), stroke_width(stroke_width), fill_opacity(fill_opacity), stroke_opacity(stroke_opacity), fill_color_index(fill_color_index), stroke_color_index(stroke_color_index) {}
    };

    float width = 300;
    float height = 100;
    glm::mat4 scale;

    drawing main_drawing; // parent drawing of all shapes

    colors clrs;  // a list of rgba colors
    std::vector<material> materials; // a list of materials for lighting
    std::vector<spot_light> spot_lights; // a list of spot lights
    std::vector<dir_light> dir_lights; // a list of directional lights
    std::vector<point_light> point_lights; // a list of point lights
    std::vector<camera> cameras; // a list of cameras

    std::vector<glm::mat4> transforms = {glm::mat4(1.f)}; // a list of transform matrices

    uint32_t bg_color_index = clrs.get_color_index("white"); // index of colors for background color

    std::unordered_map<shader_id, shader> shaders; // shader_id, shader key-value pair

    group_attributes apply_group_attributes(XMLParser::ElementContext* elem, group_attributes attribs);
    void apply_svg_attributes(XMLParser::ElementContext* svg);
    void add_svg_elements(std::vector<XMLParser::ElementContext*> elements, group_attributes attribs);

    void add_ellipse(XMLParser::ElementContext* element, group_attributes attribs);
    void add_circle(XMLParser::ElementContext* element, group_attributes attribs);
    void add_rect(XMLParser::ElementContext* element, group_attributes attribs);
    void add_line(XMLParser::ElementContext* element, group_attributes attribs);


    uint32_t string_to_color_index(std::string str) {
      //TODO: support hsl(h, s, l) and rgb(r, g, b)
      if (str.rfind("#", 0) == 0) {
        std::string red_str;
        std::string green_str;
        std::string blue_str;
        if (str.length() == 4) {
          red_str = str.substr(1, 1);
          red_str += red_str;
          green_str = str.substr(2, 1);
          green_str += green_str;
          blue_str = str.substr(3, 1);
          blue_str += blue_str;
        } else if (str.length() == 7) {
          red_str = str.substr(1, 2);
          green_str = str.substr(3, 2);
          blue_str = str.substr(5, 2);
        } else {
          std::cout << str << std::endl;
          throw "Error: Unsupported color value";
        }
        glm::vec3 color = glm::vec3(stoi(red_str, nullptr, 16) / 255.f, stoi(green_str, nullptr, 16) / 255.f, stoi(blue_str, nullptr, 16) / 255.f);
        return clrs.add_color(color);
      }
      return clrs.get_color_index(str);
    }

    float string_to_float(std::string str) {
      //TODO: handle %
      try {
        return stof(str);
      } catch (...) {
        std::cout << "Unsupported value: " << str << std::endl;
        return 0;
      }
    }

    uint32_t string_to_transform_index(std::string str);

  public:
    drw_file(float width, float height) : width(width), height(height) {
      scale = glm::scale(glm::mat4(1.f), glm::vec3(1.f, width/height, 1.f));
    }

    drw_file(const char filename[]);

    void init() { 
      main_drawing.init(); 
      for (auto s : shaders) {
        s.second.init();
      }
    }

    inline void set_main_drawing(drawing& drawing) { main_drawing = drawing; }

    inline void render() { main_drawing.render(*this); }

    inline float get_width() { return width; }
    inline float get_height() { return height; }
    inline glm::mat4& get_scale() { return scale; }

    inline void set_bg_color_index(uint32_t i) { bg_color_index = i; }

    inline glm::vec3& get_bg_color() {
      return get_color(bg_color_index);
    }

    inline shader& get_shader(shader_id sid) {
      if (shaders.find(sid) == shaders.end()) {
        shaders[sid] = shader(sid);
        shaders[sid].init();
      }
      return shaders[sid]; 
    }

    inline void add_color(glm::vec3 color) { clrs.add_color(color); }

    inline void add_material(material& material) {
      materials.push_back(material);
    }

    inline void add_spot_light(spot_light& spot_light) {
      spot_lights.push_back(spot_light);
    }

    inline void add_dir_light(dir_light& dir_light) {
      dir_lights.push_back(dir_light);
    }

    inline void add_point_light(point_light& point_light) {
      point_lights.push_back(point_light);
    }

    inline void add_camera(camera& camera) {
      cameras.push_back(camera);
    }

    inline void add_transform(glm::mat4 transform) {
      transforms.push_back(transform);
    }

    glm::vec3& get_color(uint32_t i) {
      return clrs.get_color(i);
    }

    uint32_t get_color_index(std::string name) {
      return clrs.get_color_index(name);
    }

    inline material& get_material(uint32_t i) {
      if (i >= materials.size()) throw "Material index out of bounds";
      return materials[i];
    }

    inline spot_light& get_spot_light(uint32_t i) {
      if (i >= spot_lights.size()) throw "Spot light index out of bounds";
      return spot_lights[i];
    }

    inline dir_light& get_dir_light(uint32_t i) {
      if (i >= dir_lights.size()) throw "Dir light index out of bounds";
      return dir_lights[i];
    }

    inline point_light& get_point_light(uint32_t i) {
      if (i >= point_lights.size()) throw "Point light index out of bounds";
      return point_lights[i];
    }

    inline camera& get_camera(uint32_t i) {
      if (i >= cameras.size()) throw "Camera index out of bounds";
      return cameras[i];
    }

    inline glm::mat4& get_transform(uint32_t i) {
      if (i >= transforms.size()) throw "Transform index out of bounds";
      return transforms[i];
    }
};