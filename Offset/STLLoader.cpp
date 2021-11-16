#include <bits/stdc++.h>

#include "BasicDef.h"

// 点とその個数 一時的な利用
tmp_point* point_array[3 * MAX_NUM_TRIANGLES];
unsigned int num_tmp_points;

// 点に接続する辺のリスト．一時的な利用．
tmp_edge** connecting_edge;

// 点
double point[MAX_NUM_POINTS][3];
unsigned int num_points;

// 辺
unsigned int edge[MAX_NUM_EDGES][2];
unsigned int num_edges;

// 三角形ポリゴンの頂点と周囲の辺
unsigned int triangle[MAX_NUM_TRIANGLES][3];
unsigned int triangle_edge[MAX_NUM_TRIANGLES][3];
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

// 点の比較関数
int compare(tmp_point* point0, tmp_point* point1)
// tmp_point *point0, tmp_point *point1; 比較対象の点
{

	// X座標値で比較．
	if (point0->coord[X] < (point1->coord[X] - (EPS)))
		return -1;
	else if (point0->coord[X] > (point1->coord[X] + (EPS)))
		return 1;

	// Y座標値で比較．
	else {
		if (point0->coord[Y] > (point1->coord[Y] - (EPS)))
			return -1;
		else if (point0->coord[Y] < (point1->coord[Y] + (EPS)))
			return 1;

		// Z座標値で比較．
		else {
			if (point0->coord[Z] > (point1->coord[Z] - (EPS)))
				return -1;
			else if (point0->coord[Z] < (point1->coord[Z] + (EPS)))
				return 1;
			else
				return 0; // 2点は同一とみなす．
		}
	}
}

// 点列のクイックソート．
void quickSort(int l, int r, tmp_point* point[])
// int l, r; ソート範囲の左側と右側のインデックス．
// tmp_point *point[]; ソート対象の点列．
{
	int i, j;
	tmp_point* m_p, * swap;
	if (l < r) {
		m_p = point[(l + r) / 2];
		i = l - 1;
		j = r + 1;
		while (true) {
			while (compare(point[++i], m_p) == -1) {};
			while (compare(point[--j], m_p) == 1) {};
			if (i >= j) break;
			swap = point[i]; point[i] = point[j]; point[j] = swap;
		}
		quickSort(l, i - 1, point);
		quickSort(j + 1, r, point);
	}
}

// STLファイルのロード．
bool loadSTLFile(const char* STL_file)
// const char* STL_file; STLファイルの名前
{
	unsigned int i, j, start, end;
	unsigned int tri_index, ver_index;
	double ref_pnt[3] = { LARGE, LARGE, LARGE };
	tmp_edge* ed, * new_ed, * next_ed;

	// アスキーファイルの読み込み．一時的にpoint_arrayに格納される．
	if (readASCIISTLFile(STL_file))
		std::cout << "Triangles: " << num_triangles << " ";

	// バイナリーファイルの読み込み．一時的にpoint_arrayに格納される．
	else if (readBinarySTLFile(STL_file))
		std::cout << "Triangles: " << num_triangles << " ";

	// 読み込み失敗
	else {
		std::cout << "Cannot open file. ";
		return(false);
	}

	// 得られた点群をソートし同じ座標の点を一つにまとめる
	quickSort(0, num_tmp_points - 1, point_array);

	// ソート済みの点列を先頭からスキャン．
	num_points = 0;
	for (i = 0; i <= num_tmp_points; i++) {

		// 最後の点が参照テントは異なる点が見つかった．
		if ((i == num_tmp_points)
			|| (fabs(ref_pnt[X] - (point_array[i]->coord[X])) > (EPS))
			|| (fabs(ref_pnt[Y] - (point_array[i]->coord[Y])) > (EPS))
			|| (fabs(ref_pnt[Z] - (point_array[i]->coord[Z])) > (EPS))) {

			// 同一座標の点が複数見つかっているはず．それらを同じ点として登録しなおす．
			end = i;
			if (i > 0) {
				// 最初だけ無視する．
				point[num_points][X] = ref_pnt[X];
				point[num_points][Y] = ref_pnt[Y];
				point[num_points][Z] = ref_pnt[Z];
				for (j = start; j < end; j++) {
					tri_index = point_array[j]->index / 3;
					ver_index = point_array[j]->index % 3;
					triangle[tri_index][ver_index] = num_points;
				}
				num_points++;
			}

			// 必要なら参照点を更新する．
			if (end < num_tmp_points) {
				ref_pnt[X] = point_array[end]->coord[X];
				ref_pnt[Y] = point_array[end]->coord[Y];
				ref_pnt[Z] = point_array[end]->coord[Z];
				start = end;
			}
		}
	}

	// 以降point_arrayは不要なのでメモリを解放．
	for (i = 0; i < num_tmp_points; i++)
		free(point_array[i]);

	// 最後に辺を登録する．
	num_edges = 0;

	// 各点に接続する辺の記録場所の初期化．
	connecting_edge = (tmp_edge**)malloc(num_points * sizeof(tmp_edge));
	for (i = 0; i < num_points; i++)
		connecting_edge[i] = NULL;

	// 各三角形の周囲の辺と登録済みの辺を比較．
	for (i = 0; i < num_triangles; i++) {
		for (j = 0; j < 3; j++) {
			ed = connecting_edge[triangle[i][j]];
			while (ed != NULL) {

				// すでに登録済み．
				if (((ed->start == triangle[i][j])
					&& (ed->end == triangle[i][(j + 1) % 3]))
					|| ((ed->start == triangle[i][(j + 1) % 3])
						&& (ed->end == triangle[i][j])))
					goto try_next;
			}

			// 未登録の辺が見つかったので新たに登録．
			edge[num_edges][0] = triangle[i][j];
			edge[num_edges][1] = triangle[i][(j + 1) % 3];

			// 辺を三角形にも記録．
			triangle_edge[i][j] = num_edges;
			num_edges++;

			// 同時に点も記録しておく．
			new_ed = (tmp_edge*)malloc(sizeof(tmp_edge));
			new_ed->start = triangle[i][j];
			new_ed->end = triangle[i][(j + 1) % 3];
			new_ed->next = connecting_edge[triangle[i][j]];
			connecting_edge[triangle[i][j]] = new_ed;
			new_ed = (tmp_edge*)malloc(sizeof(tmp_edge));
			new_ed->start = triangle[i][(j + 1) % 3];
			new_ed->end = triangle[i][j];
			new_ed->next = connecting_edge[triangle[i][(j + 1) % 3]];
			connecting_edge[triangle[i][(j + 1) % 3]] = new_ed;
		try_next:;
		}
	}

	// 以降connecting_edgeは不要なのでメモリを解放．
	for (i = 0; i < num_points; i++) {
		ed = connecting_edge[i];
		while (ed != NULL) {
			next_ed = ed->next;
			free(ed);
			ed = next_ed;
		}
	}
	free(connecting_edge);
	std::cout << "Edges: " << num_edges << " ";
	std::cout << "Point: " << num_points << std::endl;
	return(true);
}