#ifndef _INC_CLASS_HEADER
#define _INC_CLASS_HEADER

#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <set>
#include <unordered_map>
#include <queue>
#include <functional>

typedef std::pair<unsigned int, unsigned int> Location;

template<class T1, class T2>
std::ostream& operator<<(std::ostream& s, const std::pair<T1, T2>& p)
{
	return s << "(" << p.first << "," << p.second << ")";
}

template<typename T> void hash_combine(size_t& seed, T const& v){ seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2); }

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

template<typename V, typename W>
class Graph
{
private:
	const std::vector<V> vertices;
	std::vector<std::vector<W>> adj_mat;

	// ダイクストラ法用の重み&経路管理クラス
	class Node
	{
	private:
		const unsigned int ID;
		W weight;
		unsigned int from;

	public:
		Node(unsigned int index) : ID(index), weight(std::numeric_limits<W>::max()), from(std::numeric_limits<unsigned int>::max()) {}

		bool update_weight(const Graph<V, W> & graph, const Node& from_node)
		{
			if (graph.adj_mat[from_node.ID][ID] != 0)
			{
				W new_weight = from_node.get_weight() + graph.adj_mat[from_node.ID][ID];
				if (new_weight < weight)
				{
					weight = new_weight;
					from = from_node.ID;
					return true;
				}
			}
			return false;
		}

		void init_as_start() { weight = 0; }

		W get_weight() const { return weight; }
		unsigned int get_from() const { return from; }

		bool operator>(const Node& rhs) const { return (weight > rhs.weight); }
		bool operator<(const Node& rhs) const { return (weight < rhs.weight); }
	};

public:
	Graph(const std::vector<V> v, const std::vector<std::pair<unsigned int, unsigned int>> e, std::function<W(const V&, const V&)> weight_func)
		: vertices(v), adj_mat(v.size(), std::vector<W>(v.size(), 0))
	{
		// 受け取った辺のデータから、重み付きの隣接行列に変換
		for (auto itr : e) { adj_mat[itr.first][itr.second] = weight_func(v.at(itr.first), v.at(itr.second)); }
	}

	unsigned int get_vertex_index(const V& v) const
	{
		return static_cast<unsigned int>(std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), v)));
	}

	// ダイクストラ法による最短経路探索
	std::vector<V> dijkstra(unsigned int start, unsigned int end) const
	{
		// start,endのindexが範囲外ならエラー
		if (start >= vertices.size() || end >= vertices.size()) { return std::vector<V>(); }

		// 各頂点に付随するNodeクラスの生成(verticesとindexは共通)
		std::vector<Node> nodes;
		for (unsigned int i = 0; i < vertices.size(); i++) { nodes.emplace_back(i); }
		nodes.at(start).init_as_start();	// ダイクストラ法の開始頂点の重みを0に初期化

		// 重み最小の確定Node(作業Node)に関する配列(重みとNodeのindexのみ保持)
		std::vector<std::pair<W, unsigned int>> queue;
		queue.emplace_back(std::make_pair(0, start));	// 初期化

		// 作業Nodeがなくなるまで継続
		while (!queue.empty())
		{
			// 作業Node配列の最後尾が重み最小
			std::pair<W, unsigned int> node = queue.back();
			queue.pop_back();
			
			// 最小重み作業NodeとつながっているNodeについて重みの更新処理
			//(繋がっているかどうかの判定もupdate_weight内で行われている)
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				if (nodes[i].update_weight(*this, nodes[node.second]))
				{
					// 重みの更新が実際に行われた場合は、作業Nodeとして登録
					queue.emplace_back(std::make_pair(nodes[i].get_weight(), i));
				}
			}

			// 作業Node配列の最後尾が重み最小となるようにソート&一意化
			std::sort(queue.rbegin(), queue.rend());
			queue.erase(std::unique(queue.begin(), queue.end()), queue.end());
		}

		// 末端から最短経路を辿り、登録
		std::vector<unsigned int> route_index;
		for (unsigned int i = end; i < nodes.size(); i = nodes[i].get_from()){ route_index.push_back(i); }
		route_index.pop_back();
		// 最短経路をindexから翻訳
		std::vector<V> route(route_index.size());
		for (unsigned int i = 0; i < route.size(); i++) { route.at(i) = vertices.at(route_index.back()); route_index.pop_back(); }

		return route;
	}

};

