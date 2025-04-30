#include "drw_file.hpp"

void drw_file::apply_svg_attributes(XMLParser::ElementContext* svg) {
  std::vector<XMLParser::AttributeContext *> attributes = svg->attribute();
  bool has_viewbox = false, has_width = false, has_height = false;
  view v;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\")"), ""); // remove quotation marks
    if (attrib_name == "viewBox") {
      has_viewbox = true;
      std::string delimiter = " ";
      uint32_t x = get_next_value(attrib_value, delimiter);
      uint32_t y = get_next_value(attrib_value, delimiter);
      uint32_t w = get_next_value(attrib_value, delimiter);
      uint32_t h = get_next_value(attrib_value, delimiter);
      v = view(x, y, w, h);
      main_drawing.set_view(v);
    } else if (attrib_name == "width") {
      has_width = true;
      width = string_to_pixels(attrib_value);
    } else if (attrib_name == "height") {
      has_height = true;
      height = string_to_pixels(attrib_value);
    } else if (attrib_name == "xmlns") {
      // do nothing with namespace
    } else {
      std::cout << "Unsupported attribute: " << attrib_name << std::endl;
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

  XMLParser::ElementContext* svg = parser.element();
  //std::cout << svg->toStringTree(&parser) << std::endl;

  if (svg->Name()[0]->getText() != "svg") {
    throw "Error: SVG must be first element of file";
  }

  apply_svg_attributes(svg);

  XMLParser::ContentContext* content = svg->content();
  std::vector<XMLParser::ElementContext*> elements = content->element();
  svg_shapes.push_back(styled_multishape_2d(*this, DEFAULT_STROKE_WIDTH, DEFAULT_TRANSFORM_INDEX));
  main_drawing.add_shape(&svg_shapes.back());
  add_svg_elements(elements, svg_shapes.back(), group_attributes());

  stream.close();
}

void drw_file::add_svg_elements(std::vector<XMLParser::ElementContext*> elements, styled_multishape_2d& shapes, group_attributes attribs) {
  for (auto elem : elements) {
    std::string elem_name = elem->Name()[0]->getText();
    if (elem_name == "line") {

    } else if (elem_name == "circle") {
      add_circle(elem, shapes, attribs);
    } else if (elem_name == "ellipse") {

    } else if (elem_name == "rect") {

    } else if (elem_name == "polyline") {

    } else if (elem_name == "polygon") {

    } else if (elem_name == "g") {

    } else {
      std::cout << "Unsupported element: " << elem_name << std::endl;
    }
  }
}

void drw_file::add_circle(XMLParser::ElementContext* element, styled_multishape_2d& shapes, group_attributes attribs) {
  std::vector<XMLParser::AttributeContext*> attributes = element->attribute();
  float cx = 0, cy = 0, r = 0;
  float fill_opacity = attribs.fill_opacity;
  float stroke_opacity = attribs.stroke_opacity;
  uint32_t fill_color_index = attribs.fill_color_index;
  uint32_t stroke_color_index = attribs.stroke_color_index;
  uint32_t stroke_width = attribs.stroke_width;
  bool has_stroke = false;
  for (auto attrib : attributes) {
    std::string attrib_name = attrib->Name()[0].getText();
    std::string attrib_value = std::regex_replace(attrib->STRING()[0].getText(), std::regex(R"(\")"), ""); // remove quotation marks
    if (attrib_name == "cx") {
      cx = string_to_pixels(attrib_value);
    } else if (attrib_name == "cy") {
      cy = string_to_pixels(attrib_value);
    } else if (attrib_name == "r") {
      r = string_to_pixels(attrib_value);
    } else if (attrib_name == "fill") {
      fill_color_index = string_to_color_index(attrib_value);
    } else if (attrib_name == "stroke") {
      stroke_color_index = string_to_color_index(attrib_value);
      has_stroke = true;
    } else if (attrib_name == "fill-opacity") {
      fill_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-opacity") {
      stroke_opacity = string_to_float(attrib_value);
    } else if (attrib_name == "stroke-width") {
      //TODO: change shape
    } else if (attrib_name == "transform") {
      //TODO: change shape
    } else {
      std::cout << "Unsupported attribute: " << attrib_name << std::endl;
    }
  }
  shapes.add_fill_circle(cx, cy, r, 30, fill_color_index, fill_opacity);
  if (has_stroke) shapes.add_draw_circle(cx, cy, r, 30, stroke_color_index, stroke_opacity);
}