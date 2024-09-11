#pragma once
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class RegisterWarper;

template <typename T>
class MemoryWarper;

template <typename T>
class PtrWarper;

template <typename T>
class BaseRegisterWarper;

enum class MemoryAccessType {
    UNKOWN = 0,
    READ,
    WRITE,
    READ_WRITE
};

enum class RegisterWarperState {
    ACTIVE,
    INACTIVE,
    TMP
};

class CachelabException : public std::exception {
   public:
    CachelabException(const char* msg)
        : msg_(msg) {}
    const char* what() const noexcept override {
        return msg_;
    }

   private:
    const char* msg_;
};

class MemoryDataCalculationException : public CachelabException {
   public:
    MemoryDataCalculationException(const char* msg = "you can't directly calculate the data in memory")
        : CachelabException(msg) {}
};

class InactiveRegisterException : public CachelabException {
   public:
    InactiveRegisterException(const char* msg = "you can't operate on an inactive register")
        : CachelabException(msg) {}
};

class MemoryToMemoryAssignmentException : public CachelabException {
   public:
    MemoryToMemoryAssignmentException(const char* msg = "you can't assign a memory to another memory directly")
        : CachelabException(msg) {}
};

class OutOfRegistersException : public CachelabException {
   public:
    OutOfRegistersException(const char* msg = "no more available registers")
        : CachelabException(msg) {}
};

namespace {
constexpr int reg_num = 32;

bool reg_map[reg_num] = {0};

inline int find_reg() {
    for (int i = 0; i < reg_num; i++) {
        if (!reg_map[i]) {
            reg_map[i] = true;
            std::cerr << "allocate reg: " << i << std::endl;
            return i;
        }
    }
    throw OutOfRegistersException();
}

inline void free_reg(int reg_id) {
    std::cerr << "free reg: " << reg_id << std::endl;
    reg_map[reg_id] = false;
}

}  // namespace

template <typename T>
class BaseRegisterWarper {
   protected:
    T reg_;
    RegisterWarperState state_;
    int reg_id_;

    // you can't set state directly
    BaseRegisterWarper(T reg, RegisterWarperState state, int reg_id = -2)
        : reg_(reg), state_(state), reg_id_(reg_id) {
        if (state == RegisterWarperState::ACTIVE) {
            reg_id_ = find_reg();
        }
    }

   public:
    BaseRegisterWarper(T reg = 0)
        : reg_(reg), state_(RegisterWarperState::ACTIVE), reg_id_(find_reg()) {
    }

    BaseRegisterWarper(const MemoryWarper<T>& other)
        : reg_(*other.ptr_), state_(RegisterWarperState::ACTIVE), reg_id_(find_reg()) {
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
    }

    BaseRegisterWarper(const MemoryWarper<T>&& other)
        : reg_(*other.ptr_), state_(RegisterWarperState::ACTIVE), reg_id_(find_reg()) {
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
    }

    BaseRegisterWarper(const BaseRegisterWarper<T>& other)
        : reg_(other.reg_), state_(RegisterWarperState::ACTIVE), reg_id_(find_reg()) {
    }

    BaseRegisterWarper(BaseRegisterWarper<T>&& other)
        : reg_(other.reg_), state_(other.state_), reg_id_(other.reg_id_) {
        if (other.state_ == RegisterWarperState::ACTIVE) {
            other.state_ = RegisterWarperState::INACTIVE;
            other.reg_ = rand();
        } else if (other.state_ == RegisterWarperState::INACTIVE) {
            throw InactiveRegisterException();
        }
    }

    ~BaseRegisterWarper() {
        if (state_ == RegisterWarperState::ACTIVE) {
            free_reg(reg_id_);
        }
    }

    void check_valid() const {
        if (state_ != RegisterWarperState::ACTIVE) {
            throw InactiveRegisterException();
        }
    }

    BaseRegisterWarper<T>& operator=(T other) {
        check_valid();
        reg_ = other;
        return *this;
    }

    BaseRegisterWarper<T>& operator=(const BaseRegisterWarper<T>& other) {
        check_valid();
        reg_ = other.reg_;
        return *this;
    }

