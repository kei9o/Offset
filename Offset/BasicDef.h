#pragma once
// 座標値や係数を参照する際のインデックス
#define X 0
#define Y 1
#define Z 2

#define A 0
#define B 1
#define C 2
#define D 

// 十分に微小な値
#define EPS 0.00001

// 十分に大きな値
#define LARGE 100000.0

// 円周率
#define PI 3.141592653589793

// 図形要素の最大個数
#define MAX_NUM_POINTS 2000000
#define MAX_NUM_EDGES 5000000
#define MAX_NUM_TRIANGLES 2000000

// 一時的に利用する点の構造体
typedef struct tmp_point {
	double coord[3]; // 座標
	unsigned int index; // 点に付随するユニークなインデックス
} tmp_point;

// 一時的に利用する辺の構造体
typedef struct tmp_edge {
	unsigned int start;
	unsigned int end;
	tmp_edge *next;
} tmp_edge;