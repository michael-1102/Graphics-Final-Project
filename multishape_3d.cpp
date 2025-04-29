#include "multishape_3d.hpp"
#include "drw_file.hpp"

multishape_3d::~multishape_3d() {}

void multishape_3d::init() {
  // Create VAO,
  //  a container to have all shapes and their attributes
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  // Create VBO for vertices
  // Create an object in the VAO to store all the vertex values
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0],
               GL_STATIC_DRAW);
  // Desctribe how information is recieved in shaders
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Create SBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as a solid(filled)
  glGenBuffers(1, &sbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * solidIndices.size(),
               &solidIndices[0], GL_STATIC_DRAW);
  // Create LBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as lines(wireframe)
  glGenBuffers(1, &lbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * lineIndices.size(),
               &lineIndices[0], GL_STATIC_DRAW);

  // Create PBO
  // Create an object to hold the order at which the vertices are drawn(from
  // indices) in order to draw it as points.
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pointIndices.size(),
               &pointIndices[0], GL_STATIC_DRAW);
}

void multishape_3d::render(drw_file& drw, view& view) {

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  camera& camera = drw.get_camera(camera_index);
  shader& s = drw.get_shader(sid);

  s.use();
  s.setVec4("color", drw.get_color(color_index));
  s.setMat4("transform", drw.get_transform(transform_index));
  s.setMat4("scale", drw.get_scale());
  s.setMat4("view", camera.get_view());
  s.setMat4("projection", camera.get_projection());
    
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glLineWidth(line_width);

  // Draw Solids
  if (solidIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
    glDrawElements(GL_TRIANGLES, solidIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Draw Lines
  if (lineIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
    glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Draw Points
  if (pointIndices.size() != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
    glDrawElements(GL_POINTS, pointIndices.size(), GL_UNSIGNED_INT, 0);
  }

  // Unbind
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glBindVertexArray(0);
  glDisable(GL_DEPTH_TEST);
}

uint32_t multishape_3d::addSector(float xc, float yc, float zc, float rad, float fromAngle, float toAngle, float angleInc) {
  uint32_t points_added = 0;
  for (float i = fromAngle; i <= toAngle; i += angleInc) {
    float ang = -i * M_PI / 180.0;
    add3DPoint(float(xc + rad * cos(ang)), yc, float(zc + rad * sin(ang)));
    points_added++;
  }
  return points_added;
}



void multishape_3d::addRectPrismVertices(float x, float y, float z, float width, float height, float length) {
  // Add the 8 needed vertices
  float x2 = x + width;
  float y2 = y + height;
  float z2 = z + length;
  add3DPoint(x, y, z);  // 0
  add3DPoint(x2, y, z); // 1
  add3DPoint(x, y2, z); // 2
  add3DPoint(x2, y2, z); // 3
  add3DPoint(x, y, z2); // 4
  add3DPoint(x2, y, z2); // 5
  add3DPoint(x, y2, z2); // 6
  add3DPoint(x2, y2, z2); // 7
}
void multishape_3d:: addWedgeVertices(float x, float y, float z, float width, float height, float length){
  // Add the 6 needed vertices
  float x2 = x + width;
  float y2 = y + height;
  float z2 = z + length;
  add3DPoint(x, y, z); // 0
  add3DPoint(x2, y, z); // 1
  add3DPoint(x, y2, z); // 2
  add3DPoint(x, y, z2); // 3
  add3DPoint(x2, y, z2); // 4
  add3DPoint(x, y2, z2); // 5

}
void multishape_3d::addEllipsoidVertices(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  add3DPoint(x,y+b,z);
  for (uint32_t i = 1; i < stacks; ++i) {
    float theta = (float)i / stacks * M_PI;  // Latitude angle

    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (uint32_t j = 0; j < sectors; ++j) {
      float phi = (float)j / sectors * 2.0f * M_PI;  // Longitude angle

      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      float x_pos = x + a * sinTheta * cosPhi;
      float y_pos = y + b * cosTheta;  // Controls vertical distribution
      float z_pos = z + c * sinTheta * sinPhi;

      add3DPoint(x_pos,y_pos,z_pos);
    }
  }
  add3DPoint(x, y-b,z);
}

void multishape_3d::addRectPyramidVertices(float x, float y, float z, float width, float height, float length) {
  // Add the 5 needed vertices
  float x2 = x + width;
  float z2 = z + length;
  add3DPoint(x, y, z); // 0
  add3DPoint(x2, y, z); // 1
  add3DPoint(x, y, z2); // 2
  add3DPoint(x2, y, z2); // 3
  add3DPoint(x + width / 2, y + height, z + length / 2); // 4
}

void multishape_3d::addTorusVertices(float x, float y, float z, float bigRadius, float smallRadius, uint32_t sectors, uint32_t stacks) {
  for (uint32_t i = 0; i < sectors; ++i) {
      float theta = (float)i / sectors * 2.0f * M_PI;  // Angle around the main axis

      float cosTheta = cos(theta);
      float sinTheta = sin(theta);

      for (uint32_t j = 0; j < stacks; ++j) {
          float phi = (float)j / stacks * 2.0f * M_PI;  // Angle around the tube

          float cosPhi = cos(phi);
          float sinPhi = sin(phi);

          // Position of the point on the torus
          float x_pos = x + (bigRadius + smallRadius * cosPhi) * cosTheta;
          float y_pos = y + smallRadius * sinPhi;
          float z_pos = z + (bigRadius + smallRadius * cosPhi) * sinTheta;

          add3DPoint(x_pos, y_pos, z_pos);
      }
  }
}


void multishape_3d::sAddRectFaceIndices(uint32_t bottomLeft, uint32_t bottomRight, uint32_t topLeft, uint32_t topRight, uint32_t offset) {
  uint32_t startIndex = getPointIndex() - offset;
  solidIndices.push_back(startIndex + topLeft);
  solidIndices.push_back(startIndex + bottomLeft);
  solidIndices.push_back(startIndex + bottomRight);
  solidIndices.push_back(startIndex + topLeft);
  solidIndices.push_back(startIndex + bottomRight);
  solidIndices.push_back(startIndex + topRight);
}
void multishape_3d::sAddTriFaceIndices(uint32_t vertex1, uint32_t vertex2, uint32_t vertex3, uint32_t offset) {
  uint32_t startIndex = getPointIndex() - offset;
  solidIndices.push_back(startIndex + vertex1);
  solidIndices.push_back(startIndex + vertex2);
  solidIndices.push_back(startIndex + vertex3);
}


void multishape_3d::lAddLineIndices(uint32_t i1, uint32_t i2, uint32_t offset) {
  uint32_t startIndex = getPointIndex() - offset;
  lineIndices.push_back(startIndex + i1);
  lineIndices.push_back(startIndex + i2);
}

uint32_t multishape_3d::addLinearExtrusionVertices(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t num_vertices = shape.get_num_vertices();
  if (num_vertices <= 0) return 0;
  uint32_t total_num_vertices = 0;
  float y_inc = height / stacks;  // amount y is incremented by for every stack
  for (uint32_t i = 0; i < stacks + 1; i++) { // for every stack + 1 because of the top
    // vertices
    for (uint32_t j = 0; j < num_vertices; j++) {
      glm::vec2 point = shape.get_vertex(j);
      add3DPoint(x + point.x, y, z + point.y);
      total_num_vertices++;
    }
    y += y_inc;
  }
  return total_num_vertices;
}

uint32_t multishape_3d::addSplineExtrusionVertices(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t num_vertices = shape.get_num_vertices();
  if (num_vertices <= 0) return 0;
  uint32_t total_num_vertices = 0;

  int num_points = points.size();
  if (num_points <= 1) return 0;
  int num_segments = num_points - 1;
  std::vector<glm::vec3> curve_points; // all of the points that make up the curve
  std::vector<float> c_prime(num_points), dx_prime(num_points), dy_prime(num_points), dz_prime(num_points), Dx(num_points), Dy(num_points), Dz(num_points);
  c_prime[0] = 0.5;
  dx_prime[0] = 1.5 * (points[1].x - points[0].x);
  dy_prime[0] = 1.5 * (points[1].y - points[0].y);
  dz_prime[0] = 1.5 * (points[1].z - points[0].z);
  for (int i = 1; i < num_points - 1; i++) {
    c_prime[i] = 1 / (4 - c_prime[i - 1]);
    dx_prime[i] = (3*(points[i + 1].x - points[i - 1].x) - dx_prime[i - 1]) / (4 - c_prime[i - 1]);
    dy_prime[i] = (3*(points[i + 1].y - points[i - 1].y) - dy_prime[i - 1]) / (4 - c_prime[i - 1]);
    dz_prime[i] = (3*(points[i + 1].z - points[i - 1].z) - dz_prime[i - 1]) / (4 - c_prime[i - 1]);
  }
  dx_prime[num_segments] = (3*(points[num_segments].x - points[num_segments - 1].x) - dx_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);
  dy_prime[num_segments] = (3*(points[num_segments].y - points[num_segments - 1].y) - dy_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);
  dz_prime[num_segments] = (3*(points[num_segments].z - points[num_segments - 1].z) - dz_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);

  Dx[num_points - 1] = dx_prime[num_points - 1];
  Dy[num_points - 1] = dy_prime[num_points - 1];
  Dz[num_points - 1] = dz_prime[num_points - 1];

  for (int i = num_points - 2; i >= 0; i--) {
    Dx[i] = dx_prime[i] - c_prime[i] * Dx[i + 1];
    Dy[i] = dy_prime[i] - c_prime[i] * Dy[i + 1];
    Dz[i] = dz_prime[i] - c_prime[i] * Dz[i + 1];
  }

  std::vector<float> cx(num_segments), cy(num_segments), cz(num_segments), dx(num_segments), dy(num_segments), dz(num_segments);
  for (int i = 0; i < num_segments; i++) {
    cx[i] = 3 * (points[i + 1].x - points[i].x) - 2 * Dx[i] - Dx[i + 1];
    cy[i] = 3 * (points[i + 1].y - points[i].y) - 2 * Dy[i] - Dy[i + 1];
    cz[i] = 3 * (points[i + 1].z - points[i].z) - 2 * Dz[i] - Dz[i + 1];

    dx[i] = 2 * (points[i].x - points[i + 1].x) + Dx[i] + Dx[i + 1];
    dy[i] = 2 * (points[i].y - points[i + 1].y) + Dy[i] + Dy[i + 1];
    dz[i] = 2 * (points[i].z - points[i + 1].z) + Dz[i] + Dz[i + 1];

    float t;
    for (t = 0; t < 1; t += inc) {
      curve_points.push_back(glm::vec3(points[i].x + t * Dx[i] + t * t * cx[i] + t * t * t * dx[i],
                                       points[i].y + t * Dy[i] + t * t * cy[i] + t * t * t * dy[i],
                                       points[i].z + t * Dz[i] + t * t * cz[i] + t * t * t * dz[i]));
    }
  }
  curve_points.push_back(points[num_segments]);

  glm::vec3 a(0.f, 1.f, 0.f);
  glm::vec3 b = curve_points[1] - curve_points[0];
  b = glm::normalize(b);
  glm::vec3 v = glm::cross(a,b);
  float c = glm::dot(a, b);
  glm::mat3 vx(0, v[2], -v[1], -v[2], 0, v[0], v[1], -v[0], 0);
  glm::mat3 r = glm::mat3(1.f) + vx + vx*vx/(1+c);
  std::vector<glm::vec3> new_points;
  for (uint32_t j = 0; j < num_vertices; j++) {
    glm::vec2 point = shape.get_vertex(j);
    glm::vec3 new_point(point.x, 0, point.y);
    if (c != -1)
      new_point = r * new_point;
    new_point += curve_points[0];
    add3DPoint(new_point.x, new_point.y, new_point.z);
    new_points.push_back(new_point);
    total_num_vertices++;
  }

  uint32_t last_curve_point_index = curve_points.size() - 1;
  for (uint32_t i = 1; i < last_curve_point_index; i++) {
    glm::vec3 v1 = curve_points[i] - curve_points[i - 1];
    glm::vec3 v2 = curve_points[i + 1] - curve_points[i];
    glm::vec3 n = v1 + v2; // a = n.x, b = n.y, c = n.z
    float d = - n.x * curve_points[i].x - n.y * curve_points[i].y - n.z * curve_points[i].z;
    for (uint32_t j = 0; j < num_vertices; j++) {
      float t = -(n.x * new_points[j].x + n.y * new_points[j].y + n.z * new_points[j].z + d) / (n.x * v1.x + n.y * v1.y + n.z * v1.z);
      glm::vec3 new_point = new_points[j] + (t * v1);
      add3DPoint(new_point.x, new_point.y, new_point.z);
      new_points[j] = new_point;
      total_num_vertices++;
    }
  }

  glm::vec3 v1 = curve_points[last_curve_point_index] - curve_points[last_curve_point_index - 1];
  glm::vec3 n = v1 + v1; // a = n.x, b = n.y, c = n.z
  float d = - n.x * curve_points[last_curve_point_index].x - n.y * curve_points[last_curve_point_index].y - n.z * curve_points[last_curve_point_index].z;
  for (uint32_t j = 0; j < num_vertices; j++) {
    float t = -(n.x * new_points[j].x + n.y * new_points[j].y + n.z * new_points[j].z + d) / (n.x * v1.x + n.y * v1.y + n.z * v1.z);
    glm::vec3 new_point = new_points[j] + (t * v1);
    add3DPoint(new_point.x, new_point.y, new_point.z);
    total_num_vertices++;
  }

  return total_num_vertices;
}

void multishape_3d::lAddExtrusionIndices(shape_2d shape, float stacks, uint32_t total_num_vertices) {
  uint32_t num_vertices = shape.get_num_vertices();
  uint32_t num_pipe_indices = shape.get_num_pipe_indices();
  if (num_vertices <= 0 || num_pipe_indices <= 0) return;
  
  
  for (uint32_t i = 0; i < stacks; i++) { // for every stack
    uint32_t offset = i * num_vertices;
    for (uint32_t j = 0; j < num_pipe_indices - 1; j++) {
      // horizontal line indices
      lAddLineIndices(offset + shape.get_pipe_index(j), offset + shape.get_pipe_index(j + 1), total_num_vertices);
    }
    for (uint32_t j = 1; j < num_pipe_indices; j++) {
      // vertical line indices
      lAddLineIndices(offset + shape.get_pipe_index(j), offset + num_vertices + shape.get_pipe_index(j), total_num_vertices);
    } 
  }

  uint32_t offset = stacks * num_vertices;  
  for (uint32_t j = 0; j < num_pipe_indices - 1; j++) {
    // top horizontal line indices
    lAddLineIndices(offset + shape.get_pipe_index(j), offset + shape.get_pipe_index(j + 1), total_num_vertices);
  }

  // add end indices
  uint32_t num_end_indices = shape.get_num_line_end_indices();
  if (num_end_indices < 2) return;
  for (uint32_t i = 0; i < num_end_indices - 1; i+=2) {
    uint32_t end_index_0 = shape.get_line_end_index(i);
    uint32_t end_index_1 = shape.get_line_end_index(i + 1);

    // bottom
    lAddLineIndices(end_index_0, end_index_1, total_num_vertices);

    // top
    lAddLineIndices(offset + end_index_0, offset + end_index_1, total_num_vertices);
  }
}

void multishape_3d::sAddExtrusionIndices(shape_2d shape, float stacks, uint32_t total_num_vertices) {
  uint32_t num_vertices = shape.get_num_vertices();
  uint32_t num_pipe_indices = shape.get_num_pipe_indices();
  if (num_vertices <= 0 || num_pipe_indices <= 0) return;
  for (uint32_t i = 0; i < stacks; i++) { // for every stack
    // pipe indices
    uint32_t offset = i * num_vertices;
    for (uint32_t j = 0; j < num_pipe_indices - 1; j++) {
      uint32_t index_0 = shape.get_pipe_index(j);
      uint32_t index_1 = shape.get_pipe_index(j + 1);

      sAddRectFaceIndices(offset + index_0, offset + index_1, offset + num_vertices + index_0, offset + num_vertices + index_1, total_num_vertices);
    }
  }  

  // add end indices
  uint32_t num_end_indices = shape.get_num_solid_end_indices();
  if (num_end_indices < 3) return;
  uint32_t offset = stacks * num_vertices;
  for (uint32_t i = 0; i < num_end_indices - 1; i+=3) {
    uint32_t end_index_0 = shape.get_solid_end_index(i);
    uint32_t end_index_1 = shape.get_solid_end_index(i + 1);
    uint32_t end_index_2 = shape.get_solid_end_index(i + 2);

    // bottom
    sAddTriFaceIndices(end_index_0, end_index_1, end_index_2, total_num_vertices);

    // top
    sAddTriFaceIndices(offset + end_index_0, offset + end_index_1, offset + end_index_2, total_num_vertices);
  }
}

void multishape_3d::drawPoint(float x, float y, float z) {
  uint32_t ind = getPointIndex();
  add3DPoint(x, y, z);
  pointIndices.push_back(ind);
}

void multishape_3d::drawLine(float x1, float y1, float z1, float x2, float y2, float z2) {
  uint32_t ind = getPointIndex();
  add3DPoint(x1, y1, z1);
  add3DPoint(x2, y2, z2);
  lineIndices.push_back(ind++);
  lineIndices.push_back(ind);
}

void multishape_3d::drawTorus(float x, float y, float z, float R, float r, uint32_t slices, uint32_t stacks) {
  addTorusVertices(x, y, z, R, r, slices, stacks);
  const uint32_t numVertices = slices * stacks;

  // sides of the torus 
  for (uint32_t i = 0; i < slices; ++i) {
    for (uint32_t j = 0; j < stacks; ++j) {
      uint32_t current = i * stacks + j;
      uint32_t nextI = (i + 1) % slices; // Wrap around stacks
      uint32_t nextJ = (j + 1) % stacks; 
      uint32_t right = i * stacks + nextJ;           // Same ring, next around tube
      uint32_t down = nextI * stacks + j;            // Next ring, same point

      // Horizontal lines (theta direction)
      lAddLineIndices(current, right, numVertices);
      // Vertical lines (phi direction)
      lAddLineIndices(current, down, numVertices);
    }
  }
  //not needed because modulus is doing the same thing (?)
  // Optional: Closing the shape by connecting the last row of vertices to the first one.
  //for (uint32_t j = 0; j < slices; ++j) {
    //uint32_t current = (stacks - 1) * (slices + 1) + j;
    //uint32_t next = (stacks - 1) * (slices + 1) + (j + 1) % slices;
    //lAddLineIndices(current, next, numVertices);
  //}
}


void multishape_3d::fillRectPrism(float x, float y, float z, float width, float height, float length) {
  addRectPrismVertices(x, y, z, width, height, length);
  const uint32_t numVertices = 8;
  // bottom left, bottom right, top left, top right
  sAddRectFaceIndices(1, 0, 3, 2, numVertices); // back face, bottom = y-, left = x+ 
  sAddRectFaceIndices(4, 5, 6, 7, numVertices); // front face, bottom = y-, left = x-
  sAddRectFaceIndices(0, 4, 2, 6, numVertices); // left face, bottom = y-, left = z- 
  sAddRectFaceIndices(5, 1, 7, 3, numVertices); // right face, bottom = y-, left = z+ 
  sAddRectFaceIndices(6, 7, 2, 3, numVertices); // top face, bottom = z+, left = x- 
  sAddRectFaceIndices(0, 1, 4, 5, numVertices); // bottom face, bottom = z-, left = x-
}

void multishape_3d::fillWedge(float x, float y, float z, float width, float height, float length){
  addWedgeVertices(x,  y, z, width, height, length);
  const uint32_t numVertices = 6;
  //2 triangular faces, 3 rectangular faces
  //rectangular faces
  sAddRectFaceIndices(3, 4, 0, 1, numVertices); // 1
  sAddRectFaceIndices(2, 5, 0, 3, numVertices); // 2
  sAddRectFaceIndices(5, 2, 4, 1, numVertices); // 3

  //triangular faces
  sAddTriFaceIndices(3, 4, 5, numVertices); // 1
  sAddTriFaceIndices(0, 1, 2, numVertices); // 2
}


uint32_t multishape_3d::fillCircle(float x, float y, float z, float rad, float angleInc) {
  float toAngle = 360 - angleInc;
  uint32_t centerIndex = getPointIndex();
  add3DPoint(x, y, z);

  uint32_t toAdd = addSector(x, y, z, rad, 0, toAngle, angleInc);
  sAddClosedSectorIndices(centerIndex, toAdd);
  return toAdd;
}

void multishape_3d::fillCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  fillFrustum(x, y, z, rad, rad, height, sectors, stacks);
}

void multishape_3d::fillCube(float x, float y, float z, float size) {
  fillRectPrism(x, y, z, size, size, size);
}

void multishape_3d::fillRectPyramid(float x, float y, float z, float width, float height, float length) {
  addRectPyramidVertices(x, y, z, width, height, length);
  uint32_t numVertices = 5;
  // bottom left, bottom right, top left, top right
  sAddRectFaceIndices(0, 1, 2, 3, numVertices); // bottom face, bottom = z-, left = x-
  uint32_t topIndex = getPointIndex() - 1;
  uint32_t startIndex = getPointIndex() - numVertices;

  // front face
  solidIndices.push_back(startIndex + 2);
  solidIndices.push_back(startIndex + 3);
  solidIndices.push_back(topIndex);

  // back face
  solidIndices.push_back(startIndex + 1);
  solidIndices.push_back(startIndex);
  solidIndices.push_back(topIndex);

  // left face
  solidIndices.push_back(startIndex);
  solidIndices.push_back(startIndex + 2);
  solidIndices.push_back(topIndex);

  // right face
  solidIndices.push_back(startIndex + 3);
  solidIndices.push_back(startIndex + 1);
  solidIndices.push_back(topIndex);
}

void multishape_3d::fillCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  fillObliqueCone(x, y, z, rad, x, y + height, z, sectors, stacks);
}

void multishape_3d::fillObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  // bottom circle
  uint32_t bottomIndex = getPointIndex() + 1; // + 1 because of the center
  uint32_t toAdd = fillCircle(xBottom, yBottom, zBottom, rad, angleInc);

  float xDiff = xTop - xBottom;
  float yDiff = yTop - yBottom;
  float zDiff = zTop - zBottom;

  float topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addSector(xBottom+ratio*xDiff, yBottom+ratio*yDiff, zBottom+ratio*zDiff, rad-ratio*rad, 0, toAngle, angleInc);
    uint32_t j;
    for (j = 0; j < toAdd - 1; j++) {
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(bottomIndex + j + 1);

      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(topIndex + j + 1);
      solidIndices.push_back(bottomIndex + j + 1);
    }

    // close side
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(bottomIndex);

    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(topIndex);
    solidIndices.push_back(bottomIndex);

    bottomIndex = topIndex;
  }

  // top point
  topIndex = getPointIndex();
  add3DPoint(xTop, yTop, zTop);

  for (uint32_t j = 0; j < toAdd - 1; j++) {
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex);
    solidIndices.push_back(bottomIndex + j + 1);
  }

  // close shape
  solidIndices.push_back(bottomIndex + toAdd);
  solidIndices.push_back(topIndex);
  solidIndices.push_back(bottomIndex + 1);
}
void multishape_3d::fillEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  addEllipsoidVertices(x, y, z, a, b, c, sectors, stacks);

  const uint32_t numVertices = sectors * (stacks - 1) + 2;
  uint32_t topCenter = 0;
  uint32_t bottomCenter = numVertices - 1;

  // Middle bands
  for (uint32_t i = 0; i < stacks - 2; ++i) {
    for (uint32_t j = 0; j < sectors; ++j) {
      uint32_t curr = 1 + i * sectors + j;
      uint32_t next = 1 + i * sectors + (j + 1) % sectors;
      uint32_t below = 1 + (i + 1) * sectors + j;
      uint32_t belowRight = 1 + (i + 1) * sectors + (j + 1) % sectors;

      sAddTriFaceIndices(curr, next, below, numVertices);
      sAddTriFaceIndices(next, belowRight, below, numVertices);
    }
  }

  // Top cap
  for (uint32_t j = 0; j < sectors; ++j) {
    uint32_t next = (j + 1) % sectors;
    sAddTriFaceIndices(topCenter, 1 + j, 1 + next, numVertices);
  }

  // Bottom cap
  uint32_t lastRingStart = 1 + (stacks - 2) * sectors;
  for (uint32_t j = 0; j < sectors; ++j) {
    uint32_t next = (j + 1) % sectors;
    sAddTriFaceIndices(lastRingStart + j, lastRingStart + next, bottomCenter, numVertices);
  }
}

