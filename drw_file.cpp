#include "drw_file.hpp"
#include "block_loader.hpp"

template<typename T>
std::vector<T> delta_encode(std::vector<T> vec) {
  if (vec.size() == 0) return vec;
  for (uint32_t i = vec.size() - 1; i > 0; i--) {
    vec[i] -= vec[i - 1];
  }
  return vec;
}

std::vector<std::string> resplit(const std::string &s, const std::regex &sep_regex = std::regex{"\\s+"}) {
  std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
  std::sregex_token_iterator end;
  return {iter, end};
}

drawing& drw_file::create_main_drawing() {
  main_drawing = drawing();
  return main_drawing;
}

void drw_file::apply_svg_attributes(XMLParser::ElementContext* svg) {
  std::vector<XMLParser::AttributeContext *> attributes = svg->attribute();
  bool has_viewbox = false, has_width = false, has_height = false;
  view v;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "viewBox") {
      has_viewbox = true;
      std::vector<std::string> values = resplit(attrib_value, std::regex{"[ ,]+"});
      uint32_t x = string_to_float(values[0]);
      uint32_t y = string_to_float(values[1]);
      uint32_t w = string_to_float(values[2]);
      uint32_t h = string_to_float(values[3]);
      v = view(x, y, w, h);
      main_drawing.set_view(v);
    } else if (attrib_name == "width") {
      has_width = true;
      width = string_to_float(attrib_value);
    } else if (attrib_name == "height") {
      has_height = true;
      height = string_to_float(attrib_value);
    } else if (attrib_name == "xmlns") {
      // do nothing with namespace
    } else if (attrib_name == "version") {
      // do nothing with version, at least for now
    } else {
      std::cout << "Unsupported svg attribute: " << attrib_name << std::endl;
    }
  }
  if (!has_viewbox) {
    v = view(0, 0, width, height);
    main_drawing.set_view(v);
  } else if (!has_height && !has_width) {
    width = v.width;
    height = v.height;
  } else if (!has_height) {
    height = width * v.height / v.width;
  } else if (!has_width) {
    width = height * v.width / v.height;
  }
  scale = glm::scale(glm::mat4(1.f), glm::vec3(1.f, width/height, 1.f));
}

// parse svg
drw_file::drw_file(const char filename[]) {
  std::ifstream stream;
  stream.open(filename);
  if (!stream.is_open()) throw "Failed to open file";
  ANTLRInputStream input(stream);
  XMLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  XMLParser parser(&tokens);

  XMLParser::DocumentContext* doc = parser.document();

  XMLParser::ElementContext* svg = doc->element();
  // std::cout << svg->toStringTree(&parser) << std::endl;

  if (svg->Name()[0]->getText() != "svg") {
    throw "Error: SVG must be first element of document";
  }

  apply_svg_attributes(svg);

  XMLParser::ContentContext* content = svg->content();
  std::vector<XMLParser::ElementContext*> elements = content->element();
  add_transform(glm::mat4(1.f));
  add_svg_elements(elements, group_attributes());

  stream.close();
}

void drw_file::add_svg_elements(std::vector<XMLParser::ElementContext*> elements, group_attributes attribs) {
  for (auto elem : elements) {
    std::string elem_name = elem->Name()[0]->getText();
    if (elem_name == "line") {
      add_line(elem, attribs);
    } else if (elem_name == "circle") {
      add_circle(elem, attribs);
    } else if (elem_name == "ellipse") {
      add_ellipse(elem, attribs);
    } else if (elem_name == "rect") {
      add_rect(elem, attribs);
    } else if (elem_name == "polyline") {
      add_poly(elem, attribs, false);
    } else if (elem_name == "polygon") {
      add_poly(elem, attribs, true);
    } else if (elem_name == "path") {
      add_path(elem, attribs);
    } else if (elem_name == "g") {
      group_attributes group_attribs = apply_group_attributes(elem, attribs);
      add_svg_elements(elem->content()->element(), group_attribs);
    } else {
      std::cout << "Unsupported element: " << elem_name << std::endl;
    }
  }
}

drw_file::group_attributes drw_file::apply_group_attributes(XMLParser::ElementContext* elem, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext *> attributes = elem->attribute();
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.has_fill = true;
        attribs.fill_color_index = string_to_color_index(attrib_value);
      }
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill-opacity") {
      attribs.fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported group attribute: " << attrib_name << std::endl;
    }
  }
  return attribs;
}

