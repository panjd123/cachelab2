#pragma once
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class RegisterWarper;

enum class MemoryAccessType {
    UNKOWN = 0,
    READ,
    WRITE,
    READ_WRITE
};

template <typename T>
class MemoryAccessLog {
   public:
    MemoryAccessType type_;
    T* addr_;
    int reg_id_;
};

template <typename T>
class MemoryWarper {
   public:
    static std::vector<MemoryAccessLog<T>> access_logs;
    T* ptr_;
    MemoryWarper(T* ptr)
        : ptr_(ptr) {}
    RegisterWarper<T> operator*() {
        return RegisterWarper<T>(*ptr_, false, ptr_);
    }
    RegisterWarper<T> operator[](int offset) {
        return RegisterWarper<T>(*(ptr_ + offset), false, ptr_ + offset);
    }
    MemoryWarper<T> operator=(MemoryWarper<T> other) {
        throw std::runtime_error("cannot assign a memory to another memory directly");
    }
    MemoryWarper<T> operator=(RegisterWarper<T> other) {
        *ptr_ = other.reg_;
        return *this;
    }
    MemoryWarper<T> operator+(int offset) {
        return MemoryWarper<T>(ptr_ + offset);
    }
    MemoryWarper<T> operator-(int offset) {
        return MemoryWarper<T>(ptr_ - offset);
    }
    MemoryWarper<T> operator++() {
        ptr_++;
        return *this;
    }
    MemoryWarper<T> operator--() {
        ptr_--;
        return *this;
    }
    MemoryWarper<T> operator+=(int offset) {
        ptr_ += offset;
        return *this;
    }
    MemoryWarper<T> operator-=(int offset) {
        ptr_ -= offset;
        return *this;
    }
};

template <typename T>
std::vector<MemoryAccessLog<T>> MemoryWarper<T>::access_logs;

namespace {
constexpr int reg_num = 32;

bool reg_map[reg_num] = {0};

inline int find_reg() {
    for (int i = 0; i < reg_num; i++) {
        if (!reg_map[i]) {
            reg_map[i] = true;
            return i;
        }
    }
    throw std::runtime_error("no available register");
}

inline void free_reg(int reg_id) {
    reg_map[reg_id] = false;
}

}  // namespace

template <typename T>
class RegisterWarper {
   public:
    T reg_;
    bool real_;  // 是否是真实的寄存器
    T* ptr_;
    int reg_id_;

    RegisterWarper(T reg = 0, bool real = true, T* ptr = nullptr)
        : reg_(reg), real_(real), ptr_(ptr) {
        if (real_) {
            reg_id_ = find_reg();
        }
    }

    ~RegisterWarper() {
        if (real_) {
            free_reg(reg_id_);
        }
    }

    RegisterWarper<T> operator=(T other) {
        reg_ = other;
        real_ = true;
        return reg_;
    }
    RegisterWarper<T> operator=(RegisterWarper<T> other) {
        if (!other.real_ && !real_) {
            throw std::runtime_error("cannot assign a memory to another memory directly");
        }
        reg_ = other.reg_;
        if (real_ && !other.real_) {
            *ptr_ = reg_;
            MemoryWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
        }
        if (!real_ && other.real_) {
            *ptr_ = reg_;
            MemoryWarper<T>::access_logs.push_back({MemoryAccessType::WRITE, ptr_, other.reg_id_});
        }
        // real_ 不变
        return reg_;
    }
    RegisterWarper<T> operator+(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            return RegisterWarper<T>(reg_ + other.reg_);
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    void operator+=(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            reg_ += other.reg_;
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    RegisterWarper<T> operator-(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            return RegisterWarper<T>(reg_ - other.reg_);
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    void operator-=(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            reg_ -= other.reg_;
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    RegisterWarper<T> operator*(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            return RegisterWarper<T>(reg_ * other.reg_);
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    void operator*=(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            reg_ *= other.reg_;
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    RegisterWarper<T> operator/(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            return RegisterWarper<T>(reg_ / other.reg_);
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    void operator/=(RegisterWarper<T> other) {
        if (real_ && other.real_) {
            reg_ /= other.reg_;
        }
        throw std::runtime_error("cannot use data in memory directly");
    }
    friend std::ostream& operator<<(std::ostream& os, const RegisterWarper<T>& reg) {
        os << reg.reg_;
        return os;
    }
};

using reg = RegisterWarper<int>;
using dtype_ptr = MemoryWarper<int>;