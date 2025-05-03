#include "drw_file.hpp"

lit_multishape_3d::~lit_multishape_3d() {}

void lit_multishape_3d::init() {
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
  (void*)(3 * sizeof(float)));

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
  // Create an object to hold the order at which the vertices are drawn(f             
  // indices) in order to draw it as points.                       
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pointIndices.size(),
               &pointIndices[0], GL_STATIC_DRAW);

}

void lit_multishape_3d::render(drw_file& drw, view& view) {
  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  camera& camera = drw.get_camera(camera_index);
  material& material = drw.get_material(material_index);

  shader& s = drw.get_shader(sid);

  for (uint32_t i = 0; i < num_point_lights; i++) {
    point_lights[i] = drw.get_point_light(point_light_indices[i]);
  }
  for (uint32_t i = 0; i < num_spot_lights; i++) {
    spot_lights[i] = drw.get_spot_light(spot_light_indices[i]);
  }
  for (uint32_t i = 0; i < num_dir_lights; i++) {
    dir_lights[i] = drw.get_dir_light(dir_light_indices[i]);
  }

  s.use();
  s.setMat4("transform", drw.get_transform(transform_index));
  s.setMat4("scale", drw.get_scale());
  s.setMat4("view", camera.get_view());
  s.setMat4("projection", camera.get_projection());

  s.setVec3("viewPos", camera.get_pos());

  s.setVec3("material.ambient", material.ambient);
  s.setVec3("material.diffuse", material.diffuse);
  s.setVec3("material.specular", material.specular);
  s.setFloat("material.shininess", material.shininess);

  for (uint32_t i = 0; i < num_point_lights; i++) {
    std::string iStr = std::to_string(i);
    s.setVec3("pointLight[" + iStr + "].position", point_lights[i].position);
    s.setVec3("pointLight[" + iStr + "].ambient", point_lights[i].ambient);
    s.setVec3("pointLight[" + iStr + "].diffuse", point_lights[i].diffuse);
    s.setVec3("pointLight[" + iStr + "].specular", point_lights[i].specular);
    s.setFloat("pointLight[" + iStr + "].constant", point_lights[i].constant);
    s.setFloat("pointLight[" + iStr + "].linear", point_lights[i].linear);
    s.setFloat("pointLight[" + iStr + "].quadratic", point_lights[i].quadratic);
  }

  for (uint32_t i = 0; i < num_spot_lights; i++) {
    std::string iStr = std::to_string(i);
    s.setVec3("spotLight[" + iStr + "].position", spot_lights[i].position);
    s.setVec3("spotLight[" + iStr + "].direction", spot_lights[i].direction);
    s.setFloat("spotLight[" + iStr + "].cutOff", spot_lights[i].cutOff);
    s.setFloat("spotLight[" + iStr + "].outerCutOff", spot_lights[i].outerCutOff);
    s.setVec3("spotLight[" + iStr + "].ambient", spot_lights[i].ambient);
    s.setVec3("spotLight[" + iStr + "].diffuse", spot_lights[i].diffuse);
    s.setVec3("spotLight[" + iStr + "].specular", spot_lights[i].specular);
    s.setFloat("spotLight[" + iStr + "].constant", spot_lights[i].constant);
    s.setFloat("spotLight[" + iStr + "].linear", spot_lights[i].linear);
    s.setFloat("spotLight[" + iStr + "].quadratic", spot_lights[i].quadratic);
  }

  for (uint32_t i = 0; i < num_dir_lights; i++) {
    std::string iStr = std::to_string(i);
    s.setVec3("dirLight[" + iStr + "].direction", dir_lights[i].direction);
    s.setVec3("dirLight[" + iStr + "].ambient", dir_lights[i].ambient);
    s.setVec3("dirLight[" + iStr + "].diffuse", dir_lights[i].diffuse);
    s.setVec3("dirLight[" + iStr + "].specular", dir_lights[i].specular);
  }

  s.setInt("numPointLights", num_point_lights);
  s.setInt("numDirLights", num_dir_lights);
  s.setInt("numSpotLights", num_spot_lights);

  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glLineWidth(line_width);

  // Draw Solids
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sbo);
  glDrawElements(GL_TRIANGLES, solidIndices.size(), GL_UNSIGNED_INT, 0);

  // Draw Lines
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lbo);
  glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);

  // Draw Points
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pbo);
  glDrawElements(GL_POINTS, pointIndices.size(), GL_UNSIGNED_INT, 0);

  // Unbind
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glBindVertexArray(0);
  glDisable(GL_DEPTH_TEST);
}

void lit_multishape_3d::sAddRectFace(float topLeftX, float topLeftY, float topLeftZ,
                                   float topRightX, float topRightY, float topRightZ,
                                   float bottomLeftX, float bottomLeftY, float bottomLeftZ, 
                                   float bottomRightX, float bottomRightY, float bottomRightZ, 
                                   float normalX, float normalY, float normalZ) {
 uint32_t topLeftInd = getPointIndex();
 addSolidPoint(topLeftX, topLeftY, topLeftZ, normalX, normalY, normalZ);     
 addSolidPoint(bottomLeftX, bottomLeftY, bottomLeftZ, normalX, normalY, normalZ);
 uint32_t bottomRightInd = getPointIndex();
 addSolidPoint(bottomRightX, bottomRightY, bottomRightZ, normalX, normalY, normalZ);
 addSolidPoint(topRightX, topRightY, topRightZ, normalX, normalY, normalZ);
 solidIndices.push_back(topLeftInd);
 solidIndices.push_back(bottomRightInd);
}

void lit_multishape_3d::sAddRectFace(float topLeftX, float topLeftY, float topLeftZ,
                                   float topRightX, float topRightY, float topRightZ,
                                   float bottomLeftX, float bottomLeftY, float bottomLeftZ, 
                                   float bottomRightX, float bottomRightY, float bottomRightZ) {
 uint32_t topLeftInd = getPointIndex();
 addSolidPoint(topLeftX, topLeftY, topLeftZ);     
 addSolidPoint(bottomLeftX, bottomLeftY, bottomLeftZ);
 uint32_t bottomRightInd = getPointIndex();
 addSolidPoint(bottomRightX, bottomRightY, bottomRightZ);
 addSolidPoint(topRightX, topRightY, topRightZ); 
 solidIndices.push_back(topLeftInd); // top left
 solidIndices.push_back(bottomRightInd);   
}

