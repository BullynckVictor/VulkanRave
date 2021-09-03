#pragma once
#include <utility>

namespace rv
{
	template<typename K, typename T>
	struct Entry
	{
		using KeyType = K;
		using ValueType = T;

		Entry() : key(), value() {};
		Entry(const KeyType& key, const ValueType& value) : key(key), value(value) {}
		Entry(const KeyType& key) : key(key), value() {}
		template<typename K2, typename T2>
		Entry(K2&& key, T2&& value) : key(std::move(key)), value(std::move(value)) {}
		template<typename K2>
		Entry(K2&& key) : key(std::move(key)), value() {}
		Entry(const Entry&) = default;
		Entry(Entry&&) = default;
		~Entry() = default;

		Entry& operator= (const Entry&) = default;
		Entry& operator= (Entry&&) = default;

		bool operator== (const Entry& rhs) const { return key == rhs.key; }
		bool operator!= (const Entry& rhs) const { return key != rhs.key; }
		bool operator<= (const Entry& rhs) const { return key <= rhs.key; }
		bool operator>= (const Entry& rhs) const { return key >= rhs.key; }
		bool operator<  (const Entry& rhs) const { return key < rhs.key; }
		bool operator>  (const Entry& rhs) const { return key > rhs.key; }

		bool same(const Entry& rhs) const { return key == rhs.key && value == rhs.value; }

		KeyType key;
		ValueType value;
	};

	template<typename K, typename T>
	using ConstEntry = Entry<const K, T>;
}