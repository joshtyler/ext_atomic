#ifndef EXT_ATOMIC_HPP
#define EXT_ATOMIC_HPP

#include <atomic>
#include <type_traits>
#include <mutex>

// Allow atomic style thread safety to be used for non-trivially constuctable types
// When an object meets the criteria we use std::atomic
// Otherwise we fall back to mutex_atomic

template<class T> class mutex_atomic
{
public:
	// Copying member functions from cppreference.com
	// N.B. I haven't bothered implementing the volatile ones
	// They are deprecated if is_always_lock_free is false

	// N.B. the memory order is of no use to us in any of the functions that use it
	
	// Constructors
	mutex_atomic() noexcept = default;
	constexpr mutex_atomic(T desired) noexcept :data(desired) {};
	mutex_atomic(const mutex_atomic&) = delete;

	// Operator=
	T operator=( T desired ) noexcept
	{
		store(desired);
		return desired;
	}
	mutex_atomic& operator=( const mutex_atomic& ) = delete;

	// Store
	void store( T desired, std::memory_order order = std::memory_order_seq_cst ) noexcept
	{
		const std::lock_guard<std::mutex> lock(m);
		data = desired;
	}

	// Load
	T load( std::memory_order order = std::memory_order_seq_cst ) const noexcept
	{
		const std::lock_guard<std::mutex> lock(m);
		return data;
	}

	// Operator T
	operator T() const noexcept
	{
		return load();
	}

	// Exchange
	T exchange( T desired, std::memory_order order = std::memory_order_seq_cst ) noexcept
	{
		const std::lock_guard<std::mutex> lock(m);
		T temp = data;
		data = desired;
		return temp;
	}

	// I haven't bothered with compare_exchange_weak and compare_exchange_strong
	// They are slightly subtle in the way that they require bitwise comparisons for the underlying types (not operator=)
	// This could be interesting for some of the types we are protecting

	static constexpr bool is_always_lock_free = false; // We are never lock free!!

	// The specialised functions are only members of integral and floating point atomics
	// These would end up being 'real' atomics, so there's no point defining operators
	
private:
	mutable std::mutex m;
	T data;
};

template<class T> using ext_atomic = typename std::conditional<std::is_trivially_copyable<T>::value,std::atomic<T>,mutex_atomic<T>>::type;

#endif