void lit_multishape_3d::sAddExtrusionIndices(shape_2d shape, float stacks, uint32_t total_num_vertices) {
  uint32_t num_vertices = shape.get_num_vertices();
  uint32_t num_pipe_indices = shape.get_num_pipe_indices();
  if (num_vertices <= 0 || num_pipe_indices <= 0) return;
  uint32_t num_surfaces = 0;
  for (uint32_t i = 0; i < stacks; i++) {  // for every stack
    // pipe indices
    uint32_t offset = i * num_vertices * 2;
    for (uint32_t j = 0; j < num_pipe_indices - 1; j++) {
      uint32_t index_0 = shape.get_pipe_index(j);
      uint32_t index_1 = shape.get_pipe_index(j + 1);

      sAddRectFaceIndices(offset + 2 * num_vertices + index_0, offset + 2 * num_vertices + num_vertices + index_1, offset + index_0, offset + num_vertices + index_1, total_num_vertices);
      num_surfaces++;
    }
  }
  addRectNormals(num_surfaces);
  normalize(num_surfaces * 6);

  // add end indices
  uint32_t num_end_indices = shape.get_num_solid_end_indices();
  if (num_end_indices < 3) return;
  uint32_t offset = (stacks + 1) * num_vertices * 2;
  for (uint32_t i = 0; i < num_end_indices - 1; i+=3) {
    uint32_t end_index_0 = offset + shape.get_solid_end_index(i);
    uint32_t end_index_1 = offset + shape.get_solid_end_index(i + 1);
    uint32_t end_index_2 = offset + shape.get_solid_end_index(i + 2);

    // bottom
    sAddTriFaceIndices(end_index_0, end_index_1, end_index_2, total_num_vertices);

    // top
    sAddTriFaceIndices(num_vertices + end_index_0, num_vertices + end_index_1, num_vertices + end_index_2, total_num_vertices);
  }
}

void lit_multishape_3d::addTriNormals(uint32_t numSurfaces) {
  uint32_t totalIndices = solidIndices.size();
  uint32_t start = totalIndices - numSurfaces * 3;
  for (uint32_t i = start; i < totalIndices; i += 3) {  // for every surface
    uint32_t vertIndA = elemPerVert * solidIndices[i];
    glm::vec3 A(vertices[vertIndA], vertices[vertIndA + 1], vertices[vertIndA + 2]);
    uint32_t vertIndB = elemPerVert * solidIndices[i + 1];
    glm::vec3 B(vertices[vertIndB], vertices[vertIndB + 1], vertices[vertIndB + 2]);
    uint32_t vertIndC = elemPerVert * solidIndices[i + 2];
    glm::vec3 C(vertices[vertIndC], vertices[vertIndC + 1], vertices[vertIndC + 2]);

    glm::vec3 normal = glm::cross(B - A, C - A);
    vertices[vertIndA + elemPerVert - 3] += normal.x;
    vertices[vertIndA + elemPerVert - 2] += normal.y;
    vertices[vertIndA + elemPerVert - 1] += normal.z;

    vertices[vertIndB + elemPerVert - 3] += normal.x;
    vertices[vertIndB + elemPerVert - 2] += normal.y;
    vertices[vertIndB + elemPerVert - 1] += normal.z;

    vertices[vertIndC + elemPerVert - 3] += normal.x;
    vertices[vertIndC + elemPerVert - 2] += normal.y;
    vertices[vertIndC + elemPerVert - 1] += normal.z;
  }
}

void lit_multishape_3d::addRectNormals(uint32_t numSurfaces) {
  uint32_t totalIndices = solidIndices.size();
  uint32_t start = totalIndices - numSurfaces * 6;
  for (uint32_t i = start; i < totalIndices; i += 6) {  // for every surface
    uint32_t vertIndA = elemPerVert * solidIndices[i];
    glm::vec3 A(vertices[vertIndA], vertices[vertIndA + 1], vertices[vertIndA + 2]);
    uint32_t vertIndB = elemPerVert * solidIndices[i + 1];
    glm::vec3 B(vertices[vertIndB], vertices[vertIndB + 1], vertices[vertIndB + 2]);
    uint32_t vertIndC = elemPerVert * solidIndices[i + 2];
    glm::vec3 C(vertices[vertIndC], vertices[vertIndC + 1], vertices[vertIndC + 2]);
    // assumes that 4th rect point is last point of 2nd triangle surface
    uint32_t vertIndD = elemPerVert * solidIndices[i + 5];
    glm::vec3 D(vertices[vertIndD], vertices[vertIndD + 1], vertices[vertIndD + 2]);

    glm::vec3 normal = glm::cross(B - A, C - A);
    vertices[vertIndA + elemPerVert - 3] += normal.x;
    vertices[vertIndA + elemPerVert - 2] += normal.y;
    vertices[vertIndA + elemPerVert - 1] += normal.z;

    vertices[vertIndB + elemPerVert - 3] += normal.x;
    vertices[vertIndB + elemPerVert - 2] += normal.y;
    vertices[vertIndB + elemPerVert - 1] += normal.z;

    vertices[vertIndC + elemPerVert - 3] += normal.x;
    vertices[vertIndC + elemPerVert - 2] += normal.y;
    vertices[vertIndC + elemPerVert - 1] += normal.z;

    vertices[vertIndD + elemPerVert - 3] += normal.x;
    vertices[vertIndD + elemPerVert - 2] += normal.y;
    vertices[vertIndD + elemPerVert - 1] += normal.z;
  }
}

void lit_multishape_3d::normalize(uint32_t numIndices) {
  uint32_t totalIndices = solidIndices.size();
  uint32_t start = totalIndices - numIndices;

  // normalize
  for (uint32_t i = start; i < totalIndices; i++) {
    uint32_t vertInd = elemPerVert * solidIndices[i] + elemPerVert;
    glm::vec3 normal(vertices[vertInd - 3], vertices[vertInd - 2], vertices[vertInd - 1]);
    normal = glm::normalize(normal);
    vertices[vertInd - 3] = normal.x;
    vertices[vertInd - 2] = normal.y;
    vertices[vertInd - 1] = normal.z;
  }
}