void multishape_3d::fillSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks){
  fillEllipsoid(x, y, z, rad, rad, rad, sectors, stacks);
}

void multishape_3d::fillTorus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks) {
  addTorusVertices(x, y, z, R, r, sectors, stacks);

  const uint32_t numVertices = sectors * stacks;

  for (uint32_t i = 0; i < sectors; ++i) {
      for (uint32_t j = 0; j < stacks; ++j) {
          uint32_t current = i * stacks + j;

          uint32_t nextJ = (j + 1) % stacks;     // wrap around tube
          uint32_t nextI = (i + 1) % sectors;    // wrap around ring

          uint32_t right      = i * stacks + nextJ;
          uint32_t down       = nextI * stacks + j;
          uint32_t downRight  = nextI * stacks + nextJ;

          // Triangle 1
          sAddTriFaceIndices(current, right, down, numVertices);

          // Triangle 2
          sAddTriFaceIndices(right, downRight, down, numVertices);
      }

  }
}




void multishape_3d::fillFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  fillObliqueFrustum(x, y, z, bottomRad, x, y + height, z, topRad, sectors, stacks);
}

void multishape_3d::fillObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  uint32_t bottomIndex = getPointIndex() + 1; // + 1 because of the center
  uint32_t toAdd = fillCircle(x0, y0, z0, rad0, angleInc);
  
  float xDiff = x1 - x0;
  float yDiff = y1 - y0;
  float zDiff = z1 - z0;
  float radDiff = rad1 - rad0;

  uint32_t topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addSector(x0+ratio*xDiff, y0+ratio*yDiff, z0+ratio*zDiff, rad0+ratio*radDiff, 0, toAngle, angleInc);
    // sides
    uint32_t j;
    for (j = 0; j < toAdd - 1; j++) {
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(bottomIndex + j + 1);

      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(topIndex + j + 1);
      solidIndices.push_back(bottomIndex + j + 1);
    }

    // close side
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(bottomIndex);

    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(topIndex);
    solidIndices.push_back(bottomIndex);

    bottomIndex = topIndex;
  }

  topIndex = getPointIndex() + 1;
  fillCircle(x1, y1, z1, rad1, angleInc);

  uint32_t j;
  for (j = 0; j < toAdd - 1; j++) {
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(bottomIndex + j + 1);

    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(topIndex + j + 1);
    solidIndices.push_back(bottomIndex + j + 1);
  }

  // close side
  solidIndices.push_back(bottomIndex + j);
  solidIndices.push_back(topIndex + j);
  solidIndices.push_back(bottomIndex);

  solidIndices.push_back(topIndex + j);
  solidIndices.push_back(topIndex);
  solidIndices.push_back(bottomIndex);
}

