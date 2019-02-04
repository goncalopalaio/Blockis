#ifndef INCLUDE_QUICK_UI_H
#define INCLUDE_QUICK_UI_H

#include <GLFW/glfw3.h>
#include "stb_easy_font.h"


#ifndef QUI_MIN
#define QUI_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef QUI_MAX
#define QUI_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef QUI_ABS
#define QUI_ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef QUI_CLAMP
#define QUI_CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif


#define QUI_RGB(r, g, b) (((unsigned int)b) << 16) | (((unsigned int)g) << 8) | r

typedef struct QuiScreen {
	GLFWwindow* window;
	GLuint texture;
	int width;
	int height;
}QuiScreen;

void qui_render_string(float x, float y, char *text) { //, float r, float g, float b) {
	static char buffer[99999]; // ~500 chars
	int num_quads;

	num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

	//glColor3f(r,g,b);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 16, buffer);
	glDrawArrays(GL_QUADS, 0, num_quads*4);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void qui_render_rect(int w, int h, unsigned int* texture, int cx, int cy, int side_w, int side_h, unsigned int color) {
	int sx = QUI_MAX(cx - side_w, 0);
	int sy = QUI_MAX(cy - side_h, 0);
	int ex = QUI_MIN(cx + side_w, w - 1);
	int ey = QUI_MIN(cy + side_h, h - 1);

	for (int x = sx; x < ex; ++x) {
		for (int y = sy; y < ey; ++y) {
			texture[x + y * w] = color;
		}
	}
}

void qui_render_square(int w, int h, unsigned int* texture, int cx, int cy, int side_w, unsigned int color) {
	qui_render_rect(w, h, texture, cx, cy, side_w, side_w, color);
}

QuiScreen* qui_init(const char * window_title,
					 int window_w,
					 int window_h,
					 GLFWwindowclosefun qui_close_callback,
					 GLFWwindowsizefun  qui_size_callback,
					 GLFWkeyfun qui_key_callback,
					 GLFWmousebuttonfun qui_mousebutton_callback,
					 GLFWcharfun qui_char_callback,
					 GLFWcursorposfun qui_cursorpos_callback,
					 GLFWerrorfun qui_error_callback
					 ) {

	qui_error_callback(0, "qui_open\n");

	if (!glfwInit()) {
		qui_error_callback(1, "error in glfwInit\n");
		return NULL;
	}

	qui_error_callback(0, "qui_open init\n");

	GLFWwindow* window = glfwCreateWindow(window_w, window_h, window_title, NULL, NULL);
	
	if (!window) {
		qui_error_callback(1, "error creating window\n");		
		return NULL;
	}

	qui_error_callback(0, "qui_open window\n");

	glfwSetWindowCloseCallback(window, qui_close_callback);
    glfwSetWindowSizeCallback(window, qui_size_callback);
    glfwSetKeyCallback(window, qui_key_callback);
    glfwSetMouseButtonCallback(window, qui_mousebutton_callback);
    glfwSetCharCallback(window, qui_char_callback);
    glfwSetCursorPosCallback(window, qui_cursorpos_callback);
    glfwSetErrorCallback(qui_error_callback);

    qui_error_callback(0, "qui_open context\n");
    glfwMakeContextCurrent(window);

    qui_error_callback(0, "qui_open drawing\n");

  	// setup matrices
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, window_w, window_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    

    GLuint tex;
    glGenTextures(1, &tex);

    QuiScreen* screen = (QuiScreen*) malloc(sizeof(QuiScreen));
    screen->window = window;
    screen->texture = tex;
    screen->width = window_w;
    screen->height = window_h;

    return screen;
}

void qui_render_texture(QuiScreen* screen, unsigned int* texture_buffer) {
	glBindTexture(GL_TEXTURE_2D, screen->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen->width, screen->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, screen->texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(10, 10);
		glTexCoord2i(0, 1); glVertex2i(10, screen->height - 10);
		glTexCoord2i(1, 1); glVertex2i(screen->width - 10, screen->height - 10);
		glTexCoord2i(1, 0); glVertex2i(screen->width - 10, 10);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers

}


void qui_update(QuiScreen* screen) {
	glfwSwapBuffers(screen->window);
    glfwPollEvents();
}

void qui_clean(QuiScreen* screen) {
	glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(screen->window);
}


#endif