#include <iostream>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <GL/glew.h>
#include "drawing.hpp"
#include "styled_multishape_2d.hpp"
#include "lit_multishape_3d.hpp"
#include "drw_file.hpp"


void create_window(drw_file& drw, const std::string& title, int width, int height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    throw "Failed to initialize SDL";
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

  // Create window
  SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window) throw "Failed to create window";
  
  // Create OpenGL context
  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    std::string err = SDL_GetError();
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw err;
  }

  glewInit();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  drw.init();

  // Set background color
  glm::vec4& bg_color = drw.get_bg_color();
  glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drw.render();

  SDL_GL_SwapWindow(window);
  bool quit = false;
  SDL_Event event;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        default:
          break;
      }
    }
  }

    // Clean-up code
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char* argv[]) {
  drw_file drw(500, 500);
  drw.set_bg_color_index(0);
  drw.add_color(glm::vec4(1.f, 1.f, 1.f, 1.f));  // white (0)
  drw.add_color(glm::vec4(1.f, 0.f, 0.f, 1.f)); // red (1)
  drw.add_color(glm::vec4(0.f, 1.f, 0.f, 1.f)); // green (2)
  drw.add_color(glm::vec4(0.f, 0.f, 1.f, 1.f)); // blue (3)

  camera cam = camera(1);
  camera cam1 = camera(0, 0, 400, 100);
  drw.add_camera(cam);
  drw.add_camera(cam1);

  material mat = material(glm::vec3(0.1, 0.18725, 0.1745),
                          glm::vec3(0.396, 0.74151, 0.69102),
                          glm::vec3(0.297254, 0.30829, 0.306678), 0.1f * 128.f);
  drw.add_material(mat);  // turquoise

  spot_light spot = spot_light();
  drw.add_spot_light(spot);
  dir_light dir = dir_light();
  drw.add_dir_light(dir);
  point_light point = point_light();
  drw.add_point_light(point);

  drw.add_transform(glm::mat4(1.f));
  drw.add_transform(glm::rotate(glm::mat4(1.f), 0.3f, glm::vec3(1, 0, 0)));
  drawing main = drawing();
  view vw = view(0, 0, 400, 100);
  main.set_view(vw);
  styled_multishape_2d shapes = styled_multishape_2d(drw, 1, 0);
  shapes.add_fill_circle(50, 50, 40, 20, 1);
  shapes.add_draw_rectangle(0, 0, 400, 100, 1);
  shapes.add_fill_circle(150, 50, 40, 20, 2);
  shapes.add_fill_circle(250, 50, 40, 20, 3);
  multishape_3d wireframes = multishape_3d(1, 3, 1, 0);
  wireframes.add_draw_sphere(50, 50, 0, 40, 32, 18);
  wireframes.add_draw_rect_prism(0, 0, 0, 400, 100, 400);
  lit_multishape_3d lit_shapes = lit_multishape_3d(1, 0, 0, {0}, {0}, {0}, 1);
  lit_shapes.add_fill_torus(0, 0, 0, 0.3, 0.2, 32, 18);

  drawing child = drawing();
  view vw2 = view(0, 0, 100, 200);
  child.set_view(vw2);
  //child.add_shape(&shapes);

  main.add_shape(&shapes);
  main.add_shape(&wireframes);
  main.add_shape(&lit_shapes);
  //main.add_shape(&child);
  drw.set_main_drawing(main);

  try {
    std::cerr << "Creating window..." << std::endl;
    create_window(drw, "Drawing", drw.get_width(), drw.get_height());
    std::cerr << "Application exited normally." << std::endl;
  } catch (char const* ex) {
    std::cerr << ex << std::endl;
  }
}