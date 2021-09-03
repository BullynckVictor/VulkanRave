#pragma once
#include "Engine/Utilities/Entry.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace rv
{
	template<typename K, typename T>
	class SortedVector
	{
	public:
		using KeyType = K;
		using ValueType = T;
		using Entry = ConstEntry<KeyType, ValueType>;
		using iterator = typename std::vector<Entry>::iterator;
		using const_iterator = typename std::vector<Entry>::const_iterator;
		using reverse_iterator = typename std::vector<Entry>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<Entry>::const_reverse_iterator;

	public:
		SortedVector() = default;
		SortedVector(const SortedVector&) = default;
		SortedVector(SortedVector&&) = default;
		~SortedVector() = default;

		SortedVector& operator= (const SortedVector&) = default;
		SortedVector& operator= (SortedVector&&) = default;

		bool operator== (const SortedVector& rhs) const { return vector == rhs.vector; }
		bool operator!= (const SortedVector& rhs) const { return vector != rhs.vector; }

		template<typename InputIt>
		SortedVector(InputIt first, InputIt last)
			:
			vector(first, last)
		{
			std::sort(vector.begin(), vector.end());
		}
		SortedVector(std::initializer_list<Entry> init)
			:
			vector(init)
		{
			std::sort(vector.begin(), vector.end());
		}

		ValueType& at(const KeyType& key)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
				return it->value;
			throw std::out_of_range("key not found in SortedVector<K, T>");
		}
		const ValueType& at(const KeyType& key) const
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
				return it->value;
			throw std::out_of_range("key not found in SortedVector<K, T>");
		}
		ValueType& operator[] (const KeyType& key)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
				return it->value;
			return vector.insert(it, Entry(key))->value;
		}
		ValueType& operator[] (KeyType&& key)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
				return it->value;
			return vector.insert(it, Entry(std::move(key)))->value;
		}

		iterator				begin()				noexcept { return vector.begin(); }
		const_iterator			begin()		const	noexcept { return vector.begin(); }
		const_iterator			cbegin()	const	noexcept { return vector.cbegin(); }
		iterator				end()				noexcept { return vector.end(); }
		const_iterator			end()		const	noexcept { return vector.end(); }
		const_iterator			cend()		const	noexcept { return vector.cend(); }
		reverse_iterator		rbegin()			noexcept { return vector.rbegin(); }
		const_reverse_iterator	rbegin()	const	noexcept { return vector.rbegin(); }
		const_reverse_iterator	crbegin()	const	noexcept { return vector.crbegin(); }
		reverse_iterator		rend()				noexcept { return vector.rend(); }
		const_reverse_iterator	rend()		const	noexcept { return vector.rend(); }
		const_reverse_iterator	crend()		const	noexcept { return vector.crend(); }

		bool empty() const noexcept { return vector.empty(); }
		size_t size() const noexcept { return vector.size(); }
		size_t max_size() const noexcept { return vector.max_size(); }

		void clear() { vector.clear(); }
		std::pair<iterator, bool> insert(const Entry& entry)
		{
			auto it = lower_bound(entry.key);
			if (it_exists(it, entry.key))
				return { it, false };
			vector.insert(it, entry);
			return { it, true };
		}
		std::pair<iterator, bool> insert(Entry&& entry)
		{
			auto it = lower_bound(entry.key);
			if (it_exists(it, entry.key))
				return { it, false };
			vector.insert(it, std::move(entry));
			return { it, true };
		}
		std::pair<iterator, bool> insert(const KeyType& key, const ValueType& value) { return insert(Entry{ key, value }); }
		std::pair<iterator, bool> insert(KeyType&& key, ValueType&& value) { return insert(Entry{ std::move(key), std::move(value) }); }
		template<typename InputIt>
		void insert(InputIt first, InputIt last)
		{
			for (InputIt it = first; it != last; ++it)
				insert(*it);
		}
		void insert(std::initializer_list<Entry> ilist)
		{
			for (const auto& e : ilist)
				insert(e);
		}
		std::pair<iterator, bool> insert_or_assign(const KeyType& key, ValueType&& value)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
			{
				it->value = std::move(value);
				return { it, false };
			}
			vector.insert(it, { key, std::move(value) });
			return { it, true };
		}
		std::pair<iterator, bool> insert_or_assign(KeyType&& key, ValueType&& value)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
			{
				it->value = std::move(value);
				return { it, false };
			}
			vector.insert(it, { std::move(key), std::move(value) });
			return { it, true };
		}
		iterator erase(const_iterator pos) { return vector.erase(pos); }
		iterator erase(iterator pos) { return vector.erase(pos); }
		iterator erase(const_iterator first, const_iterator last) { return vector.erase(first, last); }
		size_t erase(const KeyType& key)
		{
			auto it = lower_bound(key);
			if (it_exists(it, key))
			{
				vector.erase(it);
				return 1;
			}
			return 0;
		}
		
		bool contains(const KeyType& key)
		{
			auto it = lower_bound(key);
			return it_exists(it, key);
		}

	private:
		iterator lower_bound(const KeyType& key)
		{
			return std::lower_bound(vector.begin(), vector.end(), key);
		}
		const_iterator lower_bound(const KeyType& key) const
		{
			return std::lower_bound(vector.begin(), vector.end(), key);
		}
		bool it_exists(const iterator& it, const KeyType& key) const
		{
			return it != vector.end() && it->key != key;
		}
		bool it_exists(const const_iterator& it, const KeyType& key) const
		{
			return it != vector.end() && it->key != key;
		}

		std::vector<Entry> vector;
	};
}