void drw_file::add_poly(XMLParser::ElementContext* element, group_attributes attribs, bool connect_ends) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  std::vector<glm::vec2> points;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "points") {
      std::vector<std::string> values = resplit(attrib_value, std::regex{"[ ,]+"});
      for (uint32_t i = 0; i < values.size(); i+=2) {
        points.push_back(glm::vec2(string_to_float(values[i]), string_to_float(values[i + 1])));
      }
    } else if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.fill_color_index = string_to_color_index(attrib_value);
        attribs.has_fill = true;
      }
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill-opacity") {
      attribs.fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported circle attribute: " << attrib_name << std::endl;
    }
  }

  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  if (connect_ends) {
    if (attribs.has_fill) shape->add_fill_polygon(points, attribs.fill_color_index, attribs.fill_opacity);
    if (attribs.has_stroke) shape->add_draw_polygon(points, attribs.stroke_color_index, attribs.stroke_opacity);
  } else {
    if (attribs.has_fill) shape->add_fill_polyline(points, attribs.fill_color_index, attribs.fill_opacity);
    if (attribs.has_stroke) shape->add_draw_polyline(points, attribs.stroke_color_index, attribs.stroke_opacity);
  }
}

void drw_file::add_path(XMLParser::ElementContext* element, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  std::string d;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks  
    if (attrib_name == "d") {
      d = attrib_value;
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.has_fill = true;
        attribs.fill_color_index = string_to_color_index(attrib_value);
    }
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "fill-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported path attribute: " << attrib_name << std::endl;
    }
  }

  if (!attribs.has_stroke) {
    attribs.stroke_opacity = 0.f;
  }
  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  parse_path_points(shape, d, attribs);
}

