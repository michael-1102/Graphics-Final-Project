#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class camera {
  private:
    glm::mat4 view;
    glm::mat4 projection;

    float width_to_height; // ratio of width to height
    glm::vec3 pos;
    glm::vec3 lookAt;
    glm::vec3 up;
    float fov;
    float zNear;
    float zFar;

  public:
    camera(float width_to_height, const glm::vec3& pos = glm::vec3(0.f, 0.f, 3.f), const glm::vec3& lookAt = glm::vec3(0.f, 0.f, 0.f),
           const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f), float fov = 45.f, float zNear = 0.1f, float zFar = 100.f)
      : width_to_height(width_to_height), pos(pos), lookAt(lookAt), up(up), fov(fov), zNear(zNear), zFar(zFar) {
      view = glm::lookAt(pos, lookAt, up);
      projection = glm::perspective(glm::radians(fov), width_to_height, zNear, zFar);
    }

    float get_width_to_height() { return width_to_height; }
    float get_fov() { return fov; }
    float get_z_near() { return zNear; }
    float get_z_far() { return zFar; }

    glm::mat4 getView() const { return view; }
    glm::mat4 getProjection() const { return projection; }


    glm::vec3 getViewPos() const { return pos; }
    glm::vec3 get_look_at() const { return lookAt; }
    glm::vec3 get_up() const { return up; }
};
