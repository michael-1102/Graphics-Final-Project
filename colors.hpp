#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <bits/stdc++.h>

class colors {
  public:
    colors() {
      add_color("black", glm::vec3(0.f, 0.f, 0.f));
      add_color("navy", glm::vec3(0.f, 0.f, 0.5f));
      add_color("darkblue", glm::vec3(0.f, 0.39f, 0.f));
      add_color("mediumblue", glm::vec3(0.f, 0.f, 0.8f));
      add_color("blue", glm::vec3(0.f, 0.f, 1.f));
      add_color("darkgreen", glm::vec3(0.f, 0.39f, 0.f));
      add_color("green", glm::vec3(0.f, 0.5f, 0.f));
      add_color("teal", glm::vec3(0.f, 0.5f, 0.5f));
      add_color("darkcyan", glm::vec3(0.f, 0.55f, 0.55f));
      add_color("deepskyblue", glm::vec3(0.f, 0.75f, 1.f));
      add_color("darkturquoise", glm::vec3(0.f, 0.81f, 0.82f));
      add_color("mediumspringgreen", glm::vec3(0.f, 0.98f, 0.6f));
      add_color("lime", glm::vec3(0.f, 1.f, 0.f));
      add_color("springgreen", glm::vec3(0.f, 1.f, 0.5f));
      add_color("cyan", glm::vec3(0.f, 1.f, 1.f)); // some colors are repeats such as cyan and aqua
      add_color("aqua", glm::vec3(0.f, 1.f, 1.f)); // TODO: better way to handle repeat colors?
      add_color("midnightblue", glm::vec3(0.1f, 0.1f, 0.44f));
      add_color("dodgerblue", glm::vec3(0.12f, 0.56f, 1.f));
      add_color("lightseagreen", glm::vec3(0.13f, 0.7f, 0.67f));
      add_color("forestgreen", glm::vec3(0.13f, 0.55f, 0.13f));
      add_color("seagreen", glm::vec3(0.18f, 0.55f, 0.34f));
      add_color("darkslategray", glm::vec3(0.18f, 0.31f, 0.31f));
      add_color("darkslategrey", glm::vec3(0.18f, 0.31f, 0.31f));
      add_color("limegreen", glm::vec3(0.2f, 0.8f, 0.2f));
      add_color("mediumseagreen", glm::vec3(0.24f, 0.7f, 0.44f));
      add_color("turquoise", glm::vec3(0.25f, 0.88f, 0.82f));
      add_color("royalblue", glm::vec3(0.25f, 0.41f, 0.88f));
      add_color("steelblue", glm::vec3(0.27f, 0.51f, 0.71f));
      add_color("darkslateblue", glm::vec3(0.28f, 0.24f, 0.55f));
      add_color("mediumturquoise", glm::vec3(0.28f, 0.82f, 0.8f));
      add_color("indigo", glm::vec3(0.29f, 0.f, 0.51f));
      add_color("darkolivegreen", glm::vec3(0.33f, 0.42f, 0.18f));
      add_color("cadetblue", glm::vec3(0.37f, 0.62f, 0.63f));
      add_color("cornflowerblue", glm::vec3(0.39f, 0.58f, 0.93f));
      add_color("mediumaquamarine", glm::vec3(0.4f, 0.8f, 0.67f));
      add_color("dimgrey", glm::vec3(0.41f, 0.41f, 0.41f));
      add_color("dimgray", glm::vec3(0.41f, 0.41f, 0.41f));
      add_color("slateblue", glm::vec3(0.42f, 0.35f, 0.8f));
      add_color("olivedrab", glm::vec3(0.42f, 0.56f, 0.14f));
      add_color("slategrey", glm::vec3(0.44f, 0.5f, 0.56f));
      add_color("slategray", glm::vec3(0.44f, 0.5f, 0.56f));
      add_color("lightslategray", glm::vec3(0.47f, 0.53f, 0.6f));
      add_color("lightslategrey", glm::vec3(0.47f, 0.53f, 0.6f));
      add_color("mediumslateblue", glm::vec3(0.48f, 0.41f, 0.93f));
      add_color("lawngreen", glm::vec3(0.49f, 0.99f, 0.f));
      add_color("chartreuse", glm::vec3(0.5f, 1.f, 0.f));
      add_color("aquamarine", glm::vec3(0.5f, 1.f, 0.83f));
      add_color("maroon", glm::vec3(0.5f, 0.f, 0.f));
      add_color("purple", glm::vec3(0.5f, 0.f, 0.5f));
      add_color("olive", glm::vec3(0.5f, 0.5f, 0.f));
      add_color("gray", glm::vec3(0.5f, 0.5f, 0.5f));
      add_color("grey", glm::vec3(0.5f, 0.5f, 0.5f));
      add_color("skyblue", glm::vec3(0.53f, 0.81f, 0.92f));
      add_color("lightskyblue", glm::vec3(0.53f, 0.81f, 0.98f));
      add_color("blueviolet", glm::vec3(0.54f, 0.17f, 0.89f));
      add_color("darkred", glm::vec3(0.55f, 0.f, 0.f));
      add_color("darkmagenta", glm::vec3(0.55f, 0.f, 0.55f));
      add_color("saddlebrown", glm::vec3(0.55f, 0.27f, 0.7f));
      add_color("darkseagreen", glm::vec3(0.56f, 0.74f, 0.56f));
      add_color("lightgreen", glm::vec3(0.56f, 0.93f, 0.56f));
      add_color("mediumpurple", glm::vec3(0.58f, 0.44f, 0.86f));
      add_color("darkviolet", glm::vec3(0.58f, 0.f, 0.83f));
      add_color("palegreen", glm::vec3(0.6f, 0.98f, 0.6f));
      add_color("darkorchid", glm::vec3(0.6f, 0.2f, 0.8f));
      add_color("yellowgreen", glm::vec3(0.6f, 0.8f, 0.2f));
      add_color("sienna", glm::vec3(0.63f, 0.32f, 0.18f));
      add_color("brown", glm::vec3(0.65f, 0.16f, 0.16f));
      add_color("darkgray", glm::vec3(0.66f, 0.66f, 0.66f));
      add_color("darkgrey", glm::vec3(0.66f, 0.66f, 0.66f));
      add_color("lightblue", glm::vec3(0.68f, 0.85f, 0.9f));
      add_color("greenyellow", glm::vec3(0.68f, 1.f, 0.18f));
      add_color("paleturquoise", glm::vec3(0.69f, 0.93f, 0.93f));
      add_color("lightsteelblue", glm::vec3(0.69f, 0.77f, 0.87f));
      add_color("powderblue", glm::vec3(0.69f, 0.88f, 0.9f));
      add_color("firebrick", glm::vec3(0.7f, 0.13f, 0.13f));
      add_color("darkgoldenrod", glm::vec3(0.72f, 0.53f, 0.4f));
      add_color("mediumorchid", glm::vec3(0.73f, 0.33f, 0.83f));
      add_color("rosybrown", glm::vec3(0.74f, 0.56f, 0.56f));
      add_color("darkkhaki", glm::vec3(0.74f, 0.72f, 0.42f));
      add_color("silver", glm::vec3(0.75f, 0.75f, 0.75f));
      add_color("mediumvioletred", glm::vec3(0.78f, 0.08f, 0.52f));
      add_color("indianred", glm::vec3(0.8f, 0.36f, 0.36f));
      add_color("peru", glm::vec3(0.8f, 0.52f, 0.25f));
      add_color("chocolate", glm::vec3(0.82f, 0.41f, 0.12f));
      add_color("tan", glm::vec3(0.82f, 0.71f, 0.55f));
      add_color("lightgray", glm::vec3(0.83f, 0.83f, 0.83f));
      add_color("lightgrey", glm::vec3(0.83f, 0.83f, 0.83f));
      add_color("thistle", glm::vec3(0.85f, 0.75f, 0.85f));
      add_color("orchid", glm::vec3(0.85f, 0.44f, 0.84f));
      add_color("goldenrod", glm::vec3(0.85f, 0.65f, 0.13f));
      add_color("palevioletred", glm::vec3(0.86f, 0.44f, 0.58f));
      add_color("crimson", glm::vec3(0.86f, 0.08f, 0.24f));
      add_color("gainsboro", glm::vec3(0.86f, 0.86f, 0.86f));
      add_color("plum", glm::vec3(0.87f, 0.63f, 0.87f));
      add_color("burlywood", glm::vec3(0.87f, 0.72f, 0.53f));
      add_color("lightcyan", glm::vec3(0.88f, 1.f, 1.f));
      add_color("lavender", glm::vec3(0.9f, 0.9f, 0.98f));
      add_color("darksalmon", glm::vec3(0.91f, 0.59f, 0.48f));
      add_color("violet", glm::vec3(0.93f, 0.51f, 0.93f));
      add_color("palegoldenrod", glm::vec3(0.93f, 0.91f, 0.67f));
      add_color("lightcoral", glm::vec3(0.94f, 0.5f, 0.5f));
      add_color("khaki", glm::vec3(0.94f, 0.9f, 0.55f));
      add_color("aliceblue", glm::vec3(0.94f, 0.97f, 1.f));
      add_color("honeydew", glm::vec3(0.94f, 1.f, 0.94f));
      add_color("azure", glm::vec3(0.94f, 1.f, 1.f));
      add_color("sandybrown", glm::vec3(0.96f, 0.64f, 0.38f));
      add_color("wheat", glm::vec3(0.96f, 0.87f, 0.7f));
      add_color("beige", glm::vec3(0.96f, 0.96f, 0.86f));
      add_color("whitesmoke", glm::vec3(0.96f, 0.96f, 0.96f));
      add_color("mintcream", glm::vec3(0.96f, 1.f, 0.98f));
      add_color("ghostwhite", glm::vec3(0.97f, 0.97f, 1.f));
      add_color("salmon", glm::vec3(0.98f, 0.5f, 0.45f));
      add_color("antiquewhite", glm::vec3(0.98f, 0.92f, 0.84f));
      add_color("linen", glm::vec3(0.98f, 0.94f, 0.9f));
      add_color("lightgoldenrodyellow", glm::vec3(0.98f, 0.98f, 0.82f));
      add_color("oldlace", glm::vec3(0.99f, 0.96f, 0.9f));
      add_color("red", glm::vec3(1.f, 0.f, 0.f));
      add_color("fuchsia", glm::vec3(1.f, 0.f, 1.f));
      add_color("magenta", glm::vec3(1.f, 0.f, 1.f));
      add_color("deeppink", glm::vec3(1.f, 0.08f, 0.58f));
      add_color("orangered", glm::vec3(1.f, 0.27f, 0.f));
      add_color("tomato", glm::vec3(1.f, 0.39f, 0.28f));
      add_color("hotpink", glm::vec3(1.f, 0.41f, 0.71f));
      add_color("coral", glm::vec3(1.f, 0.5f, 0.31f));
      add_color("darkorange", glm::vec3(1.f, 0.55f, 0.f));
      add_color("lightsalmon", glm::vec3(1.f, 0.63f, 0.48f));
      add_color("orange", glm::vec3(1.f, 0.65f, 0.f));
      add_color("lightpink", glm::vec3(1.f, 0.71f, 0.76f));
      add_color("pink", glm::vec3(1.f, 0.75f, 0.8f));
      add_color("gold", glm::vec3(1.f, 0.84f, 0.f));
      add_color("peachpuff", glm::vec3(1.f, 0.85f, 0.73f));
      add_color("navajowhite", glm::vec3(1.f, 0.87f, 0.68f));
      add_color("moccasin", glm::vec3(1.f, 0.89f, 0.71f));
      add_color("bisque", glm::vec3(1.f, 0.89f, 0.77f));
      add_color("mistyrose", glm::vec3(1.f, 0.89f, 0.88f));
      add_color("blanchdiamond", glm::vec3(1.f, 0.92f, 0.8f));
      add_color("papayawhip", glm::vec3(1.f, 0.94f, 0.84f));
      add_color("lavenderblush", glm::vec3(1.f, 0.94f, 0.96f));
      add_color("seashell", glm::vec3(1.f, 0.96f, 0.93f));
      add_color("cornsilk", glm::vec3(1.f, 0.97f, 0.86f));
      add_color("lemonchiffon", glm::vec3(1.f, 0.98f, 0.8f));
      add_color("floralwhite", glm::vec3(1.f, 0.98f, 0.94f));
      add_color("snow", glm::vec3(1.f, 0.98f, 0.98f));
      add_color("yellow", glm::vec3(1.f, 1.f, 0.f));
      add_color("lightyellow", glm::vec3(1.f, 1.f, 0.88f));
      add_color("ivory", glm::vec3(1.f, 1.f, 0.94f));
      add_color("white", glm::vec3(1.f, 1.f, 1.f));
      start_new_colors = name_list.size();
    }

    inline glm::vec3 get_color(uint32_t i) const { 
      if (i >= rgb_list.size()) throw "Color index out of bounds";
      return rgb_list[i]; 
    }

    inline uint32_t get_start() const {
      return start_new_colors;
    }

    inline uint32_t get_num_colors() const {
      return rgb_list.size();
    }

    uint32_t add_color(glm::vec3 rgb) { // returns index of newly added color
      rgb_list.push_back(rgb);
      return rgb_list.size() - 1;
    }

    uint32_t get_color_index(std::string name) {
      uint32_t dist = distance(name_list.begin(), find(name_list.begin(), name_list.end(), name));
      if (dist == name_list.size()) {
        std::cerr << "Error: Color not found" << std::endl;
        return 0;
      }
      return dist;
    }

  private:
    std::vector<glm::vec3> rgb_list;
    std::vector<std::string> name_list;
    uint32_t start_new_colors;

    void add_color(std::string name, glm::vec3 rgb) {
      rgb_list.push_back(rgb);
      name_list.push_back(name);
    }
};