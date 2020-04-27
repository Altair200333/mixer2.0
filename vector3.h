#pragma once

class Vector3
{
public:
	float x, y, z;

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3(): x(0), y(0), z(0)
	{
	}

	const Vector3& operator*=(const float& rhs)
	{
		x = x * rhs;
		y = y * rhs;
		z = z * rhs;

		return *this;
	}

	const float& operator[](int index)
	{
		return index == 0 ? x : (index == 1 ? y : z);
	}

	friend Vector3 operator*(const Vector3& v1, const float f)
	{
		return {v1.x * f, v1.y * f, v1.z * f};
	}

	friend Vector3 operator*(const float f, const Vector3& v1)
	{
		return v1 * f;
	}

	friend Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
	}

	friend Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
	}

	//dot product
	friend float operator*(const Vector3& v1, const Vector3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	//cross product!!!!
	friend Vector3 operator^(const Vector3& v1, const Vector3& v2)
	{
		return { v1.y * v2.z - v1.z * v2.y, -(v1.x * v2.z - v1.z * v2.x), v1.x * v2.y - v1.y * v2.x };
	}

	float magnitudeSq() const
	{
		return x * x + y * y + z * z;
	}
	float magnitude() const
	{
		return sqrt(magnitudeSq());
	}

	//----
	//Stupid code that has to refactored
	void rotateX(float a)
	{
		float angle = 3.1415926 * a / 180;

		float tmp = y * cos(angle) - z * sin(angle);
		z = y * sin(angle) + z * cos(angle);
		y = tmp;
	}

	void rotateY(float a)
	{
		float angle = 3.1415926 * a / 180;

		float tmp = x * cos(angle) + z * sin(angle);
		z = -x * sin(angle) + z * cos(angle);
		x = tmp;
	}

	void rotateZ(float a)
	{
		float angle = 3.1415926f * a / 180;

		float tmp = x * cos(angle) - y * sin(angle);
		y = x * sin(angle) + y * cos(angle);
		x = tmp;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector3& v)
	{
		os << v.x<<" "<<v.y<<" "<<v.z<<";";
		return os;
	}
	Vector3& normalize()
	{
		float m = magnitude();
		x /= m;
		y /= m;
		z /= m;
		return *this;
	}
};