void lit_multishape_3d::fillRectPrism(float x, float y, float z, float width, float height, float length) {
  float x2 = x + width;
  float y2 = y + height;
  float z2 = z + length;
  // top left, top right, bottom left, bottom right
  sAddRectFace(x2, y2, z,    x, y2, z,
               x2, y, z,     x, y, z,
               0.f, 0.f, -1.f); // back face, bottom = y-, left = x+ 

  sAddRectFace(x, y2, z,  x, y2, z2,
               x, y, z,   x, y, z2,
               -1.f, 0.f, 0.f); // left face, bottom = y-, left = z-

  sAddRectFace(x2, y2, z2,  x2, y2, z,
               x2, y, z2,   x2, y, z,
               1.f, 0.f, 0.f); // right face, bottom = y-, left = z+

  sAddRectFace(x, y2, z2,  x2, y2, z2,
               x, y, z2,   x2, y, z2,
               0.f, 0.f, 1.f); // front face, bottom = y-, left = x-
 
  sAddRectFace(x, y2, z,   x2, y2, z,
               x, y2, z2,  x2, y2, z2,
               0.f, 1.f, 0.f); // top face, bottom = z+, left = x-

  sAddRectFace(x, y, z2,   x2, y, z2,
               x, y, z,    x2, y, z,
               0.f, -1.f, 0.f); // bottom face, bottom = z-, left = x-
}

void lit_multishape_3d::fillCube(float x, float y, float z, float size) {
  fillRectPrism(x, y, z, size, size, size);
}

void lit_multishape_3d::fillRectPyramid(float x, float y, float z, float width, float height, float length) {
  float x2 = x + width;
  float xTop = x + width / 2;
  float y2 = y + height;
  float z2 = z + length;
  float zTop = z + length / 2;

  // top left, top right, bottom left, bottom right
  sAddRectFace(x, y, z2,    x2, y, z2,
               x, y, z,    x2, y, z,
               0.f, -1.f, 0.f); // bottom face, bottom = z-, left = x-
    
  float yn1 = length / 2.f / height;
  float yn2 = width / 2.f / height;
  glm::vec3 normal1 = glm::normalize(glm::vec3(0.f, yn1, -1.f));
  addSolidPoint(x2, y, z, normal1.x, normal1.y, normal1.z);
  addSolidPoint(x, y, z, normal1.x, normal1.y, normal1.z);
  addSolidPoint(xTop, y2, zTop, normal1.x, normal1.y, normal1.z);

  glm::vec3 normal2 = glm::normalize(glm::vec3(1.f, yn2, 0.f));
  addSolidPoint(x2, y, z2, normal2.x, normal2.y, normal2.z);
  addSolidPoint(x2, y, z, normal2.x, normal2.y, normal2.z);
  addSolidPoint(xTop, y2, zTop, normal2.x, normal2.y, normal2.z);

  normal1.z = -normal1.z;
  addSolidPoint(x, y, z2, normal1.x, normal1.y, normal1.z);
  addSolidPoint(x2, y, z2, normal1.x, normal1.y, normal1.z);
  addSolidPoint(xTop, y2, zTop, normal1.x, normal1.y, normal1.z);

  normal2.x = -normal2.x;
  addSolidPoint(x, y, z, normal2.x, normal2.y, normal2.z);
  addSolidPoint(x, y, z2, normal2.x, normal2.y, normal2.z);
  addSolidPoint(xTop, y2, zTop, normal2.x, normal2.y, normal2.z);
}

void lit_multishape_3d::fillWedge(float x, float y, float z, float width, float height, float length) {
  float x2 = x + width;
  float y2 = y + height;
  float z2 = z + length;
  // top left, top right, bottom left, bottom right
  sAddRectFace(x, y, z2,   x2, y, z2,
               x, y, z,    x2, y, z,
               0.f, -1.f, 0.f); // bottom face, bottom = z-, left = x-

  sAddRectFace(x, y2, z,  x, y2, z2,
               x, y, z,   x, y, z2,
               -1.f, 0.f, 0.f); // left face, bottom = y-, left = z-
  
  // front
  addSolidPoint(x, y, z2, 0.f, 0.f, 1.f);
  addSolidPoint(x2, y, z2, 0.f, 0.f, 1.f);
  addSolidPoint(x, y2, z2, 0.f, 0.f, 1.f);

  // back
  addSolidPoint(x, y, z, 0.f, 0.f, -1.f);
  addSolidPoint(x, y2, z, 0.f, 0.f, -1.f);
  addSolidPoint(x2, y, z, 0.f, 0.f, -1.f);

  // right (slanted rectangular face)
  glm::vec3 normal = glm::normalize(glm::vec3(1.f, width/height, 0.f));
  sAddRectFace(x, y2, z2,  x, y2, z,
               x2, y, z2,  x2, y, z,
               normal.x, normal.y, normal.z); // right face, bottom = y-, left = z+
}

void lit_multishape_3d::addEllipsoidVertices(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  add3DPoint(x, y + b, z, 0.0f, 1.0f, 0.0f); //top vertex
  for (uint32_t i = 0; i < stacks; ++i) {

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

      glm::vec3 normal(x_pos - x, y_pos - y, z_pos - z);
      normal = glm::normalize(normal);
      add3DPoint(x_pos, y_pos, z_pos, normal.x, normal.y, normal.z);
    }
  }
  add3DPoint(x, y - b, z, 0.0f, -1.0f, 0.0f);//bottom vertex
}

uint32_t lit_multishape_3d::addLinearExtrusionVertices(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t num_vertices = shape.get_num_vertices();
  if (num_vertices <= 0) return 0;
  uint32_t total_num_vertices = 0;

  // pipe
  float y_inc = height / stacks;  // amount y is incremented by for every stack
  float y2 = y;
  for (uint32_t i = 0; i < stacks + 1; i++) { // for every stack + 1 because of the top
    for (uint32_t j = 0; j < num_vertices; j++) {
      glm::vec2 point = shape.get_vertex(j);
      add3DPoint(x + point.x, y2, z + point.y);
      total_num_vertices+=2;
    }
    for (uint32_t j = 0; j < num_vertices; j++) {
      glm::vec2 point = shape.get_vertex(j);
      add3DPoint(x + point.x, y2, z + point.y);
    }
    y2 += y_inc;
  }

  uint32_t num_end_indices = shape.get_num_solid_end_indices();
  if (num_end_indices < 3) return total_num_vertices;

  y2 = y + height;
  for (uint32_t j = 0; j < num_vertices; j++) {
    glm::vec2 point = shape.get_vertex(j);
    // bottom
    add3DPoint(x + point.x, y, z + point.y, 0.f, -1.f, 0.f);    
    total_num_vertices+=2;
  }

  for (uint32_t j = 0; j < num_vertices; j++) {
    glm::vec2 point = shape.get_vertex(j);
    // top
    add3DPoint(x + point.x, y2, z + point.y, 0.f, 1.f, 0.f);
  }

  return total_num_vertices;
}