void multishape_3d::fillObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  fillObliqueFrustum(x0, y0, z0, rad, x1, y1, z1, rad, sectors, stacks);
}

void multishape_3d::fillFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;

  // bottom face vertices
  uint32_t bottomOuterIndex = getPointIndex();
  uint32_t toAdd = addSector(x, y, z, bottomOuterRad, 0, toAngle, angleInc);
  uint32_t bottomInnerIndex = getPointIndex();
  addSector(x, y, z, bottomInnerRad, 0, toAngle, angleInc);

  uint32_t j;
  for (j = 0; j < toAdd - 1; j++) {
    // bottom face indices
    solidIndices.push_back(bottomOuterIndex + j);
    solidIndices.push_back(bottomOuterIndex + j + 1);
    solidIndices.push_back(bottomInnerIndex + j);

    solidIndices.push_back(bottomInnerIndex + j);
    solidIndices.push_back(bottomOuterIndex + j + 1);
    solidIndices.push_back(bottomInnerIndex + j + 1);
  }
  
  // finish bottom face
  solidIndices.push_back(bottomOuterIndex + j);
  solidIndices.push_back(bottomOuterIndex);
  solidIndices.push_back(bottomInnerIndex + j);

  solidIndices.push_back(bottomInnerIndex + j);
  solidIndices.push_back(bottomOuterIndex);
  solidIndices.push_back(bottomInnerIndex); 
  

  float outerRadDiff = topOuterRad - bottomOuterRad;
  float innerRadDiff = topInnerRad - bottomInnerRad;

  uint32_t topOuterIndex;
  uint32_t topInnerIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topOuterIndex = getPointIndex();
    addSector(x, y+ratio*height, z, bottomOuterRad+ratio*outerRadDiff, 0, toAngle, angleInc);
    topInnerIndex = getPointIndex();
    addSector(x, y+ratio*height, z, bottomInnerRad+ratio*innerRadDiff, 0, toAngle, angleInc);
    // indices
    uint32_t j;
    for (j = 0; j < toAdd - 1; j++) {
      // outside indices
      solidIndices.push_back(bottomOuterIndex + j);
      solidIndices.push_back(topOuterIndex + j);
      solidIndices.push_back(bottomOuterIndex + j + 1);

      solidIndices.push_back(topOuterIndex + j);
      solidIndices.push_back(topOuterIndex + j + 1);
      solidIndices.push_back(bottomOuterIndex + j + 1);

      // inside indices
      solidIndices.push_back(bottomInnerIndex + j);
      solidIndices.push_back(topInnerIndex + j);
      solidIndices.push_back(bottomInnerIndex + j + 1);

      solidIndices.push_back(topInnerIndex + j);
      solidIndices.push_back(topInnerIndex + j + 1);
      solidIndices.push_back(bottomInnerIndex + j + 1);
    }

    // finish outside
    solidIndices.push_back(bottomOuterIndex + j);
    solidIndices.push_back(topOuterIndex + j);
    solidIndices.push_back(bottomOuterIndex);

    solidIndices.push_back(topOuterIndex + j);
    solidIndices.push_back(topOuterIndex);
    solidIndices.push_back(bottomOuterIndex);

    // finish inside
    solidIndices.push_back(bottomInnerIndex + j);
    solidIndices.push_back(topInnerIndex + j);
    solidIndices.push_back(bottomInnerIndex);

    solidIndices.push_back(topInnerIndex + j);
    solidIndices.push_back(topInnerIndex);
    solidIndices.push_back(bottomInnerIndex);

    bottomOuterIndex = topOuterIndex;
    bottomInnerIndex = topInnerIndex;
  }

  // top face vertices
  float y2 = y + height;
  topOuterIndex = getPointIndex();
  addSector(x, y2, z, topOuterRad, 0, toAngle, angleInc);
  topInnerIndex = getPointIndex();
  addSector(x, y2, z, topInnerRad, 0, toAngle, angleInc);

  for (j = 0; j < toAdd - 1; j++) {
    // top face indices
    solidIndices.push_back(topOuterIndex + j);
    solidIndices.push_back(topOuterIndex + j + 1);
    solidIndices.push_back(topInnerIndex + j);

    solidIndices.push_back(topInnerIndex + j);
    solidIndices.push_back(topOuterIndex + j + 1);
    solidIndices.push_back(topInnerIndex + j + 1);

    // outside indices
    solidIndices.push_back(bottomOuterIndex + j);
    solidIndices.push_back(topOuterIndex + j);
    solidIndices.push_back(bottomOuterIndex + j + 1);

    solidIndices.push_back(topOuterIndex + j);
    solidIndices.push_back(topOuterIndex + j + 1);
    solidIndices.push_back(bottomOuterIndex + j + 1);

    // inside indices
    solidIndices.push_back(bottomInnerIndex + j);
    solidIndices.push_back(topInnerIndex + j);
    solidIndices.push_back(bottomInnerIndex + j + 1);

    solidIndices.push_back(topInnerIndex + j);
    solidIndices.push_back(topInnerIndex + j + 1);
    solidIndices.push_back(bottomInnerIndex + j + 1);
  }

  // finish top face
  solidIndices.push_back(topOuterIndex + j);
  solidIndices.push_back(topOuterIndex);
  solidIndices.push_back(topInnerIndex + j);

  solidIndices.push_back(topInnerIndex + j);
  solidIndices.push_back(topOuterIndex);
  solidIndices.push_back(topInnerIndex);

  // finish outside
  solidIndices.push_back(bottomOuterIndex + j);
  solidIndices.push_back(topOuterIndex + j);
  solidIndices.push_back(bottomOuterIndex);

  solidIndices.push_back(topOuterIndex + j);
  solidIndices.push_back(topOuterIndex);
  solidIndices.push_back(bottomOuterIndex);

  // finish inside
  solidIndices.push_back(bottomInnerIndex + j);
  solidIndices.push_back(topInnerIndex + j);
  solidIndices.push_back(bottomInnerIndex);

  solidIndices.push_back(topInnerIndex + j);
  solidIndices.push_back(topInnerIndex);
  solidIndices.push_back(bottomInnerIndex);
}

