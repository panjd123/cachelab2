#pragma once
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class RegisterWarper;

template <typename T>
class MemoryWarper;

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
class PtrWarper {
   public:
    static std::vector<MemoryAccessLog<T>> access_logs;
    static T* base;
    static size_t base_offset;
    T* ptr_;
    PtrWarper(T* ptr)
        : ptr_(ptr) {}
    MemoryWarper<T> operator*() {
        return MemoryWarper<T>(ptr_);
    }
    MemoryWarper<T> operator[](int offset) {
        return MemoryWarper<T>(ptr_ + offset);
    }
    MemoryWarper<T> operator[](const RegisterWarper<T>& offset) {
        return MemoryWarper<T>(ptr_ + offset.reg_);
    }
    MemoryWarper<T> operator[](const RegisterWarper<T>&& offset) {
        return MemoryWarper<T>(ptr_ + offset.reg_);
    }
    PtrWarper<T> operator=(PtrWarper<T> other) {
        throw std::runtime_error("cannot assign a memory to another memory directly");
    }
    PtrWarper<T> operator=(RegisterWarper<T> other) {
        *ptr_ = other.reg_;
        return *this;
    }

    PtrWarper<T> operator+(int offset) {
        return PtrWarper<T>(ptr_ + offset);
    }
    PtrWarper<T> operator+(const RegisterWarper<T>& offset) {
        return PtrWarper<T>(ptr_ + offset.reg_);
    }
    PtrWarper<T> operator+(const RegisterWarper<T>&& offset) {
        return PtrWarper<T>(ptr_ + offset.reg_);
    }

    PtrWarper<T> operator-(int offset) {
        return PtrWarper<T>(ptr_ - offset);
    }
    PtrWarper<T> operator-(const RegisterWarper<T>& offset) {
        return PtrWarper<T>(ptr_ - offset.reg_);
    }
    PtrWarper<T> operator-(const RegisterWarper<T>&& offset) {
        return PtrWarper<T>(ptr_ - offset.reg_);
    }

    RegisterWarper<T> operator-(PtrWarper<T> other) {
        return RegisterWarper<T>(ptr_ - other.ptr_, true);
    }

    PtrWarper<T> operator++() {
        ptr_++;
        return *this;
    }
    PtrWarper<T> operator++(int) {
        PtrWarper ret = PtrWarper<T>(ptr_);
        ptr_++;
        return ret;
    }

    PtrWarper<T> operator--() {
        ptr_--;
        return *this;
    }

    PtrWarper<T> operator--(int) {
        PtrWarper ret = PtrWarper<T>(ptr_);
        ptr_--;
        return ret;
    }
    PtrWarper<T> operator+=(int offset) {
        ptr_ += offset;
        return *this;
    }
    PtrWarper<T> operator+=(const RegisterWarper<T>& offset) {
        ptr_ += offset.reg_;
        return *this;
    }
    PtrWarper<T> operator+=(const RegisterWarper<T>&& offset) {
        ptr_ += offset.reg_;
        return *this;
    }

    PtrWarper<T> operator-=(int offset) {
        ptr_ -= offset;
        return *this;
    }
    PtrWarper<T> operator-=(const RegisterWarper<T>& offset) {
        ptr_ -= offset.reg_;
        return *this;
    }
    PtrWarper<T> operator-=(const RegisterWarper<T>&& offset) {
        ptr_ -= offset.reg_;
        return *this;
    }
};

template <typename T>
std::vector<MemoryAccessLog<T>> PtrWarper<T>::access_logs;

template <typename T>
T* PtrWarper<T>::base = 0;

template <typename T>
size_t PtrWarper<T>::base_offset = 0;

template <typename T>
class MemoryWarper {
   public:
    T* ptr_;
    explicit MemoryWarper(T* ptr)
        : ptr_(ptr) {}
    explicit MemoryWarper(const MemoryWarper& other) = delete;
    explicit MemoryWarper(MemoryWarper&& other) = delete;
    const RegisterWarper<T>& operator=(const RegisterWarper<T>& other) {
        *ptr_ = other.reg_;
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::WRITE, ptr_, other.reg_id_});
        return other;
    }
    const RegisterWarper<T>& operator=(const RegisterWarper<T>&& other) {
        *ptr_ = other.reg_;
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::WRITE, ptr_, other.reg_id_});
        return other;
    }
    friend std::ostream& operator<<(std::ostream& os, const MemoryWarper<T>& mem) {
        os << *mem.ptr_;
        return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const MemoryWarper<T>&& mem) {
        os << *mem.ptr_;
        return os;
    }
};