// 出発地と目的地、またバスの路線図から、最適な乗り換え経路を算出(todo)
std::vector<Location> GenTransferRoute(const Graph<Location, unsigned int>& bus_graph, const Location& start, const Location& dest)
{
	// バスの路線上においてダイクストラ法により最短経路を導出
	return bus_graph.dijkstra(bus_graph.get_vertex_index(start), bus_graph.get_vertex_index(dest));
}

// バス
class bus : public std::enable_shared_from_this<bus>
{
private:
	const unsigned int ID;
	const unsigned int capacity; // 定員
	const std::vector<Location> route;
	Location current_location;
	unsigned int current_destination_index;

public:
	bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index)
		: ID(IDnum), capacity(cap), route(r), current_location(r.at(current_index)), current_destination_index((current_index + 1) % r.size()) {}

	void run(std::unordered_multimap<Location, const bus*, pair_hash>* buses_at_busstop)
	{
		// バスの移動
		if (current_location.first < route[current_destination_index].first) {
			current_location.first++;
		}
		else if (current_location.first > route[current_destination_index].first) {
			current_location.first--;
		}
		else if (current_location.second < route[current_destination_index].second) {
			current_location.second++;
		}
		else if (current_location.second > route[current_destination_index].second) {
			current_location.second--;
		}

		std::cout << "bus" << ID << " is at " << current_location << std::endl;

		// 現在の目的地に到着したら
		if (current_location == route[current_destination_index])
		{
			// 目的地を次のバス停に更新
			current_destination_index = (current_destination_index + 1) % route.size();
			// バス停にいるはずなので、登録
			buses_at_busstop->insert(std::make_pair(current_location, this));
		}

		return;
	}

	// placeにこのバスが向かうかどうか
	bool is_going_to(const Location& place) const
	{
		for (unsigned int i = 0; i < route.size(); i++){ if (route[i] == place) { return true; } }
		return false;
	}

	std::shared_ptr<const bus> ride() const
	{
		// capacityを超えないか判定
		if (shared_from_this().use_count() - 2 >= capacity) { return nullptr; }
		return shared_from_this();
	}

	Location get_current_location() const { return current_location; }
};

class passenger { // 客
private:
	const unsigned int ID;
	Location current_location; // 現在地
	std::vector<Location> destinations;
	std::vector<Location>::iterator dest_itr;
	unsigned int waiting_time;

	std::shared_ptr<const bus> riding_bus;

public:
	passenger(unsigned int IDnum, const Graph<Location, unsigned int>& bus_graph, const Location& curr, const Location& dest)
		: ID(IDnum), current_location(curr), destinations(GenTransferRoute(bus_graph,curr,dest)), dest_itr(destinations.begin()), waiting_time(0)
	{
		// 経路構築に失敗したら
		if (destinations.size() == 0)
		{
			std::cerr << "Cannot generate transfer route. (bus" << ID << ":" << current_location << "->" << dest << ")" << std::endl;
			return;
		}
	}

	// trueが戻り値のときはすでにゴールに達している
	bool update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop)
	{
		// バス乗車済み
		if (riding_bus)
		{
			// バスに従って移動
			if (ManhattanDistance(current_location, riding_bus->get_current_location()) > 1)// 1-ノルムで1より多く進んでいたらエラー
			{
				std::cerr << "fuckin' moving " << current_location << "->" << riding_bus->get_current_location() << std::endl;
				return false;
			}
			current_location = riding_bus->get_current_location();

			// 移動して現在の目的地なら降車
			if (current_location == *dest_itr)
			{
				dest_itr++;	// 目的地を移動
				// 最終目的地だったならtrueを返し通知
				if (dest_itr == destinations.end()) { riding_bus = nullptr; return true; }
				// 次の目的地にはこのままでは行けないなら降車
				if(!riding_bus->is_going_to(*dest_itr)){ riding_bus = nullptr; }
			}
		}
		else
		{
			// バスに乗っていないなら乗ろうとする
			// 今いるバス停に止まっているバスを走査
			auto itr_range = buses_at_busstop.equal_range(current_location);
			for (auto itr = itr_range.first; itr != itr_range.second; itr++)
			{
				// 次の目的地に行くかチェック
				if(itr->second->is_going_to(*dest_itr))
				{
					// 行くなら乗車
					riding_bus = itr->second->ride();
					if (riding_bus) { break; }// 乗車出来たらbreak
				}
			}

			// 乗れなかったらwait
			if (!riding_bus) { waiting_time++; }
		}

		return false;
	}

	unsigned int get_waiting_time() const { return waiting_time; }
};

#endif // _INC_CLASS_HEADER
