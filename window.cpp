#include <iostream>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <GL/glew.h>
#include "drawing.hpp"
#include "drw_file.hpp"

void create_window(drw_file& drw, const std::string& title) {
  int width = drw.get_width();
  int height = drw.get_height();
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    throw "Failed to initialize SDL";
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

  SDL_DisplayMode DM;
  if (SDL_GetDesktopDisplayMode(0, &DM) != 0)
    throw "Failed to get display mode";
  if (DM.w < width) {
    int temp = width;
    width = DM.w;
    height *= 1.f * width/temp;
  }
  if (DM.h < height) {
    int temp = height;
    height = 0.8 * DM.h;
    width *= 1.f * height/temp;
  }

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
  //TODO: this blend function does not behave like svg's default blending but it's closer 
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  drw.init();

  // Set background color
  glm::vec3 bg_color = drw.get_bg_color();
  glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.f);

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
  drw_file drw_svg = drw_file("svg/circles.svg");
  drw_svg.add_transform(glm::mat4(1.f));

  drw_file drw2(0, 0);
  drw2.add_transform(glm::mat4(1.f));
  drw_file drw(300, 100);
  drw.add_transform(glm::mat4(1.f));
  drw.set_bg_color_index(drw.get_color_index("white"));

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
  drawing& main = drw.create_main_drawing();
  view vw = view(0, 0, 300, 100);
  main.set_view(vw);
  styled_multishape_2d* shapes = main.create_styled_multishape_2d(drw, 1, 0);
  shapes->add_fill_circle(50, 50, 40, 20, drw.get_color_index("black"), 1.f);
  shapes->add_fill_circle(150, 50, 40, 20, drw.get_color_index("black"), 1.f);
  shapes->add_fill_circle(250, 50, 40, 20, drw.get_color_index("black"), 1.f);
  multishape_3d* wireframes = main.create_multishape_3d(1, drw.get_color_index("blue"), 1, 0);
  wireframes->add_draw_sphere(50, 50, 0, 40, 32, 18);
  wireframes->add_draw_rect_prism(0, 0, 0, 400, 100, 400);
  lit_multishape_3d* lit_shapes = main.create_lit_multishape_3d(1, 0, 0, {0}, {0}, {0}, 1);
  lit_shapes->add_fill_torus(0, 0, 0, 0.3, 0.2, 32, 18);

  drawing* child = main.create_child_drawing();
  view vw2 = view(0, 0, 100, 200);
  child->set_view(vw2);
  styled_multishape_2d* child_shapes = child->create_styled_multishape_2d(drw, 1, 0);
  child_shapes->add_fill_circle(50, 50, 40, 20, drw.get_color_index("black"), 1.f);
  child_shapes->add_fill_circle(150, 50, 40, 20, drw.get_color_index("black"), 1.f);
  child_shapes->add_fill_circle(250, 50, 40, 20, drw.get_color_index("black"), 1.f);

  drw_svg.save("drw/test.drw");
  drw2.load("drw/test.drw");
  try {
    std::cout << "Creating window..." << std::endl;
    create_window(drw2, "Drawing");
  } catch (char const* ex) {
    std::cerr << ex << std::endl;
    return 1;
  }
  std::cout << "Application exited normally." << std::endl;
  return 0;
}