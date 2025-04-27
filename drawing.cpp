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