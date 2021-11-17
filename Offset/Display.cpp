#include <bits/stdc++.h>
#include <gl/freeglut.h>

#include "BasicDef.h"

// 視点設定に関する変数．
double eye[3];
double center[3] = { 0.0, 0.0, 0.0 };
double up[3];

// 正投影の座標系
double x_axis[3], y_axis[3], z_axis[3]; // 視点座標系座標軸のグローバル座標系における方向

// ウィンドウ上の座標とモデル座標の変換に用いるデータ
double corner[3];
double pixel_size, ref_far, ref_near;

// 照明
GLfloat light_pos[4];

// STLLoader.cppの大域変数とBasicGeom.cppの関数を利用できるようにextern宣言する．
// 点
extern double point[MAX_NUM_POINTS][3];
extern unsigned int num_points;

// 三角形ポリゴン
extern unsigned int triangle[MAX_NUM_TRIANGLES][3];
extern unsigned int num_triangles;

// 基本的な図形計算．
extern double dot(double vec0[], double vec1[]);
extern void cross(double vec0[], double vec1[], double vec2[]);
extern void normVec(double vec[]);
extern void normal(double p0[], double p1[], double p2[], double normal[]);

// 投影処理用の行列定義．モデルをウィンドウ一杯に表示する．
void defineViewMatrix(double phi, double theta, unsigned int width, unsigned int height, double margin)
// double phi, theta; 方位角と仰角
// unsigned int width, height; ウィンドウの解像度．
// double margin; 表示する際に立体周囲に与える余裕．
{
	unsigned int i, j;
	double c, s, xy_dist;
	double x_axis[3], y_axis[3], z_axis[3], vec[3];
	double left, right, bottom, top, farVal, nearVal;
	double dx, dy, d_aspect, w_aspect, d;

	// 視点の設定
	eye[Z] = sin(theta * PI / 180.0);
	xy_dist = cos(theta * PI / 180.0);
	c = cos(phi * PI / 180.0);
	s = sin(phi * PI / 180.0);
	eye[X] = xy_dist * c;
	eye[Y] = xy_dist * s;
	up[X] = -c * eye[Z];
	up[Y] = -s * eye[Z];
	up[Z] = s * eye[Y] + c * eye[X];
	normVec(up);

	// 視点を原点とする座標系の定義
	for (i = 0; i < 3; i++)
		z_axis[i] = eye[i] - center[i];
	normVec(z_axis);
	cross(up, z_axis, x_axis);
	normVec(x_axis);
	cross(z_axis, x_axis, y_axis);

	// left, right, bottom, top, nearVal, farValの決定
	left = bottom = farVal = 10000.0;
	right = top = nearVal = -10000.0;
	for (i = 0; i < num_points; i++) {
		for (j = 0; j < 3; j++)
			vec[j] = point[i][j] - eye[j];
		if (dot(x_axis, vec) < left)
			left = dot(x_axis, vec);
		if (dot(x_axis, vec) > right)
			right = dot(x_axis, vec);
		if (dot(y_axis, vec) < bottom)
			bottom = dot(y_axis, vec);
		if (dot(y_axis, vec) > top)
			top = dot(y_axis, vec);
		if (dot(z_axis, vec) < farVal)
			farVal = dot(z_axis, vec);
		if (dot(z_axis, vec) > nearVal)
			nearVal = dot(z_axis, vec);
	}

	// 図形の周囲に5%ほど余裕を与える
	margin = (right - left) * 0.05;
	left -= margin;
	right += margin;
	margin = (top - bottom) * 0.05;
	bottom -= margin;
	top += margin;
	margin = (nearVal - farVal) * 0.05;
	farVal -= margin;
	nearVal += margin;
	ref_far = farVal;
	ref_near = nearVal;

	// 表示範囲のアスペクト比とウインドウのアスペクト比の比較
	dx = right - left;
	dy = top - bottom;
	d_aspect = dy / dx;
	w_aspect = (double)height / (double)width;

	// ウィンドウが表示範囲よりも縦長．範囲表示を縦に広げる
	if (w_aspect > d_aspect) {
		d = (dy * (w_aspect / d_aspect - 1.0)) * 0.5;
		bottom -= d;
		top += d;
	}

	// ウィンドウが表示範囲よりも横長．表示範囲を横に広げる
	else {
		d = (dx * (d_aspect / w_aspect - 1.0)) * 0.5;
		left -= d;
		right += d;
	}

	// 各ピクセルの実サイズ．
	pixel_size = (right - left) / width;

	// 視体積の左下隅奥の座標を記録しておく．
	for (i = 0; i < 3; i++)
		corner[i] = eye[i] + bottom * y_axis[i] + left * x_axis[i] + farVal * z_axis[i];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -nearVal, -farVal); //表示範囲変更
	glViewport(0, 0, width, height);    // 投影によって得られた画像をウィンドウにはめ込む
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], center[X], center[Y], center[Z], up[X], up[Y], up[Z]);
}

// モデルの表示関数
void displayModel(void)
{
	unsigned int i;
	double nrml_vec[3];

	// 光源の設定
	light_pos[0] = (float)eye[X];
	light_pos[1] = (float)eye[Y];
	light_pos[2] = (float)eye[Z];
	light_pos[3] = 0.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); // 光源の定義

	// 照明の点灯
	glEnable(GL_LIGHTING);
	
	// 描画処理
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_TRIANGLES);                            // これから描く図形のタイプ

	for (i = 0; i < num_triangles; i++) {
		normal(point[triangle[i][0]], point[triangle[i][1]], point[triangle[i][2]], nrml_vec);
		if (dot(nrml_vec, z_axis) > 0.0)
			glNormal3dv(nrml_vec);
		else
			glNormal3d(-nrml_vec[X], -nrml_vec[Y], -nrml_vec[Z]);
		glVertex3dv(point[triangle[i][0]]);
		glVertex3dv(point[triangle[i][1]]);
		glVertex3dv(point[triangle[i][2]]);
	}

	glEnd();
	glFlush();
}

// ピクセルの横方向と縦方向の番地(h, v)とデプス値dから，対応するモデル座標を得る．
void pixelCoordToModelCoord(int h, int v, float d, double point[])
// int h, v; ピクセルの横方向と縦方向の番地．
// float d; デプス値．
// double point[]; 対応するモデル座標，
{
	unsigned int i;

	// cornerには左下隅奥に対応するモデル座標が格納されている．
	for (i = 0; i < 3; i++) {
		point[i] = corner[i] + pixel_size * h * x_axis[i] + pixel_size * v * y_axis[i]

			// デプス値はnearが0.0, farが1.0に対応するように正規化してある．
			+ (ref_near - ref_far) * (1.0 - d) * z_axis[i];
	}
}