namespace {
constexpr int reg_num = 32;

bool reg_map[reg_num] = {0};

inline int find_reg() {
    for (int i = 0; i < reg_num; i++) {
        if (!reg_map[i]) {
            reg_map[i] = true;
            // std::cout << "allocate reg: " << i << std::endl;
            return i;
        }
    }
    throw std::runtime_error("no available register");
}

inline void free_reg(int reg_id) {
    // std::cout << "free reg: " << reg_id << std::endl;
    reg_map[reg_id] = false;
}

}  // namespace

template <typename T>
class RegisterWarper {
   public:
    T reg_;
    bool tmp_;
    int reg_id_;

    RegisterWarper(T reg = 0, bool tmp = false)
        : reg_(reg), tmp_(tmp) {
        if (!tmp) {
            reg_id_ = find_reg();
        }
    }

    RegisterWarper(const MemoryWarper<T>& other)
        : reg_(*other.ptr_), reg_id_(find_reg()) {
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
    }

    RegisterWarper(const MemoryWarper<T>&& other)
        : reg_(*other.ptr_), reg_id_(find_reg()) {
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
    }

    RegisterWarper(const RegisterWarper<T>& other)
        : reg_(other.reg_), reg_id_(find_reg()) {
    }

    RegisterWarper(RegisterWarper<T>&& other)
        : reg_(other.reg_), tmp_(other.tmp_), reg_id_(other.reg_id_) {
        other.tmp_ = true;
    }

    ~RegisterWarper() {
        if (!tmp_) {
            free_reg(reg_id_);
        }
    }

    RegisterWarper<T>& operator=(T other) {
        reg_ = other;
        return *this;
    }

    RegisterWarper<T>& operator=(const RegisterWarper<T>& other) {
        if (tmp_) {
            throw std::runtime_error("cannot assign");
        }
        reg_ = other.reg_;
        return *this;
    }

    RegisterWarper<T>& operator=(const RegisterWarper<T>&& other) {
        if (tmp_) {
            throw std::runtime_error("cannot assign");
        }
        reg_ = other.reg_;
        return *this;
    }

    RegisterWarper<T>& operator=(const MemoryWarper<T>& other) {
        reg_ = *(other.ptr_);
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
        return *this;
    }

    RegisterWarper<T>& operator=(const MemoryWarper<T>&& other) {
        reg_ = *(other.ptr_);
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
        return *this;
    }

    /* <=> */
    bool operator<(const RegisterWarper<T>& other) {
        return reg_ < other.reg_;
    }
    bool operator<(const RegisterWarper<T>&& other) {
        return reg_ < other.reg_;
    }
    bool operator<(const T& other) {
        return reg_ < other;
    }
    bool operator<(const T&& other) {
        return reg_ < other;
    }

    bool operator>(const RegisterWarper<T>& other) {
        return reg_ > other.reg_;
    }
    bool operator>(const RegisterWarper<T>&& other) {
        return reg_ > other.reg_;
    }
    bool operator>(const T& other) {
        return reg_ > other;
    }
    bool operator>(const T&& other) {
        return reg_ > other;
    }

    bool operator<=(const RegisterWarper<T>& other) {
        return reg_ <= other.reg_;
    }
    bool operator<=(const RegisterWarper<T>&& other) {
        return reg_ <= other.reg_;
    }
    bool operator<=(const T& other) {
        return reg_ <= other;
    }
    bool operator<=(const T&& other) {
        return reg_ <= other;
    }

    bool operator>=(const RegisterWarper<T>& other) {
        return reg_ >= other.reg_;
    }
    bool operator>=(const RegisterWarper<T>&& other) {
        return reg_ >= other.reg_;
    }
    bool operator>=(const T& other) {
        return reg_ >= other;
    }
    bool operator>=(const T&& other) {
        return reg_ >= other;
    }

    bool operator==(const RegisterWarper<T>& other) {
        return reg_ == other.reg_;
    }
    bool operator==(const RegisterWarper<T>&& other) {
        return reg_ == other.reg_;
    }
    bool operator==(const T& other) {
        return reg_ == other;
    }
    bool operator==(const T&& other) {
        return reg_ == other;
    }

    /* + */

    RegisterWarper<T> operator+(const RegisterWarper<T>& other) {
        return RegisterWarper<T>(reg_ + other.reg_, true);
    }
    RegisterWarper<T> operator+(const RegisterWarper<T>&& other) {
        return RegisterWarper<T>(reg_ + other.reg_, true);
    }
    RegisterWarper<T> operator+(const T other) {
        return RegisterWarper<T>(reg_ + other, true);
    }
    friend RegisterWarper<T> operator+(const T other, const RegisterWarper<T>& reg) {
        return RegisterWarper<T>(other + reg.reg_, true);
    }

