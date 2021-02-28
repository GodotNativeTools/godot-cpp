#ifndef GODOT_AABB_HPP
#define GODOT_AABB_HPP

#include <Math.hpp>
#include <Plane.hpp>
#include <Vector3.hpp>

/**
 * AABB / AABB (Axis Aligned Bounding Box)
 * This is implemented by a point (position) and the box size
 */

namespace godot {

class AABB {
public:
	Vector3 position;
	Vector3 size;

	real_t get_area() const; /// get area
	inline bool has_no_area() const {
		return (size.x <= 0 || size.y <= 0 || size.z <= 0);
	}

	inline bool has_no_surface() const {
		return (size.x <= 0 && size.y <= 0 && size.z <= 0);
	}

	const Vector3 &get_position() const { return position; }
	void set_position(const Vector3 &p_pos) { position = p_pos; }
	const Vector3 &get_size() const { return size; }
	void set_size(const Vector3 &p_size) { size = p_size; }

	bool operator==(const AABB &p_rval) const;
	bool operator!=(const AABB &p_rval) const;

	bool is_equal_approx(const AABB &p_aabb) const;
	inline bool intersects(const AABB &p_aabb) const; /// Both AABBs overlap
	inline bool intersects_inclusive(const AABB &p_aabb) const; /// Both AABBs (or their faces) overlap
	inline bool encloses(const AABB &p_aabb) const; /// p_aabb is completely inside this

	AABB merge(const AABB &p_with) const;
	void merge_with(const AABB &p_aabb); ///merge with another AABB
	AABB intersection(const AABB &p_aabb) const; ///get box where two intersect, empty if no intersection occurs
	bool intersects_segment(const Vector3 &p_from, const Vector3 &p_to, Vector3 *r_clip = nullptr, Vector3 *r_normal = nullptr) const;
	bool intersects_ray(const Vector3 &p_from, const Vector3 &p_dir, Vector3 *r_clip = nullptr, Vector3 *r_normal = nullptr) const;
	inline bool smits_intersect_ray(const Vector3 &p_from, const Vector3 &p_dir, real_t t0, real_t t1) const;

	inline bool intersects_convex_shape(const Plane *p_planes, int p_plane_count, const Vector3 *p_points, int p_point_count) const;
	inline bool inside_convex_shape(const Plane *p_planes, int p_plane_count) const;
	bool intersects_plane(const Plane &p_plane) const;

	inline bool has_point(const Vector3 &p_point) const;
	inline Vector3 get_support(const Vector3 &p_normal) const;

	Vector3 get_longest_axis() const;
	int get_longest_axis_index() const;
	inline real_t get_longest_axis_size() const;

	Vector3 get_shortest_axis() const;
	int get_shortest_axis_index() const;
	inline real_t get_shortest_axis_size() const;

	AABB grow(real_t p_by) const;
	inline void grow_by(real_t p_amount);

	void get_edge(int p_edge, Vector3 &r_from, Vector3 &r_to) const;
	inline Vector3 get_endpoint(int p_point) const;

	AABB expand(const Vector3 &p_vector) const;
	inline void project_range_in_plane(const Plane &p_plane, real_t &r_min, real_t &r_max) const;
	inline void expand_to(const Vector3 &p_vector); /** expand to contain a point if necessary */

	inline AABB abs() const {
		return AABB(Vector3(position.x + Math::min(size.x, (real_t)0), position.y + Math::min(size.y, (real_t)0), position.z + Math::min(size.z, (real_t)0)), size.abs());
	}

	inline void quantize(real_t p_unit);
	inline AABB quantized(real_t p_unit) const;

	inline void set_end(const Vector3 &p_end) {
		size = p_end - position;
	}

	inline Vector3 get_end() const {
		return position + size;
	}

	operator String() const;

