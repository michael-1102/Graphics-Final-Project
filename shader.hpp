#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <vector>

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

enum class shader_id {
  FIXED_COLOR,
  FIXED_COLOR_CAMERA,
  PER_VERTEX,
  MATERIAL_MULTILIGHT
};

const char* const frag_fixed_color = 1 + R"GLSL(
  #version 310 es
  precision mediump float;

  in vec4 rgba;
  out vec4 color;

  void main()
  {
    color = vec4(rgba);
  }
)GLSL";
const char* const vert_fixed_color = 1 + R"GLSL(
  #version 310 es
  precision mediump float;

  layout(location = 0) in vec3 pos;

  uniform vec4 color;
  uniform mat4 transform;
  uniform mat4 view;
  uniform mat4 scale;

  out vec4 rgba;
  void main(){
    gl_Position = transform*scale*view*vec4(pos, 1.0);
    rgba = color;
  }
)GLSL";
const char* const vert_fixed_color_camera = 1 + R"GLSL(
  #version 310 es
  precision mediump float;

  layout(location = 0) in vec3 pos;

  uniform vec4 color;
  uniform mat4 transform;
  uniform mat4 view;
  uniform mat4 projection;

  out vec4 rgba;
  void main(){
    gl_Position = projection * view * transform * vec4(pos, 1.0);
    rgba = color;
  }
)GLSL";

const char* const vert_per_vertex = 1 + R"GLSL(
  #version 310 es
  precision mediump float;

  layout(location = 0) in vec3 pos;
  layout(location = 1) in vec4 color;

  uniform mat4 transform;
  uniform mat4 view;
  uniform mat4 scale;

  out vec4 rgba;
  void main(){
    gl_Position = transform*scale*view*vec4(pos, 1.0);
    rgba = color;
  }
)GLSL";

const char* const frag_material_multilight = 1 + R"GLSL(
#version 310 es
precision mediump float;
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_NUM_TOTAL_LIGHTS 100

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform DirLight dirLight[MAX_NUM_TOTAL_LIGHTS];
uniform int numDirLights;

uniform PointLight pointLight[MAX_NUM_TOTAL_LIGHTS];
uniform int numPointLights;

uniform SpotLight spotLight[MAX_NUM_TOTAL_LIGHTS];
uniform int numSpotLights;

uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result;
    for(int i = 0; i < numDirLights; i++)
      result += CalcDirLight(dirLight[i], norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < numPointLights; i++)
      result += CalcPointLight(pointLight[i], norm, FragPos, viewDir);    
    // phase 3: spot light
    for(int i = 0; i < numSpotLights; i++)
      result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
  }
)GLSL";

const char* const vert_light = 1 + R"GLSL(
#version 310 es
precision mediump float;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  FragPos = vec3(transform * vec4(pos, 1.0));
  Normal = mat3(transpose(inverse(transform))) * normal;  
  
  gl_Position = projection * view * vec4(FragPos, 1.0);
}
)GLSL";

class shader {
 private:
  uint32_t prog_id;
  shader_id id;
  void load(const char* frag, const char* vert);
  uint32_t compile(const char* str, GLuint shader_id);

 public:
  shader(shader_id s);
  shader() {}

  void cleanup();

  void init();
  void use() const;
  void setBool(const std::string &name, bool value) const;
  void setBool(const char name[], bool value) const;
  void setBool(int pos, bool value) const;

  void setInt(const std::string &name, int value) const;
  void setInt(const char name[], int value) const;
  void setInt(int pos, int value) const;

  void setFloat(const std::string &name, float value) const;
  void setFloat(const char name[], float value) const;
  void setFloat(int pos, float value) const;

  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec2(const char name[], const glm::vec2 &value) const;
  void setVec2(int pos, const glm::vec2 &value) const;

  void setVec2(const std::string &name, float x, float y) const;
  void setVec2(const char name[], float x, float y) const;
  void setVec2(int pos, float x, float y) const;

  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec3(const char name[], const glm::vec3 &value) const;
  void setVec3(int pos, const glm::vec3 &value) const;

  void setVec3(const std::string &name, float x, float y, float z) const;
  void setVec3(const char name[], float x, float y, float z) const;
  void setVec3(int pos, float x, float y, float z) const;

  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setVec4(const char name[], const glm::vec4 &value) const;
  void setVec4(int pos, const glm::vec4 &value) const;

  void setVec4(const std::string &name, float x, float y, float z, float w);
  void setVec4(const char name[], float x, float y, float z, float w);
  void setVec4(int pos, float x, float y, float z, float w);

  void setMat2(const std::string &name, const glm::mat2 &mat) const;
  void setMat2(const char name[], const glm::mat2 &mat) const;
  void setMat2(int pos, const glm::mat2 &mat) const;

  void setMat3(const std::string &name, const glm::mat3 &mat) const;
  void setMat3(const char name[], const glm::mat3 &mat) const;
  void setMat3(int pos, const glm::mat3 &mat) const;

  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void setMat4(const char name[], const glm::mat4 &mat) const;
  void setMat4(int pos, const glm::mat4 &mat) const;

  uint32_t getID() const { return prog_id; }
};
