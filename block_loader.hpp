#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <iostream>
#include "parasitic_vector.hpp"

class block_loader {
  protected:
      uint8_t* data;
      size_t capacity; // allocated size of the file
      size_t size; // current size of the bytes used in the file
  public:
      struct header {
          uint32_t magic;
          uint16_t type;
          uint64_t size;       // total size of the block_loader file aligned to 64-bit boundary
      };
      struct block_entry {
          uint32_t type;
          uint32_t size;
          uint32_t offset;
      };
      block_loader(size_t initial_capacity, uint16_t type)
       : data((uint8_t*)aligned_alloc(64, initial_capacity)), capacity(initial_capacity), size(sizeof(header)) {
          header* h = (header*)data;
          h->magic = 0x3729464a;
          h->type = type;
          h->size = size;
      }
      block_loader(const char* filename) {
          std::ifstream f(filename, std::ios::in | std::ios::binary);
          if (!f.is_open())
            throw "File not found";

          f.seekg(0, std::ios::end);
          size = f.tellg();
          capacity = size;
          data = (uint8_t*)aligned_alloc(64, size);
          f.seekg(0, std::ios::beg);
          f.read(reinterpret_cast<char*>(data), size);
          header* h = (header*)data;
          size = h->size;
      }
      // Return a pointer to the start of the data after the header
      uint8_t* start_data() {
          return data + sizeof(header);
      }
      void check_header(uint16_t type) {
          header* h = (header*)data;
          if (h->magic != 0x3729464a)
              throw "invalid magic number";
          if (h->type != type)
              throw "unexpected type";
      }
  
      void grow(size_t new_capacity) {
          if (capacity < new_capacity) {
              const uint8_t* old = data;
              data = (uint8_t*)aligned_alloc(64, new_capacity);
              memcpy(data, old, size);
              free((void*)old);
              capacity = new_capacity;
          }
      }
      void insert(size_t offset, size_t sz, const void* src) {
          if (size + sz > capacity) {
              grow(size + sz);
          }
          memcpy(this->data + offset, src, sz);
          size += sz;
      }
      void append(const void* src, size_t sz) {
          if (size + sz > capacity) {
              grow(size + sz);
          }
          memcpy(this->data + size, src, sz);
          size += sz;  // Update the current size after appending
      }
      void increment_size(size_t sz) {
        if (size + sz > capacity) {
          grow(size + sz);
        }
        size += sz;
      }
      void save(const char filename[]) {
        std::ofstream f(filename, std::ios::out | std::ios::binary);
        f.write(reinterpret_cast<const char*>(data), size);
      }
};

  