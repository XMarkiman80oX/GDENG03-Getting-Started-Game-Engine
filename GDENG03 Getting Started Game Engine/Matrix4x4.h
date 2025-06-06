#pragma once
#include "memory"
#include "Vector3D.h"

class Matrix4x4 {
public:
	Matrix4x4() {

	}

	~Matrix4x4() {

	}

	void setIdentity() {
		::memset(this->m_mat, 0, sizeof(float) * 16);
		this->m_mat[0][0] = 1;
		this->m_mat[1][1] = 1;
		this->m_mat[2][2] = 1;
		this->m_mat[3][3] = 1;
	}
	void setTranslation(const Vector3D& translation) {
		this->setIdentity();
		this->m_mat[3][0] = translation.x;
		this->m_mat[3][1] = translation.y;
		this->m_mat[3][2] = translation.z;
	}
	void setScale(const Vector3D& scale) {
		this->setIdentity();
		this->m_mat[0][0] = scale.x;
		this->m_mat[1][1] = scale.y;
		this->m_mat[2][2] = scale.z;
	}
	void setOrthogonalProjectionMatrix(float width, float height, float near_plane, float far_plane) {
		this->setIdentity();
		this->m_mat[0][0] = 2.0f/width;
		this->m_mat[1][1] = 2.0f / height;
		this->m_mat[2][2] = 1.0f/(far_plane - near_plane);
		this->m_mat[3][2] = -(near_plane/(far_plane - near_plane));
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
public:
	float m_mat[4][4] = {};
};