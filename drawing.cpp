#include "drawing.hpp"

void drawing::init() {
  for (auto s : shapes) {
    s->init();
  }
}

void drawing::update() {
  for (auto s : shapes) {
    s->update();
  }
}

void drawing::save(std::vector<uint8_t>& instructions, std::vector<uint32_t>& uint32s, std::vector<float>& floats, std::vector<float>& x_coords, std::vector<float>& y_coords, std::vector<float>& z_coords) const {
  instructions.push_back(to_underlying(instruction::START_DRAWING));
  my_view.save(floats);
  for (auto s : shapes) {
    s->save(instructions, uint32s, floats, x_coords, y_coords, z_coords);
  }
  instructions.push_back(to_underlying(instruction::END_DRAWING));
}

void drawing::render(drw_file& drw) {
  for (auto s : shapes) {
    s->render(drw, my_view);
  }
}

void drawing::render(drw_file& drw, view& view) { render(drw); }

drawing* drawing::create_child_drawing() {
  shapes.push_back(new drawing());
  return (drawing*)shapes.back();
}

multishape_2d* drawing::create_multishape_2d(float line_width, uint32_t color_index, uint32_t transform_index, float alpha, shader_id sid) {
  shapes.push_back(new multishape_2d(line_width, color_index, transform_index, alpha, sid));
  return (multishape_2d*)shapes.back();
}

multishape_2d* drawing::create_multishape_2d(float line_width, uint32_t color_index, uint32_t transform_index, float alpha) {
  shapes.push_back(new multishape_2d(line_width, color_index, transform_index, alpha));
  return (multishape_2d*)shapes.back();
}

styled_multishape_2d* drawing::create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index) {
  shapes.push_back(new styled_multishape_2d(drw, line_width, transform_index));
  return (styled_multishape_2d*)shapes.back();
}

styled_multishape_2d* drawing::create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index, shader_id sid) {
  shapes.push_back(new styled_multishape_2d(drw, line_width, transform_index, sid));
  return (styled_multishape_2d*)shapes.back();
}

multishape_3d* drawing::create_multishape_3d(float line_width, uint32_t color_index, uint32_t camera_index, uint32_t transform_index, float alpha, shader_id sid) {
  shapes.push_back(new multishape_3d(line_width, color_index, camera_index, transform_index, alpha, sid));
  return (multishape_3d*)shapes.back();
}

multishape_3d* drawing::create_multishape_3d(float line_width, uint32_t color_index, uint32_t camera_index, uint32_t transform_index, float alpha) {
  shapes.push_back(new multishape_3d(line_width, color_index, camera_index, transform_index, alpha));
  return (multishape_3d*)shapes.back();
}

lit_multishape_3d* drawing::create_lit_multishape_3d(float line_width, uint32_t camera_index, uint32_t material_index, std::vector<uint32_t> point_light_indices, std::vector<uint32_t> spot_light_indices, std::vector<uint32_t> dir_light_indices, uint32_t transform_index, shader_id sid) {
  shapes.push_back(new lit_multishape_3d(line_width, camera_index, material_index, point_light_indices, spot_light_indices, dir_light_indices, transform_index, sid));
  return (lit_multishape_3d*)shapes.back();
}

lit_multishape_3d* drawing::create_lit_multishape_3d(float line_width, uint32_t camera_index, uint32_t material_index, std::vector<uint32_t> point_light_indices, std::vector<uint32_t> spot_light_indices, std::vector<uint32_t> dir_light_indices, uint32_t transform_index) {
  shapes.push_back(new lit_multishape_3d(line_width, camera_index, material_index, point_light_indices, spot_light_indices, dir_light_indices, transform_index));
  return (lit_multishape_3d*)shapes.back();
}

