template <typename T>
class parasitic_vector {
public:
    T* data;
    size_t sz;
    parasitic_vector() : data(nullptr), sz(0) {}
    parasitic_vector(char*& data, size_t sz) : data((T*)data), sz(sz) {
        data += sz * sizeof(T);
    }

    T& operator[](size_t i) {
        return data[i];
    }

    parasitic_vector<T>* delta_decode() {
      for (uint32_t i = 1; i < sz; i++) {
        data[i] = data[i - 1] + data[i];
      }
      return this;
    }

    const T& operator[](size_t i) const {
        return data[i];
    }

    size_t size() const {
        return sz;
    }

    std::vector<T> convert_to_std_vector() {
      std::vector<T> vec;
      for (uint32_t i = 0; i < sz; i++) {
        vec.push_back(data[i]);
      }
      return vec;
    }
};