uint32_t lit_multishape_3d::addSplineExtrusionVertices(shape_2d shape, std::vector<glm::vec3> points, float inc) {
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
    total_num_vertices+=2;
  }

  for (uint32_t j = 0; j < num_vertices; j++) {
    glm::vec2 point = shape.get_vertex(j);
    glm::vec3 new_point(point.x, 0, point.y);
    if (c != -1)
      new_point = r * new_point;
    new_point += curve_points[0];
    add3DPoint(new_point.x, new_point.y, new_point.z);
    new_points.push_back(new_point);
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
      total_num_vertices+=2;
    }

    for (uint32_t j = 0; j < num_vertices; j++) {
      float t = -(n.x * new_points[j].x + n.y * new_points[j].y + n.z * new_points[j].z + d) / (n.x * v1.x + n.y * v1.y + n.z * v1.z);
      glm::vec3 new_point = new_points[j] + (t * v1);
      add3DPoint(new_point.x, new_point.y, new_point.z);
      new_points[j] = new_point;
    }
  }

  glm::vec3 v1 = curve_points[last_curve_point_index] - curve_points[last_curve_point_index - 1];
  glm::vec3 n = v1 + v1; // a = n.x, b = n.y, c = n.z
  float d = - n.x * curve_points[last_curve_point_index].x - n.y * curve_points[last_curve_point_index].y - n.z * curve_points[last_curve_point_index].z;
  for (uint32_t j = 0; j < num_vertices; j++) {
    float t = -(n.x * new_points[j].x + n.y * new_points[j].y + n.z * new_points[j].z + d) / (n.x * v1.x + n.y * v1.y + n.z * v1.z);
    glm::vec3 new_point = new_points[j] + (t * v1);
    add3DPoint(new_point.x, new_point.y, new_point.z);
    total_num_vertices+=2;
    new_points[j] = new_point;
  }
  for (uint32_t j = 0; j < num_vertices; j++) {
    float t = -(n.x * new_points[j].x + n.y * new_points[j].y + n.z * new_points[j].z + d) / (n.x * v1.x + n.y * v1.y + n.z * v1.z);
    glm::vec3 new_point = new_points[j] + (t * v1);
    add3DPoint(new_point.x, new_point.y, new_point.z);
  }

  uint32_t num_end_indices = shape.get_num_solid_end_indices();
  if (num_end_indices < 3) return total_num_vertices;

  // bottom
  for (uint32_t j = 0; j < num_vertices; j++) {
    glm::vec2 point = shape.get_vertex(j);
    glm::vec3 new_point(point.x, 0, point.y);
    if (c != -1)
      new_point = r * new_point;
    new_point += curve_points[0];
    add3DPoint(new_point.x, new_point.y, new_point.z, -b.x, -b.y, -b.z);
    total_num_vertices+=2;
  }

  // top
  v1 = glm::normalize(v1);
  for (uint32_t j = 0; j < num_vertices; j++) {
    add3DPoint(new_points[j].x, new_points[j].y, new_points[j].z, v1.x, v1.y, v1.z);
  }

  return total_num_vertices;
}

void lit_multishape_3d::fillEllipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
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

void lit_multishape_3d:: fillSphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks){
  fillEllipsoid(x, y, z, rad, rad, rad, sectors, stacks);
}


void lit_multishape_3d::fillTorus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks) {
  uint32_t startIndex = getPointIndex(); // Store starting index for correct indexing
  
  // Loop to generate the vertices for the torus
  for (uint32_t i = 0; i < sectors; ++i) {
      float theta = (float)i / sectors * 2.0f * M_PI; // Angle for the big circle (around the central axis)
      float cosTheta = cos(theta);
      float sinTheta = sin(theta);

    for (uint32_t j = 0; j < stacks; ++j) {
      float phi = (float)j / stacks * 2.0f * M_PI; // Angle for the small circle (the cross-section)

      // Position of the point on the torus
      float x_pos = x + (R + r * cos(phi)) * cosTheta;
      float y_pos = y + r * sin(phi);
      float z_pos = z + (R + r * cos(phi)) * sinTheta;
      
      //torus center
      float cx = x + R *cosTheta;
      float cy = y;
      float cz = z + R *sinTheta;

      glm::vec3 normal = glm::vec3(x_pos - cx, y_pos - cy, z_pos - cz);
      normal = glm::normalize(normal);

      // Add vertex and its normal to the model
      add3DPoint(x_pos, y_pos, z_pos, normal.x, normal.y, normal.z);
    }
  }

  // Loop to generate faces (indices)
  for (uint32_t i = 0; i < sectors; ++i) {
    for (uint32_t j = 0; j < stacks; ++j) {
      uint32_t nextI = (i+1) % sectors;
      uint32_t nextJ = (j+1) % stacks;

      uint32_t first = startIndex + i * stacks + j;
      uint32_t second = startIndex + nextI * stacks + j;;
      uint32_t third = startIndex + i * stacks + nextJ;
      uint32_t fourth = startIndex + nextI * stacks + nextJ;

      // Triangle 1
      solidIndices.push_back(first);
      solidIndices.push_back(second);
      solidIndices.push_back(third);

      //Triangle 2
      solidIndices.push_back(second);
      solidIndices.push_back(fourth);
      solidIndices.push_back(third);
    }
  }
}