void drw_file::parse_path_points(styled_multishape_2d* shape, std::string d, group_attributes attribs) {
  d = add_spaces(d);
  glm::vec2 start;
  glm::vec2 cursor;
  std::string prev_control = " ";
  std::vector<std::string> tokens = resplit(d, std::regex{"[ ,]+"});
  if (tokens[0] == "M") {
    cursor.x = string_to_float(tokens[1]);
    cursor.y = string_to_float(tokens[2]);
  } else if (tokens[0] == "m") {
    cursor.x += string_to_float(tokens[1]);
      cursor.y += string_to_float(tokens[2]);
  } else {
    std::cout << "Error: path must start with move to command" << std::endl;
    return;
  }
  start = cursor;
  for (uint32_t i = 3; i < tokens.size(); i++) {
    if (tokens[i] == "M") {
      cursor.x = string_to_float(tokens[++i]);
      cursor.y = string_to_float(tokens[++i]);
    } else if (tokens[i] == "m") {
      cursor.x += string_to_float(tokens[++i]);
      cursor.y += string_to_float(tokens[++i]);
    } else if (tokens[i] == "L") {
      shape->add_draw_line(cursor.x, cursor.y, string_to_float(tokens[i + 1]), string_to_float(tokens[i + 2]), attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.x = string_to_float(tokens[++i]);
      cursor.y = string_to_float(tokens[++i]);
    } else if (tokens[i] == "l") {
      shape->add_draw_line(cursor.x, cursor.y, cursor.x + string_to_float(tokens[i + 1]), cursor.y + string_to_float(tokens[i + 2]), attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.x += string_to_float(tokens[++i]);
      cursor.y += string_to_float(tokens[++i]);
    } else if (tokens[i] == "H") {
      shape->add_draw_line(cursor.x, cursor.y, string_to_float(tokens[i + 1]), cursor.y, attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.x = string_to_float(tokens[++i]);
    } else if (tokens[i] == "h") {
      shape->add_draw_line(cursor.x, cursor.y, cursor.x + string_to_float(tokens[i + 1]), cursor.y, attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.x += string_to_float(tokens[++i]);
    } else if (tokens[i] == "V") {
      shape->add_draw_line(cursor.x, cursor.y, cursor.x, string_to_float(tokens[i + 1]), attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.y = string_to_float(tokens[++i]);
    } else if (tokens[i] == "v") {
      shape->add_draw_line(cursor.x, cursor.y, cursor.x, cursor.y + string_to_float(tokens[i + 1]), attribs.stroke_color_index, attribs.stroke_opacity);
      cursor.y += string_to_float(tokens[++i]);
    } else if (tokens[i] == "Z" || tokens[i] == "z") {
      shape->add_draw_line(cursor.x, cursor.y, start.x, start.y, attribs.stroke_color_index, attribs.stroke_opacity);
      cursor = start;
    } else if (tokens[i] == "C") {
    } else if (tokens[i] == "c") {
    } else if (tokens[i] == "S") {
    } else if (tokens[i] == "s") {
    } else if (tokens[i] == "Q") {
    } else if (tokens[i] == "q") {
    } else if (tokens[i] == "T") {
    } else if (tokens[i] == "t") {
    } else if (tokens[i] == "A") {
    } else if (tokens[i] == "a") {
    } else {
      std::cout << "Error while parsing path" << std::endl;
      return;
    }
    prev_control = tokens[i];
  }
  if (attribs.has_fill) {
    //TODO: support path with fill
    std::cout << "Filled path not currently supported" << std::endl;
  }
}

std::string drw_file::add_spaces(const std::string& str) {
  std::string out;
  for (char ch : str) { 
    out += ch;
    if (isalpha(ch)) {
      out += ' ';
    }
  }
  return out;
}

void drw_file::add_line(XMLParser::ElementContext* element, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "x1") {
      x1 = string_to_float(attrib_value);
    } else if (attrib_name == "y1") {
      y1 = string_to_float(attrib_value);
    } else if (attrib_name == "x2") {
      x2 = string_to_float(attrib_value);
    } else if (attrib_name == "y2") {
      y2 = string_to_float(attrib_value);
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.has_stroke = true;
        attribs.stroke_color_index = string_to_color_index(attrib_value);
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported line attribute: " << attrib_name << std::endl;
    }
  }
  if (!attribs.has_stroke) return;
  multishape_2d* shape = main_drawing.create_multishape_2d(attribs.stroke_width, attribs.stroke_color_index, attribs.transform_index, attribs.stroke_opacity);
  shape->add_draw_line(x1, y1, x2, y2);
}

void drw_file::add_ellipse(XMLParser::ElementContext* element, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  float cx = 0, cy = 0, rx = 0, ry = 0;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "cx") {
      cx = string_to_float(attrib_value);
    } else if (attrib_name == "cy") {
      cy = string_to_float(attrib_value);
    } else if (attrib_name == "rx") {
      rx = string_to_float(attrib_value);
    } else if (attrib_name == "ry") {
      ry = string_to_float(attrib_value);
    } else if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.has_fill = true;
        attribs.fill_color_index = string_to_color_index(attrib_value);
      }
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill-opacity") {
      attribs.fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported ellipse attribute: " << attrib_name << std::endl;
    }
  }
  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  if (attribs.has_fill) shape->add_fill_ellipse(cx, cy, rx, ry, NUM_SECTORS, attribs.fill_color_index, attribs.fill_opacity);
  if (attribs.has_stroke) shape->add_draw_ellipse(cx, cy, rx, ry, NUM_SECTORS, attribs.stroke_color_index, attribs.stroke_opacity);
}

void drw_file::add_circle(XMLParser::ElementContext* element, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  float cx = 0, cy = 0, r = 0;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "cx") {
      cx = string_to_float(attrib_value);
    } else if (attrib_name == "cy") {
      cy = string_to_float(attrib_value);
    } else if (attrib_name == "r") {
      r = string_to_float(attrib_value);
    } else if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.fill_color_index = string_to_color_index(attrib_value);
        attribs.has_fill = true;
      }
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill-opacity") {
      attribs.fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported circle attribute: " << attrib_name << std::endl;
    }
  }

  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  if (attribs.has_fill) shape->add_fill_circle(cx, cy, r, NUM_SECTORS, attribs.fill_color_index, attribs.fill_opacity);
  if (attribs.has_stroke) shape->add_draw_circle(cx, cy, r, NUM_SECTORS, attribs.stroke_color_index, attribs.stroke_opacity);
}

