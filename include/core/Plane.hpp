#ifndef GODOT_PLANE_HPP
#define GODOT_PLANE_HPP

#include <CoreConstants.hpp>
#include <Math.hpp>
#include <Vector3.hpp>

namespace godot {

using namespace CoreConstants;

class Plane {
public:
	Vector3 normal;
	real_t d = 0;

	void set_normal(const Vector3 &p_normal);
	inline Vector3 get_normal() const { return normal; }; ///Point is coplanar, CMP_EPSILON for precision

	void normalize();
	Plane normalized() const;

	/* Plane-Point operations */

	inline Vector3 center() const { return normal * d; }
	Vector3 get_any_perpendicular_normal() const;

	inline bool is_point_over(const Vector3 &p_point) const; ///< Point is over plane
	inline real_t distance_to(const Vector3 &p_point) const;
	inline bool has_point(const Vector3 &p_point, real_t _epsilon = CMP_EPSILON) const;

	/* intersections */

	bool intersect_3(const Plane &p_plane1, const Plane &p_plane2, Vector3 *r_result = nullptr) const;
	bool intersects_ray(const Vector3 &p_from, const Vector3 &p_dir, Vector3 *p_intersection) const;
	bool intersects_segment(const Vector3 &p_begin, const Vector3 &p_end, Vector3 *p_intersection) const;

	inline Vector3 project(const Vector3 &p_point) const {
		return p_point - normal * distance_to(p_point);
	}

	/* misc */

	Plane operator-() const { return Plane(-normal, -d); }
	bool is_equal_approx(const Plane &p_plane) const;
	bool is_equal_approx_any_side(const Plane &p_plane) const;

	inline bool operator==(const Plane &p_plane) const;
	inline bool operator!=(const Plane &p_plane) const;
	operator String() const;

	inline Plane() {}
	inline Plane(real_t p_a, real_t p_b, real_t p_c, real_t p_d) :
			normal(p_a, p_b, p_c),
			d(p_d) {}

	inline Plane(const Vector3 &p_normal, real_t p_d);
	inline Plane(const Vector3 &p_point, const Vector3 &p_normal);
	inline Plane(const Vector3 &p_point1, const Vector3 &p_point2, const Vector3 &p_point3, ClockDirection p_dir = CLOCKWISE);
	inline explicit Plane(const godot_plane &p_godot_plane) {
		*this = *((Plane *)&p_godot_plane);
	}
};

bool Plane::is_point_over(const Vector3 &p_point) const {
	return (normal.dot(p_point) > d);
}

real_t Plane::distance_to(const Vector3 &p_point) const {
	return (normal.dot(p_point) - d);
}

bool Plane::has_point(const Vector3 &p_point, real_t _epsilon) const {
	real_t dist = normal.dot(p_point) - d;
	dist = Math::abs(dist);
	return (dist <= _epsilon);
}

Plane::Plane(const Vector3 &p_normal, real_t p_d) :
		normal(p_normal),
		d(p_d) {
}

Plane::Plane(const Vector3 &p_point, const Vector3 &p_normal) :
		normal(p_normal),
		d(p_normal.dot(p_point)) {
}

Plane::Plane(const Vector3 &p_point1, const Vector3 &p_point2, const Vector3 &p_point3, ClockDirection p_dir) {
	if (p_dir == CLOCKWISE) {
		normal = (p_point1 - p_point3).cross(p_point1 - p_point2);
	} else {
		normal = (p_point1 - p_point2).cross(p_point1 - p_point3);
	}

	normal.normalize();
	d = normal.dot(p_point1);
}

bool Plane::operator==(const Plane &p_plane) const {
	return normal == p_plane.normal && d == p_plane.d;
}

bool Plane::operator!=(const Plane &p_plane) const {
	return normal != p_plane.normal || d != p_plane.d;
}
} // namespace godot

#endif // GODOT_PLANE_HPP
