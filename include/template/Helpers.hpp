#ifndef GODOT_HELPERS_HPP
#define GODOT_HELPERS_HPP

namespace godot {
namespace helpers {

template <typename T, typename ValueT>
T append_all(T appendable, ValueT value) {
	appendable.append(value);
	return appendable;
}

template <typename T, typename ValueT, typename... Args>
T append_all(T appendable, ValueT value, Args... args) {
	appendable.append(value);
	return append_all(appendable, args...);
}

template <typename T>
T append_all(T appendable) {
	return appendable;
}

template <typename KV, typename KeyT, typename ValueT>
KV add_all(KV kv, KeyT key, ValueT value) {
	kv[key] = value;
	return kv;
}

template <typename KV, typename KeyT, typename ValueT, typename... Args>
KV add_all(KV kv, KeyT key, ValueT value, Args... args) {
	kv[key] = value;
	return add_all(kv, args...);
}

template <typename KV>
KV add_all(KV kv) {
	return kv;
}

} // namespace helpers
} // namespace godot

#endif