void multishape_3d::fillTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  fillFrustumMinusFrustum(x, y, z, outerRad, innerRad, outerRad, innerRad, height, sectors, stacks);
}

void multishape_3d::fillLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t total_num_vertices = addLinearExtrusionVertices(shape, x, y, z, height, stacks);
  sAddExtrusionIndices(shape, stacks, total_num_vertices);
}

void multishape_3d::fillSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t total_num_vertices = addSplineExtrusionVertices(shape, points, inc);
  sAddExtrusionIndices(shape, total_num_vertices / shape.get_num_vertices() - 1, total_num_vertices);
}

void multishape_3d::drawRectPrism(float x, float y, float z, float width, float height, float length) {
  addRectPrismVertices(x, y, z, width, height, length);
  const uint32_t numVertices = 8;
  lAddLineIndices(0, 1, numVertices);
  lAddLineIndices(1, 3, numVertices);
  lAddLineIndices(3, 2, numVertices);
  lAddLineIndices(2, 0, numVertices);
  
  lAddLineIndices(0, 4, numVertices);
  lAddLineIndices(1, 5, numVertices);
  lAddLineIndices(2, 6, numVertices);
  lAddLineIndices(3, 7, numVertices);

  lAddLineIndices(4, 5, numVertices);
  lAddLineIndices(5, 7, numVertices);
  lAddLineIndices(7, 6, numVertices);
  lAddLineIndices(6, 4, numVertices);
}
void multishape_3d:: drawWedge(float x, float y, float z, float width, float height, float length){
  addWedgeVertices(x, y, z, width, height, length);
  const uint32_t numVertices = 6;
  lAddLineIndices(0, 1, numVertices);
  lAddLineIndices(1, 2, numVertices);
  lAddLineIndices(2, 0, numVertices);

  lAddLineIndices(3, 4, numVertices);
  lAddLineIndices(4, 5, numVertices);
  lAddLineIndices(5, 3, numVertices);

  lAddLineIndices(0, 3, numVertices);
  lAddLineIndices(1, 4, numVertices);
  lAddLineIndices(2, 5, numVertices);

}
void multishape_3d::drawEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  addEllipsoidVertices(x, y, z, a, b, c, sectors, stacks);
  const uint32_t numVertices = sectors * (stacks - 1) + 2;
  uint32_t topIndex = 0;
  uint32_t bottomIndex = numVertices - 1;

  // Connect top pole to first ring
  for (uint32_t j = 0; j < sectors; ++j) {
    uint32_t next = (j + 1) % sectors;
    lAddLineIndices(topIndex, 1 + j, numVertices);
  }

  // Middle rings
  for (uint32_t i = 0; i < stacks - 2; ++i) {
    for (uint32_t j = 0; j < sectors; ++j) {
      uint32_t current = 1 + i * sectors + j;
      uint32_t right = 1 + i * sectors + (j + 1) % sectors;
      uint32_t below = 1 + (i + 1) * sectors + j;
      lAddLineIndices(current, right, numVertices);
      lAddLineIndices(current, below, numVertices);
    }
  }

  // Bottom ring to bottom pole
  uint32_t lastRingStart = 1 + (stacks - 2) * sectors;
  for (uint32_t j = 0; j < sectors; ++j) {
    uint32_t next = (j + 1) % sectors;
    lAddLineIndices(lastRingStart + j, lastRingStart + next, numVertices);
    lAddLineIndices(lastRingStart + j, bottomIndex, numVertices);
  }
}

