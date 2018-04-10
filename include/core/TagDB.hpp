#ifndef GODOT_TAGDB_HPP
#define GODOT_TAGDB_HPP

#include <stdlib.h>

namespace godot {
namespace _TagDB {

// Use this only on class registration. The DB must be read-only after that.
void add(size_t p_tag, size_t p_base_tag = 0);

bool is_tag_compatible(size_t p_tag, size_t p_dst_tag);

}
}

#endif // GODOT_TAGDB_HPP
