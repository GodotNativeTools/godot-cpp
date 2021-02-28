#ifndef GODOT_VECTOR2I_HPP
#define GODOT_VECTOR2I_HPP

#include <Defs.hpp>
#include <Math.hpp>
#include <String.hpp>
#include <Vector2.hpp>

namespace godot {

struct Vector2i {
	enum Axis {
		AXIS_X,
		AXIS_Y,
	};

	union {
		int32_t x = 0;
		int32_t width;
	};
	union {
		int32_t y = 0;
		int32_t height;
	};

	inline int32_t &operator[](int p_idx) {
		return p_idx ? y : x;
	}
	inline const int32_t &operator[](int p_idx) const {
		return p_idx ? y : x;
	}

	Vector2i operator+(const Vector2i &p_v) const;
	void operator+=(const Vector2i &p_v);
	Vector2i operator-(const Vector2i &p_v) const;
	void operator-=(const Vector2i &p_v);
	Vector2i operator*(const Vector2i &p_v1) const;

	Vector2i operator*(const int32_t &rvalue) const;
	void operator*=(const int32_t &rvalue);

	Vector2i operator/(const Vector2i &p_v1) const;
	Vector2i operator/(const int32_t &rvalue) const;
	void operator/=(const int32_t &rvalue);

	Vector2i operator%(const Vector2i &p_v1) const;
	Vector2i operator%(const int32_t &rvalue) const;
	void operator%=(const int32_t &rvalue);

	Vector2i operator-() const;
	bool operator<(const Vector2i &p_vec2) const { return (x == p_vec2.x) ? (y < p_vec2.y) : (x < p_vec2.x); }
	bool operator>(const Vector2i &p_vec2) const { return (x == p_vec2.x) ? (y > p_vec2.y) : (x > p_vec2.x); }

	bool operator<=(const Vector2i &p_vec2) const { return x == p_vec2.x ? (y <= p_vec2.y) : (x < p_vec2.x); }
	bool operator>=(const Vector2i &p_vec2) const { return x == p_vec2.x ? (y >= p_vec2.y) : (x > p_vec2.x); }

	bool operator==(const Vector2i &p_vec2) const;
	bool operator!=(const Vector2i &p_vec2) const;

	real_t aspect() const { return width / (real_t)height; }
	Vector2i sign() const { return Vector2i(Math::sign(x), Math::sign(y)); }
	Vector2i abs() const { return Vector2i(Math::abs(x), Math::abs(y)); }

	operator String() const;

	operator Vector2() const { return Vector2(x, y); }

	inline Vector2i() {}
	inline Vector2i(const Vector2 &p_vec2) {
		x = (int32_t)p_vec2.x;
		y = (int32_t)p_vec2.y;
	}
	inline Vector2i(int32_t p_x, int32_t p_y) {
		x = p_x;
		y = p_y;
	}
	inline explicit Vector2i(const godot_vector2i &p_godot_vector2i) {
		*this = *((Vector2i *)&p_godot_vector2i);
	}
};

inline Vector2i operator*(const int32_t &p_scalar, const Vector2i &p_vector) {
	return p_vector * p_scalar;
}

inline Vector2i operator*(const int64_t &p_scalar, const Vector2i &p_vector) {
	return p_vector * p_scalar;
}

inline Vector2i operator*(const float &p_scalar, const Vector2i &p_vector) {
	return p_vector * p_scalar;
}

inline Vector2i operator*(const double &p_scalar, const Vector2i &p_vector) {
	return p_vector * p_scalar;
}

typedef Vector2i Size2i;
typedef Vector2i Point2i;

} // namespace godot

#endif // GODOT_VECTOR2I_HPP