void drawing::load(drw_file& drw, parasitic_vector<uint8_t>& instructions, parasitic_vector<float>& x_coords, parasitic_vector<float>& y_coords, parasitic_vector<float>& z_coords,
  parasitic_vector<float>& floats, parasitic_vector<uint32_t>& uint32s, uint32_t& instr_index, uint32_t& current_uint32, uint32_t& current_x_coord,
  uint32_t& current_y_coord, uint32_t& current_z_coord, uint32_t& current_float) {
    instr_index++;
    instruction current_instr = static_cast<instruction>(instructions[instr_index]);
    while (current_instr != instruction::END_DRAWING) {
      switch (current_instr) {
        case (instruction::START_MULTISHAPE_2D):
          {
          multishape_2d* shape = create_multishape_2d(floats[current_float], uint32s[current_uint32], uint32s[current_uint32 + 1], floats[current_float + 1], static_cast<shader_id>(uint32s[current_uint32 + 2]));
          current_float += 2;
          current_uint32 += 3;
          shape->load(drw, instructions, x_coords, y_coords, z_coords, floats, uint32s, instr_index, current_uint32, current_x_coord, current_y_coord, current_z_coord, current_float);
          }
          break;
        case (instruction::START_STYLED_MULTISHAPE_2D):
          {
          styled_multishape_2d* shape = create_styled_multishape_2d(drw, floats[current_float], uint32s[current_uint32], static_cast<shader_id>(uint32s[current_uint32 + 1]));
          current_float++;
          current_uint32 += 2;
          shape->load(drw, instructions, x_coords, y_coords, z_coords, floats, uint32s, instr_index, current_uint32, current_x_coord, current_y_coord, current_z_coord, current_float);  
          }
          break;
        case (instruction::START_MULTISHAPE_3D):
          {
          multishape_3d* shape = create_multishape_3d(floats[current_float], uint32s[current_uint32], uint32s[current_uint32 + 1], uint32s[current_uint32 + 2], floats[current_float + 1], static_cast<shader_id>(uint32s[current_uint32 + 3]));
          current_float+=2;
          current_uint32 += 4;    
          shape->load(drw, instructions, x_coords, y_coords, z_coords, floats, uint32s, instr_index, current_uint32, current_x_coord, current_y_coord, current_z_coord, current_float);
          }
          break;
        case (instruction::START_LIT_MULTISHAPE_3D):
          {
          uint32_t num_point_lights = uint32s[current_uint32++];
          uint32_t num_spot_lights = uint32s[current_uint32++];
          uint32_t num_dir_lights = uint32s[current_uint32++];
          std::vector<uint32_t> point_light_uint32s(num_point_lights);
          std::vector<uint32_t> spot_light_uint32s(num_spot_lights);
          std::vector<uint32_t> dir_light_uint32s(num_dir_lights);
          for (uint32_t i = 0; i < num_point_lights; i++) 
            point_light_uint32s[i] = uint32s[current_uint32++];
          for (uint32_t i = 0; i < num_spot_lights; i++)
            spot_light_uint32s[i] = uint32s[current_uint32++];
          for (uint32_t i = 0; i < num_dir_lights; i++)
            dir_light_uint32s[i] = uint32s[current_uint32++];

          lit_multishape_3d* shape = create_lit_multishape_3d(floats[current_float], uint32s[current_uint32], uint32s[current_uint32 + 1], point_light_uint32s, spot_light_uint32s, dir_light_uint32s, uint32s[current_uint32 + 2], static_cast<shader_id>(uint32s[current_uint32 + 3]));
          current_float++;
          current_uint32 += 4;
          shape->load(drw, instructions, x_coords, y_coords, z_coords, floats, uint32s, instr_index, current_uint32, current_x_coord, current_y_coord, current_z_coord, current_float);  
          }
          break;
        case (instruction::START_DRAWING):
          {
          drawing* child = create_child_drawing();
          view v = view(floats[current_float], floats[current_float + 1], floats[current_float + 2], floats[current_float + 3]);
          current_float += 4;
          child->set_view(v);
          child->load(drw, instructions, x_coords, y_coords, z_coords,floats, uint32s, instr_index, current_uint32, current_x_coord,
                            current_y_coord, current_z_coord, current_float);
          }
          break;
        default:
          break;
      }
      instr_index++;
      current_instr = static_cast<instruction>(instructions[instr_index]);
    }
}