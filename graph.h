#pragma once
#include <vector>
#include <functional>

template<typename V, typename W>
class Graph
{
private:
	const std::vector<V> vertices;
	std::vector<std::vector<W>> adj_mat;

	// �_�C�N�X�g���@�p�̏d��&�o�H�Ǘ��N���X
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
		// �󂯎�����ӂ̃f�[�^����A�d�ݕt���̗אڍs��ɕϊ�
		for (auto itr : e) { adj_mat[itr.first][itr.second] = weight_func(v.at(itr.first), v.at(itr.second)); }
	}

	unsigned int get_vertex_index(const V& v) const
	{
		return static_cast<unsigned int>(std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), v)));
	}

	// �_�C�N�X�g���@�ɂ��ŒZ�o�H�T��
	std::vector<V> dijkstra(unsigned int start, unsigned int end) const
	{
		// start,end��index���͈͊O�Ȃ�G���[
		if (start >= vertices.size() || end >= vertices.size()) { return std::vector<V>(); }

		// �e���_�ɕt������Node�N���X�̐���(vertices��index�͋���)
		std::vector<Node> nodes;
		for (unsigned int i = 0; i < vertices.size(); i++) { nodes.emplace_back(i); }
		nodes.at(start).init_as_start();	// �_�C�N�X�g���@�̊J�n���_�̏d�݂�0�ɏ�����

											// �d�ݍŏ��̊m��Node(���Node)�Ɋւ���z��(�d�݂�Node��index�̂ݕێ�)
		std::vector<std::pair<W, unsigned int>> queue;
		queue.emplace_back(std::make_pair(0, start));	// ������

														// ���Node���Ȃ��Ȃ�܂Ōp��
		while (!queue.empty())
		{
			// ���Node�z��̍Ō�����d�ݍŏ�
			std::pair<W, unsigned int> node = queue.back();
			queue.pop_back();

			// �ŏ��d�ݍ��Node�ƂȂ����Ă���Node�ɂ��ďd�݂̍X�V����
			//(�q�����Ă��邩�ǂ����̔����update_weight���ōs���Ă���)
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				if (nodes[i].update_weight(*this, nodes[node.second]))
				{
					// �d�݂̍X�V�����ۂɍs��ꂽ�ꍇ�́A���Node�Ƃ��ēo�^
					queue.emplace_back(std::make_pair(nodes[i].get_weight(), i));
				}
			}

			// ���Node�z��̍Ō�����d�ݍŏ��ƂȂ�悤�Ƀ\�[�g&��Ӊ�
			std::sort(queue.rbegin(), queue.rend());
			queue.erase(std::unique(queue.begin(), queue.end()), queue.end());
		}

		// ���[����ŒZ�o�H��H��A�o�^
		std::vector<unsigned int> route_index;
		for (unsigned int i = end; i < nodes.size(); i = nodes[i].get_from()) { route_index.push_back(i); }
		route_index.pop_back();
		// �ŒZ�o�H��index����|��
		std::vector<V> route(route_index.size());
		for (unsigned int i = 0; i < route.size(); i++) { route.at(i) = vertices.at(route_index.back()); route_index.pop_back(); }

		return route;
	}

};
