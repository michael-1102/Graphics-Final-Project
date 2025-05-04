#include "drw_file.hpp"

void test1(drw_file* drw) {
  drw->set_width(1000);
  drw->set_height(1000);
  drw->set_bg_color_index(drw->get_color_index("white"));

  camera cam = camera(1);
  camera cam1 = camera(0, 0, 400, 100);
  drw->add_camera(cam);
  drw->add_camera(cam1);

  material mat = material(glm::vec3(0.1, 0.18725, 0.1745),
                          glm::vec3(0.396, 0.74151, 0.69102),
                          glm::vec3(0.297254, 0.30829, 0.306678), 0.1f * 128.f);
  drw->add_material(mat);  // turquoise

  spot_light spot = spot_light();
  drw->add_spot_light(spot);
  dir_light dir = dir_light();
  drw->add_dir_light(dir);
  point_light point = point_light();
  drw->add_point_light(point);

  drw->add_transform(glm::mat4(1.f));
  drw->add_transform(glm::rotate(glm::mat4(1.f), (float)M_PI*0.8f, glm::vec3(0, 1, 0)));

  drawing& main = drw->create_main_drawing();
  view vw = view(0, 0, 1000, 1000);
  main.set_view(vw);
  styled_multishape_2d* shapes = main.create_styled_multishape_2d(*drw, 1, 0);
  shapes->add_fill_circle(0, 0, 40, 20, drw->get_color_index("black"), 1.f);
  //shapes->add_fill_circle(150, 50, 40, 20, drw->get_color_index("lime"), 1.f);
  shapes->add_fill_circle(1000, 1000, 40, 20, drw->get_color_index("black"), 1.f);
  shapes->add_fill_triangle_marker(250, 50, 40, drw->get_color_index("orange"), 1.f);
  multishape_3d* wireframes = main.create_multishape_3d(1, drw->get_color_index("blue"), 1, 0);
  wireframes->add_draw_sphere(50, 50, 0, 40, 32, 18);
  wireframes->add_draw_rect_pyramid(200, 50, -20, 40, 40, 40);
  wireframes->add_draw_rect_prism(200, 50, 0, 400, 100, 400);
  //wireframes->add_fill_helix(200, 50, -60, 20, 10, 10*M_PI, 2, 10, 18);
  lit_multishape_3d* lit_shapes = main.create_lit_multishape_3d(1, 0, 0, {0}, {0}, {0}, 1);
  lit_shapes->add_fill_helix(0, -0.5, 0, 0.3, 0.05, 6*M_PI, 0.2, 32, 18);
  //lit_shapes->add_fill_torus(0, 0.5, 0, 0.3, 0.2, 4, 18);

  drawing* child = main.create_child_drawing();
  view vw2 = view(0, 0, 100, 200);
  child->set_view(vw2);
  styled_multishape_2d* child_shapes = child->create_styled_multishape_2d(*drw, 1, 0);
  //child_shapes->add_fill_circle(50, 50, 40, 20, drw->get_color_index("black"), 1.f);
  //child_shapes->add_fill_circle(150, 50, 40, 20, drw->get_color_index("black"), 1.f);
  //child_shapes->add_fill_circle(250, 50, 40, 20, drw->get_color_index("black"), 1.f);

  drw->save("drw/test.drw");
}