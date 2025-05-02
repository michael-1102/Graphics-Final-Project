#pragma once

#include <vector>
#include "shape.hpp"
#include "styled_multishape_2d.hpp"
#include "lit_multishape_3d.hpp"
#include "view.hpp"
#include <memory>

class drawing : public shape {
  private:
    view my_view;
    std::vector<shape*> shapes;

  public:
    drawing() {}
    ~drawing() {
      for (uint32_t i = 0; i < shapes.size(); i++) {
        delete shapes[i];
      }
    }
    void init() override;
    void update() override;
    void render(drw_file& drw, view& view) override;
    void save(std::vector<uint16_t>& instructions, std::vector<uint32_t>& uint32s, std::vector<float>& floats, std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords) const override;
    void load(drw_file& drw, parasitic_vector<uint16_t> &instructions, parasitic_vector<float>& x_coords, parasitic_vector<float>& y_coords, parasitic_vector<float>& z_coords,
              parasitic_vector<float>& floats, parasitic_vector<uint32_t>& uint32s, uint32_t& instr_index, uint32_t& current_uint32, uint32_t& current_x_coord,
              uint32_t& current_y_coord, uint32_t& current_z_coord, uint32_t& current_float);
    
    void render(drw_file& drw);
    
    inline void set_view(view& v) { my_view = v; }

    drawing* create_child_drawing();
    multishape_2d* create_multishape_2d(float line_width, uint32_t color_index, uint32_t transform_index, float alpha, shader_id sid);
    multishape_2d* create_multishape_2d(float line_width, uint32_t color_index, uint32_t transform_index, float alpha = 1.f);
    styled_multishape_2d* create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index);
    styled_multishape_2d* create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index, shader_id sid);
    multishape_3d* create_multishape_3d(float line_width, uint32_t color_index, uint32_t camera_index, uint32_t transform_index, float alpha, shader_id sid);
    multishape_3d* create_multishape_3d(float line_width, uint32_t color_index, uint32_t camera_index, uint32_t transform_index, float alpha = 1.f);
    lit_multishape_3d* create_lit_multishape_3d(
        float line_width, uint32_t camera_index, uint32_t material_index,
        std::vector<uint32_t> point_light_indices,
        std::vector<uint32_t> spot_light_indices,
        std::vector<uint32_t> dir_light_indices, uint32_t transform_index,
        shader_id sid);
    lit_multishape_3d* create_lit_multishape_3d(
        float line_width, uint32_t camera_index, uint32_t material_index,
        std::vector<uint32_t> point_light_indices,
        std::vector<uint32_t> spot_light_indices,
        std::vector<uint32_t> dir_light_indices, uint32_t transform_index);
};