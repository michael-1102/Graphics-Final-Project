#pragma once
#include <glm/glm.hpp>

struct dir_light {
  glm::vec3 direction;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  dir_light() : direction(glm::vec3(0.f, 0.f, -1.f)),
               ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
               diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
               specular(glm::vec3(1.f, 1.f, 1.f)) {}

  dir_light(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
   : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}
};

struct point_light {
  glm::vec3 position;
  
  float constant;
  float linear;
  float quadratic;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  point_light() : position(glm::vec3(0.f, 0.f, 3.f)),
                 constant(1.f),
                 linear(0.9f),
                 quadratic(0.032f),
                 ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
                 diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
                 specular(glm::vec3(1.f, 1.f, 1.f)) {}


  point_light(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
  : position(position), constant(constant), linear(linear), quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) {}
};

struct spot_light {
  glm::vec3 position;
  glm::vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular; 
  
  spot_light() : position(glm::vec3(0.f, 0.f, 3.f)),
                direction(glm::vec3(0.f, 0.f, -1.f)),
                cutOff(glm::cos(glm::radians(12.5f))),
                outerCutOff(glm::cos(glm::radians(17.5f))),
                constant(1.f),
                linear(0.9f),
                quadratic(0.032f),
                ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
                diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
                specular(glm::vec3(1.f, 1.f, 1.f)) {}

  spot_light(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
          : position(position), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff), constant(constant), linear(linear), quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) {}
};

struct material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  
  float shininess;

  material() : ambient(glm::vec3(1.f, 1.f, 1.f)),
               diffuse(glm::vec3(1.f, 1.f, 1.f)),
               specular(glm::vec3(1.f, 1.f, 1.f)),
               shininess(32.f) {}

  material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
}; 