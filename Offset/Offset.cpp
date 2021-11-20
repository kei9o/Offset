#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
#include <gl/freeglut.h>

#include "BasicDef.h"

// STLデータのファイル名．
#define STL_FILE "uploads_files_2309336_Cole_Armchair_Maisons_Du_Monde.stl"

// モデルを確認する視点位置
#define PHI 30.0
#define THETA 60.0
double phi = PHI;
double theta = THETA;

// ウィンドウの初期位置と初期サイズ
#define INIT_X_POS 128
#define INIT_Y_POS 128
#define INIT_WIDTH 512
#define INIT_HEIGHT 512

// 想定される画像の最大サイズ．
#define MAX_WIDTH 2048
#define MAX_HEIGHT 1024

// 画像のデプス値と色データ．
GLfloat depth_data[MAX_WIDTH * MAX_HEIGHT];
GLubyte color_data[MAX_WIDTH * MAX_HEIGHT][4];

// オフセット面上の点群の座標データと，対応する点，辺，ポリゴン．
double offset_point[MAX_WIDTH * MAX_HEIGHT][3];
unsigned int offset_index[MAX_WIDTH * MAX_HEIGHT];

// ウィンドウの縦横サイズ
unsigned int window_width, window_height;

// 球や円筒形を多面体近似する際の分割数．
#define DIV 64

// 表示モード
#define DISPLAY_MODEL 0
#define DISPLAY_OFFSET 1
unsigned int display_mode = DISPLAY_MODEL;

// オフセット半径
double radius = 1.0;

// マウス処理
int mouse_old_x, mouse_old_y;
bool motion_p;

// 点
extern double point[MAX_NUM_POINTS][3];
extern unsigned int num_points;

// 辺
extern unsigned int edge[MAX_NUM_EDGES][2];
extern unsigned int num_edges;

// 三角形ポリゴンの頂点．
extern unsigned int triangle[MAX_NUM_TRIANGLES][3];
extern unsigned int num_triangles;

// STLファイルの読み込み．
extern bool loadSTLFile(const char* STL_file);

// 基本的な図形計算．
extern void cross(double vec0[], double vec1[], double vec2[]);
extern void normVec(double vec[]);
extern void normal(double p0[], double p1[], double p2[], double normal[]);

// 基本的な表示関数．
extern void defineViewMatrix(double phi, double theta, unsigned int width, unsigned int height, double margin);
extern void displayModel(void);
extern void pixelCoordToModelCoord(int h, int v, float d, double point[]);

// OpenGL関係の初期設定．
void initGL(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE); // カリングを有効にする．
	glCullFace(GL_BACK); // 裏面は無視．
}

void resize(int width, int height) // 新しいサイズを取得し記録する
{
	// ウインドウサイズの取得
	window_width = width;
	window_height = height;

	// ウィンドウサイズが変わるとオフセット計算の結果は不正となる．
	display_mode = DISPLAY_OFFSET;
}

// マウスのボタン処理
void mouse_button(int button, int state, int x, int y)
{
	if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))
		motion_p = true;
	else if (state == GLUT_UP)
		motion_p = false;
	mouse_old_x = x;
	mouse_old_y = y;
}

// マウスの移動処理
void mouse_motion(int x, int y)
{
	int dx, dy;
	dx = x - mouse_old_x;
	dy = y - mouse_old_y;
	if (motion_p)
	{
		phi -= dx * 0.2;
		theta += dy * 0.2;
	}
	mouse_old_x = x;
	mouse_old_y = y;
	glutPostRedisplay(); // 画像の強制書き換え
}

// 半球の描画．
void displaySphere(double radius)
// double radius; 球の半径
{
	unsigned int i, j;
	double da, x0, y0, z0, x1, y1, z1, r0, r1, c, s;
	da = (2 * PI / DIV);
	for (i = 0; i < (DIV / 4 + 1); i++) {
		z0 = radius * cos(da * i);
		r0 = radius * sin(da * i);
		z1 = radius * cos(da * (i + 1));
		r1 = radius * sin(da * (i + 1));
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= DIV; j++) {
			c = cos(da * (j % DIV));
			s = sin(da * (j % DIV));
			x0 = r0 * c;
			y0 = r0 * s;
			glNormal3d(x0 / radius, y0 / radius, z0 / radius);
			glVertex3d(x0, y0, z0);
			x1 = r1 * c;
			y1 = r1 * s;
			glNormal3d(x1 / radius, y1 / radius, z1 / radius);
			glVertex3d(x1, y1, z1);
		}
		glEnd();
	}
}

