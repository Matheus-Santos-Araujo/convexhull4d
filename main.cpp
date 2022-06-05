#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include<GL/freeglut.h>
#include<chrono>
#include<thread>
#include "convexhull.h"

int width = 800, height = 800;

float resolutionx = width * 1.0f;
float resolutiony = height * 1.0f;
float fovx = 2.0f / width;
float fovy = 2.0f / height;
bool tint, fourd = false;
int t = 0;
int T = 8;

Hull obj;
vector<Hull> objs, hulls;

void printhull(const Hull& o) {

	// Point cloud
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (const Point* v : o.vertices) {
		glColor3f(0.8f, 0.1f, 0.1f);
		glVertex3f(v->x, v->y, v->z);
	}
	glEnd();

	// Faces
	glLineWidth(1.0f);
	for (const Face& t : o.faces) {

		if (tint) {
			glColor3f(0.8, 0.1, 0.1);
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < 3; i++) {
				glVertex3f(t.p[i]->x, t.p[i]->y, t.p[i]->z);
			}
			glEnd();
		}
		Point n = t.normal();
		n.norm();
		glColor3f(0.1f, 0.1f, 0.1f);
		glBegin(GL_LINES);
		for (int i = 0; i < 3; i++) {
			glVertex3f(t.p[i]->x, t.p[i]->y, t.p[i]->z);
			glVertex3f(t.p[(i + 1) % 3]->x, t.p[(i + 1) % 3]->y, t.p[(i + 1) % 3]->z);

			n.x *= -1; n.y *= -1; n.z *= -1;
			glVertex3f(t.p[i]->x, t.p[i]->y, t.p[i]->z);
			glVertex3f(t.p[(i + 1) % 3]->x, t.p[(i + 1) % 3]->y, t.p[(i + 1) % 3]->z);
		}
		glEnd();
	}
}

void loader(string name) {

	string tag;
	int num_objs = -1;
	int a, b, c;
	int A, B, C;
	int i, j, k;
	double x, y, z;

	std::ifstream in(name, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << name << std::endl;
		exit(1);

	}
	std::string line;
	while (std::getline(in, line))
	{

		if (line.substr(0, 2) == "o ") {
			num_objs++;
			objs.push_back(Hull());
		}

		else if (line.substr(0, 2) == "v ") {
			std::istringstream v(line.substr(2));
			v >> x; v >> y; v >> z;
			Point* p = new Point{ x, y, z };

			obj.add_vertex(p);
			objs[num_objs].add_vertex(p);
		}
	}
}

void disp(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, resolutionx, resolutiony);
		int largura = fovx * resolutionx, altura = fovy * resolutiony;
		glFrustum(-largura / 2, largura / 2, -altura / 2, altura / 2, 1, 100);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		gluLookAt(
			5, 2, -2,
			0, 2, 0,
			0, 1, 0);

		glPointSize(10.0f);
		glColor3f(0, 0, 1);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glColor3f(1, 1, 1);
		//look at
		glBegin(GL_POINTS);
		glVertex3f(0, 0, 0); // view
		glEnd();
	}

	if (fourd == true && t < T) {
		    t += 1;
		    objs.clear();
			obj.clear();
			hulls.clear();
			std::stringstream ss;
			ss << "mywind" << t << ".obj";
			string frame = ss.str();
			cout << frame << "\n";
			loader(frame);
			hulls = convexhull(objs);

			this_thread::sleep_for(chrono::milliseconds(3000));
	}
	
		for (const Hull& o : hulls) {
			printhull(o);
		}

	glutSwapBuffers();

}

void keyboard(unsigned char key, int /*x*/, int /*y*/) {

	switch (key) {

	case(27): exit(0);
	case(' '):
		break;
	case('o'):
		fourd = false;
		objs.clear();
		// Descomentar modelo .obj desejado
		//loader("exemp.obj");
		//loader("mywind1.obj");
		loader("cube.obj");
		hulls = convexhull(objs);
		break;
	case('p'):
		tint = true;
		break;
	case('a'):
		tint = false;
		break;
	// Tema 4D
	case('t'):
		fourd = true;
		break;
	}
	glutPostRedisplay();
}

void tick(int m) {

	//Redraw the display
	glutPostRedisplay();

	/*recall timer func in 33 milliseconds
	redraw every 33 milliseconds which is
	approximatly=1000/33=30 frames per seconds
	*/
	glutTimerFunc(33, tick, 0);

}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Fecho Convexo");

	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, resolutionx, resolutiony);
	int largura = fovx * resolutionx, altura = fovy * resolutiony;
	glFrustum(-largura / 2, largura / 2, altura / 2, -altura / 2, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glutDisplayFunc(disp);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(33, tick, 0);
	glutMainLoop();

	cin.get();
	cin.get();
	system("pause");
	return 0;
}