    void operator+=(const RegisterWarper<T>& other) {
        reg_ += other.reg_;
    }
    void operator+=(const RegisterWarper<T>&& other) {
        reg_ += other.reg_;
    }
    void operator+=(const T other) {
        reg_ += other;
    }

    RegisterWarper operator++() {
        reg_++;
        return RegisterWarper<T>(reg_, true);
    }
    RegisterWarper operator++(int) {
        RegisterWarper ret = RegisterWarper<T>(reg_, true);
        reg_++;
        return ret;
    }

    /* - */

    RegisterWarper<T> operator-(const RegisterWarper<T>& other) {
        return RegisterWarper<T>(reg_ - other.reg_, true);
    }
    RegisterWarper<T> operator-(const RegisterWarper<T>&& other) {
        return RegisterWarper<T>(reg_ - other.reg_, true);
    }
    RegisterWarper<T> operator-(const T other) {
        return RegisterWarper<T>(reg_ - other, true);
    }
    friend RegisterWarper<T> operator-(const T other, const RegisterWarper<T>& reg) {
        return RegisterWarper<T>(other - reg.reg_, true);
    }

    void operator-=(const RegisterWarper<T>& other) {
        reg_ -= other.reg_;
    }
    void operator-=(const RegisterWarper<T>&& other) {
        reg_ -= other.reg_;
    }
    void operator-=(const T other) {
        reg_ -= other;
    }

    RegisterWarper operator--() {
        reg_--;
        return RegisterWarper<T>(reg_, true);
    }
    RegisterWarper operator--(int) {
        RegisterWarper ret = RegisterWarper<T>(reg_, true);
        reg_--;
        return ret;
    }

    /* * */
    RegisterWarper<T> operator*(const RegisterWarper<T>& other) {
        return RegisterWarper<T>(reg_ * other.reg_, true);
    }
    RegisterWarper<T> operator*(const RegisterWarper<T>&& other) {
        return RegisterWarper<T>(reg_ * other.reg_, true);
    }
    RegisterWarper<T> operator*(const T other) {
        return RegisterWarper<T>(reg_ * other, true);
    }
    friend RegisterWarper<T> operator*(const T other, const RegisterWarper<T>& reg) {
        return RegisterWarper<T>(other * reg.reg_, true);
    }

    void operator*=(const RegisterWarper<T>& other) {
        reg_ *= other.reg_;
    }
    void operator*=(const RegisterWarper<T>&& other) {
        reg_ *= other.reg_;
    }
    void operator*=(const T other) {
        reg_ *= other;
    }

    /* / */
    RegisterWarper<T> operator/(const RegisterWarper<T>& other) {
        return RegisterWarper<T>(reg_ / other.reg_, true);
    }
    RegisterWarper<T> operator/(const RegisterWarper<T>&& other) {
        return RegisterWarper<T>(reg_ / other.reg_, true);
    }
    RegisterWarper<T> operator/(const T other) {
        return RegisterWarper<T>(reg_ / other, true);
    }
    friend RegisterWarper<T> operator/(const T other, const RegisterWarper<T>& reg) {
        return RegisterWarper<T>(other / reg.reg_, true);
    }

    void operator/=(const RegisterWarper<T>& other) {
        reg_ /= other.reg_;
    }
    void operator/=(const RegisterWarper<T>&& other) {
        reg_ /= other.reg_;
    }
    void operator/=(const T other) {
        reg_ /= other;
    }

    friend std::ostream& operator<<(std::ostream& os, const RegisterWarper<T>& reg) {
        os << reg.reg_;
        return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const RegisterWarper<T>&& reg) {
        os << reg.reg_;
        return os;
    }
};

template <typename T>
class FastBuffer {
   public:
};

using reg = RegisterWarper<int>;
using dtype_ptr = PtrWarper<int>;

inline void print_log() {
    for (auto& log : dtype_ptr::access_logs) {
        switch (log.type_) {
            case MemoryAccessType::READ:
                std::cout << " L ";
                break;
            case MemoryAccessType::WRITE:
                std::cout << " S ";
                break;
            case MemoryAccessType::READ_WRITE:
                std::cout << " M ";
                break;
            default:
                throw std::runtime_error("unkown memory access type");
        }
        std::cout << std::noshowbase << std::hex << log.addr_ - dtype_ptr::base + dtype_ptr::base_offset;
        std::cout << std::dec << ",4 " << log.reg_id_ << std::endl;
    }
}