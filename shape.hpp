#pragma once
#include <cstdint>
#include "shader.hpp"
#include "instruction.hpp"
#include "view.hpp"

class drw_file;

class shape {
  public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render(drw_file& drw, view& view) = 0;


    struct full_instruction {
      instruction instr;
      std::vector<float> x_coords;
      std::vector<float> y_coords;
      std::vector<float> z_coords;
      std::vector<float> misc_floats;
      std::vector<uint32_t> misc_ints;
  
      full_instruction() : instr(instruction::NO_INSTRUCTION) {}
  
      full_instruction(instruction instr, std::vector<float> x_coords, std::vector<float> y_coords, std::vector<float> z_coords, std::vector<float> misc_floats, std::vector<uint32_t> misc_ints) 
                    : instr(instr), x_coords(x_coords), y_coords(y_coords), z_coords(z_coords), misc_floats(misc_floats), misc_ints(misc_ints) {}
    };
  
    std::vector<full_instruction> get_full_instructions() {
      return instructions;
    }
  
    void set_start_instr(full_instruction start_instr) {
      if (instructions.size() <= 0) instructions.push_back(start_instr);
      else instructions[0] = start_instr;
    }
  
    instruction get_end_instr() { return end_instr; }
  
  protected:
    uint32_t vao, vbo, sbo, lbo, pbo, sbo_front;
    std::vector<full_instruction> instructions;
    instruction end_instr;
  
};