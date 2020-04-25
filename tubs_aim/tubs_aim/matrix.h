#pragma once

class Matrix4x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float Matrix[4][4];
		float mm[16];

		struct
		{
			__m128 m1, m2, m3, m4;
		};
	};

	inline void Transpose()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Matrix[i][j] = Matrix[j][i];
			}
		}
	}

	inline Vector& GetAxis(int i)
	{
		return *(Vector*)&Matrix[i][0];
	}
};

class Matrix3x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
		};

		float m[3][4];
		float mm[12];
	};

	inline Vector& GetAxis(int i)
	{
		return *(Vector*)&m[i][0];
	}
};
