#include <bits/stdc++.h>

#include "BasicDef.h"

// 2本のベクトルvec0とvec1の内積
double dot(double vec0[], double vec1[])
// double vec0[];
// double vec1[];
{
	return(vec0[X] * vec1[X] + vec0[Y] * vec1[Y] + vec0[Z] * vec1[Z]);
}

// 2本のベクトルvec0とvec1の外積
void cross(double vec0[], double vec1[], double vec2[])
// double vec0[]
// double vec1[]
// double vec2[]
{
	vec2[X] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
	vec2[Y] = vec0[Z] * vec1[X] - vec0[X] * vec1[Z];
	vec2[Z] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];
}

// ベクトルの正規化
void normVec(double vec[])
// double vec[]; 注意！このベクトルは破壊的に変更される
{
	double norm;
	norm = sqrt(vec[X] * vec[X] + vec[Y] * vec[Y] + vec[Z] * vec[Z]);
	vec[X] /= norm;
	vec[Y] /= norm;
	vec[Z] /= norm;
}

// 3頂点を含っむ平面の単位法線ベクトルの計算
void normal(double p0[], double p1[], double p2[], double normal[])
// double p0[], p1[], p2[]: 凸ポリゴンの周囲の反時計回りにならぶ3頂点の座標
// double normal[]; 計算された法線ベクトル
{
	unsigned int i;
	double v0[3], v1[3];

	// 基本となる2つのベクトルを生成
	for (i = 0; i < 3; i++) {
		v0[i] = p2[i] - p1[i];
		v1[i] = p0[i] - p1[i];
	}

	// 生成したベクトルの外積を計算する
	cross(v0, v1, normal);

	// 外積によって得られた法線ベクトルを正規化
	normVec(normal);
}

