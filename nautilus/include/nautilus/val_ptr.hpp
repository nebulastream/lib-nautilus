
#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"

namespace nautilus {

template <is_arithmetic_ref ValueType>
class val<ValueType> {
public:
	using baseType = std::remove_cvref_t<ValueType>;
	using ref_less_type = std::remove_reference_t<ValueType>;
	using ptrType = ref_less_type*;

#ifdef ENABLE_TRACING
	tracing::value_ref state;
	val(ValueType ref) : state(tracing::value_ref()), ptr(&ref) {};
	val(ValueType ref, tracing::value_ref value_ref) : state(value_ref), ptr(&ref) {};
	val(val<ptrType> ptr, tracing::value_ref ref) : state(ref), ptr(ptr) {};
#else
	val(ValueType ref) : ptr(&ref) {};
	val(val<ptrType> ptr) : ptr(ptr) {};
#endif

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(T other) noexcept {
		auto value = make_value<baseType>(other);

		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceStore(state, value.state, tracing::to_type<ValueType>());
			return;
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		*rawPtr = details::getRawValue(value);
	}

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(val<T> other) noexcept {
		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceStore(ptr.state, other.state, tracing::to_type<ValueType>());
			return;
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		*rawPtr = details::getRawValue(other);
	}

	operator val<baseType>() {
		// load
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto ref = tracing::traceLoad(ptr.state, tracing::to_type<ValueType>());
			return val<baseType>(ref);
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		return val<baseType>(*rawPtr);
	}

private:
	val<ptrType> ptr;
	friend val<ptrType>;
};

template <is_ptr ValuePtrType>
class base_ptr_val {
public:
	using ValType = std::remove_pointer_t<ValuePtrType>;
	using raw_type = ValuePtrType;
	using basic_type = std::remove_pointer_t<ValuePtrType>;
	using pointer_type = ValuePtrType;

	base_ptr_val() : value() {};
	base_ptr_val(ValuePtrType ptr) : value(ptr) {};
	base_ptr_val(ValuePtrType ptr, tracing::value_ref tc) : state(tc), value(ptr) {};
	base_ptr_val(val<ValuePtrType>& otherValue) : state(otherValue.state), value(otherValue.value) {
	}
	base_ptr_val(tracing::value_ref ref) : state(ref), value(nullptr) {
	}

	val<ValuePtrType>& operator=(const val<ValuePtrType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(this->state, other.state, tracing::to_type<ValuePtrType>());
		}
#endif
		this->value = other.value;
		return *this;
	};

	tracing::value_ref state;
	ValuePtrType value;
};

template <is_ptr ValuePtrType>
class val<ValuePtrType> : public base_ptr_val<ValuePtrType> {
public:
	using base_ptr_val<ValuePtrType>::base_ptr_val;
};

template <is_arithmetic_ptr ValuePtrType>
class val<ValuePtrType> : public base_ptr_val<ValuePtrType> {
public:
	using base_ptr_val<ValuePtrType>::base_ptr_val;
	using ValType = base_ptr_val<ValuePtrType>::ValType;

	val<ValType&> operator*() {
#ifdef ENABLE_TRACING
		return val<ValType&>(*this, this->state);
#else
		return val<ValType&>(*this);
#endif
	};

	val<ValType&> operator[](val<int32_t> indexOffset) {
		auto valuePtr = (*this) + indexOffset;
#ifdef ENABLE_TRACING
		return val<ValType&>(valuePtr, this->state);
#else
		return val<ValType&>(valuePtr);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}

	const val<ValuePtrType>& operator++() {
		// increment
		++this->value;
		// assign
		return *this;
	}
};

template <is_void_ptr ValuePtrType>
class val<ValuePtrType> : public base_ptr_val<ValuePtrType> {
public:
	using base_ptr_val<ValuePtrType>::base_ptr_val;
	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}
};

template <is_arithmetic_ptr ValueType, typename IndexType>
val<ValueType> inline operator+(val<ValueType> left, IndexType offset) {
	auto offsetValue = make_value(offset);
	auto size = ((typename IndexType::raw_type)(sizeof(typename IndexType::raw_type)));
	auto offsetBytes = offsetValue * size;
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp<tracing::ADD, ValueType>(left.state, offsetBytes.state);
		return val<ValueType>(tc);
	}
#endif
	auto newPtr = (ValueType) (((uint8_t*) left.value) + details::getRawValue(offsetBytes));
	return val<ValueType>(newPtr);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator-(val<ValueType> left, std::remove_pointer_t<ValueType> right) {
	return val<ValueType>(left.ptr, left.index - right);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator==(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value == right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator<=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value <= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator<(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value < right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator>(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value > right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator>=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value >= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator!=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value != right.value);
}

} // namespace nautilus
