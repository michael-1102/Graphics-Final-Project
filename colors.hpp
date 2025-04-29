#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <bits/stdc++.h>

class colors {
  public:
    colors() {
      add_color("black", glm::vec4(0.f, 0.f, 0.f, 1.f));
      add_color("navy", glm::vec4(0.f, 0.f, 0.5f, 1.f));
      add_color("darkblue", glm::vec4(0.f, 0.39f, 0.f, 1.f));
      add_color("mediumblue", glm::vec4(0.f, 0.f, 0.8f, 1.f));
      add_color("blue", glm::vec4(0.f, 0.f, 1.f, 1.f));
      add_color("darkgreen", glm::vec4(0.f, 0.39f, 0.f, 1.f));
      add_color("green", glm::vec4(0.f, 0.5f, 0.f, 1.f));
      add_color("teal", glm::vec4(0.f, 0.5f, 0.5f, 1.f));
      add_color("darkcyan", glm::vec4(0.f, 0.55f, 0.55f, 1.f));
      add_color("deepskyblue", glm::vec4(0.f, 0.75f, 1.f, 1.f));
      add_color("darkturquoise", glm::vec4(0.f, 0.81f, 0.82f, 1.f));
      add_color("mediumspringgreen", glm::vec4(0.f, 0.98f, 0.6f, 1.f));
      add_color("lime", glm::vec4(0.f, 1.f, 0.f, 1.f));
      add_color("springgreen", glm::vec4(0.f, 1.f, 0.5f, 1.f));
      add_color("cyan", glm::vec4(0.f, 1.f, 1.f, 1.f)); // some colors are repeats such as cyan and aqua
      add_color("aqua", glm::vec4(0.f, 1.f, 1.f, 1.f)); // TODO: better way to handle repeat colors?
      add_color("midnightblue", glm::vec4(0.1f, 0.1f, 0.44f, 1.f));
      add_color("dodgerblue", glm::vec4(0.12f, 0.56f, 1.f, 1.f));
      add_color("lightseagreen", glm::vec4(0.13f, 0.7f, 0.67f, 1.f));
      add_color("forestgreen", glm::vec4(0.13f, 0.55f, 0.13f, 1.f));
      add_color("seagreen", glm::vec4(0.18f, 0.55f, 0.34f, 1.f));
      add_color("darkslategray", glm::vec4(0.18f, 0.31f, 0.31f, 1.f));
      add_color("darkslategrey", glm::vec4(0.18f, 0.31f, 0.31f, 1.f));
      add_color("limegreen", glm::vec4(0.2f, 0.8f, 0.2f, 1.f));
      add_color("mediumseagreen", glm::vec4(0.24f, 0.7f, 0.44f, 1.f));
      add_color("turquoise", glm::vec4(0.25f, 0.88f, 0.82f, 1.f));
      add_color("royalblue", glm::vec4(0.25f, 0.41f, 0.88f, 1.f));
      add_color("steelblue", glm::vec4(0.27f, 0.51f, 0.71f, 1.f));
      add_color("darkslateblue", glm::vec4(0.28f, 0.24f, 0.55f, 1.f));
      add_color("mediumturquoise", glm::vec4(0.28f, 0.82f, 0.8f, 1.f));
      add_color("indigo", glm::vec4(0.29f, 0.f, 0.51f, 1.f));
      add_color("darkolivegreen", glm::vec4(0.33f, 0.42f, 0.18f, 1.f));
      add_color("cadetblue", glm::vec4(0.37f, 0.62f, 0.63f, 1.f));
      add_color("cornflowerblue", glm::vec4(0.39f, 0.58f, 0.93f, 1.f));
      add_color("mediumaquamarine", glm::vec4(0.4f, 0.8f, 0.67f, 1.f));
      add_color("dimgrey", glm::vec4(0.41f, 0.41f, 0.41f, 1.f));
      add_color("dimgray", glm::vec4(0.41f, 0.41f, 0.41f, 1.f));
      add_color("slateblue", glm::vec4(0.42f, 0.35f, 0.8f, 1.f));
      add_color("olivedrab", glm::vec4(0.42f, 0.56f, 0.14f, 1.f));
      add_color("slategrey", glm::vec4(0.44f, 0.5f, 0.56f, 1.f));
      add_color("slategray", glm::vec4(0.44f, 0.5f, 0.56f, 1.f));
      add_color("lightslategray", glm::vec4(0.47f, 0.53f, 0.6f, 1.f));
      add_color("lightslategrey", glm::vec4(0.47f, 0.53f, 0.6f, 1.f));
      add_color("mediumslateblue", glm::vec4(0.48f, 0.41f, 0.93f, 1.f));
      add_color("lawngreen", glm::vec4(0.49f, 0.99f, 0.f, 1.f));
      add_color("chartreuse", glm::vec4(0.5f, 1.f, 0.f, 1.f));
      add_color("aquamarine", glm::vec4(0.5f, 1.f, 0.83f, 1.f));
      add_color("maroon", glm::vec4(0.5f, 0.f, 0.f, 1.f));
      add_color("purple", glm::vec4(0.5f, 0.f, 0.5f, 1.f));
      add_color("olive", glm::vec4(0.5f, 0.5f, 0.f, 1.f));
      add_color("gray", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
      add_color("grey", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
      add_color("skyblue", glm::vec4(0.53f, 0.81f, 0.92f, 1.f));
      add_color("lightskyblue", glm::vec4(0.53f, 0.81f, 0.98f, 1.f));
      add_color("blueviolet", glm::vec4(0.54f, 0.17f, 0.89f, 1.f));
      add_color("darkred", glm::vec4(0.55f, 0.f, 0.f, 1.f));
      add_color("darkmagenta", glm::vec4(0.55f, 0.f, 0.55f, 1.f));
      add_color("saddlebrown", glm::vec4(0.55f, 0.27f, 0.7f, 1.f));
      add_color("darkseagreen", glm::vec4(0.56f, 0.74f, 0.56f, 1.f));
      add_color("lightgreen", glm::vec4(0.56f, 0.93f, 0.56f, 1.f));
      add_color("mediumpurple", glm::vec4(0.58f, 0.44f, 0.86f, 1.f));
      add_color("darkviolet", glm::vec4(0.58f, 0.f, 0.83f, 1.f));
      add_color("palegreen", glm::vec4(0.6f, 0.98f, 0.6f, 1.f));
      add_color("darkorchid", glm::vec4(0.6f, 0.2f, 0.8f, 1.f));
      add_color("yellowgreen", glm::vec4(0.6f, 0.8f, 0.2f, 1.f));
      add_color("sienna", glm::vec4(0.63f, 0.32f, 0.18f, 1.f));
      add_color("brown", glm::vec4(0.65f, 0.16f, 0.16f, 1.f));
      add_color("darkgray", glm::vec4(0.66f, 0.66f, 0.66f, 1.f));
      add_color("darkgrey", glm::vec4(0.66f, 0.66f, 0.66f, 1.f));
      add_color("lightblue", glm::vec4(0.68f, 0.85f, 0.9f, 1.f));
      add_color("greenyellow", glm::vec4(0.68f, 1.f, 0.18f, 1.f));
      add_color("paleturquoise", glm::vec4(0.69f, 0.93f, 0.93f, 1.f));
      add_color("lightsteelblue", glm::vec4(0.69f, 0.77f, 0.87f, 1.f));
      add_color("powderblue", glm::vec4(0.69f, 0.88f, 0.9f, 1.f));
      add_color("firebrick", glm::vec4(0.7f, 0.13f, 0.13f, 1.f));
      add_color("darkgoldenrod", glm::vec4(0.72f, 0.53f, 0.4f, 1.f));
      add_color("mediumorchid", glm::vec4(0.73f, 0.33f, 0.83f, 1.f));
      add_color("rosybrown", glm::vec4(0.74f, 0.56f, 0.56f, 1.f));
      add_color("darkkhaki", glm::vec4(0.74f, 0.72f, 0.42f, 1.f));
      add_color("silver", glm::vec4(0.75f, 0.75f, 0.75f, 1.f));
      add_color("mediumvioletred", glm::vec4(0.78f, 0.08f, 0.52f, 1.f));
      add_color("indianred", glm::vec4(0.8f, 0.36f, 0.36f, 1.f));
      add_color("peru", glm::vec4(0.8f, 0.52f, 0.25f, 1.f));
      add_color("chocolate", glm::vec4(0.82f, 0.41f, 0.12f, 1.f));
      add_color("tan", glm::vec4(0.82f, 0.71f, 0.55f, 1.f));
      add_color("lightgray", glm::vec4(0.83f, 0.83f, 0.83f, 1.f));
      add_color("lightgrey", glm::vec4(0.83f, 0.83f, 0.83f, 1.f));
      add_color("thistle", glm::vec4(0.85f, 0.75f, 0.85f, 1.f));
      add_color("orchid", glm::vec4(0.85f, 0.44f, 0.84f, 1.f));
      add_color("goldenrod", glm::vec4(0.85f, 0.65f, 0.13f, 1.f));
      add_color("palevioletred", glm::vec4(0.86f, 0.44f, 0.58f, 1.f));
      add_color("crimson", glm::vec4(0.86f, 0.08f, 0.24f, 1.f));
      add_color("gainsboro", glm::vec4(0.86f, 0.86f, 0.86f, 1.f));
      add_color("plum", glm::vec4(0.87f, 0.63f, 0.87f, 1.f));
      add_color("burlywood", glm::vec4(0.87f, 0.72f, 0.53f, 1.f));
      add_color("lightcyan", glm::vec4(0.88f, 1.f, 1.f, 1.f));
      add_color("lavender", glm::vec4(0.9f, 0.9f, 0.98f, 1.f));
      add_color("darksalmon", glm::vec4(0.91f, 0.59f, 0.48f, 1.f));
      add_color("violet", glm::vec4(0.93f, 0.51f, 0.93f, 1.f));
      add_color("palegoldenrod", glm::vec4(0.93f, 0.91f, 0.67f, 1.f));
      add_color("lightcoral", glm::vec4(0.94f, 0.5f, 0.5f, 1.f));
      add_color("khaki", glm::vec4(0.94f, 0.9f, 0.55f, 1.f));
      add_color("aliceblue", glm::vec4(0.94f, 0.97f, 1.f, 1.f));
      add_color("honeydew", glm::vec4(0.94f, 1.f, 0.94f, 1.f));
      add_color("azure", glm::vec4(0.94f, 1.f, 1.f, 1.f));
      add_color("sandybrown", glm::vec4(0.96f, 0.64f, 0.38f, 1.f));
      add_color("wheat", glm::vec4(0.96f, 0.87f, 0.7f, 1.f));
      add_color("beige", glm::vec4(0.96f, 0.96f, 0.86f, 1.f));
      add_color("whitesmoke", glm::vec4(0.96f, 0.96f, 0.96f, 1.f));
      add_color("mintcream", glm::vec4(0.96f, 1.f, 0.98f, 1.f));
      add_color("ghostwhite", glm::vec4(0.97f, 0.97f, 1.f, 1.f));
      add_color("salmon", glm::vec4(0.98f, 0.5f, 0.45f, 1.f));
      add_color("antiquewhite", glm::vec4(0.98f, 0.92f, 0.84f, 1.f));
      add_color("linen", glm::vec4(0.98f, 0.94f, 0.9f, 1.f));
      add_color("lightgoldenrodyellow", glm::vec4(0.98f, 0.98f, 0.82f, 1.f));
      add_color("oldlace", glm::vec4(0.99f, 0.96f, 0.9f, 1.f));
      add_color("red", glm::vec4(1.f, 0.f, 0.f, 1.f));
      add_color("fuchsia", glm::vec4(1.f, 0.f, 1.f, 1.f));
      add_color("magenta", glm::vec4(1.f, 0.f, 1.f, 1.f));
      add_color("deeppink", glm::vec4(1.f, 0.08f, 0.58f, 1.f));
      add_color("orangered", glm::vec4(1.f, 0.27f, 0.f, 1.f));
      add_color("tomato", glm::vec4(1.f, 0.39f, 0.28f, 1.f));
      add_color("hotpink", glm::vec4(1.f, 0.41f, 0.71f, 1.f));
      add_color("coral", glm::vec4(1.f, 0.5f, 0.31f, 1.f));
      add_color("darkorange", glm::vec4(1.f, 0.55f, 0.f, 1.f));
      add_color("lightsalmon", glm::vec4(1.f, 0.63f, 0.48f, 1.f));
      add_color("orange", glm::vec4(1.f, 65.f, 0.f, 1.f));
      add_color("lightpink", glm::vec4(1.f, 0.71f, 0.76f, 1.f));
      add_color("pink", glm::vec4(1.f, 0.75f, 0.8f, 1.f));
      add_color("gold", glm::vec4(1.f, 0.84f, 0.f, 1.f));
      add_color("peachpuff", glm::vec4(1.f, 0.85f, 0.73f, 1.f));
      add_color("navajowhite", glm::vec4(1.f, 0.87f, 0.68f, 1.f));
      add_color("moccasin", glm::vec4(1.f, 0.89f, 0.71f, 1.f));
      add_color("bisque", glm::vec4(1.f, 0.89f, 0.77f, 1.f));
      add_color("mistyrose", glm::vec4(1.f, 0.89f, 0.88f, 1.f));
      add_color("blanchdiamond", glm::vec4(1.f, 0.92f, 0.8f, 1.f));
      add_color("papayawhip", glm::vec4(1.f, 0.94f, 0.84f, 1.f));
      add_color("lavenderblush", glm::vec4(1.f, 0.94f, 0.96f, 1.f));
      add_color("seashell", glm::vec4(1.f, 0.96f, 0.93f, 1.f));
      add_color("cornsilk", glm::vec4(1.f, 0.97f, 0.86f, 1.f));
      add_color("lemonchiffon", glm::vec4(1.f, 0.98f, 0.8f, 1.f));
      add_color("floralwhite", glm::vec4(1.f, 0.98f, 0.94f, 1.f));
      add_color("snow", glm::vec4(1.f, 0.98f, 0.98f, 1.f));
      add_color("yellow", glm::vec4(1.f, 1.f, 0.f, 1.f));
      add_color("lightyellow", glm::vec4(1.f, 1.f, 0.88f, 1.f));
      add_color("ivory", glm::vec4(1.f, 1.f, 0.94f, 1.f));
      add_color("white", glm::vec4(1.f, 1.f, 1.f, 1.f));
    }

    inline glm::vec4& get_color(uint32_t i) { 
      if (i >= rgba_list.size()) throw "Color index out of bounds";
      return rgba_list[i]; 
    }
    uint32_t add_color(glm::vec4 rgba) { // returns index of newly added color
      rgba_list.push_back(rgba);
      return rgba_list.size() - 1;
    }

    uint32_t get_color_index(std::string name) {
      return distance(name_list.begin(), find(name_list.begin(), name_list.end(), name));
    } 


  private:
    std::vector<glm::vec4> rgba_list;
    std::vector<std::string> name_list;

    void add_color(std::string name, glm::vec4 rgba) {
      rgba_list.push_back(rgba);
      name_list.push_back(name);
    }
};