#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>

#define GL_PI 4.0f*atan(1.0f)

GLfloat xRot, yRot;

char *lpszAppName = "glDemo";

void IdleFunction() {
	xRot += 0.1f;
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

}

void RenderScene(void) {
	GLfloat x, y, z, angle;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);
	// Save matrix state and do the rotation...
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	z = -50.0f;
	for (angle=0.0f; angle <= (2.0f*GL_PI)*3.0f; angle += 0.1f) {
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		glVertex3f(x, y, z);
		z += 0.5f;
	}
	glEnd();
	glPopMatrix();
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
