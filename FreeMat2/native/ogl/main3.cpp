#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>

#define GL_PI 4.0f*atan(1.0f)

enum { 
    PAN = 1,				/* pan state bit */
    ROTATE,				/* rotate state bits */
    ZOOM				/* zoom state bit */
};

int state;
int mx, my, omx, omy;
GLfloat xRot, yRot;
GLfloat trans[3];			/* current translation */
GLfloat rot[2];				/* current rotation */


static void update(int state, int ox, int nx, int oy, int ny)
{
    int dx = ox - nx;
    int dy = ny - oy;

    switch(state) {
    case PAN:
	trans[0] -= dx / 100.0f;
	trans[1] -= dy / 100.0f;
	break;
    case ROTATE:
#if 0
	rot[0] += (dy * 180.0f) / 500.0f;
	rot[1] -= (dx * 180.0f) / 500.0f;
#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x
	clamp(rot[0]);
	clamp(rot[1]);
#endif
    glMatrixMode(GL_MODELVIEW);
	glRotatef((dy * 180.0f) / 500.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-(dx * 180.0f) / 500.0f, 0.0f, 1.0f, 0.0f);
	break;
    case ZOOM:
	trans[2] -= (dx+dy) / 100.0f;
	break;
    }
//	glTranslatef(trans[0],trans[1],trans[2]);
}


char *lpszAppName = "glDemo";

void IdleFunction() {
	xRot += 0.0f;
	yRot += 0.1f;
}

void ChangeSize(GLsizei w, GLsizei h) 
{ 
	GLfloat nRange = 100.0f;

	if (h == 0)
		h = 1;
 
    glViewport( 0, 0, w, h ); 
 
//    aspect = (GLfloat) width / height; 
 
    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity(); 
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
	else
		glOrtho(-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);
    glMatrixMode( GL_MODELVIEW ); 
	glLoadIdentity();
}     
 
void SetupRC(void) {
	// Set background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Use flat shading
	glShadeModel(GL_FLAT);
	glFrontFace(GL_CW);
}

void RenderScene(void) {
	GLfloat x, y, z, angle;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColor3f(1.0f, 0.0f, 0.0f);
	// Save matrix state and do the rotation...
//	glPushMatrix();

//	glBegin(GL_POINTS);
//	z = -50.0f;
//	for (angle=0.0f; angle <= (2.0f*GL_PI)*3.0f; angle += 0.1f) {
//		x = 50.0f*sin(angle);
//		y = 50.0f*cos(angle);
//		glVertex3f(x, y, z);
//		z += 0.5f;
//	}

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 75.0f);
	int iPivot = 0;
	for (angle=0.0f; angle < (2.0f*GL_PI); angle += (GL_PI/8.0f)) {
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		if ((iPivot % 2) == 0)
			glColor3f(0.0f,1.0f,0.0f);
		else
			glColor3f(1.0f,0.0f,0.0f);
		iPivot++;
		glVertex2f(x, y);
	}

	glVertex2f(0.0f, 0.0f);
	iPivot = 0;
	for (angle=0.0f; angle < (2.0f*GL_PI); angle += (GL_PI/8.0f)) {
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		if ((iPivot % 2) == 0)
			glColor3f(0.0f,1.0f,0.0f);
		else
			glColor3f(1.0f,0.0f,0.0f);
		iPivot++;
		glVertex2f(x, y);
	}

	glEnd();
//	glPopMatrix();
//	glFlush();
}

void SetDCPixelFormat(HDC hDC) {
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0,0,0,0,0,0,
			0,0,
			0,0,0,0,0,
			16,
			0,0,0,0,
			0,0,0};

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam) {
	static HGLRC hRC = NULL; // Permanent Rendering Context
	static HDC hDC = NULL;   // Private GDI device context

	switch(message) {
	case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
	    /* if we don't set the capture we won't get mouse move
		messages when the mouse moves outside the window. */
		SetCapture(hWnd);
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (message == WM_LBUTTONDOWN)
			state |= PAN;
		if (message == WM_RBUTTONDOWN)
			state |= ROTATE;
		break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
		/* remember to release the capture when we are finished. */
		ReleaseCapture();
		state = 0;
		break;
    case WM_MOUSEMOVE:
		if (state) {
			omx = mx;
			omy = my;
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			/* Win32 is pretty braindead about the x, y position that
			it returns when the mouse is off the left or top edge
			of the window (due to them being unsigned). therefore,
			roll the Win32's 0..2^16 pointer co-ord range to the
			more amenable (and useful) 0..+/-2^15. */
			if(mx & 1 << 15) mx -= (1 << 16);
			if(my & 1 << 15) my -= (1 << 16);
			update(state, omx, mx, omy, my);
			PostMessage(hWnd, WM_PAINT, 0, 0);
		}
		break;
	case WM_CREATE:
		// Get the device context
		hDC = GetDC(hWnd);
		SetDCPixelFormat(hDC);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC,hRC);
		SetupRC();
		xRot = 0;
		yRot = 0;
		SetTimer(hWnd,33,1,NULL);
		break;
	case WM_DESTROY:
		KillTimer(hWnd,101);
		wglMakeCurrent(hDC,NULL);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_TIMER:
		IdleFunction();
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		RenderScene();
		SwapBuffers(hDC);
		ValidateRect(hWnd,NULL);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0L;
}

int APIENTRY WinMain( HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
						LPSTR lpCmdLine,
						int nCmdShow) {
	MSG msg; // Windows message structure
	WNDCLASS wc;
	HWND hWnd;

	// Register window style
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No background brush
	wc.hbrBackground = NULL;
	// No menu
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszAppName;
	if (RegisterClass(&wc) == 0)
		return FALSE;

	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		100, 100,
		250, 250,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hWnd == NULL)
		return FALSE;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
