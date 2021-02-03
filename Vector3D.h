#pragma once
#include"includes.h"

using namespace std;

class Vector3D
{
public:
	float x, y, z;

	// Construcors
	Vector3D(); //Contructor
	Vector3D(float x1, float y1, float z1 = 0); // Construct with values
	Vector3D(const Vector3D& vec); // Copy Constructor

	// Arithmetic operations
	Vector3D operator+(const Vector3D& vec);	//Addition
	Vector3D &operator+=(const Vector3D& vec);	//Assign new result to vector
	Vector3D operator-(const Vector3D& vec);	//Subtraction
	Vector3D &operator-=(const Vector3D& vec);	//Assign new result to vector
	Vector3D operator*(float value);	//Multiplication
	Vector3D &operator*=(float value);	//Assign new result to vector
	Vector3D operator/(float value);	//Division
	Vector3D &operator/=(float value);	//Assign new result to vector
	Vector3D& operator=(const Vector3D& vec);

	// Vector operations
	float dot_product(const Vector3D& vec);		//Scalar dot_product
	Vector3D cross_product(const Vector3D& vec);	//Cross_Product
	Vector3D Normalization();	//Normalized vector

	// Scalar operations
	float square();	//Gives square of the vector
	float distance(const Vector3D& vec);	//Distance between two vectors
	float magnitude();		//Magnitude of the vector

	//Display operations
	float show_x() { return x; }		//Return x
	float show_y() { return y; }		//Return y
	float show_z() { return z; }		//Return z
	void disp() { cout << "X = " << x << " Y = " << y << " Z = " << z << endl; }		//Display value of vectors
};