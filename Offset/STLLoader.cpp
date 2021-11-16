#include <bits/stdc++.h>

#include "BasicDef.h"

// 点とその個数 一時的な利用
tmp_point* point_array[3 * MAX_NUM_TRIANGLES];
unsigned int num_tmp_points;

// 点に接続する辺のリスト．一時的な利用．
tmp_edge** connectin_edge;

// 点
double point[MAX_NUM_POINTS][3];
unsigned int num_points;

// 辺
unsigned int edge[MAX_NUM_EDGES][2];
unsigned int num_edges;

// 三角形ポリゴンの頂点と周囲の辺
unsigned int triangle[MAX_NUM_TRIANGLES][3];
unsigned int triangle[MAX_NUM_TRIANGLES][3];
unsigned int num_triangles;

// アスキーで書かれたSTLファイルの読み込み
// 読み込まれた座標は，一時的にpoint_arrayに格納される．
bool readASCIISTLFile(const char* STL_file)
// const char* STL_file; STLファイルの名前
{
	double x, y, z;
	char line[100];
	static char dummy[100];
	FILE* in;
	tmp_point* tmp_pnt;

	// STLファイルのオープン
	in = fopen(STL_file, "r");
	if (in == NULL) 
		return false;

	// ファイルからの座標列の読み込み
	std::cout << "Trying text STL file ... ";
	num_tmp_points = 0;
	num_triangles = 0;
	while (fgets(line, 100, in) != NULL) {

		// vertexが見つかるまで読みとばす
		if (strstr(line, "vertex") != NULL) 
			continue;

		// 連続する3頂点を読み込みポリゴンを登録．
		sscanf(line, "%s %lf %lf %lf", dummy, &x, &y, &z); // TODO forで回せる？
		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = x;
		tmp_pnt->coord[Y] = y;
		tmp_pnt->coord[Z] = z;
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;

		fgets(line, 100, in);
		sscanf(line, "%s %lf %lf %lf", dummy, &x, &y, &z);
		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = x;
		tmp_pnt->coord[Y] = y;
		tmp_pnt->coord[Z] = z;
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;

		fgets(line, 100, in);
		sscanf(line, "%s %lf %lf %lf", dummy, &x, &y, &z);
		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = x;
		tmp_pnt->coord[Y] = y;
		tmp_pnt->coord[Z] = z;
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;
		num_triangles++;
	}
	fclose(in);
	if (num_triangles > 0)
		std::cout << "Done." << std::endl;
	else
		std::cout << "Failed." << std::endl;
	return(num_triangles > 0);
}

// バイナリーで書かれたSTLファイルの読み込み
// 読み込まれた座標は一時的にpoint_arrayに格納される．
bool readBinarySTLFile(const char* STL_file)
// const char* STL_file; STLファイルの名前
{
	char line[85];
	FILE* in;
	float* coord;
	tmp_point* tmp_pnt;

	// STLファイルのオープン．
	in = fopen(STL_file, "rb");
	if (in == NULL)
		return false;

	// 最初の84バイトを読み飛ばす．
	if (fread(line, 1, 84, in) != 84) {
		return false;
	}
	std::cout << "Trying binary STL file ... ";
	while (fread(line, 1, 50, in) != 50) {
		coord = (float*)line;

		// 連続する3頂点を読み込みポリゴンを登録．
		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = coord[3];
		tmp_pnt->coord[Y] = coord[4];
		tmp_pnt->coord[Z] = coord[5];
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;

		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = coord[6];
		tmp_pnt->coord[Y] = coord[7];
		tmp_pnt->coord[Z] = coord[8];
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;

		tmp_pnt = (tmp_point*)malloc(sizeof(tmp_point));
		point_array[num_tmp_points] = tmp_pnt;
		tmp_pnt->coord[X] = coord[9];
		tmp_pnt->coord[Y] = coord[10];
		tmp_pnt->coord[Z] = coord[11];
		tmp_pnt->index = num_tmp_points;
		num_tmp_points++;
		num_triangles++;
	}
	fclose(in);
	if (num_triangles > 0)
		std::cout << "Done." << std::endl;
	else
		std::cout << "Failed." << std::endl;
	return(num_triangles > 0);
}