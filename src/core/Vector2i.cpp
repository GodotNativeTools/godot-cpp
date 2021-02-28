#include <Vector2i.hpp>

#include <String.hpp>

namespace godot {

Vector2i::operator String() const {
	return String((int64_t)x) + ", " + String((int64_t)y);
}

Vector2i Vector2i::operator+(const Vector2i &p_v) const {
	return Vector2i(x + p_v.x, y + p_v.y);
}

void Vector2i::operator+=(const Vector2i &p_v) {
	x += p_v.x;
	y += p_v.y;
}

Vector2i Vector2i::operator-(const Vector2i &p_v) const {
	return Vector2i(x - p_v.x, y - p_v.y);
}

void Vector2i::operator-=(const Vector2i &p_v) {
	x -= p_v.x;
	y -= p_v.y;
}

Vector2i Vector2i::operator*(const Vector2i &p_v1) const {
	return Vector2i(x * p_v1.x, y * p_v1.y);
}

Vector2i Vector2i::operator*(const int32_t &rvalue) const {
	return Vector2i(x * rvalue, y * rvalue);
}

void Vector2i::operator*=(const int32_t &rvalue) {
	x *= rvalue;
	y *= rvalue;
}

Vector2i Vector2i::operator/(const Vector2i &p_v1) const {
	return Vector2i(x / p_v1.x, y / p_v1.y);
}

Vector2i Vector2i::operator/(const int32_t &rvalue) const {
	return Vector2i(x / rvalue, y / rvalue);
}

void Vector2i::operator/=(const int32_t &rvalue) {
	x /= rvalue;
	y /= rvalue;
}

Vector2i Vector2i::operator%(const Vector2i &p_v1) const {
	return Vector2i(x % p_v1.x, y % p_v1.y);
}

Vector2i Vector2i::operator%(const int32_t &rvalue) const {
	return Vector2i(x % rvalue, y % rvalue);
}

void Vector2i::operator%=(const int32_t &rvalue) {
	x %= rvalue;
	y %= rvalue;
}

Vector2i Vector2i::operator-() const {
	return Vector2i(-x, -y);
}

bool Vector2i::operator==(const Vector2i &p_vec2) const {
	return x == p_vec2.x && y == p_vec2.y;
}

bool Vector2i::operator!=(const Vector2i &p_vec2) const {
	return x != p_vec2.x || y != p_vec2.y;
}

} // namespace godot
