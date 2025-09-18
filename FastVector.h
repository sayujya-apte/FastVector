#pragma once

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <new>
#include <utility>
#include <type_traits>
#include <cassert>


// Why Apte::FastVector is faster :-
// 1) Explicit use of aligned allocation
// 2) Uses move vs copy whenever possible
// 3) Avoids allocator and debugging overhead
// 4) Less abstracted code

namespace Apte
{
	template<typename T>
	class FastVector
	{
	public:
		FastVector() noexcept : m_Data(nullptr), m_Size(0), m_Capacity(0)
		{
		}

		~FastVector()
		{
			// erases and deallocates vector 
			nuke();
			if (m_Data)
			{
				deallocate(m_Data, m_Capacity);
			}
		}

		// copy constructor
		FastVector(const FastVector& other) : m_Size(other.m_Size), m_Capacity(other.m_Capacity)
		{
			m_Data = allocate(m_Capacity);
			for (size_t i = 0; i < m_Size; i++)
			{
				// since this is a copy constructor, we have to copy elements instead of std::move-ing them
				// copy == SLOW
				new (m_Data + i) T(other.m_Data[i]);
			}
		}

		// move constructor
		FastVector(FastVector&& other) noexcept : m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
		{
			other.m_Data = nullptr;
			other.m_Size = 0;
			other.m_Capacity = 0;
		}

		// push_back by copying the value into the vector
		void push_back(const T& value)
		{
			if (m_Size == m_Capacity)
			{
				grow();
			}
			new (m_Data + m_Size) T(value);
			m_Size++;
		}

		// push_back by moving the value into the vector
		// ideal because moving is faster than copying
		// if we already have a named lvalue x, pass it into push back as push_back(std::move(x))
		// that way, it gets converted into a temporary rvalue, which invokes the faster move implementation
		void push_back(T&& value)
		{
			if (m_Size == m_Capacity)
			{
				grow();
			}
			new (m_Data + m_Size) T(std::move(value));
			m_Size++;
		}

		void pop_back()
		{
			if (m_Size == 0)
			{
				throw std::out_of_range("Vector is empty");
			}

			// reduces the size of the vector
			m_Size--;

			// calls the destructor of the value stored at the last index
			m_Data[m_Size].~T();
		}

		void clear()
		{
			nuke();
		}

		size_t size() const noexcept
		{
			return m_Size;
		}

		bool empty() const noexcept
		{
			return (m_Size == 0);
		}

		// move operator
		FastVector& operator=(FastVector&& other)
		{
			if (this != &other)
			{
				nuke();
				if (m_Data)
				{
					deallocate(m_Data, m_Size);
				}

				m_Data = other.m_Data;
				m_Size = other.m_Size;
				m_Capacity = other.m_Capacity;

				other.m_Data = nullptr;
				other.m_Size = 0;
				other.m_Capacity = 0;
			}

			return *this;
		}

		// [] operator to access elements
		T& operator[] (size_t index) noexcept
		{
			assert(index < m_Size);
			return m_Data[index];
		}

		// const [] operator to access const elements
		const T& operator[] (size_t index) const noexcept
		{
			assert(index < m_Size);
			return m_Data[index];
		}


		// reserves p_Capacity elements in the vector
		void reserve(size_t p_Capacity)
		{
			if (p_Capacity <= m_Capacity)
			{
				return;
			}

			T* newData = allocate(p_Capacity);
			for (size_t i = 0; i < m_Size; i++)
			{
				new (newData + i) T(std::move(m_Size[i]));
			}

			nuke();
			if (m_Data)
			{
				deallocate(m_Data, m_Capacity);
			}

			m_Data = newData;
			m_Capacity = p_Capacity;
		}

	private:
		T* m_Data;
		size_t m_Size;
		size_t m_Capacity;

		static constexpr size_t initialCapacity = 4;

		static T* allocate(size_t n)
		{
			// allocates n * sizeof(T) bytes of memory
			// allocates memory block with proper alignment
			// casts void ptr returned by "::operator new" into T*
			// calling ::operator new() instead of new() forces a call to global operator and not an overriden operator
			return static_cast<T*>(::operator new(n * sizeof(T), std::align_val_t(alignof(T))));
		}

		static void deallocate(T* ptr, size_t n)
		{
			// since allocation is aligned, deallocation must be aligned too
			// calling ::operator delete() instead of delete() forces a call to global operator and not an overriden operator
			::operator delete(ptr, n * sizeof(T), std::align_val_t(alignof(T)));
		}

		void nuke()
		{
			// destroys every element in the vector
			// sets the size of the vector to 0
			for (size_t i = 0; i < m_Size; i++)
			{
				m_Data[i].~T();
			}
		}

		void grow()
		{
			size_t newCapacity = m_Capacity > 0 ? m_Capacity * 2 : initialCapacity;
			T* newData = allocate(newCapacity);


			// check if the data type has a valid move constructor that wont throw a runtime error
			if constexpr (std::is_nothrow_move_constructible_v<T>)
			{
				for (size_t i = 0; i < m_Size; i++)
				{
					// placement new operator : 
					//     * normal new operator does two things - allocates memory, and constructs object in allocated memory
					//     * placement new operator allows us to separate those two things
					//     * new (address) dtype (initializer)
					//     * because allocate() returns raw uninitialized memory, it is necessary to use placement new to construct the object inside that memory
					new (newData + i) T(std::move(m_Data[i]));
				}
			}
			// if move operator can throw a runtime error, just copy the elements
			// copy is not the default because std::move is faster than copy
			else
			{
				for (size_t i = 0; i < m_Size; i++)
				{
					new (newData + i) T(m_Data[i]);
				}
			}

			// erases and deallocates old, small vector
			nuke();
			deallocate(m_Data, m_Capacity);

			// reassigns to new, expanded vector
			m_Data = newData;
			m_Capacity = newCapacity;
		}

	};
};