// 円筒形の描画．
void displayCylinder(double start[], double end[], double radius)
// double start[], end[]; 円筒形の両端点．
// double radius; 円筒形の半径．
{
	int i;
	double x_axis[3], y_axis[3], z_axis[3], ref[3], tmp[3];
	double da, c, s, x, y, z;

	// 円筒形の中心軸をz軸とするローカル座標系の設定．
	for (i = 0; i < 3; i++)
		z_axis[i] = end[i] - start[i];
	normVec(z_axis);
	ref[X] = 1.0;
	ref[Y] = ref[Z] = 0.0;
	cross(z_axis, ref, tmp);

	// z_axisとrefが平行なケース．
	if ((fabs(tmp[X]) < EPS) && (fabs(tmp[Y]) < EPS) && (fabs(tmp[Z]) < EPS)) {
		ref[Y] = 1.0;
		ref[X] = ref[Z] = 0.0;
		cross(z_axis, ref, tmp);
	}
	cross(tmp, z_axis, x_axis);
	normVec(x_axis);
	cross(z_axis, x_axis, y_axis);

	// 円筒形を四辺形の連続で描く．
	da = (2 * PI / DIV);
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= DIV; i++) {
		c = cos(da * (i % DIV));
		s = sin(da * (i % DIV));
		x = radius * (c * x_axis[X] + s * y_axis[X]);
		y = radius * (c * x_axis[Y] + s * y_axis[Y]);
		z = radius * (c * x_axis[Z] + s * y_axis[Z]);
		glNormal3d(x / radius, y / radius, z / radius);
		glVertex3d(x + end[X], y + end[Y], z + end[Z]);
		glVertex3d(x + start[X], y + start[Y], z + start[Z]);
	}
	glEnd();
}

// スラブ形状の描画．
void displaySlab(double p0[], double p1[], double p2[], double radius)
// double p0[], p1[], p2[]; ポリゴンの周囲の3点．反時計回りを仮定．
// double radius; ポリゴンのシフト量．
{
	double nrml_vec[3];

	// 上向きにシフトしたポリゴン．
	glNormal3dv(nrml_vec);
	glVertex3d(p0[X] + radius * nrml_vec[X], p0[Y] + radius * nrml_vec[Y], p0[Z] + radius * nrml_vec[Z]);
	glVertex3d(p1[X] + radius * nrml_vec[X], p1[Y] + radius * nrml_vec[Y], p1[Z] + radius * nrml_vec[Z]);
	glVertex3d(p2[X] + radius * nrml_vec[X], p2[Y] + radius * nrml_vec[Y], p2[Z] + radius * nrml_vec[Z]);

	// 下向きにシフトしたポリゴン．
	glNormal3d(-nrml_vec[X], - nrml_vec[Y], -nrml_vec[Z]);
	glVertex3d(p2[X] - radius * nrml_vec[X], p2[Y] - radius * nrml_vec[Y], p2[Z] - radius * nrml_vec[Z]);
	glVertex3d(p1[X] - radius * nrml_vec[X], p1[Y] - radius * nrml_vec[Y], p1[Z] - radius * nrml_vec[Z]);
	glVertex3d(p0[X] - radius * nrml_vec[X], p0[Y] - radius * nrml_vec[Y], p0[Z] - radius * nrml_vec[Z]);
}

// 指定したIDに対応した色(r, g, b)を与える処理
void IDToColor(unsigned int id)
// unsigned int id; ID値
{
	GLubyte r, g, b;
	unsigned int tmp;
	r = (GLubyte)(id / 65536); // 65536 = 256 * 256
	tmp = id % 65536;
	g = (GLubyte)(tmp / 256);
	b = (GLubyte)(tmp % 256);
	glColor3ub(r, g, b);
}

// 色から対応するIDを得る処理
unsigned int colorToID(GLubyte r, GLubyte g, GLubyte b)
// GLubyte r, g, b 色データ
{
	return(r * 65536 + g * 256 + b);
}

// 球のディスプレーリスト．
GLuint sphere_list = 1;

