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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dragon_tex_w, dragon_tex_h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, dragon_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Ocean
	glGenTextures(1, &oceanBindIndex);
	glBindTexture(GL_TEXTURE_2D, oceanBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ocean_tex_w, ocean_tex_h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, ocean_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Submarine
	glGenTextures(1, &submarineBindIndex);
	glBindTexture(GL_TEXTURE_2D, submarineBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sm_tex_w, sm_tex_h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, submarine_texels);
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sky_tex_w, sky_tex_h, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, sky_texels[i]);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	
	// Moon
	glGenTextures(1, &moonBindIndex);
	glBindTexture(GL_TEXTURE_2D, moonBindIndex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 2048);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moon_tex_w, moon_tex_h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, moon_texels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	

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


void loadBMP(const char* path, GLubyte texels[2048][2048][4], int* outW, int* outH)
{
    FILE* f = fopen(path, "rb");
    if (!f) { printf("cannot open %s\n", path); return; }

    unsigned char header[54];
    fread(header, 1, 54, f);

    int dataOffset = *(int*)&header[10];
    int w          = *(int*)&header[18];
    int h          = *(int*)&header[22];
    short bpp      = *(short*)&header[28];
    if (h < 0) h = -h;
    int ch = bpp / 8;

    int size = ch * w * h;
    unsigned char* data = new unsigned char[size];
    fseek(f, dataOffset, SEEK_SET); 
    fread(data, 1, size, f);
    fclose(f);

    int k = 0;
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            texels[j][i][0] = data[k*ch + 2];
            texels[j][i][1] = data[k*ch + 1]; 
            texels[j][i][2] = data[k*ch + 0];
			texels[j][i][3] = (ch==4) ? data[k*ch + 3] : 255;
            k++;
        }

    *outW = w;
    *outH = h;
    delete[] data;
}

// 텍스처 입혀 오브젝트 하나 그리기
// Dragon 안에는 여러 object들이 존재해서, 개별적으로 그릴 수 있는 툴.
void drawDragonObject(int g)
{
    int s = obj_groups[g].face_start;
    int c = obj_groups[g].face_count;
    glBegin(GL_TRIANGLES);
    for (int jj = s; jj < s + c; jj++){
        glTexCoord2f(vertex_color1[dragon[jj].T1-1].X, vertex_color1[dragon[jj].T1-1].Y);
        glVertex3f(vertex1[dragon[jj].V1-1].X, vertex1[dragon[jj].V1-1].Y, vertex1[dragon[jj].V1-1].Z);
        glTexCoord2f(vertex_color1[dragon[jj].T2-1].X, vertex_color1[dragon[jj].T2-1].Y);
        glVertex3f(vertex1[dragon[jj].V2-1].X, vertex1[dragon[jj].V2-1].Y, vertex1[dragon[jj].V2-1].Z);
        glTexCoord2f(vertex_color1[dragon[jj].T3-1].X, vertex_color1[dragon[jj].T3-1].Y);
        glVertex3f(vertex1[dragon[jj].V3-1].X, vertex1[dragon[jj].V3-1].Y, vertex1[dragon[jj].V3-1].Z);
    }
    glEnd();
}

void drawDragon(){
    glPushMatrix();
	glTranslatef(dragonX, dragonY, dragonZ); // 궤적 위치
    glRotatef(dragonYaw+YAW_OFFSET, 0, 1, 0);  // 진행방향 좌우
    glRotatef(dragonPitch+PITCH_OFFSET, 1, 0, 0); // 상하 기울기
	glScalef(1.8f, 1.8f, 1.8f);
    glTranslatef(-dragon_cx, -dragon_cy, -dragon_cz);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dragonBindIndex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);

    // 완전형 몸통 (index 3) — 그대로
    drawDragonObject(3);

    // left 날개 (idx : 6) 
	float piv6X = -0.35f, piv6Y = 0.2f, piv6Z =  0.12f;
	glPushMatrix();
		glTranslatef(piv6X, piv6Y, piv6Z);
		glRotatef(flapAngle, 0, 0, 1);
		glTranslatef(-1.089f, -0.022f, 0.219f);
		drawDragonObject(6);
	glPopMatrix();

	// right 날개 (idx : 10)
	float piv10X = -0.4f, piv10Y = 0.2f, piv10Z = 0.12f;
	glPushMatrix();
		glTranslatef(piv10X, piv10Y, piv10Z);
		glRotatef(-flapAngle, 0, 0, 1);
		glTranslatef(-1.0750f, -0.0316f, 0.2415f);
		drawDragonObject(10);
	glPopMatrix();

    glDisable(GL_TEXTURE_2D);   // ← 오타 주의: glDisable
    glPopMatrix();
}


