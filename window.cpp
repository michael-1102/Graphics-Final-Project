#include <iostream>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <GL/glew.h>
#include "drawing.hpp"
#include "styled_multishape_2d.hpp"
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
  drw_file drw(300, 150);
  drw.set_bg_color_index(0);
  drw.add_color(glm::vec4(1.f, 1.f, 1.f, 1.f));  // white (0)
  drw.add_color(glm::vec4(1.f, 0.f, 0.f, 1.f)); // red (1)
  drw.add_color(glm::vec4(0.f, 1.f, 0.f, 1.f)); // green (2)
  drw.add_color(glm::vec4(0.f, 0.f, 1.f, 1.f)); // blue (3)

  drw.add_transform(glm::mat4(1.f));
  drawing main = drawing();
  view vw = view(0, 0, 300, 200);
  main.set_view(vw);
  styled_multishape_2d shapes = styled_multishape_2d(drw, 1, 0);
  shapes.add_fill_circle(50, 50, 40, 20, 1);
  shapes.add_fill_circle(150, 50, 40, 20, 2);
  shapes.add_fill_circle(250, 50, 40, 20, 3);

  drawing child = drawing();
  view vw2 = view(0, 0, 100, 200);
  child.set_view(vw2);
  //child.add_shape(&shapes);

  main.add_shape(&shapes);
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