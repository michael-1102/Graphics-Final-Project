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

void drawing::render(drw_file& drw) {
  for (auto s : shapes) {
    s->render(drw, my_view);
  }
}

void drawing::render(drw_file& drw, view& view) { render(drw); }

void drawing::add_shape(shape* shape) {
  shapes.push_back(shape);
}

styled_multishape_2d* drawing::create_styled_multishape_2d(drw_file& drw, float line_width, uint32_t transform_index) {
  shapes.push_back(new styled_multishape_2d(drw, line_width, transform_index));
  return (styled_multishape_2d*) shapes.back();
}