    BaseRegisterWarper<T>& operator=(BaseRegisterWarper<T>&& other) {
        // TODO:
        check_valid();
        reg_ = other.reg_;
        if (other.state_ == RegisterWarperState::ACTIVE) {
            other.state_ = RegisterWarperState::INACTIVE;
        }
        return *this;
    }

    BaseRegisterWarper<T>& operator=(const MemoryWarper<T>& other) {
        check_valid();
        reg_ = *(other.ptr_);
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
        return *this;
    }

    BaseRegisterWarper<T>& operator=(const MemoryWarper<T>&& other) {
        check_valid();
        reg_ = *(other.ptr_);
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::READ, other.ptr_, reg_id_});
        return *this;
    }

    /* <=> */
    bool operator<(const BaseRegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ < other.reg_;
    }
    bool operator<(const BaseRegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ < other.reg_;
    }
    bool operator<(const T& other) {
        check_valid();
        return reg_ < other;
    }
    bool operator<(const T&& other) {
        check_valid();
        return reg_ < other;
    }

    bool operator>(const BaseRegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ > other.reg_;
    }
    bool operator>(const BaseRegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ > other.reg_;
    }
    bool operator>(const T& other) {
        check_valid();
        return reg_ > other;
    }
    bool operator>(const T&& other) {
        check_valid();
        return reg_ > other;
    }

    bool operator<=(const BaseRegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ <= other.reg_;
    }
    bool operator<=(const BaseRegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ <= other.reg_;
    }
    bool operator<=(const T& other) {
        check_valid();
        return reg_ <= other;
    }
    bool operator<=(const T&& other) {
        check_valid();
        return reg_ <= other;
    }

    bool operator>=(const BaseRegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ >= other.reg_;
    }
    bool operator>=(const BaseRegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ >= other.reg_;
    }
    bool operator>=(const T& other) {
        check_valid();
        return reg_ >= other;
    }
    bool operator>=(const T&& other) {
        check_valid();
        return reg_ >= other;
    }

    bool operator==(const BaseRegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ == other.reg_;
    }
    bool operator==(const BaseRegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ == other.reg_;
    }
    bool operator==(const T& other) {
        check_valid();
        return reg_ == other;
    }
    bool operator==(const T&& other) {
        check_valid();
        return reg_ == other;
    }

    friend std::ostream& operator<<(std::ostream& os, const RegisterWarper<T>& reg) {
        os << reg.reg_;
        return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const RegisterWarper<T>&& reg) {
        os << reg.reg_;
        return os;
    }

    std::string info() {
        std::string state;
        switch (state_) {
            case RegisterWarperState::ACTIVE:
                state = "ACTIVE";
                break;
            case RegisterWarperState::INACTIVE:
                state = "INACTIVE";
                break;
            case RegisterWarperState::TMP:
                state = "TMP";
                break;
            default:
                state = "UNKOWN";
                break;
        }
        return "$" + std::to_string(reg_id_) + "(" + state + "): " + std::to_string(reg_);
    }

    friend class MemoryWarper<T>;
    // friend class PtrWarper<T>;
};

template <typename T>
class MemoryAccessLog {
   public:
    MemoryAccessType type_;
    T* addr_;
    int reg_id_;
};

/************************************************************************************/

template <typename T>
class PtrWarper : public BaseRegisterWarper<int> {
    // 本来 PtrWarper 设计是不占用寄存器的，后来决定改成占用一个寄存器
    // 为了不修改原有代码，我们只借用 BaseRegisterWarper<int> 的构造和析构，以让他占用一个寄存器，而不真的使用它
   public:
    static std::vector<MemoryAccessLog<T>> access_logs;
    static T* base;
    static size_t base_offset;
    T* ptr_;
    PtrWarper(T* ptr)
        : BaseRegisterWarper(), ptr_(ptr) {
    }
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

    T operator-(PtrWarper<T> other) {
        return ptr_ - other.ptr_;
    }

    PtrWarper<T> operator++() {
        ptr_++;
        return *this;
    }
    // don't implement this
    // PtrWarper<T> operator++(int) {
    // }

    PtrWarper<T> operator--() {
        ptr_--;
        return *this;
    }
    // don't implement this
    // PtrWarper<T> operator--(int) {
    // }

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

