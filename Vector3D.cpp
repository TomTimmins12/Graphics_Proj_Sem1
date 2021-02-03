#include "Vector3D.h"

Vector3D::Vector3D()		//Constructor
{
	x = 0;
	y = 0;
	z = 0;
}


Vector3D::Vector3D(float x1, float y1, float z1)		//Construct with values
{
	x = x1;
	y = y1;
	z = z1;
}


Vector3D::Vector3D(const Vector3D& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}


//Addition
Vector3D Vector3D ::operator+(const Vector3D& vec)
{
	//Returns a new vector summing the values for each component with the 
	// corresponding component in the added vector

	return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}


Vector3D &Vector3D ::operator+=(const Vector3D& vec)
{
	//Returns `this' pointer (i.e. self-reference summing the values for
	//Each component with the corresponding component in the added vector

	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}


//Subtraction
Vector3D Vector3D ::operator-(const Vector3D& vec)
{
	//Returns a new vector subtracting the values for each component with the 
	//corresponding component in the subtracted vector

	return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}


Vector3D& Vector3D ::operator-=(const Vector3D& vec)
{
	//Returns `this' pointer (i.e. self-reference subtracting the values for
	//Each component with the corresponding component in the subtrated vector

	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}


//Scalar multiplication
Vector3D Vector3D ::operator*(float value)
{
	//Returns a new vector Multiplying the values for each component with the 
	//corresponding component in the multiplied vector

	return Vector3D(x * value, y * value, z * value);
}


Vector3D& Vector3D ::operator*=(float value)
{
	//Returns `this' pointer (i.e. self-reference multiplying the values for
	//Each component with the corresponding component in the multiplied vector
	x *= value;
	y *= value;
	z *= value;
	return *this;
}


//Scalar division
Vector3D Vector3D ::operator/(float value)
{
	assert(value != 0); //Prevent division by 0

	//Returns a new vector Dividing the values for each component with the 
	//corresponding component in the divided vector

	return Vector3D(x / value, y / value, z / value);
}


Vector3D& Vector3D ::operator/=(float value)
{
	assert(value != 0);	//Prevent division by 0
	//Returns `this' pointer (i.e. self-reference dividing the values for
	//Each component with the corresponding component in the divided vector
	x /= value;
	y /= value;
	z /= value;
	return *this;
}


// Set Vector position
Vector3D& Vector3D::operator=(const Vector3D& vec)
{
	return Vector3D(x = vec.x, y = vec.y, z = vec.z);
}


//Dot product
float Vector3D::dot_product(const Vector3D& vec)
{
	return (x * vec.x + y * vec.y + z * vec.z);
}


//Cross product
Vector3D Vector3D::cross_product(const Vector3D& vec)
{
	float ni = y * vec.z - z * vec.y;
	float nj = x * vec.z - z * vec.x;
	float nk = x * vec.y - y * vec.x;
	return Vector3D(ni, nj, nk);
}


float Vector3D::magnitude()
{
	return(sqrtf(square()));
}


float Vector3D::square()
{
	return x * x + y * y + z * z;
}


Vector3D Vector3D::Normalization()
{
	return(*this/magnitude());
}


float Vector3D::distance(const Vector3D& vec)
{
	return sqrtf((x-vec.x)+(y-vec.y)+(z-vec.z));
}