uint32_t lit_multishape_3d::addSector(float xc, float yc, float zc, float xn, float yn, float zn, float rad, float fromAngle, float toAngle, float angleInc) {
  uint32_t points_added = 0;
  float i;
  for (i = fromAngle; i < toAngle; i += angleInc) {
    float ang = -i * M_PI / 180.0;
    add3DPoint(float(xc + rad * cos(ang)), yc, float(zc + rad * sin(ang)), xn, yn, zn);
    points_added++;
  }
  if (i >= toAngle) {  // finish sector
    float ang = -toAngle * M_PI / 180.0;
    add3DPoint(float(xc + rad * cos(ang)), yc, float(zc + rad * sin(ang)), xn, yn, zn);
    points_added++;
  }
  return points_added;
}


uint32_t lit_multishape_3d::addOuterSector(float xc, float yc, float zc, float yn, float rad, float fromAngle, float toAngle, float angleInc) {
  uint32_t points_added = 0;
  float i;
  for (i = fromAngle; i < toAngle; i += angleInc) {
    float ang = -i * M_PI / 180.0;
    float x = float(xc + rad * cos(ang));
    float z = float(zc + rad * sin(ang));
    glm::vec3 normal = glm::normalize(glm::vec3(x - xc, 0, z - zc));
    normal.y = yn;
    normal = glm::normalize(normal);    
    add3DPoint(x, yc, z, normal.x, normal.y, normal.z);
    points_added++;
  }
  if (i >= toAngle) {  // finish sector
    float ang = -toAngle * M_PI / 180.0;
    float x = float(xc + rad * cos(ang));
    float z = float(zc + rad * sin(ang));
    glm::vec3 normal = glm::normalize(glm::vec3(x - xc, 0, z - zc));
    normal.y = yn;
    normal = glm::normalize(normal);
    add3DPoint(x, yc, z, normal.x, normal.y, normal.z);
    points_added++;
  }
  return points_added;
}

uint32_t lit_multishape_3d::addInnerSector(float xc, float yc, float zc, float yn, float rad, float fromAngle, float toAngle, float angleInc) {
  uint32_t points_added = 0;
  float i;
  for (i = fromAngle; i < toAngle; i += angleInc) {
    float ang = -i * M_PI / 180.0;
    float x = float(xc + rad * cos(ang));
    float z = float(zc + rad * sin(ang));
    glm::vec3 normal = glm::normalize(glm::vec3(xc - x, 0, zc - z));
    normal.y = yn;
    normal = glm::normalize(normal);  
    add3DPoint(x, yc, z, normal.x, normal.y, normal.z);
    points_added++;
  }
  if (i >= toAngle) {  // finish sector
    float ang = -toAngle * M_PI / 180.0;
    float x = float(xc + rad * cos(ang));
    float z = float(zc + rad * sin(ang));
    glm::vec3 normal = glm::normalize(glm::vec3(xc - x, 0, zc - z));
    normal.y = yn;
    normal = glm::normalize(normal);  
    add3DPoint(x, yc, z, normal.x, normal.y, normal.z);
    points_added++;
  }
  return points_added;
}

uint32_t lit_multishape_3d::fillCircle(float x, float y, float z, float xn, float yn, float zn, float rad, float angleInc) {
  float toAngle = 360 - angleInc;
  uint32_t centerIndex = getPointIndex();
  add3DPoint(x, y, z, xn, yn, zn);

  uint32_t toAdd = addSector(x, y, z, xn, yn, zn, rad, 0, toAngle, angleInc);
  sAddClosedSectorIndices(centerIndex, toAdd);
  return toAdd;
}

void lit_multishape_3d::fillFrustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float y2 = y + height;
  uint32_t toAdd = fillCircle(x, y, z, 0.f, -1.f, 0.f, bottomRad, angleInc);
  fillCircle(x, y2, z, 0.f, 1.f, 0.f, topRad, angleInc);

  // sides
  float yn = (bottomRad - topRad) / height;
  float toAngle = 360 - angleInc;

  uint32_t bottomIndex = getPointIndex();
  addOuterSector(x, y, z, yn, bottomRad, 0, toAngle, angleInc);

  float radDiff = topRad - bottomRad;

  uint32_t topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addOuterSector(x, y+ratio*height, z, yn, bottomRad+ratio*radDiff, 0, toAngle, angleInc);
    
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

  topIndex = getPointIndex();
  addOuterSector(x, y2, z, yn, topRad, 0, toAngle, angleInc);

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

void lit_multishape_3d::fillObliqueFrustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  if (y0 > y1) { // for accurate normal calculations, swap coords if y0 > y1
    float temp = x0;
    x0 = x1;
    x1 = temp;

    temp = y0;
    y0 = y1;
    y1 = temp;

    temp = z0;
    z0 = z1;
    z1 = temp;

    temp = rad0;
    rad0 = rad1;
    rad1 = temp;
  }

  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  uint32_t toAdd = fillCircle(x0, y0, z0, 0.f, -1.f, 0.f, rad0, angleInc);
  fillCircle(x1, y1, z1, 0.f, 1.f, 0.f, rad1, angleInc);

  // sides
  uint32_t bottomIndex = getPointIndex();
  addSector(x0, y0, z0, 0, 0, 0, rad0, 0, toAngle, angleInc);

  float xDiff = x1 - x0;
  float yDiff = y1 - y0;
  float zDiff = z1 - z0;
  float radDiff = rad1 - rad0;

  uint32_t topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addSector(x0+ratio*xDiff, y0+ratio*yDiff, z0+ratio*zDiff, 0, 0, 0, rad0+ratio*radDiff, 0, toAngle, angleInc);

    uint32_t j; 
    for (j = 0; j < toAdd - 1; j++) {
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(bottomIndex + j + 1);
      solidIndices.push_back(topIndex + j);

      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(bottomIndex + j + 1);
      solidIndices.push_back(topIndex + j + 1);
    }

    // close side
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(bottomIndex);
    solidIndices.push_back(topIndex + j);

    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(bottomIndex);
    solidIndices.push_back(topIndex);
    
    bottomIndex = topIndex;
  } 

  topIndex = getPointIndex();
  addSector(x1, y1, z1, 0, 0, 0, rad1, 0, toAngle, angleInc);

  uint32_t j;
  for (j = 0; j < toAdd - 1; j++) {
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(bottomIndex + j + 1);
    solidIndices.push_back(topIndex + j);

    solidIndices.push_back(topIndex + j);
    solidIndices.push_back(bottomIndex + j + 1);
    solidIndices.push_back(topIndex + j + 1);
  }

  // close side
  solidIndices.push_back(bottomIndex + j);
  solidIndices.push_back(bottomIndex);
  solidIndices.push_back(topIndex + j);

  solidIndices.push_back(topIndex + j);
  solidIndices.push_back(bottomIndex);
  solidIndices.push_back(topIndex);

  uint32_t numSurfaces = stacks * toAdd;
  addRectNormals(numSurfaces);
  normalize(numSurfaces * 6); // 6 indices per surface
}

