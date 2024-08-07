#include "../exercise.h"
#include <utility>  // for std::move

class DynFibonacci {
    size_t *cache;
    int cached;
    int capacity;

public:
    // 动态设置容量的构造函数
    DynFibonacci(int capacity) : cache(new size_t[capacity]), cached(0), capacity(capacity) {
        // 初始化前两个斐波那契数
        if (capacity > 0) cache[0] = 0;
        if (capacity > 1) cache[1] = 1;
    }

    // 移动构造函数
    DynFibonacci(DynFibonacci &&other) noexcept
        : cache(other.cache), cached(other.cached), capacity(other.capacity) {
        other.cache = nullptr;  // 使其他对象的指针为空，以避免析构时重复释放
        other.cached = 0;
        other.capacity = 0;
    }

    // 移动赋值操作符
    DynFibonacci &operator=(DynFibonacci &&other) noexcept {
        if (this != &other) {  // 避免自我赋值
            delete[] cache;  // 释放当前对象的资源
            cache = other.cache;
            cached = other.cached;
            capacity = other.capacity;
            other.cache = nullptr;  // 使其他对象的指针为空
            other.cached = 0;
            other.capacity = 0;
        }
        return *this;
    }

    // 析构函数
    ~DynFibonacci() {
        delete[] cache;
    }

    // 缓存优化的斐波那契计算
    size_t operator[](int i) {
        if (i >= capacity) {
            throw std::out_of_range("Index exceeds the capacity");
        }

        if (i >= cached) {
            for (int j = cached; j <= i; ++j) {
                if (j == 0) {
                    cache[j] = 0;
                } else if (j == 1) {
                    cache[j] = 1;
                } else {
                    cache[j] = cache[j - 1] + cache[j - 2];
                }
            }
            cached = i + 1;  // 更新已缓存的最大索引
        }
        return cache[i];
    }

    // 只读方法，检查对象是否仍然存在
    bool is_alive() const {
        return cache != nullptr;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    DynFibonacci fib(12);
    ASSERT(fib[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci const fib_ = std::move(fib);
    ASSERT(!fib.is_alive(), "Object moved");
    ASSERT(fib_[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci fib0(6);
    DynFibonacci fib1(12);

    fib0 = std::move(fib1);
    fib0 = std::move(fib0);
    ASSERT(fib0[10] == 55, "fibonacci(10) should be 55");

    return 0;
}

