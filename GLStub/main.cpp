#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

void display();
void mouseButton(int button, int state, int x, int y);
void keyPress(unsigned char key, int x, int y);
void mouseMovement(int x, int y);
void dispCamera();
void drawAxis();
void reshape(int w, int h);
void enable();
void drawLand();
void selectPoint(int xx, int yy);

float lastx, lasty, xpos = 0, ypos = 0, zpos = 0, xrot = 45, yrot = 0, angle = 0.0;
int isPressed = 0;

float cRadius = 80;

float frametime = 0; //frametime

void calculateFrameRate() //calculate framerate
{
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	frametime = currentTime - lastTime;
	lastTime = currentTime;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("GL VS Stub");
	enable(); //Enabler

	glutDisplayFunc(display); //display and update caller
	glutReshapeFunc(reshape); //reshape function

	glutMouseFunc(mouseButton); //mouse press
	glutKeyboardFunc(keyPress); //key press
	glutMotionFunc(mouseMovement); //mouse motion

	glutMainLoop();

	return(0);
}

// GL enable functions
void enable(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_3D);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
}

//Draw a point in 3D space
void drawpoint()
{
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslated(10.0, 0.0, 10.0);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}

//Screen reshape
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

//The camera position and rotation functions
void dispCamera()
{
	glTranslatef(0.0f, 0.0f, -cRadius); //The distance is the cRadius.
	glRotatef(xrot, 1.0, 0.0, 0.0); //The X rotation

	glRotatef(yrot, 0.0, 1.0, 0.0); //The Y rotation
	glTranslated(-xpos, 0.0f, -zpos); //The x and z center position of the camera
}

// Draw the axis in the center of the 3D space
void drawAxis()
{
	glColor3f(1.0, 0.0, 0.0); //x red
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);
	glEnd();

	glColor3f(0.0, 1.0, 0.0); //y green
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);
	glEnd();

	glColor3f(0.0, 0.0, 1.0); //z blue
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 5.0);
	glEnd();
}

// Draw a square for the basic flat terrain
void drawLand()
{
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-20, 0, -20);
	glVertex3f(-20, 0, 20);
	glVertex3f(20, 0, 20);
	glVertex3f(20, 0, -20);
	glEnd();
}

void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	calculateFrameRate();

	dispCamera(); //camera must always be first display oriented function called

	drawAxis(); //draw our axis

	drawLand(); //draw land

	drawpoint(); //draw a random object

	glutPostRedisplay();
	glutSwapBuffers();
}

// Mouse Movement if wil move the camera
void mouseMovement(int x, int y)
{
	if (isPressed == 1)
	{
		int diffx = x - lastx;
		int diffy = y - lasty;
		lastx = x;
		lasty = y;
		xrot += (float)diffy;
		yrot += (float)diffx;
	}
}

// Pressing of mouse buttons
void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON) //Right button will enable the rotation of the screen
	{
		if (state == GLUT_DOWN)
		{
			isPressed = 1;
			lastx = x;
			lasty = y;
		}
		else
		{
			isPressed = 0;
		}
	}

	if (button == GLUT_LEFT_BUTTON) //Left button will select a location in 3D space and print the result to the console
	{
		if (state == GLUT_DOWN)
		{
			selectPoint(x, y);
		}
	}
}

void keyPress(unsigned char key, int x, int y)
{
	if (key == 'd')
	{
		printf("Hello, this key 'd' has been pressed..\n");
	}

	if (key == 'z') //Zoom In
	{
		cRadius -= 1;
	}

	if (key == 'x') //Zoom Out
	{
		cRadius += 1;
	}
}

// Select a Point in 3D space
void selectPoint(int xx, int yy)
{
	GLdouble tempx, tempy, tempz;
	unsigned char res[4];
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;

	//Get the scene matrices
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = xx;
	winY = (float)viewport[3] - yy;
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	//read in the point of the mouse selection
	GLdouble x, y, z;
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &x, &y, &z);
	printf("x: %f\ny: %f\nz: %f\n", x, y, z);
}