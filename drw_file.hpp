#pragma once

#include <vector>
#include "drawing.hpp"
#include "lighting.hpp"
#include "camera.hpp"
#include "colors.hpp"


class drw_file {
  private:
    float width;
    float height;
    glm::mat4 scale;

    drawing main_drawing; // parent drawing of all shapes

    colors clrs;  // a list of rgba colors
    std::vector<material> materials; // a list of materials for lighting
    std::vector<spot_light> spot_lights; // a list of spot lights
    std::vector<dir_light> dir_lights; // a list of directional lights
    std::vector<point_light> point_lights; // a list of point lights
    std::vector<camera> cameras; // a list of cameras

    std::vector<glm::mat4> transforms; // a list of transform matrices

    uint32_t bg_color_index; // index of colors for background color

    std::unordered_map<shader_id, shader> shaders; // shader_id, shader key-value pair

  public:
    drw_file(float width, float height) : width(width), height(height) {
      scale = glm::scale(glm::mat4(1.f), glm::vec3(1.f, width/height, 1.f));
    }

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

    inline glm::vec4& get_bg_color() {
      return get_color(bg_color_index);
    }

    inline shader& get_shader(shader_id sid) {
      if (shaders.find(sid) == shaders.end()) {
        shaders[sid] = shader(sid);
        shaders[sid].init();
      }
      return shaders[sid]; 
    }

    inline void add_color(glm::vec4 color) { clrs.add_color(color); }

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

    glm::vec4& get_color(uint32_t i) {
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