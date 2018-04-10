#include "TagDB.hpp"
#include "Dictionary.hpp"

#include <unordered_map>

namespace godot {
namespace _TagDB {

// Can't use Godot Dictionary, Variant cannot contain size_t
std::unordered_map<size_t, size_t> s_tag_to_parent;

void add(size_t p_tag, size_t p_base_tag) {
	s_tag_to_parent[p_tag] = p_base_tag;
}

bool is_tag_compatible(size_t p_tag, size_t p_dst_tag) {
	if(p_tag == p_dst_tag)
		return true;
	while(true) {
		size_t v = s_tag_to_parent[p_tag];
		if(v == 0)
			return false;
		if(v == p_dst_tag)
			return true;
	}
}

}
}
