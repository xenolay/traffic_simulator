#pragma once
#include <iostream>
#include <algorithm>
#include <vector>

typedef std::pair<unsigned int, unsigned int> Location;

typedef std::vector<std::vector<int>> Matrix;

template<class T1, class T2>
std::ostream& operator<<(std::ostream& s, const std::pair<T1, T2>& p)
{
	return s << "(" << p.first << "," << p.second << ")";
}

template<typename T> void hash_combine(size_t& seed, T const& v){ seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2); }

struct pair_hash
{
	template<class T1, class T2> std::size_t operator()(const std::pair<T1, T2>& p) const
	{
		std::size_t seed = 0;
		hash_combine(seed, p.first);
		hash_combine(seed, p.second);
		return seed;
	}
};

inline unsigned int ManhattanDistance(const Location& start, const Location& dist)
{
	return static_cast<unsigned int>(std::abs(static_cast<int>(start.first) - static_cast<int>(dist.first)) + std::abs(static_cast<int>(start.second) - static_cast<int>(dist.second)));
}
