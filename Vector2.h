#pragma once
#include <cmath>

class Vector2
{
public:
	float x, y;

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	Vector2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	Vector2(): x(0), y(0)
	{
	}

	const Vector2& operator*=(const float& rhs)
	{
		x = x * rhs;
		y = y * rhs;

		return *this;
	}

	const float& operator[](int index)
	{
		return index == 0 ? x : y;
	}

	friend Vector2 operator*(const Vector2& v1, const float f)
	{
		return {v1.x * f, v1.y * f};
	}

	friend Vector2 operator*(const float f, const Vector2& v1)
	{
		return v1 * f;
	}

	friend Vector2 operator+(const Vector2& v1, const Vector2& v2)
	{
		return {v1.x + v2.x, v1.y + v2.y};
	}

	friend Vector2 operator-(const Vector2& v1, const Vector2& v2)
	{
		return {v1.x - v2.x, v1.y - v2.y};
	}

	//dot product
	friend float operator*(const Vector2& v1, const Vector2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}


	float magnitudeSq() const
	{
		return x * x + y * y;
	}

	float magnitude() const
	{
		return sqrt(magnitudeSq());
	}
	friend std::ostream& operator<<(std::ostream& os, const Vector2& v)
	{
		os << v.x << " " << v.y << ";";
		return os;
	}
};
