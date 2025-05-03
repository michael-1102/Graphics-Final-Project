#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class camera {
  private:
    glm::mat4 view;
    glm::mat4 projection;

    float width; // ratio of width to height
    float height;
    glm::vec3 pos;
    glm::vec3 lookAt;
    glm::vec3 up;
    float fov;
    float zNear;
    float zFar;
    bool perspective;
    float x = 0;
    float y = 0;

   public:
    // perspective projection
    camera(float width_to_height, const glm::vec3& pos = glm::vec3(0.f, 0.f, 3.f), const glm::vec3& lookAt = glm::vec3(0.f, 0.f, 0.f),
           const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f), float fov = 45.f, float zNear = 0.1f, float zFar = 100.f)
      : width(width_to_height), height(1), pos(pos), lookAt(lookAt), up(up), fov(fov), zNear(zNear), zFar(zFar) {
      view = glm::lookAt(pos, lookAt, up);
      projection = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f/width_to_height, 1.f)) * glm::perspective(glm::radians(fov), width_to_height, zNear, zFar);
      perspective = true;
    }

    // orthographic projection
    camera(float x, float y, float width, float height, const glm::vec3& pos = glm::vec3(0.f, 0.f, 3.f), const glm::vec3& lookAt = glm::vec3(0.f, 0.f, 0.f),
    const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f), float zNear = 0.1f, float zFar = 100.f) : x(x), y(y), width(width), height(height), 
      pos(pos), lookAt(lookAt), up(up), fov(0), zNear(zNear), zFar(zFar) {
        view = glm::lookAt(pos, lookAt, up);
        projection = glm::scale(glm::mat4(1.f), glm::vec3(1.f, height/width, 1.f)) * glm::ortho(x, x + width, y, y + height, zNear, zFar);
        perspective = false;
    }

    inline bool has_perspective() { return perspective; }
    inline float get_width_to_height() { return width/height; }
    inline float get_width() { return width; }
    inline float get_height() { return height; }
    inline float get_x() { return x; }
    inline float get_y() { return y; }
    inline float get_fov() { return fov; }
    inline float get_z_near() { return zNear; }
    inline float get_z_far() { return zFar; }

    inline glm::mat4 get_view() const { return view; }
    inline glm::mat4 get_projection() const { return projection; }


    inline glm::vec3 get_pos() const { return pos; }
    inline glm::vec3 get_look_at() const { return lookAt; }
    inline glm::vec3 get_up() const { return up; }
};