void multishape_3d::drawSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks) {
  drawEllipsoid(x, y, z, rad, rad, rad, sectors, stacks);
}

void multishape_3d::drawRectPyramid(float x, float y, float z, float width, float height, float length) {
  addRectPyramidVertices(x, y, z, width, height, length);
  const uint32_t numVertices = 5;
  
  lAddLineIndices(0, 1, numVertices);
  lAddLineIndices(1, 3, numVertices);
  lAddLineIndices(3, 2, numVertices);
  lAddLineIndices(2, 0, numVertices);
  
  lAddLineIndices(0, 4, numVertices);
  lAddLineIndices(1, 4, numVertices);
  lAddLineIndices(2, 4, numVertices);
  lAddLineIndices(3, 4, numVertices);
}

void multishape_3d::drawCube(float x, float y, float z, float size) {
  drawRectPrism(x, y, z, size, size, size);
}

uint32_t multishape_3d::drawCircle(float x, float y, float z, float rad, float angleInc) {
  float toAngle = 360 - angleInc;
  uint32_t center_index = getPointIndex();
  add3DPoint(x, y, z);

  uint32_t toAdd = addSector(x, y, z, rad, 0, toAngle, angleInc);
  lAddClosedIndices(toAdd); // draw border of circle

  for (uint32_t i = 1; i < toAdd + 1; i++) {
    lineIndices.push_back(center_index);
    lineIndices.push_back(center_index + i);
  }
  return toAdd;
}

void multishape_3d::drawCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  drawFrustum(x, y, z, rad, rad, height, sectors, stacks);
}

void multishape_3d::drawCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  drawObliqueCone(x, y, z, rad, x, y + height, z, sectors, stacks);
}

void multishape_3d::drawObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  // bottom circle
  uint32_t startIndex = getPointIndex();
  uint32_t toAdd = drawCircle(xBottom, yBottom, zBottom, rad, angleInc);

  // top point
  uint32_t topIndex = getPointIndex();
  add3DPoint(xTop, yTop, zTop);

  for (uint32_t i = 1; i < toAdd + 1; i++) { // start at 1 because of center index
    lineIndices.push_back(startIndex + i);
    lineIndices.push_back(topIndex);
  }

  // stacks
  float xDiff = xTop - xBottom;
  float yDiff = yTop - yBottom;
  float zDiff = zTop - zBottom;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    uint32_t toAdd = addSector(xBottom+ratio*xDiff, yBottom+ratio*yDiff, zBottom+ratio*zDiff, rad-ratio*rad, 0, toAngle, angleInc);
    lAddClosedIndices(toAdd); // draw border of circle
  }
}

void multishape_3d::drawFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  drawObliqueFrustum(x, y, z, bottomRad, x, y + height, z, topRad, sectors, stacks);
}

