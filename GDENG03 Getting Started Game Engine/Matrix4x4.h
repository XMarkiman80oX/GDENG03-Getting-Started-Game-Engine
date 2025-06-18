#pragma once
#include "memory"
#include "Vector3D.h"
#include "Vector4D.h"
#include "cstring"
#include <iostream>


class Matrix4x4 {
public:
	Matrix4x4() {
		this->setIdentity();
	}

	~Matrix4x4() {

	}
	void setMatrix(const Matrix4x4& other) {
		::memcpy(m_mat, other.m_mat, sizeof(float) * 16);
	}
	Vector3D getZDirection() {
		return Vector3D(this->m_mat[2][0], this->m_mat[2][1], this->m_mat[2][2]);
	}
	Vector3D getXDirection() {
		return Vector3D(this->m_mat[0][0], this->m_mat[0][1], this->m_mat[0][2]);
	}
	Vector3D getTranslation() {
		return Vector3D(this->m_mat[3][0], this->m_mat[3][1], this->m_mat[3][2]);
	}
	void setIdentity() {
		::memset(this->m_mat, 0, sizeof(float) * 16);
		this->m_mat[0][0] = 1;
		this->m_mat[1][1] = 1;
		this->m_mat[2][2] = 1;
		this->m_mat[3][3] = 1;
	}
	void setTranslation(const Vector3D& translation) {
		this->m_mat[3][0] = translation.x;
		this->m_mat[3][1] = translation.y;
		this->m_mat[3][2] = translation.z;
	}
	void setScale(const Vector3D& scale) {
		this->m_mat[0][0] = scale.x;
		this->m_mat[1][1] = scale.y;
		this->m_mat[2][2] = scale.z;
	}
	void setRotationX(float x) {
		this->m_mat[1][1] = cos(x);
		this->m_mat[1][2] = sin(x);
		this->m_mat[2][1] = -sin(x);
		this->m_mat[2][2] = cos(x); 
	}
	void setRotationY(float y) {
		this->m_mat[0][0] = cos(y);
		this->m_mat[0][2] = -sin(y);
		this->m_mat[2][0] = sin(y);
		this->m_mat[2][2] = cos(y); 
	}
	void setRotationZ(float z) {
		this->m_mat[0][0] = cos(z);
		this->m_mat[0][1] = sin(z);
		this->m_mat[1][0] = -sin(z);
		this->m_mat[1][1] = cos(z); 
	}
	void setOrthogonalProjectionMatrix(float width, float height, float near_plane, float far_plane) {
		this->setIdentity();
		this->m_mat[0][0] = 2.0f/width;
		this->m_mat[1][1] = 2.0f / height;
		this->m_mat[2][2] = 1.0f/(far_plane - near_plane);
		this->m_mat[3][2] = -(near_plane/(far_plane - near_plane));
	}
	void setPerspectiveFOVLH(float fov, float aspect, float znear, float zfar) {
		
		float yscale = 1.0f/tan(fov/2.0f);
		float xscale = yscale / aspect;
		this->m_mat[0][0] = xscale;
		this->m_mat[1][1] = yscale;
		this->m_mat[2][2] = zfar / (zfar - znear);
		this->m_mat[2][3] =1.0f;
		this->m_mat[3][2] =(-znear*zfar)/(zfar-znear);
	}
	float getDeterminant() {
		Vector4D minor, v1, v2, v3;
		float det;

		v1 = Vector4D(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
		v2 = Vector4D(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
		v3 = Vector4D(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);

		minor.cross(v1, v2, v3);

		det = -(this->m_mat[0][3] * minor.x +
			this->m_mat[1][3] * minor.y +
			this->m_mat[2][3] * minor.z +
			this->m_mat[3][3] * minor.w);
		return det;
	}
	void setInverse()
	{
		int a, i, j;
		Matrix4x4 out;
		Vector4D v, vec[3];
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].x = (this->m_mat[j][0]);
					vec[a].y = (this->m_mat[j][1]);
					vec[a].z = (this->m_mat[j][2]);
					vec[a].w = (this->m_mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);

			out.m_mat[0][i] = pow(-1.0f, i) * v.x / det;
			out.m_mat[1][i] = pow(-1.0f, i) * v.y / det;
			out.m_mat[2][i] = pow(-1.0f, i) * v.z / det;
			out.m_mat[3][i] = pow(-1.0f, i) * v.w / det;
		}

		this->setMatrix(out);
	}
	void operator *= (const Matrix4x4& matrix) {
		Matrix4x4 out;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out.m_mat[i][j] =
					this->m_mat[i][0] * matrix.m_mat[0][j] +
					this->m_mat[i][1] * matrix.m_mat[1][j] +
					this->m_mat[i][2] * matrix.m_mat[2][j] +
					this->m_mat[i][3] * matrix.m_mat[3][j];
			}
		}

		::memcpy(m_mat, out.m_mat, sizeof(float) * 16);
	}
	Matrix4x4 operator * (const Matrix4x4& matrix) {
		Matrix4x4 result = *this;
		result *= matrix;
		return result;
	}
	bool operator == (const Matrix4x4& other) {
		bool isEqual = true;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (other.m_mat[i][j] != this->m_mat[i][j]) isEqual = false;
			}
		}

		return isEqual;
	}
	void printMatrix() {
		std::cout << std::endl;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << this->m_mat[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
public:
	float m_mat[4][4] = {};
};