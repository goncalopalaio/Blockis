#include <iostream>
#include <time.h>
#include "quick_ui.h"

using namespace std;

// Input state

    bool left_pressed = false;
    bool right_pressed = false;
    bool down_pressed = false;
    

// Current piece state
    float x_current_piece = 150;
    float y_current_piece = 150;

    float speed_h_current_piece = 0.0;
    float speed_v_current_piece = 0.0;

	float acc_h_current_piece_default = 0.2;
    float acc_v_current_piece_default = 1.1;

    float acc_h_current_piece = 0.0;
    float acc_v_current_piece = 0.0;


void qui_close_callback(GLFWwindow* window) {
    printf("close_callback\n");
}

void qui_size_callback(GLFWwindow * window, int width, int height) {
    printf("size_callback\n");
}

void qui_cursorpos_callback(GLFWwindow * window, double mx, double my) {}

void qui_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {glfwSetWindowShouldClose(window, 1); return;}

    if (key == GLFW_KEY_DOWN) {
		down_pressed = (action != GLFW_RELEASE);    	
    }

	if (key == GLFW_KEY_LEFT) {
		left_pressed = (action != GLFW_RELEASE);
    }    

	if (key == GLFW_KEY_RIGHT) {
		right_pressed = (action != GLFW_RELEASE);
    }
}

void qui_mousebutton_callback(GLFWwindow * window, int button, int action, int mods) {}

void qui_char_callback(GLFWwindow * window, unsigned int key) {
    printf("char_callback\n");
}

void qui_error_callback(int error, const char* description) {
    printf("%s\n", description);
}

void start_clock(clock_t* c) {
    *c = clock();
}

double stop_clock(clock_t* start) {
     clock_t end = clock();
     return ((double) (end - *start)) / CLOCKS_PER_SEC;
}

int main(int argc, char** argv) {  

    clock_t timer;
    double render_time = 0;
 
    int WIDTH = 400;
    int HEIGHT = 800;
    
    unsigned int* texture_buffer = (unsigned int*) malloc(WIDTH * HEIGHT * sizeof(unsigned int));

    QuiScreen* screen = qui_init("Blockis",
                     WIDTH,
                     HEIGHT,
                     qui_close_callback,
                     qui_size_callback,
                     qui_key_callback,
                     qui_mousebutton_callback,
                     qui_char_callback,
                     qui_cursorpos_callback,
                     qui_error_callback
                     );

    char debug_string [200];
    float timestep = 0;

    float tick_time_elapsed = 0;
    float tick_time_total = 0.3;
    float tick_time_transition = 0.1;

    bool  tick_is_transitioning = true;
    bool  tick_is_new = false;

    while (!glfwWindowShouldClose(screen->window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        start_clock(&timer);

		if (tick_time_elapsed >= tick_time_total) {
        	tick_is_new = true;
        	tick_is_transitioning = false;
        	tick_time_elapsed = 0;
        } else if (tick_time_elapsed >= (tick_time_total - tick_time_transition)) {
        	tick_is_transitioning = true;
        } else {
        	tick_is_transitioning = false;
			tick_is_new = false;
        }

        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            texture_buffer[i] = QUI_RGB(60, 60, 60);
        }

        qui_render_rect(WIDTH,
                          HEIGHT,
                          texture_buffer,
                          x_current_piece,
                          y_current_piece,
                          40,
                          40,
                          QUI_RGB(100, 100, 40));

        qui_render_texture(screen, texture_buffer);

        render_time = stop_clock(&timer) * 1000;
		sprintf (debug_string, "-> %f ms keys: %d %d %d tick: %.3f (total %.1f) %d %d \n", render_time, left_pressed, down_pressed, right_pressed, tick_time_elapsed, tick_time_total, tick_is_new, tick_is_transitioning);
        qui_render_string(20, 20, debug_string);
    
        qui_update(screen);

        // Update current piece values

        if (!tick_is_transitioning) {
        	y_current_piece += 1.0f;
        }

        if (left_pressed || right_pressed) {
        	acc_h_current_piece += acc_h_current_piece_default;
        	speed_h_current_piece += acc_h_current_piece;
        } else {
        	acc_h_current_piece = 0;
        	speed_h_current_piece = 0;
        }

        if (left_pressed) {
        	x_current_piece -= speed_h_current_piece;
        }

        if (right_pressed) {
        	x_current_piece += speed_h_current_piece;
        }

        if (down_pressed) {
        	y_current_piece += 1.0f;
        }

    	//left_pressed = false;
    	//right_pressed = false;
    	//down_pressed = false;
    
    	// Time stuff
    	timestep += 0.01;
        tick_time_elapsed += 0.01;
    }

    qui_clean(screen);
    free(texture_buffer);
}