void drw_file::add_rect(XMLParser::ElementContext* element, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  float x = 0, y = 0, rx = 0, ry = 0, width = 0, height = 0;
  bool rounded_x = false;
  bool rounded_y = false;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\"|\')"), ""); // remove quotation marks
    if (attrib_name == "x") {
      x = string_to_float(attrib_value);
    } else if (attrib_name == "y") {
      y = string_to_float(attrib_value);
    } else if (attrib_name == "width") {
      width = string_to_float(attrib_value);
    } else if (attrib_name == "height") {
      height = string_to_float(attrib_value);
    } else if (attrib_name == "rx") {
      rounded_x = true;
      rx = string_to_float(attrib_value);
    } else if (attrib_name == "ry") {
      rounded_y = true;
      ry = string_to_float(attrib_value);
    } else if (attrib_name == "fill") {
      if (attrib_value == "transparent" || attrib_value == "none") {
        attribs.has_fill = false;
      } else {
        attribs.fill_color_index = string_to_color_index(attrib_value);
        attribs.has_fill = true;
      }
    } else if (attrib_name == "stroke") {
      if (!(attrib_value == "transparent" || attrib_value == "none")) {
        attribs.stroke_color_index = string_to_color_index(attrib_value);
        attribs.has_stroke = true;
      } else {
        attribs.has_stroke = false;
      }
    } else if (attrib_name == "fill-opacity") {
      attribs.fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      attribs.stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      attribs.stroke_width = string_to_float(attrib_value);
    } else if (attrib_name == "transform") {
      attribs.transform_index = string_to_transform_index(attrib_value);
    } else {
      std::cout << "Unsupported rect attribute: " << attrib_name << std::endl;
    }
  }

  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  if (!(rounded_x && rounded_y)) {
    if (width == height && width > 0) {
      if (attribs.has_fill) shape->add_fill_square(x, y, width, attribs.fill_color_index, attribs.fill_opacity);
      if (attribs.has_stroke) shape->add_draw_square(x, y, width, attribs.stroke_color_index, attribs.stroke_opacity);
    } else {
      if (attribs.has_fill) shape->add_fill_rectangle(x, y, width, height, attribs.fill_color_index, attribs.fill_opacity);
      if (attribs.has_stroke) shape->add_draw_rectangle(x, y, width, height, attribs.stroke_color_index, attribs.stroke_opacity);
    }
  } else {
    if (!rounded_x) rx = ry;
    else if (!rounded_y) ry = rx;
    if (rx > 0.5 * width) rx = 0.5 * width;
    if (ry > 0.5 * width) ry = 0.5 * width;
    if (attribs.has_fill) shape->add_fill_round_rect(x, y, width, height, rx, ry, NUM_SECTORS, attribs.fill_color_index, attribs.fill_opacity);
    if (attribs.has_stroke) shape->add_draw_round_rect(x, y, width, height, rx, ry, NUM_SECTORS, attribs.stroke_color_index, attribs.stroke_opacity);
  }
}

uint32_t drw_file::string_to_transform_index(std::string str) {
  //TODO: parse transform and return transform index
  return 0;
}

