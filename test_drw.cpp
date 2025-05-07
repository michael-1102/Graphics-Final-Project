#include "drw_file.hpp"

void test1(drw_file* drw) {
  drw->set_width(1000);
  drw->set_height(1000);

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

void wireframes(drw_file* drw) {
  float width = 1280;
  float height = 720;
  drw->set_width(width);
  drw->set_height(height);
  drawing& main = drw->create_main_drawing();
  view vw = view(0, 0, width, height);
  main.set_view(vw);

  drw->add_transform(glm::mat4(1.f));

  camera cam = camera(width/height, glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), 45.f, 0.1f, 100.f);
  drw->add_camera(cam);

  multishape_3d* wireframes = main.create_multishape_3d(1, drw->get_color_index("blue"), 0, 0);
  wireframes->add_draw_sphere(-1.4, -0.9, 0, 0.3, 32, 18);
  wireframes->add_draw_ellipsoid(-0.7, -0.8, 0, 0.3, 0.4, 0.3, 32, 18);
  wireframes->add_draw_cone(0, -1.05, 0, 0.3, 0.4, 32, 1);
  wireframes->add_draw_cylinder(0.7, -1.05, 0, 0.3, 0.4, 32, 1);
  wireframes->add_draw_tube(1.4, -1.05, 0, 0.3, 0.2, 0.4, 32, 1);
  wireframes->add_draw_cube(-1, 0.25, 0, 0.3);
  wireframes->add_draw_rect_prism(-0.7, -0.3, 0, 0.3, 0.4, 0.3);
  wireframes->add_draw_spline_extrusion(circle(0.1, 20), {glm::vec3(0, -0.3, 0), glm::vec3(0, 0.3, 0), glm::vec3(-0.5, 0.5, 0), glm::vec3(-0.6, 1, 0)}, 0.03);
  wireframes->add_draw_rect_pyramid(1.5, 0.7, 0, 0.4, 0.4, 0.4);
  wireframes->add_draw_wedge(-1.85, 0.7, 0, 0.3, 0.3, 0.3);
  wireframes->add_draw_oblique_cone(1.1, 0.45, 0, 0.3, 1.3, 0.95, 0.1, 32, 3);
  wireframes->add_draw_frustum(0.4, 0.6, 0, 0.5, 0.3, 0.3, 32, 1);
  wireframes->add_draw_torus(-1.2, 0.8, 0, 0.3, 0.05, 32, 18);
  wireframes->add_draw_helix(-1.5, -0.3, 0, 0.3, 0.05, 6*M_PI, 0.3, 32, 18);
  wireframes->add_draw_oblique_cylinder(0.5, -0.3, 0, 0.7, 0.2, 0, 0.3, 32, 3);
  wireframes->add_draw_oblique_frustum(1.5, -0.3, 0, 0.5, 1.8, 0.4, 0, 0.2, 32, 1);

  drw->save("drw/wireframes.drw");
}