void multishape_3d::drawObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;

  uint32_t bottomIndex = getPointIndex();
  uint32_t toAdd = drawCircle(x0, y0, z0, rad0, angleInc);
  uint32_t topIndex = getPointIndex();
  drawCircle(x1, y1, z1, rad1, angleInc);

  // sides
  for (uint32_t i = 1; i < toAdd + 1; i++) { // start at 1 because of center index
    lineIndices.push_back(bottomIndex + i);
    lineIndices.push_back(topIndex + i);
  }

  // stacks
  float xDiff = x1 - x0;
  float yDiff = y1 - y0;
  float zDiff = z1 - z0;
  float radDiff = rad1 - rad0;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    uint32_t toAdd = addSector(x0+ratio*xDiff, y0+ratio*yDiff, z0+ratio*zDiff, rad0+ratio*radDiff, 0, toAngle, angleInc);
    lAddClosedIndices(toAdd); // draw border of circle
  }
}

void multishape_3d::drawObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  drawObliqueFrustum(x0, y0, z0, rad, x1, y1, z1, rad, sectors, stacks);
}

void multishape_3d::drawTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  drawFrustumMinusFrustum(x, y, z, outerRad, innerRad, outerRad, innerRad, height, sectors, stacks);
}

void multishape_3d::drawFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  float y2 = y + height;

  // bottom face vertices
  uint32_t bottomOuterIndex = getPointIndex();
  uint32_t toAdd = addSector(x, y, z, bottomOuterRad, 0, toAngle, angleInc);
  lAddClosedIndices(toAdd);

  uint32_t bottomInnerIndex = getPointIndex();
  addSector(x, y, z, bottomInnerRad, 0, toAngle, angleInc);
  lAddClosedIndices(toAdd);

  // top face vertices
  uint32_t topOuterIndex = getPointIndex();
  addSector(x, y2, z, topOuterRad, 0, toAngle, angleInc);
  lAddClosedIndices(toAdd);

  uint32_t topInnerIndex = getPointIndex();
  addSector(x, y2, z, topInnerRad, 0, toAngle, angleInc);
  lAddClosedIndices(toAdd);

  // indices
  for (uint32_t i = 0; i < toAdd; i++) {
    // bottom face indices
    lineIndices.push_back(bottomOuterIndex + i);
    lineIndices.push_back(bottomInnerIndex + i);
    
    // top face indices
    lineIndices.push_back(topOuterIndex + i);
    lineIndices.push_back(topInnerIndex + i);
    
    // outside indices
    lineIndices.push_back(bottomOuterIndex + i);
    lineIndices.push_back(topOuterIndex + i);

    // inside indices
    lineIndices.push_back(bottomInnerIndex + i);
    lineIndices.push_back(topInnerIndex + i);
  }

  // stacks
  float outerRadDiff = topOuterRad - bottomOuterRad;
  float innerRadDiff = topInnerRad - bottomInnerRad;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    uint32_t toAdd = addSector(x, y+ratio*height, z, bottomOuterRad+ratio*outerRadDiff, 0, toAngle, angleInc);
    lAddClosedIndices(toAdd); // draw border of circle
    addSector(x, y+ratio*height, z, bottomInnerRad+ratio*innerRadDiff, 0, toAngle, angleInc);
    lAddClosedIndices(toAdd); // draw border of circle
  }
}

void multishape_3d::drawLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t total_num_vertices = addLinearExtrusionVertices(shape, x, y, z, height, stacks);
  lAddExtrusionIndices(shape, stacks, total_num_vertices);
}

void multishape_3d::drawSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t total_num_vertices = addSplineExtrusionVertices(shape, points, inc);
  lAddExtrusionIndices(shape, total_num_vertices / shape.get_num_vertices() - 1, total_num_vertices);
}

uint32_t multishape_3d::addCubicSplineVertices(std::vector<glm::vec3> points, float inc) {
  int num_points = points.size();
  if (num_points <= 0) return 0;
  else if (num_points == 1) {
    drawPoint(points[0].x, points[0].y, points[0].z);
    return 0;
  }
  int num_segments = num_points - 1;
  uint32_t points_added = 0;
  std::vector<float> c_prime(num_points), dx_prime(num_points), dy_prime(num_points), dz_prime(num_points), Dx(num_points), Dy(num_points), Dz(num_points);
  c_prime[0] = 0.5;
  dx_prime[0] = 1.5 * (points[1].x - points[0].x);
  dy_prime[0] = 1.5 * (points[1].y - points[0].y);
  dz_prime[0] = 1.5 * (points[1].z - points[0].z);
  for (int i = 1; i < num_points - 1; i++) {
    c_prime[i] = 1 / (4 - c_prime[i - 1]);
    dx_prime[i] = (3*(points[i + 1].x - points[i - 1].x) - dx_prime[i - 1]) / (4 - c_prime[i - 1]);
    dy_prime[i] = (3*(points[i + 1].y - points[i - 1].y) - dy_prime[i - 1]) / (4 - c_prime[i - 1]);
    dz_prime[i] = (3*(points[i + 1].z - points[i - 1].z) - dz_prime[i - 1]) / (4 - c_prime[i - 1]);
  }
  dx_prime[num_segments] = (3*(points[num_segments].x - points[num_segments - 1].x) - dx_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);
  dy_prime[num_segments] = (3*(points[num_segments].y - points[num_segments - 1].y) - dy_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);
  dz_prime[num_segments] = (3*(points[num_segments].z - points[num_segments - 1].z) - dz_prime[num_segments - 1]) / (2 - c_prime[num_segments - 1]);

  Dx[num_points - 1] = dx_prime[num_points - 1];
  Dy[num_points - 1] = dy_prime[num_points - 1];
  Dz[num_points - 1] = dz_prime[num_points - 1];

  for (int i = num_points - 2; i >= 0; i--) {
    Dx[i] = dx_prime[i] - c_prime[i] * Dx[i + 1];
    Dy[i] = dy_prime[i] - c_prime[i] * Dy[i + 1];
    Dz[i] = dz_prime[i] - c_prime[i] * Dz[i + 1];
  }

  std::vector<float> cx(num_segments), cy(num_segments), cz(num_segments), dx(num_segments), dy(num_segments), dz(num_segments);
  for (int i = 0; i < num_segments; i++) {
    cx[i] = 3 * (points[i + 1].x - points[i].x) - 2 * Dx[i] - Dx[i + 1];
    cy[i] = 3 * (points[i + 1].y - points[i].y) - 2 * Dy[i] - Dy[i + 1];
    cz[i] = 3 * (points[i + 1].z - points[i].z) - 2 * Dz[i] - Dz[i + 1];

    dx[i] = 2 * (points[i].x - points[i + 1].x) + Dx[i] + Dx[i + 1];
    dy[i] = 2 * (points[i].y - points[i + 1].y) + Dy[i] + Dy[i + 1];
    dz[i] = 2 * (points[i].z - points[i + 1].z) + Dz[i] + Dz[i + 1];

    float t;
    for (t = 0; t < 1; t += inc) {
      add3DPoint(points[i].x + t * Dx[i] + t * t * cx[i] + t * t * t * dx[i],
               points[i].y + t * Dy[i] + t * t * cy[i] + t * t * t * dy[i],
               points[i].z + t * Dz[i] + t * t * cz[i] + t * t * t * dz[i]);
      points_added++;
    }
  }
  add3DPoint(points[num_segments].x, points[num_segments].y, points[num_segments].z);
  points_added++;
  return points_added;
}

void multishape_3d::drawCubicSpline(std::vector<glm::vec3> points, float inc) {
  lAddIndices(addCubicSplineVertices(points, inc));
}

