#pragma once

#include <vector>
#include "shape.hpp"
#include "styled_multishape_2d.hpp"
#include "view.hpp"
#include <memory>

class drawing : public shape {
  private:
    view my_view;
    std::vector<shape*> shapes;

  public:
    drawing() {}
    void init() override;
    void update() override;
    void render(drw_file& drw, view& view) override;
    void render(drw_file& drw);
    void save(std::vector<instruction>& instructions, std::vector<uint32_t> uint32s, std::vector<float> floats, std::vector<float> x_coords, std::vector<float> y_coords, std::vector<float> z_coords);

    void add_shape(shape* shape);
    
    inline void set_view(view& v) { my_view = v; }

    styled_multishape_2d* create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index);
};