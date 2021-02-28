#ifndef GODOT_VECTOR2_HPP
#define GODOT_VECTOR2_HPP

#include <Defs.hpp>
#include <Math.hpp>
#include <String.hpp>

namespace godot {

struct Vector2i;

struct Vector2 {
	enum Axis {
		AXIS_X,
		AXIS_Y,
	};

	union {
		real_t x = 0;
		real_t width;
	};
	union {
		real_t y = 0;
		real_t height;
	};

	inline real_t &operator[](int p_idx) {
		return p_idx ? y : x;
	}
	inline const real_t &operator[](int p_idx) const {
		return p_idx ? y : x;
	}

	void normalize();
	Vector2 normalized() const;
	bool is_normalized() const;

	real_t length() const;
	real_t length_squared() const;

	Vector2 min(const Vector2 &p_vector2) const {
		return Vector2(Math::min(x, p_vector2.x), Math::min(y, p_vector2.y));
	}

	Vector2 max(const Vector2 &p_vector2) const {
		return Vector2(Math::max(x, p_vector2.x), Math::max(y, p_vector2.y));
	}

	real_t distance_to(const Vector2 &p_vector2) const;
	real_t distance_squared_to(const Vector2 &p_vector2) const;
	real_t angle_to(const Vector2 &p_vector2) const;
	real_t angle_to_point(const Vector2 &p_vector2) const;
	inline Vector2 direction_to(const Vector2 &p_to) const;

	real_t dot(const Vector2 &p_other) const;
	real_t cross(const Vector2 &p_other) const;
	Vector2 posmod(const real_t p_mod) const;
	Vector2 posmodv(const Vector2 &p_modv) const;
	Vector2 project(const Vector2 &p_to) const;

	Vector2 plane_project(real_t p_d, const Vector2 &p_vec) const;

	Vector2 clamped(real_t p_len) const;

	inline Vector2 lerp(const Vector2 &p_to, real_t p_weight) const;
	inline Vector2 slerp(const Vector2 &p_to, real_t p_weight) const;
	Vector2 cubic_interpolate(const Vector2 &p_b, const Vector2 &p_pre_a, const Vector2 &p_post_b, real_t p_weight) const;
	Vector2 move_toward(const Vector2 &p_to, const real_t p_delta) const;

	Vector2 slide(const Vector2 &p_normal) const;
	Vector2 bounce(const Vector2 &p_normal) const;
	Vector2 reflect(const Vector2 &p_normal) const;

	bool is_equal_approx(const Vector2 &p_v) const;

	Vector2 operator+(const Vector2 &p_v) const;
	void operator+=(const Vector2 &p_v);
	Vector2 operator-(const Vector2 &p_v) const;
	void operator-=(const Vector2 &p_v);
	Vector2 operator*(const Vector2 &p_v1) const;

	Vector2 operator*(const real_t &rvalue) const;
	void operator*=(const real_t &rvalue);
	void operator*=(const Vector2 &rvalue) { *this = *this * rvalue; }

	Vector2 operator/(const Vector2 &p_v1) const;

	Vector2 operator/(const real_t &rvalue) const;

	void operator/=(const real_t &rvalue);
	void operator/=(const Vector2 &rvalue) { *this = *this / rvalue; }

	Vector2 operator-() const;

	bool operator==(const Vector2 &p_vec2) const;
	bool operator!=(const Vector2 &p_vec2) const;

	bool operator<(const Vector2 &p_vec2) const { return x == p_vec2.x ? (y < p_vec2.y) : (x < p_vec2.x); }
	bool operator>(const Vector2 &p_vec2) const { return x == p_vec2.x ? (y > p_vec2.y) : (x > p_vec2.x); }
	bool operator<=(const Vector2 &p_vec2) const { return x == p_vec2.x ? (y <= p_vec2.y) : (x < p_vec2.x); }
	bool operator>=(const Vector2 &p_vec2) const { return x == p_vec2.x ? (y >= p_vec2.y) : (x > p_vec2.x); }

