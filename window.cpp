#include <iostream>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <GL/glew.h>
#include "drawing.hpp"
#include "drw_file.hpp"
#include "test_drw.cpp"

void set_viewport(int window_width, int window_height, float drw_width, float drw_height) {
  float window_ratio = 1.f * window_width / window_height;
  float drw_ratio = drw_width / drw_height;
  if (window_ratio < drw_ratio) {
    float new_height = window_width / drw_ratio;
    glViewport(0, (window_height - new_height)/2.f, window_width, new_height);
  } else {
    float new_width = window_height * drw_ratio;
    glViewport((window_width - new_width)/2.f, 0, new_width, window_height);
  }
}

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

  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  glewInit();
  //TODO: this blend function does not behave like svg's default blending but it's closer 
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  drw.init();

  // Set background color
  glm::vec3 bg_color = drw.get_bg_color();

  bool resized = true;
  bool right_ctrl = false;
  bool left_ctrl = false;

  SDL_GL_SwapWindow(window);
  bool quit = false;
  SDL_Event event;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
            resized = true;
          break;
        default:
          break;
      }
    }
    if (resized) {
      resized = false;
      SDL_GetWindowSize(window, &width, &height);
      set_viewport(width, height, drw.get_width(), drw.get_height());

      // update
      glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      drw.render();
      SDL_GL_SwapWindow(window);
    }
    
  }

    // Clean-up code
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char* argv[]) {  
  try {
    drw_file drw;
    if (argc > 1) {
      if (std::regex_match(argv[1], std::regex(".*.drw$"))) {
        drw.load(argv[1]);
      } else if (std::regex_match(argv[1], std::regex(".*.svg$"))) {
        std::cout << "Converting svg to drawing..." << std::endl;
        drw.load_svg(argv[1]);
        std::string path(argv[1]);
        path = "drw/" + path.substr(path.find_last_of("/") + 1, path.find_last_of('.') - path.find_last_of("/") - 1) + ".drw";
        drw.save(path.c_str());
      } else {
        switch (atoi(argv[1])) {
          case 1:
            std::cout << "Creating test drawing..." << std::endl;
            test1(&drw);
            break;
          case 0:
          default:
            std::cout << "Opening empty drawing..." << std::endl;
            break;
        }
      }
    } else {
      std::cerr << "Usage: ./drawing <file_name>.drw or ./drawing <test_number>" << std::endl;
      return 1;
    }

    std::cout << "Creating window..." << std::endl;
    create_window(drw, "Drawing");
  } catch (char const* ex) {
    std::cerr << ex << std::endl;
    return 1;
  }
  std::cout << "Application exited normally." << std::endl;
  return 0;
}