void multishape_3d::add_draw_point(float x, float y, float z) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_POINT, {x}, {y}, {z}, {}, {}));
  drawPoint(x, y, z);
}
void multishape_3d::add_draw_line(float x1, float y1, float z1, float x2, float y2, float z2) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_LINE, {x1, x2}, {y1, y2}, {z1, z2}, {}, {}));
  drawLine(x1, y1, z1, x2, y2, z2);
}
void multishape_3d::add_fill_rect_prism(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::FILL_RECT_PRISM, {x}, {y}, {z}, {width, height, length}, {}));
  fillRectPrism(x, y, z, width, height, length);
}
void multishape_3d::add_fill_cube(float x, float y, float z, float size) {
  instructions.push_back(shape::full_instruction(instruction::FILL_CUBE, {x}, {y}, {z}, {size}, {}));
  fillCube(x, y, z, size);
}
void multishape_3d::add_fill_rect_pyramid(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::FILL_RECT_PYRAMID, {x}, {y}, {z}, {width, height, length}, {}));
  fillRectPyramid(x, y, z, width, height, length);
}
void multishape_3d::add_fill_wedge(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::FILL_WEDGE, {x}, {y}, {z}, {width, height, length}, {}));
  fillWedge(x, y, z, width, height, length);
}
void multishape_3d::add_fill_cylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_CYLINDER, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  fillCylinder(x, y, z, rad, height, sectors, stacks);
}
void multishape_3d::add_fill_cone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_CONE, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  fillCone(x, y, z, rad, height, sectors, stacks);
}
void multishape_3d::add_fill_frustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_FRUSTUM, {x}, {y}, {z}, {bottomRad, topRad, height}, {sectors, stacks}));
  fillFrustum(x, y, z, bottomRad, topRad, height, sectors, stacks);
}
void multishape_3d::add_fill_frustum_minus_frustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_FRUSTUM_MINUS_FRUSTUM, {x}, {y}, {z}, {bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height}, {sectors, stacks}));
  fillFrustumMinusFrustum(x, y, z, bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height, sectors, stacks);
}
void multishape_3d::add_fill_tube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_TUBE, {x}, {y}, {z}, {outerRad, innerRad, height}, {sectors, stacks}));
  fillTube(x, y, z, outerRad, innerRad, height, sectors, stacks);
}
void multishape_3d::add_fill_oblique_frustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_OBLIQUE_FRUSTUM, {x0, x1}, {y0, y1}, {z0, z1}, {rad0, rad1}, {sectors, stacks}));
  fillObliqueFrustum(x0, y0, z0, rad0, x1, y1, z1, rad1, sectors, stacks);
}
void multishape_3d::add_fill_oblique_cylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_OBLIQUE_CYLINDER, {x0, x1}, {y0, y1}, {z0, z1}, {rad}, {sectors, stacks}));
  fillObliqueCylinder(x0, y0, z0, x1, y1, z1, rad, sectors, stacks);
}
void multishape_3d::add_fill_oblique_cone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_OBLIQUE_CONE, {xBottom, xTop}, {yBottom, yTop}, {zBottom, zTop}, {rad}, {sectors, stacks}));
  fillObliqueCone(xBottom, yBottom, zBottom, rad, xTop, yTop, zTop, sectors, stacks);
}
void multishape_3d::add_fill_torus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_TORUS, {x}, {y}, {z}, {R, r}, {sectors, stacks}));
  fillTorus(x, y, z, R, r, sectors, stacks);
}
void multishape_3d::add_fill_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_ELLIPSOID, {x}, {y}, {z}, {a, b, c}, {sectors, stacks}));
  fillEllipsoid(x, y, z, a, b, c, sectors, stacks);
}
void multishape_3d::add_fill_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::FILL_SPHERE, {x}, {y}, {z}, {rad}, {sectors, stacks}));
  fillSphere(x, y, z, rad, sectors, stacks);
}

void multishape_3d::add_draw_rect_prism(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_RECT_PRISM, {x}, {y}, {z}, {width, height, length}, {}));
  drawRectPrism(x, y, z, width, height, length);
}
void multishape_3d::add_draw_cube(float x, float y, float z, float size) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CUBE, {x}, {y}, {z}, {size}, {}));
  drawCube(x, y, z, size);
}
void multishape_3d::add_draw_rect_pyramid(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_RECT_PYRAMID, {x}, {y}, {z}, {width, height, length}, {}));
  drawRectPyramid(x, y, z, width, height, length);
}
void multishape_3d::add_draw_wedge(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_WEDGE, {x}, {y}, {z}, {width, height, length}, {}));
  drawWedge(x, y, z, width, height, length);
}
void multishape_3d::add_draw_cylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CYLINDER, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  drawCylinder(x, y, z, rad, height, sectors, stacks);
}
void multishape_3d::add_draw_cone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_CONE, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  drawCone(x, y, z, rad, height, sectors, stacks);
}
void multishape_3d::add_draw_frustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_FRUSTUM, {x}, {y}, {z}, {bottomRad, topRad, height}, {sectors, stacks}));
  drawFrustum(x, y, z, bottomRad, topRad, height, sectors, stacks);
}
void multishape_3d::add_draw_frustum_minus_frustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_FRUSTUM_MINUS_FRUSTUM, {x}, {y}, {z}, {bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height}, {sectors, stacks}));
  drawFrustumMinusFrustum(x, y, z, bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height, sectors, stacks);
}
void multishape_3d::add_draw_tube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_TUBE, {x}, {y}, {z}, {outerRad, innerRad, height}, {sectors, stacks}));
  drawTube(x, y, z, outerRad, innerRad, height, sectors, stacks);
}
void multishape_3d::add_draw_oblique_frustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_OBLIQUE_FRUSTUM, {x0, x1}, {y0, y1}, {z0, z1}, {rad0, rad1}, {sectors, stacks}));
  drawObliqueFrustum(x0, y0, z0, rad0, x1, y1, z1, rad1, sectors, stacks);
}
void multishape_3d::add_draw_oblique_cylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_OBLIQUE_CYLINDER, {x0, x1}, {y0, y1}, {z0, z1}, {rad}, {sectors, stacks}));
  drawObliqueCylinder(x0, y0, z0, x1, y1, z1, rad, sectors, stacks);
}
void multishape_3d::add_draw_oblique_cone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_OBLIQUE_CONE, {xBottom, xTop}, {yBottom, yTop}, {zBottom, zTop}, {rad}, {sectors, stacks}));
  drawObliqueCone(xBottom, yBottom, zBottom, rad, xTop, yTop, zTop, sectors, stacks);
}
void multishape_3d::add_draw_torus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_TORUS, {x}, {y}, {z}, {R, r}, {sectors, stacks}));
  drawTorus(x, y, z, R, r, sectors, stacks);
}
void multishape_3d::add_draw_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_ELLIPSOID, {x}, {y}, {z}, {a, b, c}, {sectors, stacks}));
  drawEllipsoid(x, y, z, a, b, c, sectors, stacks);
}
void multishape_3d::add_draw_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(shape::full_instruction(instruction::DRAW_SPHERE, {x}, {y}, {z}, {rad}, {sectors, stacks}));
  drawSphere(x, y, z, rad, sectors, stacks);
}
void multishape_3d::add_draw_cubic_spline(std::vector<glm::vec3> points, float inc) {
  std::vector<float> x_coords;
  std::vector<float> y_coords;
  std::vector<float> z_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
    z_coords.push_back(p.z);
  }
  instructions.push_back(shape::full_instruction(instruction::DRAW_CUBIC_SPLINE, x_coords, y_coords, z_coords, {inc}, {(uint32_t) points.size()}));
  drawCubicSpline(points, inc);
}

void multishape_3d::add_fill_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t shape_type = to_underlying(shape.get_id());
  std::vector<float> x_coords = shape.get_x_coords();
  x_coords.push_back(x);
  std::vector<float> y_coords = shape.get_y_coords();
  y_coords.push_back(y);
  std::vector<float> floats = shape.get_floats();
  floats.push_back(height);
  std::vector<uint32_t> ints = shape.get_ints();
  ints.push_back(stacks);
  ints.push_back(shape_type);
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  instructions.push_back(shape::full_instruction(instruction::FILL_LINEAR_EXTRUSION, x_coords, y_coords, {z}, floats, ints));
  fillLinearExtrusion(shape, x, y, z, height, stacks);
}

void multishape_3d::add_draw_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t shape_type = to_underlying(shape.get_id());
  std::vector<float> x_coords = shape.get_x_coords();
  x_coords.push_back(x);
  std::vector<float> y_coords = shape.get_y_coords();
  y_coords.push_back(y);
  std::vector<float> floats = shape.get_floats();
  floats.push_back(height);
  std::vector<uint32_t> ints = shape.get_ints();
  ints.push_back(stacks);
  ints.push_back(shape_type);
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  instructions.push_back(shape::full_instruction(instruction::DRAW_LINEAR_EXTRUSION, x_coords, y_coords, {z}, floats, ints));
  drawLinearExtrusion(shape, x, y, z, height, stacks);
}