void drw_file::save(const char filename[]) const {
  std::ofstream f(filename, std::ios::out | std::ios::binary);
  std::vector<uint16_t> instructions;
  std::vector<uint32_t> uint32s; // any values that are stored as a uint32_t
  std::vector<float> floats;  // any values that are stored as a float besides x, y, z coords

  std::vector<float> x_coords;
  std::vector<float> y_coords;
  std::vector<float> z_coords;

  // save cameras
  for (auto c : cameras) {
    if (c.has_perspective()) {
      instructions.push_back(to_underlying(instruction::CREATE_PERSPECTIVE_CAMERA));
      floats.push_back(c.get_width_to_height());
      glm::vec3 pos = c.get_pos();
      x_coords.push_back(pos.x);
      y_coords.push_back(pos.y);
      z_coords.push_back(pos.z);
      glm::vec3 look_at = c.get_look_at();
      x_coords.push_back(look_at.x);
      y_coords.push_back(look_at.y);
      z_coords.push_back(look_at.z);
      glm::vec3 up = c.get_up();
      floats.push_back(up.x);
      floats.push_back(up.y);
      floats.push_back(up.z);
      floats.push_back(c.get_fov());
      floats.push_back(c.get_z_near());
      floats.push_back(c.get_z_far());
    } else {
      instructions.push_back(to_underlying(instruction::CREATE_ORTHO_CAMERA));
      floats.push_back(c.get_x());
      floats.push_back(c.get_y());
      floats.push_back(c.get_width());
      floats.push_back(c.get_height());
      glm::vec3 pos = c.get_pos();
      x_coords.push_back(pos.x);
      y_coords.push_back(pos.y);
      z_coords.push_back(pos.z);
      glm::vec3 look_at = c.get_look_at();
      x_coords.push_back(look_at.x);
      y_coords.push_back(look_at.y);
      z_coords.push_back(look_at.z);
      glm::vec3 up = c.get_up();
      floats.push_back(up.x);
      floats.push_back(up.y);
      floats.push_back(up.z);
      floats.push_back(c.get_z_near());
      floats.push_back(c.get_z_far());
    }
  }

  // save materials
  for (auto m : materials) {
    instructions.push_back(to_underlying(instruction::CREATE_MATERIAL));
    glm::vec3 ambient = m.ambient;
    floats.push_back(ambient.x);
    floats.push_back(ambient.y);
    floats.push_back(ambient.z);
    glm::vec3 diffuse = m.diffuse;
    floats.push_back(diffuse.x);
    floats.push_back(diffuse.y);
    floats.push_back(diffuse.z);
    glm::vec3 specular = m.specular;
    floats.push_back(specular.x);
    floats.push_back(specular.y);
    floats.push_back(specular.z);

    floats.push_back(m.shininess);
  }

  
  // save point lights
  for (auto pl: point_lights) {
    instructions.push_back(to_underlying(instruction::CREATE_POINT_LIGHT));

    glm::vec3 position = pl.position;
    floats.push_back(position.x);
    floats.push_back(position.y);
    floats.push_back(position.z);

    floats.push_back(pl.constant);
    floats.push_back(pl.linear);
    floats.push_back(pl.quadratic);

    glm::vec3 ambient = pl.ambient;
    floats.push_back(ambient.x);
    floats.push_back(ambient.y);
    floats.push_back(ambient.z);
    glm::vec3 diffuse = pl.diffuse;
    floats.push_back(diffuse.x);
    floats.push_back(diffuse.y);
    floats.push_back(diffuse.z);
    glm::vec3 specular = pl.specular;
    floats.push_back(specular.x);
    floats.push_back(specular.y);
    floats.push_back(specular.z);

  }

  // save spot lights
  for (auto sl: spot_lights) {
    instructions.push_back(to_underlying(instruction::CREATE_SPOT_LIGHT));
    glm::vec3 position = sl.position;
    floats.push_back(position.x);
    floats.push_back(position.y);
    floats.push_back(position.z);

    glm::vec3 direction = sl.direction;
    floats.push_back(direction.x);
    floats.push_back(direction.y);
    floats.push_back(direction.z);

    floats.push_back(sl.cutOff);
    floats.push_back(sl.outerCutOff);
    floats.push_back(sl.constant);
    floats.push_back(sl.linear);
    floats.push_back(sl.quadratic);

    glm::vec3 ambient = sl.ambient;
    floats.push_back(ambient.x);
    floats.push_back(ambient.y);
    floats.push_back(ambient.z);
    glm::vec3 diffuse = sl.diffuse;
    floats.push_back(diffuse.x);
    floats.push_back(diffuse.y);
    floats.push_back(diffuse.z);
    glm::vec3 specular = sl.specular;
    floats.push_back(specular.x);
    floats.push_back(specular.y);
    floats.push_back(specular.z);
  }

  // save dir lights
  for (auto dl : dir_lights) {
    instructions.push_back(to_underlying(instruction::CREATE_DIR_LIGHT));

    glm::vec3 direction = dl.direction;
    floats.push_back(direction.x);
    floats.push_back(direction.y);
    floats.push_back(direction.z);

    glm::vec3 ambient = dl.ambient;
    floats.push_back(ambient.x);
    floats.push_back(ambient.y);
    floats.push_back(ambient.z);
    glm::vec3 diffuse = dl.diffuse;
    floats.push_back(diffuse.x);
    floats.push_back(diffuse.y);
    floats.push_back(diffuse.z);
    glm::vec3 specular = dl.specular;
    floats.push_back(specular.x);
    floats.push_back(specular.y);
    floats.push_back(specular.z);
  }

  main_drawing.save(instructions, uint32s, floats, x_coords, y_coords, z_coords);

  std::vector<float> reds;
  std::vector<float> greens;
  std::vector<float> blues;
  for (uint32_t i = clrs.get_start(); i < clrs.get_num_colors(); i++) {
    glm::vec3 c = clrs.get_color(i);
    reds.push_back(c.r);
    greens.push_back(c.g);
    blues.push_back(c.b);
  }

  uint32_t num_instructions = instructions.size();
  uint32_t num_x_coords = x_coords.size();
  uint32_t num_y_coords = y_coords.size();
  uint32_t num_z_coords = z_coords.size();
  uint32_t num_uint32s = uint32s.size();
  uint32_t num_floats = floats.size();
  uint32_t num_colors = reds.size();
  uint32_t num_transforms = transforms.size();

  block_loader bl(1024, 1);
  size_t total_size = sizeof(drw_header) + 
                      num_instructions * sizeof(uint16_t) + 
                      num_uint32s * sizeof(uint32_t) + 
                      num_x_coords * sizeof(float) + 
                      num_y_coords * sizeof(float) +
                      num_z_coords * sizeof(float) + 
                      num_floats * sizeof(float) +  
                      num_colors * sizeof(glm::vec3) + 
                      num_transforms * sizeof(glm::mat4); 

  bl.grow(total_size);
  drw_header* header = (drw_header*)bl.start_data();
  header->width = width;
  header->height = height;
  header->bg_color_index = bg_color_index;
  header->num_instructions = num_instructions;
  header->num_uint32s = num_uint32s;
  header->num_x_coords = num_x_coords;
  header->num_y_coords = num_y_coords;
  header->num_z_coords = num_z_coords;
  header->num_floats = num_floats;
  header->num_colors = num_colors;
  header->num_transforms = num_transforms;
  bl.increment_size(sizeof(drw_header));


  bl.append(instructions.data(), num_instructions * sizeof(uint16_t));
  bl.append(uint32s.data(), num_uint32s * sizeof(uint32_t));
  bl.append(delta_encode(x_coords).data(), num_x_coords * sizeof(float));
  bl.append(delta_encode(y_coords).data(), num_y_coords * sizeof(float));
  bl.append(delta_encode(z_coords).data(), num_z_coords * sizeof(float));
  bl.append(floats.data(), num_floats * sizeof(float));
  bl.append(delta_encode(reds).data(), num_colors * sizeof(float));
  bl.append(delta_encode(greens).data(), num_colors * sizeof(float));
  bl.append(delta_encode(blues).data(), num_colors * sizeof(float));
  bl.append(transforms.data(), num_transforms * sizeof(glm::mat4));

  bl.save(filename);

  f.close();
}

