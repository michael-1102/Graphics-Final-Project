#include "drw_file.hpp"

std::vector<std::string> resplit(const std::string &s, const std::regex &sep_regex = std::regex{"\\s+"}) {
  std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
  std::sregex_token_iterator end;
  return {iter, end};
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
      std::vector<std::string> values = resplit(attrib_value, std::regex{"[ ,]"});
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
      //TODO: path support
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
      std::vector<std::string> values = resplit(attrib_value, std::regex{"[ ,]"});
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
  main_drawing.add_shape(shape);
  if (connect_ends) {
    if (attribs.has_fill) shape->add_fill_polygon(points, attribs.fill_color_index, attribs.fill_opacity);
    if (attribs.has_stroke) shape->add_draw_polygon(points, attribs.stroke_color_index, attribs.stroke_opacity);
  } else {
    if (attribs.has_fill) shape->add_fill_polyline(points, attribs.fill_color_index, attribs.fill_opacity);
    if (attribs.has_stroke) shape->add_draw_polyline(points, attribs.stroke_color_index, attribs.stroke_opacity);
  }
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
  styled_multishape_2d* shape = main_drawing.create_styled_multishape_2d(*this, attribs.stroke_width, attribs.transform_index);
  main_drawing.add_shape(shape);
  shape->add_draw_line(x1, y1, x2, y2, attribs.stroke_color_index, attribs.stroke_opacity);
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
  main_drawing.add_shape(shape);
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
  main_drawing.add_shape(shape);
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
  main_drawing.add_shape(shape);
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