void lit_multishape_3d::fillObliqueCylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  fillObliqueFrustum(x0, y0, z0, rad, x1, y1, z1, rad, sectors, stacks);
}


void lit_multishape_3d::fillCylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  fillFrustum(x, y, z, rad, rad, height, sectors, stacks);
}

void lit_multishape_3d::fillCone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  // bottom circle
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  fillCircle(x, y, z, 0.f, -1.f, 0.f, rad, angleInc);

  // sides
  float y2 = y + height;
  
  uint32_t bottomIndex = getPointIndex();
  uint32_t toAdd = addOuterSector(x, y, z, rad/height, rad, 0, toAngle, angleInc);

  uint32_t topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addOuterSector(x, y+ratio*height, z, rad/height, rad-ratio*rad, 0, toAngle, angleInc);
    
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

  topIndex = getPointIndex();
  add3DPoint(x, y2, z, 0.f, 0.f, 0.f);
  uint32_t j;
  for (j = 0; j < toAdd - 1; j++) {
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex);
    solidIndices.push_back(bottomIndex + j + 1);
  }

  // close sides
  solidIndices.push_back(bottomIndex + j);
  solidIndices.push_back(topIndex);
  solidIndices.push_back(bottomIndex);
}

void lit_multishape_3d::fillObliqueCone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  // bottom circle
  if (yTop > yBottom) 
    fillCircle(xBottom, yBottom, zBottom, 0.f, -1.f, 0.f, rad, angleInc);
  else
    fillCircle(xBottom, yBottom, zBottom, 0.f, 1.f, 0.f, rad, angleInc);

  float xDiff = xTop - xBottom;
  float yDiff = yTop - yBottom;
  float zDiff = zTop - zBottom;

  // sides
  uint32_t bottomIndex = getPointIndex();
  uint32_t toAdd = addSector(xBottom, yBottom, zBottom, 0, 0, 0, rad, 0, toAngle, angleInc);

  uint32_t topIndex;
  for (uint32_t i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topIndex = getPointIndex();
    addSector(xBottom+ratio*xDiff, yBottom+ratio*yDiff, zBottom+ratio*zDiff, 0, 0, 0, rad-ratio*rad, 0, toAngle, angleInc);

    uint32_t j; 
    if (yTop > yBottom) {
      for (j = 0; j < toAdd - 1; j++) {
        solidIndices.push_back(bottomIndex + j);
        solidIndices.push_back(bottomIndex + j + 1);
        solidIndices.push_back(topIndex + j);

        solidIndices.push_back(topIndex + j);
        solidIndices.push_back(bottomIndex + j + 1);
        solidIndices.push_back(topIndex + j + 1);
      }

      // close side
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(bottomIndex);
      solidIndices.push_back(topIndex + j);

      solidIndices.push_back(topIndex + j);
      solidIndices.push_back(bottomIndex);
      solidIndices.push_back(topIndex);
    } else {
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

    bottomIndex = topIndex;
  }  

  addRectNormals((stacks - 1) * toAdd);

  topIndex = getPointIndex();
  add3DPoint(xTop, yTop, zTop);

  uint32_t j;
  if (yTop > yBottom) {
    for (j = 0; j < toAdd - 1; j++) {
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(bottomIndex + j + 1);
      solidIndices.push_back(topIndex);
    }

    // close sides
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(bottomIndex);
    solidIndices.push_back(topIndex);
  } else {
    for (j = 0; j < toAdd - 1; j++) {
      solidIndices.push_back(bottomIndex + j);
      solidIndices.push_back(topIndex);
      solidIndices.push_back(bottomIndex + j + 1);
    }

    // close sides
    solidIndices.push_back(bottomIndex + j);
    solidIndices.push_back(topIndex);
    solidIndices.push_back(bottomIndex);
  }

  addTriNormals(toAdd);
  normalize((stacks - 1) * toAdd * 6 + toAdd * 3);
  vertices[topIndex * elemPerVert + (elemPerVert - 3)] = 0.f;
  vertices[topIndex * elemPerVert + (elemPerVert - 2)] = 0.f;
  vertices[topIndex * elemPerVert + (elemPerVert - 1)] = 0.f;
}

void lit_multishape_3d::fillTube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  fillFrustumMinusFrustum(x, y, z, outerRad, innerRad, outerRad, innerRad, height, sectors, stacks);
}

