#include "shader.hpp"

shader::shader(shader_id id) : id(id) {}

void shader::init() {
  switch (id) {
    case shader_id::MATERIAL_MULTILIGHT:
      load(frag_material_multilight, vert_light);
      break;
    case shader_id::PER_VERTEX:
      load(frag_fixed_color, vert_per_vertex);
      break;
    case shader_id::FIXED_COLOR_CAMERA:
      load(frag_fixed_color, vert_fixed_color_camera);
      break;
    case shader_id::FIXED_COLOR:
    default:
      load(frag_fixed_color, vert_fixed_color);
      break;
  }
}

void shader::load(const char *frag, const char *vert) {
  uint32_t frag_id = compile(frag, GL_FRAGMENT_SHADER);
  uint32_t vert_id = compile(vert, GL_VERTEX_SHADER);
  prog_id = glCreateProgram();
  glAttachShader(prog_id, vert_id);
  glAttachShader(prog_id, frag_id);
  GLint success;
  GLchar infoLog[1024];
  glLinkProgram(prog_id);
  glGetProgramiv(prog_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(prog_id, 1024, NULL, infoLog);
    throw infoLog;
  }
  glDeleteShader(vert_id);
  glDeleteShader(frag_id);
}

uint32_t shader::compile(const char *str, GLuint prog_id) {
  uint32_t id = glCreateShader(prog_id);
  glShaderSource(id, 1, &str, nullptr);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[1024];
    glGetShaderInfoLog(id, 1024, NULL, infoLog);
    throw infoLog;
  }
  return id;
}

void shader::cleanup() {
  if (glIsProgram(prog_id) == GL_TRUE) {
    glDeleteProgram(prog_id);
  }
}


void shader::use() const { glUseProgram(prog_id); }

void shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(prog_id, name.c_str()), int(value));
}

void shader::setBool(const char name[], bool value) const {
  glUniform1i(glGetUniformLocation(prog_id, name), int(value));
}

void shader::setBool(int pos, bool value) const {
  glUniform1i(pos, int(value));
}

void shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(prog_id, name.c_str()), value);
}

void shader::setInt(const char name[], int value) const {
  glUniform1i(glGetUniformLocation(prog_id, name), value);
}

void shader::setInt(int pos, int value) const { glUniform1i(pos, value); }

void shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(prog_id, name.c_str()), value);
}

void shader::setFloat(const char name[], float value) const {
  glUniform1f(glGetUniformLocation(prog_id, name), value);
}

void shader::setFloat(int pos, float value) const { glUniform1f(pos, value); }

void shader::setVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
}

void shader::setVec2(const char name[], const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(prog_id, name), 1, &value[0]);
}

void shader::setVec2(int pos, const glm::vec2 &value) const {
  glUniform2fv(pos, 1, &value[0]);
}

void shader::setVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(prog_id, name.c_str()), x, y);
}

void shader::setVec2(const char name[], float x, float y) const {
  glUniform2f(glGetUniformLocation(prog_id, name), x, y);
}

void shader::setVec2(int pos, float x, float y) const {
  glUniform2f(pos, x, y);
}

void shader::setVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
}

void shader::setVec3(const char name[], const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(prog_id, name), 1, &value[0]);
}

void shader::setVec3(int pos, const glm::vec3 &value) const {
  glUniform3fv(pos, 1, &value[0]);
}

void shader::setVec3(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(prog_id, name.c_str()), x, y, z);
}

void shader::setVec3(const char name[], float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(prog_id, name), x, y, z);
}

void shader::setVec3(int pos, float x, float y, float z) const {
  glUniform3f(pos, x, y, z);
}

void shader::setVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
}

void shader::setVec4(const char name[], const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(prog_id, name), 1, &value[0]);
}

void shader::setVec4(int pos, const glm::vec4 &value) const {
  glUniform4fv(pos, 1, &value[0]);
}

void shader::setVec4(const std::string &name, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(prog_id, name.c_str()), x, y, z, w);
}

void shader::setVec4(const char name[], float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(prog_id, name), x, y, z, w);
}

void shader::setVec4(int pos, float x, float y, float z, float w) {
  glUniform4f(pos, x, y, z, w);
}

void shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
  glUniformMatrix2fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat2(const char name[], const glm::mat2 &mat) const {
  glUniformMatrix2fv(glGetUniformLocation(prog_id, name), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat2(int pos, const glm::mat2 &mat) const {
  glUniformMatrix2fv(pos, 1, GL_FALSE, &mat[0][0]);
}

void shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat3(const char name[], const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(prog_id, name), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat3(int pos, const glm::mat3 &mat) const {
  glUniformMatrix3fv(pos, 1, GL_FALSE, &mat[0][0]);
}

void shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat4(const char name[], const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(prog_id, name), 1, GL_FALSE,
                     &mat[0][0]);
}

void shader::setMat4(int pos, const glm::mat4 &mat) const {
  glUniformMatrix4fv(pos, 1, GL_FALSE, &mat[0][0]);
}
