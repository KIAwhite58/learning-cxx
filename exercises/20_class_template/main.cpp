#include "../exercise.h"
#include <cstring>
#include <algorithm>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        std::memcpy(shape, shape_, sizeof(shape));
        for (unsigned int i = 0; i < 4; ++i) {
            size *= shape[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }

    ~Tensor4D() {
        delete[] data;
    }

    // 禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    Tensor4D &operator+=(Tensor4D const &others) {
        // 检查是否可以进行广播
        for (int i = 0; i < 4; ++i) {
            if (shape[i] != others.shape[i] && others.shape[i] != 1) {
                throw std::invalid_argument("Shapes are not compatible for broadcasting");
            }
        }

        unsigned int strides[4];
        unsigned int other_strides[4];
        strides[3] = 1;
        other_strides[3] = 1;

        for (int i = 2; i >= 0; --i) {
            strides[i] = strides[i + 1] * shape[i + 1];
            other_strides[i] = other_strides[i + 1] * (others.shape[i + 1] == 1 ? shape[i + 1] : others.shape[i + 1]);
        }

        unsigned int size = 1;
        for (int i = 0; i < 4; ++i) {
            size *= shape[i];
        }

        // 执行加法操作
        for (unsigned int i = 0; i < size; ++i) {
            unsigned int indices[4];
            unsigned int temp = i;
            for (int j = 3; j >= 0; --j) {
                indices[j] = temp / strides[j];
                temp %= strides[j];
            }

            unsigned int other_indices[4];
            for (int j = 0; j < 4; ++j) {
                other_indices[j] = (others.shape[j] == 1) ? indices[j] : indices[j] % others.shape[j];
            }

            unsigned int other_index = 0;
            for (int j = 0; j < 4; ++j) {
                other_index += other_indices[j] * other_strides[j];
            }

            data[i] += others.data[other_index];
        }

        return *this;
    }

    // NOTICE: 不要修改这个方法
    size_t operator[](int i) const {
        ASSERT(i <= cached, "i out of range");
        return cache[i];
    }

    // NOTICE: 不要修改这个方法
    bool is_alive() const {
        return cache;
    }
};


// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
