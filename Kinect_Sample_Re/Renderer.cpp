#include "Renderer.h"

void draw_center(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); /* R */
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); /* G */
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); /* B */
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.0f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
}

void idle() {
	static GLuint previousClock = glutGet(GLUT_ELAPSED_TIME);
	static GLuint currentClock = glutGet(GLUT_ELAPSED_TIME);
	static GLfloat deltaT;

	currentClock = glutGet(GLUT_ELAPSED_TIME);
	deltaT = currentClock - previousClock;
	if (deltaT < 1000.0 / 20.0) { return; }
	else { previousClock = currentClock; }

	//char buff[256];
	//sprintf_s(buff, "Frame Rate = %f", 1000.0 / deltaT);
	//frameRate = buff;

	glutPostRedisplay();
}

void close()
{
	glDeleteTextures(1, &dispBindIndex);
	glutLeaveMainLoop();
	CloseHandle(hMutex);
}

void add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count = 0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1, t1);
	vscale(t1, q2[3]);

	vcopy(q2, t2);
	vscale(t2, q1[3]);

	vcross(q2, q1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = q1[3] * q2[3] - vdot(q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(58, (double)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	GLfloat spin_quat[4];
	float gain;
	gain = 2.0; /* trackball gain */

	if (drag_state == GLUT_DOWN)
	{
		if (button_state == GLUT_LEFT_BUTTON)
		{
			trackball(spin_quat,
				(gain * rot_x - 500) / 500,
				(500 - gain * rot_y) / 500,
				(gain * x - 500) / 500,
				(500 - gain * y) / 500);
			add_quats(spin_quat, quat, quat);
		}
		else if (button_state == GLUT_RIGHT_BUTTON)
		{
			t[0] -= (((float)trans_x - x) / 500);
			t[1] += (((float)trans_y - y) / 500);
		}
		else if (button_state == GLUT_MIDDLE_BUTTON)
			t[2] -= (((float)trans_z - y) / 500 * 4);
		else if (button_state == 3 || button_state == 4) // scroll
		{

		}
		//glutPostRedisplay();
	}

	rot_x = x;
	rot_y = y;

	trans_x = x;
	trans_y = y;
	trans_z = y;
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rot_x = x;
			rot_y = y;

			//t[0] = t[0] + 1;


		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			trans_x = x;
			trans_y = y;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			//trcon = trcon + 1;
			trans_z = y;
		}
		else if (button == 3 || button == 4)
		{
			const float sign = (static_cast<float>(button)-3.5f) * 2.0f;
			t[2] -= sign * 500 * 0.00015f;
		}
	}

	drag_state = state;
	button_state = button;
}