// オフセット形状の算出．
void compOffset(unsigned int width, unsigned int height, double offset)
// unsigned int width, height; ウィンドウの解像度．
// double offset; オフセット量．
{
	unsigned int i, v, h, shift;

	// 投影行列の計算．立体を真上から見下ろす．
	defineViewMatrix(270.0, 90.0, width, height, offset);

	// フレームバッファの初期化．
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 照明の消灯．
	glDisable(GL_LIGHTING);

	// ディスプレーリストの作成．
	glNewList(sphere_list, GL_COMPILE);
	displaySphere(offset);
	glEndList();

	// スラブ形状の描画．
	glBegin(GL_TRIANGLES);
	for (i = 0; i < num_triangles; i++) {
		IDToColor(i);
		displaySlab(point[triangle[i][0]], point[triangle[i][1]], point[triangle[i][2]], offset);
	}
	glEnd();
	if (offset > 0.0) {

		// 球の描画．
		for (i = 0; i < num_triangles; i++) {
			IDToColor(i + num_triangles);
			glPushMatrix();
			glTranslated(point[i][X], point[i][Y], point[i][Z]); // 座標変換: 平行移動
			glCallList(sphere_list);
			glPopMatrix(); // glPushMatrix()以前の座標系に復元
		}

		// 円筒形の描画．
		for (i = 0; i < num_edges; i++) {
			IDToColor(i + num_triangles + num_points);
			displayCylinder(point[edge[i][0]], point[edge[i][1]], offset);
		}
	}
	glFlush();

	// ディスプレーリストの削除．
	glDeleteLists(sphere_list, 1);

	// 各ピクセルの色データとデプス値を取得．
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, color_data);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_data);

	// 対応する座標値と点，辺，ポリゴンのインデックスを取得．
	for (v = 0; v < height; v++) {
		for (h = 0; h < width; h++) {
			shift = width * v * h;
			offset_index[shift] = colorToID(color_data[shift][0], color_data[shift][1], color_data[shift][2]);
			if (offset_index[shift] < (num_points + num_edges + num_triangles))
				pixelCoordToModelCoord(h, v, depth_data[shift], offset_point[shift]);
		}
	}
}

// 格子状の点群で定義された曲面の描画．
void displayGridSurface(unsigned int width, unsigned int height)
// unsigned int width, height; 格子の解像度
{
	unsigned int i, j, shift;
	double nrml_vec[3];

	// フレームバッファの初期化
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 照明の点灯．
	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < (height - 1); i++) {
		for (j = 0; j < (width - 1); j++) {
			shift = width * i + j;

			// 隣接する4つの格子のすべてでオフセット面のデータが計算されている．
			if((offset_index[shift] < (num_points + num_edges + num_triangles))
				 && (offset_index[shift + 1] < (num_points + num_edges + num_triangles))
					 && (offset_index[shift + width] < (num_points + num_edges + num_triangles))
						 && (offset_index[shift + width + 1] < (num_points + num_edges + num_triangles))) {

				// 1枚目のポリゴン
				normal(offset_point[shift], offset_point[shift + 1], offset_point[shift + width], nrml_vec);
				glNormal3dv(offset_point[shift]);
				glNormal3dv(offset_point[shift + 1]);
				glNormal3dv(offset_point[shift + width]);

				// 2枚目のポリゴン
				normal(offset_point[shift + 1], offset_point[shift + width + 1], offset_point[shift + width], nrml_vec);
				glNormal3dv(offset_point[shift + 1]);
				glNormal3dv(offset_point[shift + width + 1]);
				glNormal3dv(offset_point[shift + width]);
			}
		}
	}
	glEnd();
	glFlush();
}

// 表示
void display(void)
{
	switch (display_mode) {
	case DISPLAY_MODEL:
		defineViewMatrix(phi, theta, window_width, window_height, radius);
		displayModel();
		break;
	case DISPLAY_OFFSET:
		defineViewMatrix(phi, theta, window_width, window_height, radius);
		displayGridSurface(window_width, window_height);
		break;
	default:
		break;
	}
}

// キー処理
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);


		// STLファイルの読み込み
	case 'r':
	case 'R':
		if (!loadSTLFile(STL_FILE))
			break;
		display_mode = DISPLAY_MODEL;
		glutPostRedisplay();
		break;
		// STLモデルの表示．
	case 'd':
	case 'D':
		display_mode = DISPLAY_MODEL;
		glutPostRedisplay();
		break;
		// オフセット面の生成．
	case 'c':
	case 'C':
		compOffset(window_width, window_height, radius);
		display_mode = DISPLAY_OFFSET;
		glutPostRedisplay();
		break;
		// オフセット面の表示．
	case 'o':
	case 'O':
		display_mode = DISPLAY_OFFSET;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{

	glutInitWindowPosition(INIT_X_POS, INIT_Y_POS);
	glutInitWindowSize(INIT_WIDTH, INIT_HEIGHT);
	glutInit(&argc, argv);            // glutライブラリの初期化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);   // 表示に用いるフレームバッファの設定 GLUT_RGBA:ビットごとの論理和 GLUT_DEPTH:デプスバッファ処理，全要素の値が1.0で初期化
	glutCreateWindow("Offset");        // OpenGLによる表示用ウインドウの生成 引数は名前
	glutDisplayFunc(display);         // 表示用コールバック関数の設定 
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
	initGL();
	glutMainLoop();                   // イベントの発生を待ち続けるループ関数の一種 ユーザの強制終了まで待機し続ける

	return 0;
}