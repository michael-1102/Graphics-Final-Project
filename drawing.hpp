#pragma once

#include <vector>
#include "shape.hpp"
#include "view.hpp"

class drawing : public shape {
  private:
    view my_view;
    std::vector<shape*> shapes;

  public:
    void init() override;
    void render(view& view) override;
};