	inline AABB() {}
	inline AABB(const Vector3 &p_pos, const Vector3 &p_size) :
			position(p_pos),
			size(p_size) {
	}
	inline explicit AABB(const godot_aabb &p_godot_aabb) {
		*this = *((AABB *)&p_godot_aabb);
	}
};

inline bool AABB::intersects(const AABB &p_aabb) const {
	if (position.x >= (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) <= p_aabb.position.x) {
		return false;
	}
	if (position.y >= (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) <= p_aabb.position.y) {
		return false;
	}
	if (position.z >= (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) <= p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABB::intersects_inclusive(const AABB &p_aabb) const {
	if (position.x > (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) < p_aabb.position.x) {
		return false;
	}
	if (position.y > (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) < p_aabb.position.y) {
		return false;
	}
	if (position.z > (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) < p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABB::encloses(const AABB &p_aabb) const {
	Vector3 src_min = position;
	Vector3 src_max = position + size;
	Vector3 dst_min = p_aabb.position;
	Vector3 dst_max = p_aabb.position + p_aabb.size;

	return (
			(src_min.x <= dst_min.x) &&
			(src_max.x > dst_max.x) &&
			(src_min.y <= dst_min.y) &&
			(src_max.y > dst_max.y) &&
			(src_min.z <= dst_min.z) &&
			(src_max.z > dst_max.z));
}

Vector3 AABB::get_support(const Vector3 &p_normal) const {
	Vector3 half_extents = size * 0.5;
	Vector3 ofs = position + half_extents;

	return Vector3(
				   (p_normal.x > 0) ? half_extents.x : -half_extents.x,
				   (p_normal.y > 0) ? half_extents.y : -half_extents.y,
				   (p_normal.z > 0) ? half_extents.z : -half_extents.z) +
		   ofs;
}

Vector3 AABB::get_endpoint(int p_point) const {
	switch (p_point) {
		case 0:
			return Vector3(position.x, position.y, position.z);
		case 1:
			return Vector3(position.x, position.y, position.z + size.z);
		case 2:
			return Vector3(position.x, position.y + size.y, position.z);
		case 3:
			return Vector3(position.x, position.y + size.y, position.z + size.z);
		case 4:
			return Vector3(position.x + size.x, position.y, position.z);
		case 5:
			return Vector3(position.x + size.x, position.y, position.z + size.z);
		case 6:
			return Vector3(position.x + size.x, position.y + size.y, position.z);
		case 7:
			return Vector3(position.x + size.x, position.y + size.y, position.z + size.z);
	}

	ERR_FAIL_V(Vector3());
}

bool AABB::intersects_convex_shape(const Plane *p_planes, int p_plane_count, const Vector3 *p_points, int p_point_count) const {
	Vector3 half_extents = size * 0.5;
	Vector3 ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane &p = p_planes[i];
		Vector3 point(
				(p.normal.x > 0) ? -half_extents.x : half_extents.x,
				(p.normal.y > 0) ? -half_extents.y : half_extents.y,
				(p.normal.z > 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	// Make sure all points in the shape aren't fully separated from the AABB on
	// each axis.
	int bad_point_counts_positive[3] = { 0 };
	int bad_point_counts_negative[3] = { 0 };

	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < p_point_count; i++) {
			if (p_points[i].coord[k] > ofs.coord[k] + half_extents.coord[k]) {
				bad_point_counts_positive[k]++;
			}
			if (p_points[i].coord[k] < ofs.coord[k] - half_extents.coord[k]) {
				bad_point_counts_negative[k]++;
			}
		}

		if (bad_point_counts_negative[k] == p_point_count) {
			return false;
		}
		if (bad_point_counts_positive[k] == p_point_count) {
			return false;
		}
	}

	return true;
}

bool AABB::inside_convex_shape(const Plane *p_planes, int p_plane_count) const {
	Vector3 half_extents = size * 0.5;
	Vector3 ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane &p = p_planes[i];
		Vector3 point(
				(p.normal.x < 0) ? -half_extents.x : half_extents.x,
				(p.normal.y < 0) ? -half_extents.y : half_extents.y,
				(p.normal.z < 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	return true;
}

bool AABB::has_point(const Vector3 &p_point) const {
	if (p_point.x < position.x) {
		return false;
	}
	if (p_point.y < position.y) {
		return false;
	}
	if (p_point.z < position.z) {
		return false;
	}
	if (p_point.x > position.x + size.x) {
		return false;
	}
	if (p_point.y > position.y + size.y) {
		return false;
	}
	if (p_point.z > position.z + size.z) {
		return false;
	}

	return true;
}

inline void AABB::expand_to(const Vector3 &p_vector) {
	Vector3 begin = position;
	Vector3 end = position + size;

	if (p_vector.x < begin.x) {
		begin.x = p_vector.x;
	}
	if (p_vector.y < begin.y) {
		begin.y = p_vector.y;
	}
	if (p_vector.z < begin.z) {
		begin.z = p_vector.z;
	}

	if (p_vector.x > end.x) {
		end.x = p_vector.x;
	}
	if (p_vector.y > end.y) {
		end.y = p_vector.y;
	}
	if (p_vector.z > end.z) {
		end.z = p_vector.z;
	}

	position = begin;
	size = end - begin;
}

void AABB::project_range_in_plane(const Plane &p_plane, real_t &r_min, real_t &r_max) const {
	Vector3 half_extents(size.x * 0.5, size.y * 0.5, size.z * 0.5);
	Vector3 center(position.x + half_extents.x, position.y + half_extents.y, position.z + half_extents.z);

	real_t length = p_plane.normal.abs().dot(half_extents);
	real_t distance = p_plane.distance_to(center);
	r_min = distance - length;
	r_max = distance + length;
}

inline real_t AABB::get_longest_axis_size() const {
	real_t max_size = size.x;

	if (size.y > max_size) {
		max_size = size.y;
	}

	if (size.z > max_size) {
		max_size = size.z;
	}

	return max_size;
}

inline real_t AABB::get_shortest_axis_size() const {
	real_t max_size = size.x;

	if (size.y < max_size) {
		max_size = size.y;
	}

	if (size.z < max_size) {
		max_size = size.z;
	}

	return max_size;
}

bool AABB::smits_intersect_ray(const Vector3 &p_from, const Vector3 &p_dir, real_t t0, real_t t1) const {
	real_t divx = 1.0 / p_dir.x;
	real_t divy = 1.0 / p_dir.y;
	real_t divz = 1.0 / p_dir.z;

	Vector3 upbound = position + size;
	real_t tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (p_dir.x >= 0) {
		tmin = (position.x - p_from.x) * divx;
		tmax = (upbound.x - p_from.x) * divx;
	} else {
		tmin = (upbound.x - p_from.x) * divx;
		tmax = (position.x - p_from.x) * divx;
	}
	if (p_dir.y >= 0) {
		tymin = (position.y - p_from.y) * divy;
		tymax = (upbound.y - p_from.y) * divy;
	} else {
		tymin = (upbound.y - p_from.y) * divy;
		tymax = (position.y - p_from.y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}
	if (p_dir.z >= 0) {
		tzmin = (position.z - p_from.z) * divz;
		tzmax = (upbound.z - p_from.z) * divz;
	} else {
		tzmin = (upbound.z - p_from.z) * divz;
		tzmax = (position.z - p_from.z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}
	return ((tmin < t1) && (tmax > t0));
}

void AABB::grow_by(real_t p_amount) {
	position.x -= p_amount;
	position.y -= p_amount;
	position.z -= p_amount;
	size.x += 2.0 * p_amount;
	size.y += 2.0 * p_amount;
	size.z += 2.0 * p_amount;
}

void AABB::quantize(real_t p_unit) {
	size += position;

	position.x -= Math::fposmodp(position.x, p_unit);
	position.y -= Math::fposmodp(position.y, p_unit);
	position.z -= Math::fposmodp(position.z, p_unit);

	size.x -= Math::fposmodp(size.x, p_unit);
	size.y -= Math::fposmodp(size.y, p_unit);
	size.z -= Math::fposmodp(size.z, p_unit);

	size.x += p_unit;
	size.y += p_unit;
	size.z += p_unit;

	size -= position;
}

AABB AABB::quantized(real_t p_unit) const {
	AABB ret = *this;
	ret.quantize(p_unit);
	return ret;
}

} // namespace godot

#endif // GODOT_AABB_HPP