    const T& operator=(const T& other) {
        *ptr_ = other;
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::WRITE, ptr_, -1});
        return other;
    }
    const T& operator=(const T&& other) {
        *ptr_ = other;
        PtrWarper<T>::access_logs.push_back({MemoryAccessType::WRITE, ptr_, -1});
        return other;
    }

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

template <typename T>
class RegisterWarper : public BaseRegisterWarper<int> {
    static_assert(std::is_same<T, int>::value, "T must be int, may be fixed in the future");

   private:
    // you can't set state directly
    RegisterWarper(T reg, RegisterWarperState state, int reg_id = -2)
        : BaseRegisterWarper<T>(reg, state, reg_id) {
    }

   public:
    using BaseRegisterWarper<T>::BaseRegisterWarper;

    /* + */

    T operator+(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ + other.reg_;
    }
    T operator+(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ + other.reg_;
    }
    T operator+(const T other) {
        check_valid();
        return reg_ + other;
    }
    friend T operator+(const T other, const RegisterWarper<T>& reg) {
        reg.check_valid();
        return other + reg.reg_;
    }

    T operator+=(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        reg_ += other.reg_;
        return reg_;
    }
    T operator+=(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        reg_ += other.reg_;
        return reg_;
    }
    T operator+=(const T other) {
        check_valid();
        reg_ += other;
        return reg_;
    }
    T operator++() {
        check_valid();
        reg_++;
        return reg_;
    }

    // don't implement this
    // RegisterWarper operator++(int) {
    // }

    /* - */

    T operator-(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ - other.reg_;
    }
    T operator-(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ - other.reg_;
    }
    T operator-(const T other) {
        check_valid();
        other.check_valid();
        return reg_ - other;
    }
    friend T operator-(const T other, const RegisterWarper<T>& reg) {
        reg.check_valid();
        return other - reg.reg_;
    }

    T operator-=(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        reg_ -= other.reg_;
        return reg_;
    }
    T operator-=(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        reg_ -= other.reg_;
        return reg_;
    }
    T operator-=(const T other) {
        check_valid();
        reg_ -= other;
        return reg_;
    }
    T operator--() {
        check_valid();
        reg_--;
        return reg_;
    }

    // don't implement this
    // RegisterWarper operator--(int) {
    // }

    /* * */
    T operator*(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ * other.reg_;
    }
    T operator*(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ * other.reg_;
    }
    T operator*(const T other) {
        check_valid();
        return reg_ * other;
    }
    friend T operator*(const T other, const RegisterWarper<T>& reg) {
        reg.check_valid();
        return other * reg.reg_;
    }

    T operator*=(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        reg_ *= other.reg_;
        return reg_;
    }
    T operator*=(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        reg_ *= other.reg_;
        return reg_;
    }
    T operator*=(const T other) {
        check_valid();
        reg_ *= other;
        return reg_;
    }

    /* / */
    T operator/(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ / other.reg_;
    }
    T operator/(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ / other.reg_;
    }
    T operator/(const T other) {
        check_valid();
        return reg_ / other;
    }
    friend T operator/(const T other, const RegisterWarper<T>& reg) {
        reg.check_valid();
        return other / reg.reg_;
    }

    T operator/=(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        reg_ /= other.reg_;
        return reg_;
    }
    T operator/=(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        reg_ /= other.reg_;
        return reg_;
    }
    T operator/=(const T other) {
        check_valid();
        reg_ /= other;
        return reg_;
    }

    /* % */
    T operator%(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        return reg_ % other.reg_;
    }
    T operator%(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        return reg_ % other.reg_;
    }
    T operator%(const T other) {
        check_valid();
        return reg_ % other;
    }
    friend T operator%(const T other, const RegisterWarper<T>& reg) {
        reg.check_valid();
        return other % reg.reg_;
    }

    T operator%=(const RegisterWarper<T>& other) {
        check_valid();
        other.check_valid();
        reg_ %= other.reg_;
        return reg_;
    }
    T operator%=(const RegisterWarper<T>&& other) {
        check_valid();
        other.check_valid();
        reg_ %= other.reg_;
        return reg_;
    }
    T operator%=(const T other) {
        check_valid();
        reg_ %= other;
        return reg_;
    }

    friend class MemoryWarper<T>;
    friend class PtrWarper<T>;
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