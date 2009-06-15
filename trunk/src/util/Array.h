// Array.h

#ifndef ARRAY_H
#define ARRAY_H

#include <ccsponge.h>

/*
 * This is your standard templated array class. It represents a non-resizable
 * array.
 */
template <typename T>
class Array
{
public:
	Array();
	explicit Array(uint32 size);
	Array(T* existingArray, uint32 size);
	Array(const Array &other);
	~Array();

	T get(uint32 index) const;
	void set(uint32 index, T value);
	uint32 size() const;
	T* getRawPtr();

	Array<T>& operator=(const Array<T>& other);
	T &operator[](int index); // Returns non-modifiable copy
	T operator[](int index) const; // Returns modifiable copy

private:
	T* m_data;
	uint32 m_size;
};

template <typename T>
Array<T>::Array()
{
	m_size = 0;
	m_data = NULL;
}

template <typename T>
Array<T>::Array(uint32 size)
{
	m_size = size;
	m_data = new T[size];
}

template <typename T>
Array<T>::Array(T* existingArray, uint32 size)
{
	m_size = size;
	m_data = new T[size];
	for (uint32 i = 0; i < size; i++)
	{
		m_data[i] = existingArray[i];
	}
}

template <typename T>
Array<T>::Array(const Array& other)
{
	m_size = other.m_size;
	m_data = new T[m_size];
	for (uint32 i = 0; i < m_size; i++)
	{
		m_data[i] = other.m_data[i];
	}
}

template <typename T>
Array<T>::~Array()
{
	delete[] m_data;
}

template <typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
	if (this == &other)
		return *this;

	// Resize only if needed
	if (m_size != other.m_size)
	{
		m_size = other.m_size;
		delete[] m_data;
		m_data = new T[m_size];
	}

	// Copy over data
	for (uint32 i = 0; i < m_size; i++)
	{
		m_data[i] = other.m_data[i];
	}
	return *this;
}

template <typename T>
T Array<T>::get(uint32 index) const
{
	return m_data[index];
}

template <typename T>
void Array<T>::set(uint32 index, T value)
{
	m_data[index] = value;
}

template <typename T>
uint32 Array<T>::size() const
{
	return m_size;
}

template <typename T>
T* Array<T>::getRawPtr()
{
	return m_data;
}

template <typename T>
T &Array<T>::operator[](int index)
{
	return m_data[index];
}

template <typename T>
T Array<T>::operator[](int index) const
{
	return m_data[index];
}

#endif // ARRAY_H