void lighting(drw_file* drw) {
  float width = 1280;
  float height = 720;
  drw->set_width(width);
  drw->set_height(height);
  drawing& main = drw->create_main_drawing();
  view vw = view(0, 0, width, height);
  main.set_view(vw);

  drw->add_transform(glm::mat4(1.f));

  camera cam = camera(width/height, glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), 45.f, 0.1f, 100.f);
  drw->add_camera(cam);


  material mat0(glm::vec3(0.25, 0.25, 0.25), glm::vec3(0.4, 0.4, 0.4),
                        glm::vec3(0.774597, 0.774597, 0.774597), 0.6 * 128); // chrome (0)
  material mat1(glm::vec3(0.329412, 0.223529, 0.027451), glm::vec3(0.780392, 0.568627, 0.113725),
                        glm::vec3(0.992157, 0.941176, 0.807843), 0.21794872 * 128); // brass (1)
  material mat2(glm::vec3(0.1, 0.18725, 0.1745),
                        glm::vec3(0.396, 0.74151, 0.69102),
                        glm::vec3(0.297254, 0.30829, 0.306678), 0.1f * 128.f); // turquoise (2)
  material mat3(glm::vec3(0.24725, 0.1995, 0.0745),
                        glm::vec3(0.75164, 0.60648, 0.22648),
                        glm::vec3(0.628281, 0.555802, 0.366065), 0.4 * 128); // gold (3)
  material mat4(glm::vec3(0.0215, 0.1745, 0.0215),
                        glm::vec3(0.07568, 0.61424, 0.07568),
                        glm::vec3(0.633, 0.727811, 0.633), 0.6 * 128); // emerald (4)
  material mat5(glm::vec3(0.1745, 0.01175, 0.01175),
                        glm::vec3(0.61424, 0.04136, 0.04136),
                        glm::vec3(0.727811, 0.626959, 0.626959), 0.6 * 128); // ruby (5)
  material mat6(glm::vec3(0.05, 0.05, 0),
                        glm::vec3(0.5, 0.5, 0.4),
                        glm::vec3(0.7, 0.7, 0.04), 0.078125 * 128); // yellow rubber (6)
  material mat7(glm::vec3(0.25, 0.20725, 0.20725),
                        glm::vec3(1, 0.829, 0.829),
                        glm::vec3(0.296648, 0.296648, 0.296648), 0.088 * 128); // pearl (7)

  drw->add_material(mat0);
  drw->add_material(mat1);
  drw->add_material(mat2);
  drw->add_material(mat3);
  drw->add_material(mat4);
  drw->add_material(mat5);
  drw->add_material(mat6);
  drw->add_material(mat7);

  drw->create_dir_light();
  drw->create_dir_light(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f),
                         glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f));
  drw->create_spot_light();
  drw->create_point_light();
  drw->create_point_light(glm::vec3(0.f, 2.f, 3.f), 1.f, 0.9f, 0.032f,glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f));

  lit_multishape_3d* material0 = main.create_lit_multishape_3d(1, 0, 0, {0}, {0}, {0}, 0);
  material0->add_fill_cube(0.4, -0.7, 0, 0.4);
  lit_multishape_3d* material1 = main.create_lit_multishape_3d(1, 0, 1, {0}, {0}, {0}, 0);
  material1->add_fill_torus(-0.35, 0.35, 0, 0.3, 0.1, 32, 18);
  lit_multishape_3d* material2 = main.create_lit_multishape_3d(1, 0, 2, {0}, {0}, {0}, 0);
  material2->add_fill_cylinder(-0.5, -0.9, 0, 0.4, 0.5, 32, 1);
  lit_multishape_3d* material3 = main.create_lit_multishape_3d(1, 0, 3, {0}, {0}, {0}, 0);
  material3->add_fill_sphere(0.6, 0.7, 0, 0.4, 32, 18);
  lit_multishape_3d* material4 = main.create_lit_multishape_3d(1, 0, 4, {0}, {0}, {0}, 0);
  material4->add_fill_wedge(-0.6, 0.8, -0.3, 0.5, 0.3, 0.3);
  lit_multishape_3d* material5 = main.create_lit_multishape_3d(1, 0, 5, {0}, {0}, {0}, 0);
  material5->add_fill_cone(-1.5, -0.8, 0, 0.4, 0.5, 32, 1);
  lit_multishape_3d* material6 = main.create_lit_multishape_3d(1, 0, 6, {0}, {0}, {0}, 0);
  material6->add_fill_spline_extrusion(circle(0.1, 10), {glm::vec3(1.5, -0.6, 0.3), glm::vec3(1.2, -0.6, 0.3), glm::vec3(1.1, 0.3, 0), glm::vec3(1.5, 0.5, 0), glm::vec3(1.6, 1, 0)}, 0.03);
  lit_multishape_3d* material7 = main.create_lit_multishape_3d(1, 0, 7, {0}, {0}, {0}, 0);
  material7->add_fill_frustum(-1.3f, 0.5f, 0.f, 0.3f, 0.4f, 0.4f, 32, 1);

  drw->save("drw/lighting.drw");
}