	real_t angle() const;

	inline Vector2 abs() const {
		return Vector2(Math::abs(x), Math::abs(y));
	}

	Vector2 rotated(real_t p_by) const;
	Vector2 orthogonal() const {
		return Vector2(y, -x);
	}

	Vector2 sign() const;
	Vector2 floor() const;
	Vector2 ceil() const;
	Vector2 round() const;
	Vector2 snapped(const Vector2 &p_by) const;
	real_t aspect() const { return width / height; }

	operator String() const;

	inline Vector2() {}
	inline Vector2(real_t p_x, real_t p_y) {
		x = p_x;
		y = p_y;
	}
	inline explicit Vector2(const godot_vector2 &p_godot_vector2) {
		*this = *((Vector2 *)&p_godot_vector2);
	}
};

inline Vector2 Vector2::plane_project(real_t p_d, const Vector2 &p_vec) const {
	return p_vec - *this * (dot(p_vec) - p_d);
}

inline Vector2 operator*(float p_scalar, const Vector2 &p_vec) {
	return p_vec * p_scalar;
}

inline Vector2 operator*(double p_scalar, const Vector2 &p_vec) {
	return p_vec * p_scalar;
}

inline Vector2 operator*(int32_t p_scalar, const Vector2 &p_vec) {
	return p_vec * p_scalar;
}

inline Vector2 operator*(int64_t p_scalar, const Vector2 &p_vec) {
	return p_vec * p_scalar;
}

inline Vector2 Vector2::operator+(const Vector2 &p_v) const {
	return Vector2(x + p_v.x, y + p_v.y);
}

inline void Vector2::operator+=(const Vector2 &p_v) {
	x += p_v.x;
	y += p_v.y;
}

inline Vector2 Vector2::operator-(const Vector2 &p_v) const {
	return Vector2(x - p_v.x, y - p_v.y);
}

inline void Vector2::operator-=(const Vector2 &p_v) {
	x -= p_v.x;
	y -= p_v.y;
}

inline Vector2 Vector2::operator*(const Vector2 &p_v1) const {
	return Vector2(x * p_v1.x, y * p_v1.y);
}

inline Vector2 Vector2::operator*(const real_t &rvalue) const {
	return Vector2(x * rvalue, y * rvalue);
}

inline void Vector2::operator*=(const real_t &rvalue) {
	x *= rvalue;
	y *= rvalue;
}

inline Vector2 Vector2::operator/(const Vector2 &p_v1) const {
	return Vector2(x / p_v1.x, y / p_v1.y);
}

inline Vector2 Vector2::operator/(const real_t &rvalue) const {
	return Vector2(x / rvalue, y / rvalue);
}

inline void Vector2::operator/=(const real_t &rvalue) {
	x /= rvalue;
	y /= rvalue;
}

inline Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

inline bool Vector2::operator==(const Vector2 &p_vec2) const {
	return x == p_vec2.x && y == p_vec2.y;
}

inline bool Vector2::operator!=(const Vector2 &p_vec2) const {
	return x != p_vec2.x || y != p_vec2.y;
}

Vector2 Vector2::lerp(const Vector2 &p_to, real_t p_weight) const {
	Vector2 res = *this;

	res.x += (p_weight * (p_to.x - x));
	res.y += (p_weight * (p_to.y - y));

	return res;
}

Vector2 Vector2::slerp(const Vector2 &p_to, real_t p_weight) const {
#ifdef MATH_CHECKS
	ERR_FAIL_COND_V(!is_normalized(), Vector2());
#endif
	real_t theta = angle_to(p_to);
	return rotated(theta * p_weight);
}

Vector2 Vector2::direction_to(const Vector2 &p_to) const {
	Vector2 ret(p_to.x - x, p_to.y - y);
	ret.normalize();
	return ret;
}

typedef Vector2 Size2;
typedef Vector2 Point2;

} // namespace godot

#endif // GODOT_VECTOR2_HPP