void multishape_3d::add_fill_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t shape_type = to_underlying(shape.get_id());
  std::vector<float> x_coords = shape.get_x_coords();
  std::vector<float> y_coords = shape.get_y_coords();
  std::vector<float> z_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
    z_coords.push_back(p.z);
  }
  std::vector<float> floats = shape.get_floats();
  floats.push_back(inc);
  std::vector<uint32_t> ints = shape.get_ints();
  ints.push_back(shape_type);
  ints.push_back(points.size());
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  instructions.push_back(shape::full_instruction(instruction::FILL_SPLINE_EXTRUSION, x_coords, y_coords, z_coords, floats, ints));
  fillSplineExtrusion(shape, points, inc);
}


void multishape_3d::add_draw_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t shape_type = to_underlying(shape.get_id());
  std::vector<float> x_coords = shape.get_x_coords();
  std::vector<float> y_coords = shape.get_y_coords();
  std::vector<float> z_coords;
  for (auto p : points) {
    x_coords.push_back(p.x);
    y_coords.push_back(p.y);
    z_coords.push_back(p.z);
  }
  std::vector<float> floats = shape.get_floats();
  floats.push_back(inc);
  std::vector<uint32_t> ints = shape.get_ints();
  ints.push_back(shape_type);
  ints.push_back(points.size());
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  std::rotate(ints.rbegin(), ints.rbegin() + 1, ints.rend());
  instructions.push_back(shape::full_instruction(instruction::DRAW_SPLINE_EXTRUSION, x_coords, y_coords, z_coords, floats, ints));
  drawSplineExtrusion(shape, points, inc);
}

const std::unordered_map<instruction, std::function<void(multishape::dispatch_inputs)>> multishape_3d::dispatch_table = std::unordered_map<instruction, std::function<void(dispatch_inputs)>> {
  {instruction::DRAW_POINT, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawPoint(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
  }}, 
  {instruction::DRAW_LINE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawLine(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
  }}, 
  {instruction::FILL_RECT_PRISM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillRectPrism(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_CUBE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillCube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
  }},
  {instruction::FILL_RECT_PYRAMID, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillRectPyramid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_WEDGE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillWedge(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_CYLINDER, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_CONE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_FRUSTUM_MINUS_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillFrustumMinusFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.floats[i.current_float + 4], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 5;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_TUBE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillTube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillObliqueFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_CYLINDER, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillObliqueCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_CONE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillObliqueCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_TORUS, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillTorus(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=2;
    i.current_uint32+=2;
  }},
  {instruction::FILL_ELLIPSOID, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillEllipsoid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=3;
    i.current_uint32+=2;
  }},
  {instruction::FILL_SPHERE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->fillSphere(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
    i.current_uint32+=2;
  }},
  {instruction::FILL_LINEAR_EXTRUSION, [](dispatch_inputs i) {
    shape_2d::type shape_type = static_cast<shape_2d::type>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    shape_2d shape_2d = get_shape_2d(shape_type, i);
    ((multishape_3d*)i.shape)->fillLinearExtrusion(shape_2d, i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
    i.current_uint32++;
  }},
  {instruction::FILL_SPLINE_EXTRUSION, [](dispatch_inputs i) {
    shape_2d::type shape_type = static_cast<shape_2d::type>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    shape_2d shape_2d = get_shape_2d(shape_type, i);
    std::vector<glm::vec3> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec3(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
      i.current_z_coord++;
    }
    ((multishape_3d*)i.shape)->fillSplineExtrusion(shape_2d, points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_CUBIC_SPLINE, [](dispatch_inputs i) {
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    std::vector<glm::vec3> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec3(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
      i.current_z_coord++;
    }
    ((multishape_3d*) i.shape)->drawCubicSpline(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_RECT_PRISM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawRectPrism(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_CUBE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawCube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_RECT_PYRAMID, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawRectPyramid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_WEDGE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawWedge(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_CYLINDER, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_CONE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_FRUSTUM_MINUS_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawFrustumMinusFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.floats[i.current_float + 4], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 5;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_TUBE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawTube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_FRUSTUM, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawObliqueFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_CYLINDER, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawObliqueCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_CONE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawObliqueCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_TORUS, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawTorus(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=2;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_ELLIPSOID, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawEllipsoid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=3;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_SPHERE, [](dispatch_inputs i) {
    ((multishape_3d*) i.shape)->drawSphere(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_LINEAR_EXTRUSION, [](dispatch_inputs i) {
    shape_2d::type shape_type = static_cast<shape_2d::type>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    shape_2d shape_2d = get_shape_2d(shape_type, i);
    ((multishape_3d*)i.shape)->drawLinearExtrusion(shape_2d, i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
    i.current_uint32++;
  }},
  {instruction::DRAW_SPLINE_EXTRUSION, [](dispatch_inputs i) {
    shape_2d::type shape_type = static_cast<shape_2d::type>(i.uint32s[i.current_uint32]);
    i.current_uint32++;
    uint32_t num_points = i.uint32s[i.current_uint32];
    i.current_uint32++;
    shape_2d shape_2d = get_shape_2d(shape_type, i);
    std::vector<glm::vec3> points;
    for (uint32_t j = 0; j < num_points; j++) {
      points.push_back(glm::vec3(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord]));
      i.current_x_coord++;
      i.current_y_coord++;
      i.current_z_coord++;
    }
    ((multishape_3d*)i.shape)->drawSplineExtrusion(shape_2d, points, i.floats[i.current_float]);
    i.current_float++;
  }},
};

shape_2d multishape_3d::get_shape_2d(shape_2d::type shape_type, dispatch_inputs i) {
  shape_2d shape;
  switch (shape_type) {
    case(shape_2d::type::LINE):
      shape = line(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]);
      i.current_x_coord++;
      i.current_y_coord++;
      break;
    case(shape_2d::type::POLYLINE):
      {
      uint32_t num_points = i.uint32s[i.current_uint32];
      i.current_uint32++;
      std::vector<glm::vec2> points;
      for (uint32_t j = 0; j < num_points; j++) {
        points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
        i.current_x_coord++;
        i.current_y_coord++;
      }
      shape = polyline(points);
      } 
      break;
    case(shape_2d::type::POLYGON):
      {
      uint32_t num_points = i.uint32s[i.current_uint32];
      i.current_uint32++;
      std::vector<glm::vec2> points;
      for (uint32_t j = 0; j < num_points; j++) {
        points.push_back(glm::vec2(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord]));
        i.current_x_coord++;
        i.current_y_coord++;
      }
      shape = polygon(points);
      }
      break;
    case(shape_2d::type::RECTANGLE):
      shape = rectangle(i.floats[i.current_float], i.y_coords[i.current_float + 1]);
      i.current_float+=2;
      break;
    case(shape_2d::type::SQUARE):
      shape = square(i.floats[i.current_float]);
      i.current_float++;
      break;
    case(shape_2d::type::TRIANGLE):
      shape = triangle(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.x_coords[i.current_x_coord+1], i.y_coords[i.current_y_coord+1]);
      i.current_x_coord += 2;
      i.current_y_coord += 2;
      break;
    case(shape_2d::type::CIRCLE):
      shape = circle(i.floats[i.current_float], i.uint32s[i.current_uint32]);
      i.current_float++;
      i.current_uint32++;
      break;
    case(shape_2d::type::ELLIPSE):
      shape = ellipse(i.floats[i.current_float], i.floats[i.current_float+1], i.uint32s[i.current_uint32]);
      i.current_float+=2;
      i.current_uint32++;
      break;
    case(shape_2d::type::ROUND_RECTANGLE):
      shape = round_rectangle(i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32]);
      i.current_float+=4;
      i.current_uint32++;
      break;
  }
  return shape;
}