void lit_multishape_3d::fillFrustumMinusFrustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  float angleInc = 360 / sectors;
  float toAngle = 360 - angleInc;
  // bottom face vertices
  uint32_t bottomOuterStart = vertices.size();
  uint32_t bottomOuterIndex = bottomOuterStart / elemPerVert;
  uint32_t toAdd = addSector(x, y, z, 0.f, -1.f, 0.f, bottomOuterRad, 0, toAngle, angleInc);
  uint32_t bottomInnerStart = vertices.size();
  uint32_t bottomInnerIndex = bottomInnerStart / elemPerVert;
  addSector(x, y, z, 0.f, -1.f, 0.f, bottomInnerRad, 0, toAngle, angleInc);

  // top face vertices
  float y2 = y + height;
  uint32_t topOuterStart = vertices.size();
  uint32_t topOuterIndex = topOuterStart / elemPerVert;
  addSector(x, y2, z, 0.f, 1.f, 0.f, topOuterRad, 0, toAngle, angleInc);
  uint32_t topInnerStart = vertices.size();
  uint32_t topInnerIndex = topInnerStart / elemPerVert;
  addSector(x, y2, z, 0.f, 1.f, 0.f, topInnerRad, 0, toAngle, angleInc);

  // indices
  uint32_t i;
  for (i = 0; i < toAdd - 1; i++) {
    // bottom face indices
    solidIndices.push_back(bottomOuterIndex + i);
    solidIndices.push_back(bottomOuterIndex + i + 1);
    solidIndices.push_back(bottomInnerIndex + i);

    solidIndices.push_back(bottomInnerIndex + i);
    solidIndices.push_back(bottomOuterIndex + i + 1);
    solidIndices.push_back(bottomInnerIndex + i + 1);

    // top face indices
    solidIndices.push_back(topOuterIndex + i);
    solidIndices.push_back(topOuterIndex + i + 1);
    solidIndices.push_back(topInnerIndex + i);

    solidIndices.push_back(topInnerIndex + i);
    solidIndices.push_back(topOuterIndex + i + 1);
    solidIndices.push_back(topInnerIndex + i + 1);
  }
  // finish bottom face
  solidIndices.push_back(bottomOuterIndex + i);
  solidIndices.push_back(bottomOuterIndex);
  solidIndices.push_back(bottomInnerIndex + i);

  solidIndices.push_back(bottomInnerIndex + i);
  solidIndices.push_back(bottomOuterIndex);
  solidIndices.push_back(bottomInnerIndex); 

  // finish top face
  solidIndices.push_back(topOuterIndex + i);
  solidIndices.push_back(topOuterIndex);
  solidIndices.push_back(topInnerIndex + i);

  solidIndices.push_back(topInnerIndex + i);
  solidIndices.push_back(topOuterIndex);
  solidIndices.push_back(topInnerIndex);

  // sides
  float ynOuter = (bottomOuterRad - topOuterRad) / height;
  float ynInner = (topInnerRad - bottomInnerRad) / height;

  bottomOuterIndex = getPointIndex();
  toAdd = addOuterSector(x, y, z, ynOuter, bottomOuterRad, 0, toAngle, angleInc);

  bottomInnerIndex = getPointIndex();
  addInnerSector(x, y, z, ynInner, bottomInnerRad, 0, toAngle, angleInc);

  float outerRadDiff = topOuterRad - bottomOuterRad;
  float innerRadDiff = topInnerRad - bottomInnerRad;

  for (i = 1; i < stacks; i++) {
    float ratio = (1.0*i / stacks);
    topOuterIndex = getPointIndex();
    addOuterSector(x, y+ratio*height, z, ynOuter, bottomOuterRad+ratio*outerRadDiff, 0, toAngle, angleInc);
    topInnerIndex = getPointIndex();
    addInnerSector(x, y+ratio*height, z, ynInner, bottomInnerRad+ratio*innerRadDiff, 0, toAngle, angleInc);
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

  topOuterIndex = getPointIndex();
  addOuterSector(x, y2, z, ynOuter, topOuterRad, 0, toAngle, angleInc);

  topInnerIndex = getPointIndex();
  addInnerSector(x, y2, z, ynInner, topInnerRad, 0, toAngle, angleInc);

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
}

void lit_multishape_3d::fillLinearExtrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
  uint32_t num_vertices = addLinearExtrusionVertices(shape, x, y, z, height, stacks);
  sAddExtrusionIndices(shape, stacks, num_vertices);
}

void lit_multishape_3d::fillSplineExtrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
  uint32_t total_num_vertices = addSplineExtrusionVertices(shape, points, inc);
  sAddExtrusionIndices(shape, total_num_vertices / shape.get_num_vertices() / 2 - 2, total_num_vertices);
}

void lit_multishape_3d::add_fill_rect_prism(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_RECT_PRISM, {x}, {y}, {z}, {width, height, length}, {}));
  fillRectPrism(x, y, z, width, height, length);
}
void lit_multishape_3d::add_fill_cube(float x, float y, float z, float size) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_CUBE, {x}, {y}, {z}, {size}, {}));
  fillCube(x, y, z, size);
}
void lit_multishape_3d::add_fill_rect_pyramid(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_RECT_PYRAMID, {x}, {y}, {z}, {width, height, length}, {}));
  fillRectPyramid(x, y, z, width, height, length);
}
void lit_multishape_3d::add_fill_wedge(float x, float y, float z, float width, float height, float length) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_WEDGE, {x}, {y}, {z}, {width, height, length}, {}));
  fillWedge(x, y, z, width, height, length);
}
void lit_multishape_3d::add_fill_cylinder(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_CYLINDER, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  fillCylinder(x, y, z, rad, height, sectors, stacks);
}
void lit_multishape_3d::add_fill_cone(float x, float y, float z, float rad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_CONE, {x}, {y}, {z}, {rad, height}, {sectors, stacks}));
  fillCone(x, y, z, rad, height, sectors, stacks);
}
void lit_multishape_3d::add_fill_frustum(float x, float y, float z, float bottomRad, float topRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_FRUSTUM, {x}, {y}, {z}, {bottomRad, topRad, height}, {sectors, stacks}));
  fillFrustum(x, y, z, bottomRad, topRad, height, sectors, stacks);
}
void lit_multishape_3d::add_fill_frustum_minus_frustum(float x, float y, float z, float bottomOuterRad, float bottomInnerRad, float topOuterRad, float topInnerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_FRUSTUM_MINUS_FRUSTUM, {x}, {y}, {z}, {bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height}, {sectors, stacks}));
  fillFrustumMinusFrustum(x, y, z, bottomOuterRad, bottomInnerRad, topOuterRad, topInnerRad, height, sectors, stacks);
}
void lit_multishape_3d::add_fill_tube(float x, float y, float z, float outerRad, float innerRad, float height, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_TUBE, {x}, {y}, {z}, {outerRad, innerRad, height}, {sectors, stacks}));
  fillTube(x, y, z, outerRad, innerRad, height, sectors, stacks);
}
void lit_multishape_3d::add_fill_oblique_frustum(float x0, float y0, float z0, float rad0, float x1, float y1, float z1, float rad1, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_OBLIQUE_FRUSTUM, {x0, x1}, {y0, y1}, {z0, z1}, {rad0, rad1}, {sectors, stacks}));
  fillObliqueFrustum(x0, y0, z0, rad0, x1, y1, z1, rad1, sectors, stacks);
}
void lit_multishape_3d::add_fill_oblique_cylinder(float x0, float y0, float z0, float x1, float y1, float z1, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_OBLIQUE_CYLINDER, {x0, x1}, {y0, y1}, {z0, z1}, {rad}, {sectors, stacks}));
  fillObliqueCylinder(x0, y0, z0, x1, y1, z1, rad, sectors, stacks);
}
void lit_multishape_3d::add_fill_oblique_cone(float xBottom, float yBottom, float zBottom, float rad, float xTop, float yTop, float zTop, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_OBLIQUE_CONE, {xBottom, xTop}, {yBottom, yTop}, {zBottom, zTop}, {rad}, {sectors, stacks}));
  fillObliqueCone(xBottom, yBottom, zBottom, rad, xTop, yTop, zTop, sectors, stacks);
}
void lit_multishape_3d::add_fill_torus(float x, float y, float z, float R, float r, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_TORUS, {x}, {y}, {z}, {R, r}, {sectors, stacks}));
  fillTorus(x, y, z, R, r, sectors, stacks);
}
void lit_multishape_3d::add_fill_ellipsoid(float x, float y, float z, float a, float b, float c, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_ELLIPSOID, {x}, {y}, {z}, {a, b, c}, {sectors, stacks}));
  fillEllipsoid(x, y, z, a, b, c, sectors, stacks);
}
void lit_multishape_3d::add_fill_sphere(float x, float y, float z, float rad, uint32_t sectors, uint32_t stacks) {
  instructions.push_back(multishape::full_instruction(instruction::FILL_SPHERE, {x}, {y}, {z}, {rad}, {sectors, stacks}));
  fillSphere(x, y, z, rad, sectors, stacks);
}

