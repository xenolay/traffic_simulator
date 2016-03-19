#pragma once
#include <vector>
#include <functional>

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
		for (unsigned int i = end; i < nodes.size(); i = nodes[i].get_from()) { route_index.push_back(i); }
		route_index.pop_back();
		// 最短経路をindexから翻訳
		std::vector<V> route(route_index.size());
		for (unsigned int i = 0; i < route.size(); i++) { route.at(i) = vertices.at(route_index.back()); route_index.pop_back(); }

		return route;
	}

};
