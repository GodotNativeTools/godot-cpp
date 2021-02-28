#ifndef GODOT_VECTOR3I_HPP
#define GODOT_VECTOR3I_HPP

#include <Defs.hpp>
#include <Math.hpp>
#include <String.hpp>

namespace godot {

struct Vector3i {
	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {
		struct {
			int32_t x;
			int32_t y;
			int32_t z;
		};

		int32_t coord[3] = { 0 };
	};

	inline const int32_t &operator[](int p_axis) const {
		return coord[p_axis];
	}

	inline int32_t &operator[](int p_axis) {
		return coord[p_axis];
	}

	void set_axis(int p_axis, int32_t p_value);
	int32_t get_axis(int p_axis) const;

	int min_axis() const;
	int max_axis() const;

	inline void zero();

	inline Vector3i abs() const;
	inline Vector3i sign() const;

	/* Operators */

	inline Vector3i &operator+=(const Vector3i &p_v);
	inline Vector3i operator+(const Vector3i &p_v) const;
	inline Vector3i &operator-=(const Vector3i &p_v);
	inline Vector3i operator-(const Vector3i &p_v) const;
	inline Vector3i &operator*=(const Vector3i &p_v);
	inline Vector3i operator*(const Vector3i &p_v) const;
	inline Vector3i &operator/=(const Vector3i &p_v);
	inline Vector3i operator/(const Vector3i &p_v) const;
	inline Vector3i &operator%=(const Vector3i &p_v);
	inline Vector3i operator%(const Vector3i &p_v) const;

	inline Vector3i &operator*=(int32_t p_scalar);
	inline Vector3i operator*(int32_t p_scalar) const;
	inline Vector3i &operator/=(int32_t p_scalar);
	inline Vector3i operator/(int32_t p_scalar) const;
	inline Vector3i &operator%=(int32_t p_scalar);
	inline Vector3i operator%(int32_t p_scalar) const;

	inline Vector3i operator-() const;

	inline bool operator==(const Vector3i &p_v) const;
	inline bool operator!=(const Vector3i &p_v) const;
	inline bool operator<(const Vector3i &p_v) const;
	inline bool operator<=(const Vector3i &p_v) const;
	inline bool operator>(const Vector3i &p_v) const;
	inline bool operator>=(const Vector3i &p_v) const;

	operator String() const;

	inline Vector3i() {}
	inline Vector3i(int32_t p_x, int32_t p_y, int32_t p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	}
	inline explicit Vector3i(const godot_vector3i &p_godot_vector3i) {
		*this = *((Vector3i *)&p_godot_vector3i);
	}
};

Vector3i Vector3i::abs() const {
	return Vector3i(Math::abs(x), Math::abs(y), Math::abs(z));
}

Vector3i Vector3i::sign() const {
	return Vector3i(Math::sign(x), Math::sign(y), Math::sign(z));
}

/* Operators */

Vector3i &Vector3i::operator+=(const Vector3i &p_v) {
	x += p_v.x;
	y += p_v.y;
	z += p_v.z;
	return *this;
}

Vector3i Vector3i::operator+(const Vector3i &p_v) const {
	return Vector3i(x + p_v.x, y + p_v.y, z + p_v.z);
}

Vector3i &Vector3i::operator-=(const Vector3i &p_v) {
	x -= p_v.x;
	y -= p_v.y;
	z -= p_v.z;
	return *this;
}

Vector3i Vector3i::operator-(const Vector3i &p_v) const {
	return Vector3i(x - p_v.x, y - p_v.y, z - p_v.z);
}

Vector3i &Vector3i::operator*=(const Vector3i &p_v) {
	x *= p_v.x;
	y *= p_v.y;
	z *= p_v.z;
	return *this;
}

Vector3i Vector3i::operator*(const Vector3i &p_v) const {
	return Vector3i(x * p_v.x, y * p_v.y, z * p_v.z);
}

Vector3i &Vector3i::operator/=(const Vector3i &p_v) {
	x /= p_v.x;
	y /= p_v.y;
	z /= p_v.z;
	return *this;
}

Vector3i Vector3i::operator/(const Vector3i &p_v) const {
	return Vector3i(x / p_v.x, y / p_v.y, z / p_v.z);
}

Vector3i &Vector3i::operator%=(const Vector3i &p_v) {
	x %= p_v.x;
	y %= p_v.y;
	z %= p_v.z;
	return *this;
}

Vector3i Vector3i::operator%(const Vector3i &p_v) const {
	return Vector3i(x % p_v.x, y % p_v.y, z % p_v.z);
}

Vector3i &Vector3i::operator*=(int32_t p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	z *= p_scalar;
	return *this;
}

inline Vector3i operator*(int32_t p_scalar, const Vector3i &p_vec) {
	return p_vec * p_scalar;
}

Vector3i Vector3i::operator*(int32_t p_scalar) const {
	return Vector3i(x * p_scalar, y * p_scalar, z * p_scalar);
}

Vector3i &Vector3i::operator/=(int32_t p_scalar) {
	x /= p_scalar;
	y /= p_scalar;
	z /= p_scalar;
	return *this;
}

Vector3i Vector3i::operator/(int32_t p_scalar) const {
	return Vector3i(x / p_scalar, y / p_scalar, z / p_scalar);
}

Vector3i &Vector3i::operator%=(int32_t p_scalar) {
	x %= p_scalar;
	y %= p_scalar;
	z %= p_scalar;
	return *this;
}

Vector3i Vector3i::operator%(int32_t p_scalar) const {
	return Vector3i(x % p_scalar, y % p_scalar, z % p_scalar);
}

Vector3i Vector3i::operator-() const {
	return Vector3i(-x, -y, -z);
}

bool Vector3i::operator==(const Vector3i &p_v) const {
	return (x == p_v.x && y == p_v.y && z == p_v.z);
}

bool Vector3i::operator!=(const Vector3i &p_v) const {
	return (x != p_v.x || y != p_v.y || z != p_v.z);
}

bool Vector3i::operator<(const Vector3i &p_v) const {
	if (x == p_v.x) {
		if (y == p_v.y) {
			return z < p_v.z;
		} else {
			return y < p_v.y;
		}
	} else {
		return x < p_v.x;
	}
}

bool Vector3i::operator>(const Vector3i &p_v) const {
	if (x == p_v.x) {
		if (y == p_v.y) {
			return z > p_v.z;
		} else {
			return y > p_v.y;
		}
	} else {
		return x > p_v.x;
	}
}

bool Vector3i::operator<=(const Vector3i &p_v) const {
	if (x == p_v.x) {
		if (y == p_v.y) {
			return z <= p_v.z;
		} else {
			return y < p_v.y;
		}
	} else {
		return x < p_v.x;
	}
}

bool Vector3i::operator>=(const Vector3i &p_v) const {
	if (x == p_v.x) {
		if (y == p_v.y) {
			return z >= p_v.z;
		} else {
			return y > p_v.y;
		}
	} else {
		return x > p_v.x;
	}
}

void Vector3i::zero() {
	x = y = z = 0;
}

} // namespace godot

#endif // GODOT_VECTOR3I_HPP
