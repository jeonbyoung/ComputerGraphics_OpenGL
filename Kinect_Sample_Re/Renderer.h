#pragma once

#include <math.h>
#include <iostream>
#include <fstream>
#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <GLUT/glut.h>
typedef bool BOOLEAN;
typedef void* HANDLE;
inline void CloseHandle(HANDLE) {}
inline void glutLeaveMainLoop() { exit(0); }
inline void glutSetOption(int, int) {}
#define glutCloseFunc glutWMCloseFunc
#define GLUT_ACTION_ON_WINDOW_CLOSE 0
#define GLUT_ACTION_GLUTMAINLOOP_RETURNS 0
#else
#include "gl\freeglut.h"		// OpenGL header files
#endif
#include <list>
//#define TIME_CHECK_
#define DEPTH_CALIB_
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
#define BLOCK 4
#define READ_SIZE 110404//5060//110404
#define scale 20

#define MAX_OBJECTS 20
//#define TEST 14989
//#define STATIC_

using namespace std;

struct ObjectGroup {
	int face_start; // start idx of each object
	int face_count; // # of faces
	char name[128]; 	// name of object
};

ObjectGroup obj_groups[MAX_OBJECTS];
int obj_group_cnt = 0;
int current_obj = 0; // idx of current obj

struct Vertex{
	float X;
	float Y;
	float Z;
	int index_1;
	int index_2;
	int index_3;
};

struct MMesh {
	int V1;
	int V2;
	int V3;
	int V4;
	int T1;
	int T2;
	int T3;
	int T4;
	int N1;
	int N2;
	int N3;
	int N4;
};

// variables for GUI
const float TRACKBALLSIZE = 0.8f;
const int RENORMCOUNT = 97;
//const int width = KinectBasic::nColorWidth;
//const int height = KinectBasic::nColorHeight;

GLint drag_state = 0;
GLint button_state = 0;

GLint rot_x = 0;
GLint rot_y = 0;
GLint trans_x = 0;
GLint trans_y = 0;
GLint trans_z = 0;


// Dragon
Vertex *vertex1;
Vertex *vertex_color1;
MMesh *dragon;

GLubyte dragon_texels[2048][2048][4];
GLuint dragonBindIndex = 0;
int dragon_tex_w = 0, dragon_tex_h = 0;
int dragon_face_cnt = 0;

// Ocean
Vertex *vertex2;
Vertex *vertex_color2;
MMesh *ocean;

Vertex *ocean_normal;

GLubyte ocean_texels[2048][2048][4];
GLuint oceanBindIndex = 1;
int ocean_tex_w = 0, ocean_tex_h = 0;
int ocean_face_cnt = 0;

// Submarine
Vertex *vertex3;
Vertex *vertex_color3;
MMesh *submarine;

GLubyte submarine_texels[2048][2048][4];
GLuint submarineBindIndex = 2;
int sm_tex_w = 0, sm_tex_h = 0;
int sm_face_cnt = 0;

// Sky
Vertex *vertex4;
Vertex *vertex_color4;
MMesh *sky;

GLubyte sky_texels[4][2048][2048][4];
GLuint skyBindIndex[4];
int sky_tex_w = 0, sky_tex_h = 0;
int sky_face_cnt = 0;
int sky_band_start[4] = {0};


// Moon
Vertex *vertex5;
Vertex *vertex_color5;
MMesh *moon;

GLubyte moon_texels[2048][2048][4];
GLuint moonBindIndex = 4;
int moon_tex_w = 0, moon_tex_h = 0;
int moon_face_cnt = 0;

int add_depth_flag = 0;
int model_flag = 0;
int depth_display_flag = 0;
int geodesic_skel[23][5] = { 0 };
int trcon = 0;
float zmin = 100000, zmax = -100000;

int side_status[50] = { 0 };

float quat[4] = {0};
float t[3] = {0};




Vertex skt[23];
BOOLEAN bTracked = false;
bool checkt = false;
Vertex *vertex;
Vertex *vertex_color;
MMesh *mymesh;


bool recheck;

// variables for display OpenGL based point viewer
int dispWindowIndex = 0;
GLuint dispBindIndex = 0;
const float dispPointSize = 2.0f;

// variables for display text
string dispString = "";
const string dispStringInit = "Depth Threshold: D\nInfrared Threshold: I\nNonlocal Means Filter: N\nPick BodyIndex: P\nAccumulate Mode: A\nSelect Mode: C,B(select)\nSave: S\nReset View: R\nQuit: ESC";
string frameRate;

HANDLE hMutex;
//KinectBasic kinect;

// functions for GUIs
void InitializeWindow(int argc, char* argv[]);

// high-level functions for GUI
void draw_center();
void idle();
void display();
void close();
void special(int, int, int) {}
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void reshape(int, int);
void motion(int, int);

void loadBMP(const char* path, GLubyte texels[2048][2048][4], int* outW, int* outH);
void drawDragon();
void drawSubmarine();

// basic functions for computation/GUI
// trackball codes were imported from those of Gavin Bell
// which appeared in SIGGRAPH '88
void vzero(float*);
void vset(float*, float, float, float);
void vsub(const float*, const float*, float*);
void vcopy(const float*, float*);
void vcross(const float *v1, const float *v2, float *cross);
float vlength(const float *v);
void vscale(float *v, float div);
void vnormal(float *v);
float vdot(const float *v1, const float *v2);
void vadd(const float *src1, const float *src2, float *dst);

void trackball(float q[4], float, float, float, float);
//void add_quats(float*, float*, float*);
void axis_to_quat(float a[3], float phi, float q[4]);
void normalize_quat(float q[4]);
float tb_project_to_sphere(float, float, float);
void build_rotmatrix(float m[4][4], float q[4]);
void Reader();
void DrawObj();
void DrawMeshObj();
void Setskt();

//CameraSpacePoint m_SpacePoint[JointType::JointType_Count];
void Track();
