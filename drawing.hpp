#pragma once

#include <vector>
#include "shape.hpp"
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

    void add_shape(shape* shape);
    
    inline void set_view(view& v) { my_view = v; }
};