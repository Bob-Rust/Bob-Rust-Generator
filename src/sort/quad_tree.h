#include "sort_util.h"
#include <vector>
#include <set>

#ifndef __QUAD_TREE_H__
#define __QUAD_TREE_H__
// Best size for 512x512
constexpr auto QUAD_TREE_MIN_SIZE = 8;

#define add_piece_macro(blob, index) \
	{ \
		if(!node_##index) { \
			node_##index = new QuadTree(this->x + (index & 1) * this->hs, this->y + (index / 2) * this->hs, this->hs + (this->s & 1)); \
		} \
		node_##index->add_piece(blob); \
	};

#define get_pieces_macro(blob, set, index) \
	{ \
		if(node_##index) node_##index->get_pieces(blob, set); \
	};

class QuadTree {
	private:
		QuadTree *node_0 = NULL;
		QuadTree *node_1 = NULL;
		QuadTree *node_2 = NULL;
		QuadTree *node_3 = NULL;

		std::vector<int> list;
		int x;
		int y;
		int s;
		int hs;

		void get_pieces(Blob& blob, std::set<int>& set) {
			if(hs < QUAD_TREE_MIN_SIZE) {
				set.insert(list.begin(), list.end());
				return;
			}

			int x = blob.x - this->x;
			int y = blob.y - this->y;
			int r = blob.size;

			bool xmr = x - r <= hs;
			bool xpr = x + r >= hs;

			if(y - r <= hs) {
				if(xmr) get_pieces_macro(blob, set, 0);
				if(xpr) get_pieces_macro(blob, set, 1);
			}

			if(y + r >= hs) {
				if(xmr) get_pieces_macro(blob, set, 2);
				if(xpr) get_pieces_macro(blob, set, 3);
			}
		}

		QuadTree(int x, int y, int s) {
			this->x = x;
			this->y = y;
			this->s = s;
			this->hs = s / 2;
			this->list.reserve(10);
		}
	public:
		QuadTree(int w, int h) {
			this->x = 0;
			this->y = 0;
			this->s = (w > h ? w:h);
			this->hs = (w > h ? w:h) / 2;
			this->list.reserve(10);
		}

		~QuadTree() {
			delete node_0;
			delete node_1;
			delete node_2;
			delete node_3;
		}

		void add_piece(Blob& blob) {
			if(hs < QUAD_TREE_MIN_SIZE) {
				list.push_back(blob.index);
				return;
			}
			
			int x = blob.x - this->x;
			int y = blob.y - this->y;
			int r = blob.size;

			bool xmr = x - r <= hs;
			bool xpr = x + r >= hs;

			if(y - r <= hs) {
				if(xmr) add_piece_macro(blob, 0);
				if(xpr) add_piece_macro(blob, 1);
			}

			if(y + r >= hs) {
				if(xmr) add_piece_macro(blob, 2);
				if(xpr) add_piece_macro(blob, 3);
			}
		}
		
		std::set<int> get_pieces(Blob& blob) {
			std::set<int> set;
			get_pieces(blob, set);
			return set;
		}
};

#endif