void drawSubmarine(){
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
}

// Lightning의 경우, 25개의 object로 구성됐음. 
// 5개씩 group으로 나눠서 시간 별로 다른 번개들이 치게 구성.
void drawLightningGroup(int group)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(0.8f, 0.8f, 1.0f);

    float size = 0.4f;
	float Llength = 0.6f;

	for (int j = 0; j < 5; j++){
		int oi = group*5 + j;
		float px, py, pz;
		lightningPlacePos(group, j, &px, &py, &pz);

		glPushMatrix();
		glTranslatef(px,py,pz);
		glTranslatef(-(float)light_obj_cx[oi]*size, -(float)light_obj_cy[oi]*Llength, -(float)light_obj_cz[oi]*size);
		int start = light_obj_start[oi];
		int end = (oi+1 < light_obj_cnt) ? light_obj_start[oi+1] : light_tri_cnt;
		glBegin(GL_TRIANGLES);
		for (int f = start; f < end; f++){
			for (int k=0; k< 3; k++){
				int vi = light_tris[f].v[k] - 1;
				glVertex3f(vertex6[vi].X*size, vertex6[vi].Y*Llength, vertex6[vi].Z*size);
			}
		}
		glEnd();
		glPopMatrix();
	}
}


void lightningPlacePos(int group, int j, float *px, float *py, float *pz){
	float cX = 0.0f, cY = 1.2f, cZ = 0.0f;
	float ringR = 1.6f;
	float ang = (j*72.0f + group*24.0f) * 3.141592 / 180.0f;
	*px = cX + ringR * cosf(ang);
	*py = cY;
	*pz = cZ + ringR * sinf(ang);
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
	glTranslatef(t[0], t[1]-1, t[2]-1);
	glMultMatrixf(&m[0][0]);

	flapAngle += flapDir * 1.0f;
	if (flapAngle> 0.0f)
		flapDir = -1;
	if (flapAngle < -70.0f)
		flapDir = 1;

	float tsec = glutGet(GLUT_ELAPSED_TIME) * 0.001f;   // 초 단위

	float orbitspeed = 0.6f;
	float freq8 = 1.8f;    // 8자 주기성
	float Rorbit = 2.0f; 	// orbit의 반경
	float upNdownHeight = 0.8f;    // 상하 진폭
	float Bweave = 0.4f;    // topview에서의 떨림 폭
	float baseY  = 0.0f;    // updown의 기준

	float orbitAngle = tsec * orbitspeed;
	float s = tsec * freq8;

	// 원 궤적 중심
	float ox = Rorbit * cosf(orbitAngle);
	float oz = Rorbit * sinf(orbitAngle);

	float weave  = Bweave * sinf(s);
	float updown = upNdownHeight * sinf(2.0f * s);

	// x-z on the top-view, 약간의 떨림을 weave로 반영.
	// up down 은 그대로 반영.
	dragonX = ox + weave * cosf(orbitAngle);
	dragonZ = oz + weave * sinf(orbitAngle);
	dragonY = baseY + updown;

	// 약간의 시간 뒤의 위치를 아니, 그 방향대로
	float dt  = 0.05f;
	float oa2 = (tsec+dt)*orbitspeed;
	float s2  = (tsec+dt)*freq8;
	float ox2 = Rorbit*cosf(oa2), oz2 = Rorbit*sinf(oa2);
	float weave2 = Bweave*sinf(s2);
	float nx = ox2 + weave2*cosf(oa2);
	float nz = oz2 + weave2*sinf(oa2);
	float ny = baseY + upNdownHeight*sinf(2.0f*s2);

	float dx = nx - dragonX, dy = ny - dragonY, dz = nz - dragonZ;
	dragonYaw   = atan2f(dx, dz) * 180.0f / 3.141592f;        // 좌우 회전
	float horiz = sqrtf(dx*dx + dz*dz);
	dragonPitch = -atan2f(dy, horiz) * 180.0f / 3.141592f;    // 상하 회전
	
	drawDragon();
	
	// Lighting
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glPushMatrix();
	glTranslatef(0.0f, 2.5f, 0.0f);
	GLfloat lightpos[4] = { 0, 0, 0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glPopMatrix();

	GLfloat ambient0[4]  = { 0.15f, 0.15f, 0.20f, 1 };
	GLfloat diffuse0[4]  = { 0.9f,  0.9f,  1.0f,  1 };
	GLfloat specular0[4] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);


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
	
	float waterY = 0.0f;

	glPushMatrix();

	double clip[4] = {0.0, -1.0, 0.0, (double)waterY };
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, clip);

	glTranslatef(0, waterY, 0);
	glScalef(1.0f, -1.0f, 1.0f);
	glTranslatef(0,-waterY, 0);

	// mirrored dragon draw
	drawDragon();

	// mirrored submarine draw
	drawSubmarine();

	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	float cycle = 5.0f;
	int activeGroup = ((int)(tsec / cycle)) % 3;
	float phase = fmodf(tsec, cycle);
	bool flashing = (phase < 0.4f) && (fmodf(phase*25.0f, 2.0f) < 1.0f);

	float size = 0.15f;
	float Llength = 0.5f;

	if (flashing){
		for (int j = 0; j < 5; j++){
			int id = GL_LIGHT1 + j;
			glEnable(id);
			float px, py, pz;
			lightningPlacePos(activeGroup, j, &px, &py, &pz);
			GLfloat pos[4] = {px, py, pz, 1.0f};
			glLightfv(id, GL_POSITION, pos);
			GLfloat col[4] = {0.7f, 0.8f, 1.0f, 1.0f};
			glLightfv(id, GL_DIFFUSE, col);
			glLightfv(id, GL_SPECULAR, col);
			glLightf(id, GL_CONSTANT_ATTENUATION, 1.0f);
			glLightf(id, GL_LINEAR_ATTENUATION, 0.0f);
			glLightf(id, GL_QUADRATIC_ATTENUATION, 0.0f);

		}
	}
	else{
		for (int j=0; j<5; j++){
			glDisable(GL_LIGHT1 + j);
		}
	}

	glPushMatrix();
	glScalef(4.0f, 4.0f, 4.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat mat_ambient[4]  = { 0.10f, 0.15f, 0.25f, 1 };
	GLfloat mat_diffuse[4]  = { 0.8f, 0.8f, 0.8f, 0.6f };
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oceanBindIndex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	for (int jj = 0; jj < ocean_face_cnt; jj++) {
		glNormal3f(ocean_normal[ocean[jj].N1-1].X, ocean_normal[ocean[jj].N1-1].Y, ocean_normal[ocean[jj].N1-1].Z);
		glTexCoord2f(vertex_color2[ocean[jj].T1-1].X, vertex_color2[ocean[jj].T1-1].Y);
		glVertex3f(vertex2[ocean[jj].V1-1].X, vertex2[ocean[jj].V1-1].Y, vertex2[ocean[jj].V1-1].Z);

		glNormal3f(ocean_normal[ocean[jj].N2-1].X, ocean_normal[ocean[jj].N2-1].Y, ocean_normal[ocean[jj].N2-1].Z);
		glTexCoord2f(vertex_color2[ocean[jj].T2-1].X, vertex_color2[ocean[jj].T2-1].Y);
		glVertex3f(vertex2[ocean[jj].V2-1].X, vertex2[ocean[jj].V2-1].Y, vertex2[ocean[jj].V2-1].Z);

		glNormal3f(ocean_normal[ocean[jj].N3-1].X, ocean_normal[ocean[jj].N3-1].Y, ocean_normal[ocean[jj].N3-1].Z);
		glTexCoord2f(vertex_color2[ocean[jj].T3-1].X, vertex_color2[ocean[jj].T3-1].Y);
		glVertex3f(vertex2[ocean[jj].V3-1].X, vertex2[ocean[jj].V3-1].Y, vertex2[ocean[jj].V3-1].Z);

		glNormal3f(ocean_normal[ocean[jj].N4-1].X, ocean_normal[ocean[jj].N4-1].Y, ocean_normal[ocean[jj].N4-1].Z);
		glTexCoord2f(vertex_color2[ocean[jj].T4-1].X, vertex_color2[ocean[jj].T4-1].Y);
		glVertex3f(vertex2[ocean[jj].V4-1].X, vertex2[ocean[jj].V4-1].Y, vertex2[ocean[jj].V4-1].Z);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glPopMatrix();

	// real dragon and submarine draw
	drawDragon();
	drawSubmarine();

	if(flashing){
		drawLightningGroup(activeGroup);
	}

	glPushMatrix();
	glTranslatef(0.0f, -0.7f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(0.4f,0.4f,0.5f,1.0f);

	for (int i=0; i<4; i++){
		glBindTexture(GL_TEXTURE_2D, skyBindIndex[i]);
		int start = sky_band_start[i];
		int end = (i<3) ? sky_band_start[i+1] : sky_face_cnt;

		for (int jj = start; jj < end; jj++) {
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

	glDisable(GL_TEXTURE_2D);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glPopMatrix();

	glPushMatrix();
	glRotatef(180, 1, 0 ,0);

	glTranslatef(0.0f, -2.5f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, moonBindIndex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1,1,1);

	glBegin(GL_QUADS);
	for (int jj = 0; jj < moon_face_cnt; jj++) {
		glTexCoord2f(vertex_color5[moon[jj].T1-1].X, 1.0f - vertex_color5[moon[jj].T1-1].Y);
		glVertex3f(vertex5[moon[jj].V1-1].X, vertex5[moon[jj].V1-1].Y, vertex5[moon[jj].V1-1].Z);
		glTexCoord2f(vertex_color5[moon[jj].T2-1].X, 1.0f - vertex_color5[moon[jj].T2-1].Y);
		glVertex3f(vertex5[moon[jj].V2-1].X, vertex5[moon[jj].V2-1].Y, vertex5[moon[jj].V2-1].Z);
		glTexCoord2f(vertex_color5[moon[jj].T3-1].X, 1.0f - vertex_color5[moon[jj].T3-1].Y);
		glVertex3f(vertex5[moon[jj].V3-1].X, vertex5[moon[jj].V3-1].Y, vertex5[moon[jj].V3-1].Z);
		glTexCoord2f(vertex_color5[moon[jj].T4-1].X, 1.0f - vertex_color5[moon[jj].T4-1].Y);
		glVertex3f(vertex5[moon[jj].V4-1].X, vertex5[moon[jj].V4-1].Y, vertex5[moon[jj].V4-1].Z);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	





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

	ocean_normal = new Vertex[300000];

	// Submarine
	vertex3 = new Vertex[100000];
	vertex_color3 = new Vertex[100000];
	submarine = new MMesh[100000];

	// Sky
	vertex4 = new Vertex[100000];
	vertex_color4 = new Vertex[300000];
	sky = new MMesh[100000];

	// Moon
	vertex5 = new Vertex[100000];
	vertex_color5 = new Vertex[300000];
	moon = new MMesh[100000];

	// Lightning
	vertex6 = new Vertex[200000];
	light_tris = new LTri[300000];


	

	
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Dragon/All_pBR_textures/dragon.bmp", dragon_texels, &dragon_tex_w, &dragon_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Ocean/textures/ocean.bmp", ocean_texels, &ocean_tex_w, &ocean_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Submarine/tm_2k/submarine.bmp", submarine_texels, &sm_tex_w, &sm_tex_h);
	
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky1.bmp", sky_texels[0], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky2.bmp", sky_texels[1], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky3.bmp", sky_texels[2], &sky_tex_w, &sky_tex_h);
	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Sky/SkyTexture_PNG/Sky4.bmp", sky_texels[3], &sky_tex_w, &sky_tex_h);

	loadBMP("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Moon/moon_sq.bmp", moon_texels, &moon_tex_w, &moon_tex_h);
	// Lightning은 그냥 단색으로 칠할 예정.

	int cnt_vert = 0, cnt_color = 0, cnt_norm = 0, cnt_face = 0;
	FILE* fp;
	fp = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Dragon/Dragon.obj", "r");
	if (!fp){
		printf("cannot open the file : dragon.obj");
	}

	char line[512];

	float cx = 0, cy = 0, cz = 0;
	int nforc = 0;
	
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
				if (obj_group_cnt==4){
					cx += x / scale;
					cy += y / scale;
					cz += z / scale;
					nforc++;
				}
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
	dragon_cx = cx/nforc;
	dragon_cy = cy/nforc;
	dragon_cz = cz/nforc;

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
			cnt = sscanf(oline, "vn %f %f %f", &x, &y, &z);
			if (cnt==3){
				ocean_normal[ocean_norm].X = x;
				ocean_normal[ocean_norm].Y = y;
				ocean_normal[ocean_norm].Z = z;
				ocean_norm++;
			}
			
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

				ocean[ocean_face].N1 = n1;
				ocean[ocean_face].N2 = n2;
				ocean[ocean_face].N3 = n3;
				ocean[ocean_face].N4 = n4;

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
	int sky_band = 0;
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
			sky_band_start[sky_band] = sky_face;
			sky_band++;
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


	// Moon Load
	int moon_vert = 0, moon_color = 0, moon_norm = 0, moon_face = 0;
	FILE* fmoon;
	fmoon = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Moon/Moon.obj", "r");
	if (!fmoon){
		printf("cannot open the file : Moon.obj");
	}

	char mline[512];
	while(fgets(mline, sizeof(mline), fmoon)){
		float x,y,z;
		float u,v;
		int cnt = 0;

		if(mline[0]=='v' && mline[1]==' '){
			cnt = sscanf(mline, "v %f %f %f", &x, &y, &z);
			if(cnt == 3){
				vertex5[moon_vert].X = x/2;
				vertex5[moon_vert].Y = y/2;
				vertex5[moon_vert].Z = z/2;
				moon_vert++;
			}
		}
		else if (mline[0]=='v'&&mline[1]=='t'){
			cnt = sscanf(mline, "vt %f %f", &u, &v);
			if(cnt == 2){
				vertex_color5[moon_color].X = u;
				vertex_color5[moon_color].Y = v;
				moon_color++;
			}
		}
		else if(mline[0]=='v' && mline[1]=='n'){
			moon_norm++;
		}
		else if(mline[0]=='f' && mline[1]==' '){
			int v1, t1, n1,
				v2, t2, n2,
				v3, t3, n3,
				v4, t4, n4;
			cnt = sscanf(mline, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
			if(cnt==12){
				moon[moon_face].V1 = v1;
				moon[moon_face].V2 = v2;
				moon[moon_face].V3 = v3;
				moon[moon_face].V4 = v4;

				moon[moon_face].T1 = t1;
				moon[moon_face].T2 = t2;
				moon[moon_face].T3 = t3;
				moon[moon_face].T4 = t4;

				moon_face++;

			}
		}
		

	}
	
	fclose(fmoon);

	moon_face_cnt = moon_face;
	printf("v=%d  vt=%d  vn=%d  f=%d\n", moon_vert, moon_color, moon_norm, moon_face);


	/*
		Lightning의 경우,
		f 2545/179/93 2546/180/94 2547/181/95 2548/182/96 2549/183/97 2550/184/98 2551/185/99 2552/186/100 2553/187/101 2554/188/102
		위와 같이, 10개의 vertex로 구성된 poly도 있어서, 모두 삼각형으로 subdivision 수행했습니다.
	*/
	int lightning_vertex = 0, lightning_tri = 0;
	FILE *fl = fopen("/Users/w/Desktop/Computer_Grahphics/Code/glcode5/HW/Lightning/Lightning.obj","r");
	if (!fl){
		printf("cannot open the file : Lightning.obj");
	}

	char lline[1024];
	while (fgets(lline, sizeof(lline), fl)){
		if (lline[0]=='v' && lline[1]==' '){
			float x,y,z;
			int cnt = sscanf(lline, "v %f %f %f", &x, &y, &z);
			if (cnt==3){
				vertex6[lightning_vertex].X = x;
				vertex6[lightning_vertex].Y = y;
				vertex6[lightning_vertex].Z = z;
				lightning_vertex++;
			}
		}
		else if (lline[0] == 'o' && lline[1]== ' '){
			light_obj_start[light_obj_cnt] = lightning_tri;
			light_obj_cnt++;
		}
		else if (lline[0]=='f' && lline[1]==' '){
			int idx[16];
			int cnt = 0;
			char *tok = strtok(lline + 2, " \n\r");
			while (tok && cnt < 16){
				int vi = 0;
				// 어짜피 지금 상황에서는 vn,vt가 필요없으니, v만 받아옴.
				sscanf(tok, "%d", &vi);
				idx[cnt] = vi;
				cnt++;
				tok = strtok(NULL, " \n\r");
			}
			for (int k=1; k < cnt -1; k++){
				// fan
				light_tris[lightning_tri].v[0] = idx[0];
				light_tris[lightning_tri].v[1] = idx[k];
				light_tris[lightning_tri].v[2] = idx[k+1];
				lightning_tri++;
			}
		}
	}
	fclose(fl);
	light_tri_cnt = lightning_tri;
	printf("lightning_vertex=%d  lightning_tri=%d  lightning_objs=%d\n", lightning_vertex, lightning_tri, light_obj_cnt);

	for (int oi = 0; oi < light_obj_cnt; oi++){
		int start = light_obj_start[oi];
		int end = (oi+1 < light_obj_cnt) ? light_obj_start[oi+1] : light_tri_cnt;
		double cx=0, cy=0, cz=0;
		long n=0;
		for (int f = start; f < end; f++){
			for (int k=0; k < 3; k++){
				int vi = light_tris[f].v[k] - 1;
				cx += vertex6[vi].X;
				cy += vertex6[vi].Y;
				cz += vertex6[vi].Z;
				n++;
			}
		}
		light_obj_cx[oi] = cx/n;
		light_obj_cy[oi] = cy/n;
		light_obj_cz[oi] = cz/n;
	}

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

	delete[] vertex5;
	delete[] moon;
	delete[] vertex_color5;

	return 0;
}