void vzero(float* v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void vset(float* v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void vsub(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void vcopy(const float *v1, float *v2)
{
	register int i;
	for (i = 0; i < 3; i++)
		v2[i] = v1[i];
}

void vcross(const float *v1, const float *v2, float *cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float vlength(const float *v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vscale(float *v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void vnormal(float *v)
{
	vscale(v, 1.0f / vlength(v));
}

float vdot(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vadd(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		/* Zero rotation */
		vzero(q);
		q[3] = 1.0;
		return;
	}

	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */
	vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
	vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

	/*
	 *  Now, we want the cross product of P1 and P2
	 */
	vcross(p2, p1, a);

	/*
	 *  Figure out how much to rotate around that axis.
	 */
	vsub(p1, p2, d);
	t = vlength(d) / (2.0f*TRACKBALLSIZE);

	/*
	 * Avoid problems with out-of-control values...
	 */
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = 2.0f * asin(t);

	axis_to_quat(a, phi, q);
}

void axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal(a);
	vcopy(a, q);
	vscale(q, sin(phi / 2.0f));
	q[3] = cos(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
		z = sqrt(r*r - d*d);
	}
	else {           /* On hyperbola */
		t = r / 1.41421356237309504880f;
		z = t*t / d;
	}
	return z;
}

void normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	for (i = 0; i < 4; i++) q[i] /= mag;
}

void build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void InitializeWindow(int argc, char* argv[])
{
	// initialize glut settings
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000 / 2, 1000 / 2);

	glutInitWindowPosition(0, 0);

	dispWindowIndex = glutCreateWindow("3D Model");

	trackball(quat, 90.0, 0.0, 0.0, 0.0);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutCloseFunc(close);
	//GLuint image = load   ("./my_texture.bmp");
	
	//glBindTexture(1,)

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// bind textures
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	// Dragon
	glGenTextures(1, &dragonBindIndex);
	glBindTexture(GL_TEXTURE_2D, dragonBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dragon_tex_w, dragon_tex_h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, dragon_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Ocean
	glGenTextures(1, &oceanBindIndex);
	glBindTexture(GL_TEXTURE_2D, oceanBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ocean_tex_w, ocean_tex_h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, ocean_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Dragon
	glGenTextures(1, &submarineBindIndex);
	glBindTexture(GL_TEXTURE_2D, submarineBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sm_tex_w, sm_tex_h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, submarine_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// Sky
	glGenTextures(4, skyBindIndex);
	for(int i = 0; i < 4; i++){
		glBindTexture(GL_TEXTURE_2D, skyBindIndex[i]);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sky_tex_w, sky_tex_h, 0,
					GL_RGB, GL_UNSIGNED_BYTE, sky_texels[i]);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	

	reshape(1000, 1000);

	/*glGenTextures(1, &dispBindIndex);
	glBindTexture(GL_TEXTURE_2D, dispBindIndex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}

void keyboard(unsigned char key, int x, int y){
	printf("KEY PRESSED: %c (%d)\n", key, key);
	fflush(stdout);
	if (key == 'n' || key == 'N'){
		current_obj = (current_obj + 1)% obj_group_cnt;
		printf("Object %d: %s (faces: %d)\n",
				current_obj,
				obj_groups[current_obj].name,
				obj_groups[current_obj].face_count);
		glutPostRedisplay();
	}
	else if (key == 'p' || key == 'P'){
		current_obj = (current_obj - 1 + obj_group_cnt) %obj_group_cnt;
		printf("Object %d: %s (faces: %d)\n",
				current_obj,
				obj_groups[current_obj].name,
				obj_groups[current_obj].face_count);
		glutPostRedisplay();
	}

}


void loadBMP(const char* path, GLubyte texels[2048][2048][3], int* outW, int* outH)
{
    FILE* f = fopen(path, "rb");
    if (!f) { printf("cannot open %s\n", path); return; }

    unsigned char header[54];
    fread(header, 1, 54, f);

    int dataOffset = *(int*)&header[10];   // 픽셀 시작 위치
    int w          = *(int*)&header[18];
    int h          = *(int*)&header[22];
    short bpp      = *(short*)&header[28];  // 24 or 32
    if (h < 0) h = -h;                       // top-down 대응
    int ch = bpp / 8;                        // 채널 수 (3 or 4)

    int size = ch * w * h;
    unsigned char* data = new unsigned char[size];
    fseek(f, dataOffset, SEEK_SET);          // 헤더 건너뛰고 픽셀로 점프
    fread(data, 1, size, f);
    fclose(f);

    int k = 0;
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            texels[j][i][0] = data[k*ch + 2];   // BMP는 BGR 순서 → R
            texels[j][i][1] = data[k*ch + 1];   // G
            texels[j][i][2] = data[k*ch + 0];   // B
            k++;                                  // 32bit면 알파(+3)는 무시
        }

    *outW = w;
    *outH = h;
    delete[] data;
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 0.1, 200);

	GLfloat m[4][4],m1[4][4];
	build_rotmatrix(m, quat);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 2.0, 2.0, 0, 0, 0, 0, 1.0, 0); 
	glTranslatef(t[0], t[1], t[2]+1);
	glMultMatrixf(&m[0][0]);

	glBegin(GL_LINES);
		glColor3f(1.0,0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);

		glColor3f(0,1.0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);

		glColor3f(0,0,1.0);
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
	glEnd();



	glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(0, 0.3f, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, dragonBindIndex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1,1,1);

	int start = obj_groups[3].face_start;
	int cnt = obj_groups[3].face_count;

	glBegin(GL_TRIANGLES);

	for (int jj = start; jj < start + cnt; jj++){
		glTexCoord2f(vertex_color1[dragon[jj].T1-1].X, vertex_color1[dragon[jj].T1-1].Y);
		glVertex3f(vertex1[dragon[jj].V1-1].X, vertex1[dragon[jj].V1-1].Y, vertex1[dragon[jj].V1-1].Z);

		glTexCoord2f(vertex_color1[dragon[jj].T2-1].X, vertex_color1[dragon[jj].T2-1].Y);
		glVertex3f(vertex1[dragon[jj].V2-1].X, vertex1[dragon[jj].V2-1].Y, vertex1[dragon[jj].V2-1].Z);

		glTexCoord2f(vertex_color1[dragon[jj].T3-1].X, vertex_color1[dragon[jj].T3-1].Y);
		glVertex3f(vertex1[dragon[jj].V3-1].X, vertex1[dragon[jj].V3-1].Y, vertex1[dragon[jj].V3-1].Z);
	}
	
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oceanBindIndex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1,1,1);
	glScalef(4.0f, 4.0f, 4.0f);

	glBegin(GL_QUADS);
	for (int jj = 0; jj < ocean_face_cnt; jj++) {
		glTexCoord2f(vertex_color2[ocean[jj].T1-1].X, vertex_color2[ocean[jj].T1-1].Y);
		glVertex3f(vertex2[ocean[jj].V1-1].X, vertex2[ocean[jj].V1-1].Y, vertex2[ocean[jj].V1-1].Z);

		glTexCoord2f(vertex_color2[ocean[jj].T2-1].X, vertex_color2[ocean[jj].T2-1].Y);
		glVertex3f(vertex2[ocean[jj].V2-1].X, vertex2[ocean[jj].V2-1].Y, vertex2[ocean[jj].V2-1].Z);

		glTexCoord2f(vertex_color2[ocean[jj].T3-1].X, vertex_color2[ocean[jj].T3-1].Y);
		glVertex3f(vertex2[ocean[jj].V3-1].X, vertex2[ocean[jj].V3-1].Y, vertex2[ocean[jj].V3-1].Z);

		glTexCoord2f(vertex_color2[ocean[jj].T4-1].X, vertex_color2[ocean[jj].T4-1].Y);
		glVertex3f(vertex2[ocean[jj].V4-1].X, vertex2[ocean[jj].V4-1].Y, vertex2[ocean[jj].V4-1].Z);
	}
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.4f, 0.0f, 0.4f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, submarineBindIndex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1,1,1);

	glBegin(GL_QUADS);
	for (int jj = 0; jj < sm_face_cnt; jj++) {
		glTexCoord2f(vertex_color3[submarine[jj].T1-1].X, 1.0f - vertex_color3[submarine[jj].T1-1].Y);
		glVertex3f(vertex3[submarine[jj].V1-1].X, vertex3[submarine[jj].V1-1].Y, vertex3[submarine[jj].V1-1].Z);

		glTexCoord2f(vertex_color3[submarine[jj].T2-1].X, 1.0f - vertex_color3[submarine[jj].T2-1].Y);
		glVertex3f(vertex3[submarine[jj].V2-1].X, vertex3[submarine[jj].V2-1].Y, vertex3[submarine[jj].V2-1].Z);

		glTexCoord2f(vertex_color3[submarine[jj].T3-1].X, 1.0f - vertex_color3[submarine[jj].T3-1].Y);
		glVertex3f(vertex3[submarine[jj].V3-1].X, vertex3[submarine[jj].V3-1].Y, vertex3[submarine[jj].V3-1].Z);

		glTexCoord2f(vertex_color3[submarine[jj].T4-1].X, 1.0f - vertex_color3[submarine[jj].T4-1].Y);
		glVertex3f(vertex3[submarine[jj].V4-1].X, vertex3[submarine[jj].V4-1].Y, vertex3[submarine[jj].V4-1].Z);
	}
	glEnd();

	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, -0.7f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1,1,1);

	for (int i=0; i<4; i++){
		glBindTexture(GL_TEXTURE_2D, skyBindIndex[i]);
		int start = sky_band_start[i];
		int end = (i<3) ? sky_band_start[i+1] : sky_face_cnt;

		for (int jj = 0; jj < sm_face_cnt; jj++) {
			if (sky[jj].V4 == 0) glBegin(GL_TRIANGLES);
			else				 glBegin(GL_QUADS);

			glTexCoord2f(vertex_color4[sky[jj].T1-1].X, 1.0f - vertex_color4[sky[jj].T1-1].Y);
			glVertex3f(vertex4[sky[jj].V1-1].X, vertex4[sky[jj].V1-1].Y, vertex4[sky[jj].V1-1].Z);

			glTexCoord2f(vertex_color4[sky[jj].T2-1].X, 1.0f - vertex_color4[sky[jj].T2-1].Y);
			glVertex3f(vertex4[sky[jj].V2-1].X, vertex4[sky[jj].V2-1].Y, vertex4[sky[jj].V2-1].Z);

			glTexCoord2f(vertex_color4[sky[jj].T3-1].X, 1.0f - vertex_color4[sky[jj].T3-1].Y);
			glVertex3f(vertex4[sky[jj].V3-1].X, vertex4[sky[jj].V3-1].Y, vertex4[sky[jj].V3-1].Z);

			if (sky[jj].V4 != 0){
				glTexCoord2f(vertex_color4[sky[jj].T4-1].X, 1.0f - vertex_color4[sky[jj].T4-1].Y);
				glVertex3f(vertex4[sky[jj].V4-1].X, vertex4[sky[jj].V4-1].Y, vertex4[sky[jj].V4-1].Z);
			}

			glEnd();
		}

	}

	glPopMatrix();


	glDisable(GL_TEXTURE_2D);



	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	vertex = new Vertex[100000];
	vertex_color = new Vertex[100000];
	mymesh = new MMesh[100000];

	// Dragon
	vertex1 = new Vertex[100000];
	vertex_color1 = new Vertex[100000];
	dragon = new MMesh[100000];

	// Ocean
	vertex2 = new Vertex[100000];
	vertex_color2 = new Vertex[300000];
	ocean = new MMesh[100000];

	// Submarine
	vertex3 = new Vertex[100000];
	vertex_color3 = new Vertex[100000];
	submarine = new MMesh[100000];

	// Sky
	vertex4 = new Vertex[100000];
	vertex_color4 = new Vertex[300000];
	sky = new MMesh[100000];

	

	
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Dragon/All_pBR_textures/dragon.bmp", dragon_texels, &dragon_tex_w, &dragon_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Ocean/textures/ocean.bmp", ocean_texels, &ocean_tex_w, &ocean_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Submarine/tm_2k/submarine.bmp", submarine_texels, &sm_tex_w, &sm_tex_h);
	
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky1.bmp", sky_texels[0], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky2.bmp", sky_texels[1], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky3.bmp", sky_texels[2], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky4.bmp", sky_texels[3], &sky_tex_w, &sky_tex_h);

	int cnt_vert = 0, cnt_color = 0, cnt_norm = 0, cnt_face = 0;
	FILE* fp;
	fp = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Dragon/Dragon.obj", "r");
	if (!fp){
		printf("cannot open the file : dragon.obj");
	}

	char line[512];
	while(fgets(line, sizeof(line), fp)){
		float x,y,z;
		float u,v;
		int cnt = 0;

		if(line[0]=='v' && line[1]==' '){
			cnt = sscanf(line, "v %f %f %f", &x, &y, &z);
			if(cnt == 3){
				vertex1[cnt_vert].X = x / scale;
				vertex1[cnt_vert].Y = y / scale;
				vertex1[cnt_vert].Z = z / scale;
				cnt_vert++;
			}
		}
		else if (line[0]=='v'&&line[1]=='t'){
			cnt = sscanf(line, "vt %f %f", &u, &v);
			if(cnt == 2){
				vertex_color1[cnt_color].X = u;
				vertex_color1[cnt_color].Y = v;
				cnt_color++;
			}
		}
		else if(line[0]=='v' && line[1]=='n'){
			cnt_norm++;
		}
		else if(line[0]=='o' && line[1]==' '){
			int idx = obj_group_cnt;
			obj_groups[idx].face_start = cnt_face;
			obj_groups[idx].face_count = 0;
			sscanf(line, "o %127s", obj_groups[idx].name);
			obj_group_cnt++;
		}
		else if(line[0]=='f' && line[1]==' '){
			int v1, t1, n1,
				v2, t2, n2,
				v3, t3, n3;
			cnt = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3);
			if(cnt==9){
				dragon[cnt_face].V1 = v1;
				dragon[cnt_face].V2 = v2;
				dragon[cnt_face].V3 = v3;

				dragon[cnt_face].T1 = t1;
				dragon[cnt_face].T2 = t2;
				dragon[cnt_face].T3 = t3;

				cnt_face++;
				if (obj_group_cnt > 0){
					obj_groups[obj_group_cnt-1].face_count++;
				}

			}
		}

	}

	fclose(fp);

	printf("v=%d  vt=%d  vn=%d  f=%d\n", cnt_vert, cnt_color, cnt_norm, cnt_face);


	// Ocean Load
	int ocean_vert = 0, ocean_color = 0, ocean_norm =0, ocean_face = 0;
	FILE* fo;
	fo = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Ocean/Ocean.obj", "r");
	if (!fo){
		printf("cannot open the file : ocean.obj");
	}

	char oline[512];
	while(fgets(oline, sizeof(oline), fo)){
		float x,y,z;
		float u,v;
		int cnt = 0;

		if(oline[0]=='v' && oline[1]==' '){
			cnt = sscanf(oline, "v %f %f %f", &x, &y, &z);
			if(cnt == 3){
				vertex2[ocean_vert].X = x;
				vertex2[ocean_vert].Y = y;
				vertex2[ocean_vert].Z = z;
				ocean_vert++;
			}
		}
		else if (oline[0]=='v'&&oline[1]=='t'){
			cnt = sscanf(oline, "vt %f %f", &u, &v);
			if(cnt == 2){
				vertex_color2[ocean_color].X = u;
				vertex_color2[ocean_color].Y = v;
				ocean_color++;
			}
		}
		else if(oline[0]=='v' && oline[1]=='n'){
			ocean_norm++;
		}
		else if(oline[0]=='f' && oline[1]==' '){
			int v1, t1, n1,
				v2, t2, n2,
				v3, t3, n3,
				v4, t4, n4;
			cnt = sscanf(oline, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
			if(cnt==12){
				ocean[ocean_face].V1 = v1;
				ocean[ocean_face].V2 = v2;
				ocean[ocean_face].V3 = v3;
				ocean[ocean_face].V4 = v4;

				ocean[ocean_face].T1 = t1;
				ocean[ocean_face].T2 = t2;
				ocean[ocean_face].T3 = t3;
				ocean[ocean_face].T4 = t4;

				ocean_face++;

			}
		}
		

	}
	
	fclose(fo);

	ocean_face_cnt = ocean_face;
	printf("v=%d  vt=%d  vn=%d  f=%d\n", ocean_vert, ocean_color, ocean_norm, ocean_face);


	// Submarine Load
	int sm_vert = 0, sm_color = 0, sm_norm = 0, sm_face = 0;
	FILE* fs;
	fs = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Submarine/submarine1.obj", "r");
	if (!fs){
		printf("cannot open the file : submarine1.obj");
	}

	char sline[512];
	while(fgets(sline, sizeof(sline), fs)){
		float x,y,z;
		float u,v;
		int cnt = 0;

		if(sline[0]=='v' && sline[1]==' '){
			cnt = sscanf(sline, "v %f %f %f", &x, &y, &z);
			if(cnt == 3){
				vertex3[sm_vert].X = x/1000;
				vertex3[sm_vert].Y = y/1000;
				vertex3[sm_vert].Z = z/1000;
				sm_vert++;
			}
		}
		else if (sline[0]=='v'&&sline[1]=='t'){
			cnt = sscanf(sline, "vt %f %f", &u, &v);
			if(cnt == 2){
				vertex_color3[sm_color].X = u;
				vertex_color3[sm_color].Y = v;
				sm_color++;
			}
		}
		else if(sline[0]=='v' && sline[1]=='n'){
			sm_norm++;
		}
		else if(sline[0]=='f' && sline[1]==' '){
			int v1, t1, n1,
				v2, t2, n2,
				v3, t3, n3,
				v4, t4, n4;
			cnt = sscanf(sline, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
			if(cnt==12){
				submarine[sm_face].V1 = v1;
				submarine[sm_face].V2 = v2;
				submarine[sm_face].V3 = v3;
				submarine[sm_face].V4 = v4;

				submarine[sm_face].T1 = t1;
				submarine[sm_face].T2 = t2;
				submarine[sm_face].T3 = t3;
				submarine[sm_face].T4 = t4;

				sm_face++;

			}
		}
		

	}
	
	fclose(fs);

	sm_face_cnt = sm_face;
	printf("v=%d  vt=%d  vn=%d  f=%d\n", sm_vert, sm_color, sm_norm, sm_face);



	// Sky Load
	int sky_vert = 0, sky_color = 0, sky_norm = 0, sky_face = 0;
	int sky_band = 1;
	FILE* fsky;
	fsky = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/Sky.obj", "r");
	if (!fsky){
		printf("cannot open the file : Sky.obj");
	}

	char skyline[512];
	while(fgets(skyline, sizeof(skyline), fsky)){
		float x,y,z;
		float u,v;
		int cnt = 0;

		if(skyline[0]=='v' && skyline[1]==' '){
			cnt = sscanf(skyline, "v %f %f %f", &x, &y, &z);
			if(cnt == 3){
				vertex4[sky_vert].X = x;
				vertex4[sky_vert].Y = y;
				vertex4[sky_vert].Z = z;
				sky_vert++;
			}
		}
		else if (skyline[0]=='v'&&skyline[1]=='t'){
			cnt = sscanf(skyline, "vt %f %f", &u, &v);
			if(cnt == 2){
				vertex_color4[sky_color].X = u;
				vertex_color4[sky_color].Y = v;
				sky_color++;
			}
		}
		else if (strncmp(skyline, "usemtl",6)==0){
			sky_band++;
			sky_band_start[sky_band] = sky_face;
		}
		else if(skyline[0]=='v' && skyline[1]=='n'){
			sky_norm++;
		}
		else if(skyline[0]=='f' && skyline[1]==' '){
			int v1, t1, n1,
				v2, t2, n2,
				v3, t3, n3,
				v4, t4, n4;
			cnt = sscanf(skyline, "f %d/%d %d/%d %d/%d %d/%d",&v1,&t1,&v2,&t2,&v3,&t3,&v4,&t4);
			if(cnt==6){
				sky[sky_face].V1 = v1;
				sky[sky_face].V2 = v2;
				sky[sky_face].V3 = v3;
				sky[sky_face].V4 = 0;

				sky[sky_face].T1 = t1;
				sky[sky_face].T2 = t2;
				sky[sky_face].T3 = t3;
				sky[sky_face].T4 = 0;

				sky_face++;

			}
			
			if(cnt==8){
				sky[sky_face].V1 = v1;
				sky[sky_face].V2 = v2;
				sky[sky_face].V3 = v3;
				sky[sky_face].V4 = v4;

				sky[sky_face].T1 = t1;
				sky[sky_face].T2 = t2;
				sky[sky_face].T3 = t3;
				sky[sky_face].T4 = t4;

				sky_face++;

			}
		}
		

	}
	
	fclose(fsky);

	sky_face_cnt = sky_face;
	printf("v=%d  vt=%d  vn=%d  f=%d\n", sky_vert, sky_color, sky_norm, sky_face);

	/*
	for(int i = 0; i < obj_group_cnt; i++){
		printf("obj[%d]: %s  start=%d  count=%d\n",
			i, obj_groups[i].name,
			obj_groups[i].face_start,
			obj_groups[i].face_count);
	}
	printf("\n");
	*/

	InitializeWindow(argc, argv);

	display();

	glutMainLoop();
	delete[] vertex;
	delete[] mymesh;
	delete[] vertex_color;

	delete[] vertex1;
	delete[] dragon;
	delete[] vertex_color1;

	delete[] vertex2;
	delete[] ocean;
	delete[] vertex_color2;

	delete[] vertex3;
	delete[] submarine;
	delete[] vertex_color3;

	delete[] vertex4;
	delete[] sky;
	delete[] vertex_color4;

	return 0;
}