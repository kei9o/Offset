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

// ウィンドウの縦横サイズ
unsigned int window_width, window_height;

// 球や円筒形を多面体近似する際の分割数．
#define DIV 64

// 表示モード
#define DISPLAY_MODEL 0
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

// 表示
void display(void)
{
	switch (display_mode) {
	case DISPLAY_MODEL:
		defineViewMatrix(phi, theta, window_width, window_height, radius);
		displayModel();
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
	default:
		break;
	}
}

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

int main(int argc, char* argv[])
{
	glutInitWindowPosition(INIT_X_POS, INIT_Y_POS);
	glutInitWindowSize(INIT_WIDTH, INIT_HEIGHT);
	glutInit(&argc, argv);            // glutライブラリの初期化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);   // 表示に用いるフレームバッファの設定 GLUT_RGBA:ビットごとの論理和 GLUT_DEPTH:デプスバッファ処理，全要素の値が1.0で初期化
	glutCreateWindow("Offset");        // OpenGLによる表示用ウインドウの生成 引数は名前
	glutDisplayFunc(display);         // 表示用コールバック関数の設定 
	glutKeyboardFunc(keyboard);
	/*glutReshapeFunc(resize);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);*/
	initGL();
	glutMainLoop();                   // イベントの発生を待ち続けるループ関数の一種 ユーザの強制終了まで待機し続ける

	return 0;
}