void drw_file::load(const char filename[]) {
  block_loader bl(filename);
  bl.check_header(1);
  drw_header* mh = (drw_header*)bl.start_data();
  char* p = (char*)(mh + 1);

  width = mh->width;
  height = mh->height;
  scale = glm::scale(glm::mat4(1.f), glm::vec3(1.f, width/height, 1.f));
  bg_color_index = mh->bg_color_index;

  parasitic_vector<uint16_t> instructions = parasitic_vector<uint16_t>(p, mh->num_instructions);
  parasitic_vector<uint32_t> uint32s = parasitic_vector<uint32_t>(p, mh->num_uint32s);
  parasitic_vector<float> x_coords = parasitic_vector<float>(p, mh->num_x_coords);
  x_coords.delta_decode();
  parasitic_vector<float> y_coords = parasitic_vector<float>(p, mh->num_y_coords);
  y_coords.delta_decode();
  parasitic_vector<float> z_coords = parasitic_vector<float>(p, mh->num_z_coords);
  z_coords.delta_decode();
  parasitic_vector<float> floats = parasitic_vector<float>(p, mh->num_floats);
  parasitic_vector<float> reds = parasitic_vector<float>(p, mh->num_colors);
  reds.delta_decode();
  parasitic_vector<float> greens = parasitic_vector<float>(p, mh->num_colors);
  greens.delta_decode();
  parasitic_vector<float> blues = parasitic_vector<float>(p, mh->num_colors);
  blues.delta_decode();
  parasitic_vector<glm::mat4> parasitic_transforms = parasitic_vector<glm::mat4>(p, mh->num_transforms);

  for (uint32_t i = 0; i < mh->num_colors; i++) {
    add_color(glm::vec3(reds[i], greens[i], blues[i]));
  }
  for (uint32_t i = 0; i < mh->num_transforms; i++) {
    add_transform(parasitic_transforms[i]);
  }

  uint32_t current_uint32 = 0;
  uint32_t current_uint16 = 0;
  uint32_t current_x_coord = 0;
  uint32_t current_y_coord = 0;
  uint32_t current_z_coord = 0;
  uint32_t current_float = 0;
  uint32_t current_font_path_offset = 0;
  uint32_t current_textures_text_offset = 0;
  uint32_t current_boxes_text_offset = 0;
  uint32_t current_webp_data_offset = 0;
  for (uint32_t i = 0; i < instructions.size(); i++) {
    switch(static_cast<instruction>(instructions[i])) {
      case (instruction::CREATE_PERSPECTIVE_CAMERA):
        create_camera(floats[current_float], glm::vec3(x_coords[current_x_coord], y_coords[current_y_coord], z_coords[current_z_coord]), 
                      glm::vec3(x_coords[current_x_coord + 1], y_coords[current_y_coord + 1], z_coords[current_z_coord + 1]),
                      glm::vec3(floats[current_float + 1], floats[current_float + 2], floats[current_float + 3]),
                      floats[current_float + 4], floats[current_float + 5], floats[current_float + 6]);
        current_float += 7;
        current_x_coord += 2;
        current_y_coord += 2;
        current_z_coord += 2;
        break;
      case (instruction::CREATE_ORTHO_CAMERA):
        create_camera(floats[current_float], floats[current_float + 1], floats[current_float + 2], floats[current_float + 3], glm::vec3(x_coords[current_x_coord], y_coords[current_y_coord], z_coords[current_z_coord]), 
                      glm::vec3(x_coords[current_x_coord + 1], y_coords[current_y_coord + 1], z_coords[current_z_coord + 1]),
                      glm::vec3(floats[current_float + 4], floats[current_float + 5], floats[current_float + 6]),
                      floats[current_float + 7], floats[current_float + 8]);
        current_float += 9;
        current_x_coord += 2;
        current_y_coord += 2;
        current_z_coord += 2;
        break;
      case (instruction::CREATE_MATERIAL):
        create_material(glm::vec3(floats[current_float], floats[current_float + 1], floats[current_float + 2]),
                        glm::vec3(floats[current_float + 3], floats[current_float + 4], floats[current_float + 5]),
                        glm::vec3(floats[current_float + 6], floats[current_float + 7], floats[current_float + 8]),
                        floats[current_float + 9]);
        current_float += 10;
        break;
      case (instruction::CREATE_DIR_LIGHT):
        create_dir_light(glm::vec3(floats[current_float], floats[current_float + 1], floats[current_float + 2]),
                     glm::vec3(floats[current_float + 3], floats[current_float + 4], floats[current_float + 5]),
                     glm::vec3(floats[current_float + 6], floats[current_float + 7], floats[current_float + 8]),
                     glm::vec3(floats[current_float + 9], floats[current_float + 10], floats[current_float + 11]));
        current_float += 12;
        break;
      case (instruction::CREATE_SPOT_LIGHT):
        create_spot_light(glm::vec3(floats[current_float], floats[current_float + 1], floats[current_float + 2]),
                          glm::vec3(floats[current_float + 3], floats[current_float + 4], floats[current_float + 5]),
                          floats[current_float + 6], floats[current_float + 7], floats[current_float + 8], 
                          floats[current_float + 9], floats[current_float + 10],
                          glm::vec3(floats[current_float + 11], floats[current_float + 12], floats[current_float + 13]),
                          glm::vec3(floats[current_float + 14], floats[current_float + 15], floats[current_float + 16]),
                          glm::vec3(floats[current_float + 17], floats[current_float + 18], floats[current_float + 19]));
        current_float += 20;
        break;
      case (instruction::CREATE_POINT_LIGHT):
        create_point_light(glm::vec3(floats[current_float], floats[current_float + 1], floats[current_float + 2]),
                           floats[current_float + 3], floats[current_float + 4], floats[current_float + 5],
                           glm::vec3(floats[current_float + 6], floats[current_float + 7], floats[current_float + 8]),
                           glm::vec3(floats[current_float + 9], floats[current_float + 10], floats[current_float + 11]),
                           glm::vec3(floats[current_float + 12], floats[current_float + 13], floats[current_float + 14]));
        current_float += 15;
        break;
      case (instruction::START_DRAWING):
        {
        view v = view(floats[current_float], floats[current_float + 1], floats[current_float + 2], floats[current_float + 3]);
        current_float += 4;
        main_drawing.set_view(v);
        main_drawing.load(*this, instructions, x_coords, y_coords, z_coords,floats, uint32s, i, current_uint32, current_x_coord,
                          current_y_coord, current_z_coord, current_float);
        }
        break;
      default:
        break;
    }
  }
}