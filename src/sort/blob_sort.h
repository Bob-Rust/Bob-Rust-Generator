#include "../recode/all.h"
#include "../utils.h"
#include "sort_util.h"
#include "quad_tree.h"

#ifndef __BLOB_SORT_H__
#define __BLOB_SORT_H__

#include <ppl.h>

#include <vector>
#include <set>
using std::vector;
using std::set;

#ifdef _DEBUG_BLOB_SORTER
#include <iostream>
#endif

namespace bobrust {
	vector<int> get_intersections(Blob& target, Blob* buffer, QuadTree& tree) {
		set<int> set = tree.get_pieces(target);

		const int target_x = target.x;
		const int target_y = target.y;
		const int target_size = target.size;
		const int target_color = target.color;
		const int target_index = target.index;

		vector<int> result;
		result.reserve(set.size() / 10);
		
		Blob blob;
		for(auto blob_id : set) {
			if(blob_id >= target_index) break;
			blob = buffer[blob_id];

			if(blob.size != target_size || blob.color != target_color) {
				int x = blob.x - target_x;
				int y = blob.y - target_y;
				int sum = blob.size + target_size;
				if(x * x + y * y < sum * sum) {
					result.push_back(blob_id);
				}
			}
		}
		
		return result;
	}

	int find_best_fast_cache(unsigned int start, vector<int>& all_cache, vector<int>& any_cache, vector<int>* map, Blob* buffer) {
		unsigned int i;
		Blob blob;

		int value;
		int write_index = 0;
		for(i = 0; i < all_cache.size(); i++) {
			value = all_cache.at(i);
			blob = buffer[value];
			if(blob.index == -1) {
				continue;
			}

			vector<int>& cols = map[value];
			while(!cols.empty()) {
				if(buffer[cols.back()].index != -1) {
					break;
				}
				
				cols.pop_back();
			}
				
			if(!cols.empty()) {
				all_cache[write_index++] = value;
				continue;
			}

			all_cache.erase(all_cache.begin() + write_index, all_cache.begin() + i);
			return value;
		}

		if(write_index != all_cache.size()) {
			all_cache.erase(all_cache.begin() + write_index, all_cache.end());
		}

		write_index = 0;
		for(i = 0; i < any_cache.size(); i++) {
			value = any_cache.at(i);
			blob = buffer[value];
			if(blob.index == -1) {
				continue;
			}
			
			vector<int>& cols = map[value];
			while(!cols.empty()) {
				if(buffer[cols.back()].index != -1) {
					break;
				}
					
				cols.pop_back();
			}
				
			if(!cols.empty()) {
				any_cache[write_index++] = value;
				continue;
			}
			
			any_cache.erase(any_cache.begin() + write_index, any_cache.begin() + i);
			return value;
		}

		if(write_index != any_cache.size()) {
			all_cache.erase(all_cache.begin() + write_index, all_cache.end());
		}

		return start;
	}

	void compute_cache(vector<int>* all, vector<int>* any, Blob* data, unsigned int length) {
		Blob blob;

		for(unsigned int index = 1; index < length; index++) {
			blob = data[index];

			int size = SIZE_INDEX(blob.size);
			int color = blob.color;

			/* all */ {
				all[size + color * 6].push_back(index);
			}

			/* any */ {
				for(int i = 0; i < NUM_COLORS; i++) {
					// Colors is the X axis and has the upper value
					// Sizes is the Y axis and has the lowest value
					
					if(i == color) {
						// Add all the colors
						for(int j = 0; j < NUM_SIZES; j++) {
							if(j == size) continue;
							any[j + color * 6].push_back(index);
						}
					} else {
						any[size + i * 6].push_back(index);
					}
				}
			}
		}
	}

	int* sort_blob_list_0(Blob data[], unsigned int length, unsigned int width, unsigned int height) {
		int* out = new int[length];
		data[0].index = -1;
		out[0] = 0;

		vector<int>* map = new vector<int>[length];
		
		QuadTree tree(width, height);
		/* Calculate the intersections */ {
			for(unsigned int i = 1; i < length; i++) {
				tree.add_piece(data[i]);
			}

			concurrency::parallel_for(size_t(1), size_t(length), [&](int i) {
				vector<int> set = get_intersections(data[i], data, tree);
				std::reverse(set.begin(), set.end());
				map[i] = set;
			});
		}

		vector<int> all_cache[6 * 20];
		vector<int> any_cache[6 * 20];
		compute_cache(all_cache, any_cache, data, length);
		
		short last_color = data[0].color;
		short last_size = data[0].size;
		unsigned int cache_index = SIZE_INDEX(last_size) + last_color * 6;
		unsigned int start = 1;
		unsigned int i = 0;

		while(++i < length) {
			int index = find_best_fast_cache(start, all_cache[cache_index], any_cache[cache_index], map, data);
			out[i] = index;
			last_color = data[index].color;
			last_size = data[index].size;
			cache_index = SIZE_INDEX(last_size) + last_color * 6;
			data[index].index = -1;

			// Change the starting point
			if(index == start) {
				for(; start < length; start++) {
					if(data[start].index != -1) break;
				}
			}
		}

		delete[] map;
		return out;
	}

	void sort_blob_list(Blob* data, unsigned int length, unsigned int width, unsigned int height) {
		Blob* clone = new Blob[length];
		std::copy(data, data + length, clone);

		int* out = sort_blob_list_0(clone, length, width, height);
		for(unsigned int i = 0; i < length; i++) {
			data[i] = clone[out[i]];
		}

		delete[] out;
		delete[] clone;
	}

#ifdef _DEBUG_BLOB_SORTER
	int score(Blob* buffer, unsigned int length) {
		int changes = 0;
		short last_size = -1;
		short last_color = -1;
		for(unsigned int i = 0; i < length; i++) {
			Blob blob = buffer[i];
			changes += (last_size != blob.size);
			changes += (last_color != blob.color);
			last_size = blob.size;
			last_color = blob.color;
		}

		return changes;
	}
#endif

	void sort_blob_list(Model* model) {
		vector<Circle>& shapes = model->shapes;
		vector<Color>& colors = model->colors;

		size_t len = colors.size();
		Blob* list = new Blob[len];
			
		for(unsigned int i = 0; i < len; i++) {
			Circle& shape = shapes[i];
			Color& color = colors[i];
			list[i] = Blob{ (int)i, shape.x, shape.y, shape.r, closestColorIndex(color) };
		}

#ifdef _DEBUG_BLOB_SORTER
		printf("Before sort: score=%d\n", score(list, len));
		bobrust::sort_blob_list(list, len, model->width, model->height);
		printf("After sort: score=%d\n", score(list, len));
#else
		bobrust::sort_blob_list(list, len, model->width, model->height);
#endif
		
		for(unsigned int i = 0; i < len; i++) {
			Blob blob = list[i];
			Circle& cr = shapes[i];
			cr.r = blob.size;
			cr.x = blob.x;
			cr.y = blob.y;

			Color& rc = colors[i];
			rc = ARR_COLORS[blob.color];
		}

		delete[] list;
	}
}

#endif