void lit_multishape_3d::add_fill_linear_extrusion(shape_2d shape, float x, float y, float z, float height, uint32_t stacks) {
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
  instructions.push_back(multishape::full_instruction(instruction::FILL_LINEAR_EXTRUSION, x_coords, y_coords, {z}, floats, ints));
  fillLinearExtrusion(shape, x, y, z, height, stacks);
}

void lit_multishape_3d::add_fill_spline_extrusion(shape_2d shape, std::vector<glm::vec3> points, float inc) {
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
  instructions.push_back(multishape::full_instruction(instruction::FILL_SPLINE_EXTRUSION, x_coords, y_coords, z_coords, floats, ints));
  fillSplineExtrusion(shape, points, inc);
}


void lit_multishape_3d::drawNormals() {
  float factor = 0.25;
  uint32_t numVerts = vertices.size();
  for (uint32_t i = 0; i < numVerts; i += elemPerVert) {
    lineIndices.push_back(i/elemPerVert);
    add3DPoint(vertices[i] + vertices[i + 3] * factor, 
               vertices[i + 1] + vertices[i + 4] * factor, 
              vertices[i + 2] + vertices[i + 5] * factor);
    lineIndices.push_back(getPointIndex() - 1);
  }
}

const std::unordered_map<instruction, std::function<void(multishape::dispatch_inputs)>> lit_multishape_3d::dispatch_table = std::unordered_map<instruction, std::function<void(dispatch_inputs)>> {
  {instruction::DRAW_POINT, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawPoint(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
  }}, 
  {instruction::DRAW_LINE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawLine(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
  }}, 
  {instruction::FILL_RECT_PRISM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillRectPrism(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_CUBE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillCube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
  }},
  {instruction::FILL_RECT_PYRAMID, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillRectPyramid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_WEDGE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillWedge(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::FILL_CYLINDER, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_CONE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_FRUSTUM_MINUS_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillFrustumMinusFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.floats[i.current_float + 4], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 5;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_TUBE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillTube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillObliqueFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_CYLINDER, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillObliqueCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_OBLIQUE_CONE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillObliqueCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::FILL_TORUS, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillTorus(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=2;
    i.current_uint32+=2;
  }},
  {instruction::FILL_HELIX, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillHelix(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=4;
    i.current_uint32+=2;
  }},
  {instruction::FILL_ELLIPSOID, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillEllipsoid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=3;
    i.current_uint32+=2;
  }},
  {instruction::FILL_SPHERE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->fillSphere(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
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
    ((lit_multishape_3d*)i.shape)->fillLinearExtrusion(shape_2d, i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
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
    ((lit_multishape_3d*)i.shape)->fillSplineExtrusion(shape_2d, points, i.floats[i.current_float]);
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
    ((lit_multishape_3d*) i.shape)->drawCubicSpline(points, i.floats[i.current_float]);
    i.current_float++;
  }},
  {instruction::DRAW_RECT_PRISM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawRectPrism(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_CUBE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawCube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float++;
  }},
  {instruction::DRAW_RECT_PYRAMID, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawRectPyramid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_WEDGE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawWedge(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
  }},
  {instruction::DRAW_CYLINDER, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_CONE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_FRUSTUM_MINUS_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawFrustumMinusFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.floats[i.current_float + 4], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 5;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_TUBE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawTube(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float += 3;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_FRUSTUM, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawObliqueFrustum(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float += 2;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_CYLINDER, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawObliqueCylinder(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_OBLIQUE_CONE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawObliqueCone(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.x_coords[i.current_x_coord + 1], i.y_coords[i.current_y_coord + 1], i.z_coords[i.current_z_coord + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord+=2;
    i.current_y_coord+=2;
    i.current_z_coord+=2;
    i.current_float++;
    i.current_uint32 += 2;
  }},
  {instruction::DRAW_TORUS, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawTorus(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=2;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_HELIX, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawHelix(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.floats[i.current_float + 3], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=4;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_ELLIPSOID, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawEllipsoid(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.floats[i.current_float + 1], i.floats[i.current_float + 2], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
    i.current_x_coord++;
    i.current_y_coord++;
    i.current_z_coord++;
    i.current_float+=3;
    i.current_uint32+=2;
  }},
  {instruction::DRAW_SPHERE, [](dispatch_inputs i) {
    ((lit_multishape_3d*) i.shape)->drawSphere(i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32], i.uint32s[i.current_uint32 + 1]);
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
    ((lit_multishape_3d*)i.shape)->drawLinearExtrusion(shape_2d, i.x_coords[i.current_x_coord], i.y_coords[i.current_y_coord], i.z_coords[i.current_z_coord], i.floats[i.current_float], i.uint32s[i.current_uint32]);
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
    ((lit_multishape_3d*)i.shape)->drawSplineExtrusion(shape_2d, points, i.floats[i.current_float]);
    i.current_float++;
  }},
};