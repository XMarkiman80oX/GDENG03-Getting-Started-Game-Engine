#pragma once
#include "memory"
#include "Vector3D.h"
#include "Vector4D.h"
#include "cstring"

class Matrix4x4 {
public:
	Matrix4x4() {

	}

	~Matrix4x4() {

	}
	void setMatrix(const Matrix4x4& other) {
		memcpy(this->m_mat, other.m_mat, sizeof(float) * 16);
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
	void setInverse() {
		Matrix4x4 cofactors;
		int p, q, n, m;

		for (p = 0; p < 4; p++) {
			for (q = 0; q < 4; q++) {
				float minor[3][3];
				int i = 0;
				for (n = 0; n < 4; n++) {
					if (n == p) continue;
					int j = 0;
					for (m = 0; m < 4; m++) {
						if (m == q) continue;
						minor[i][j] = m_mat[n][m];
						j++;
					}
					i++;
				}

				float det_minor = minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1]) -
					minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
					minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);

				cofactors.m_mat[p][q] = ((p + q) % 2 == 1 ? -1.0f : 1.0f) * det_minor;
			}
		}

		float det = m_mat[0][0] * cofactors.m_mat[0][0] +
			m_mat[0][1] * cofactors.m_mat[0][1] +
			m_mat[0][2] * cofactors.m_mat[0][2] +
			m_mat[0][3] * cofactors.m_mat[0][3];

		if (abs(det) < 1e-6) {
			return;
		}

		float inv_det = 1.0f / det;
		Matrix4x4 adjugate;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				adjugate.m_mat[i][j] = cofactors.m_mat[j][i];
			}
		}

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m_mat[i][j] = adjugate.m_mat[i][j] * inv_det;
			}
		}
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