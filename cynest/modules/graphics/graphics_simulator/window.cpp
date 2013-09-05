#include "window.h"

using namespace std;

/*
int width = 1200;
int height = 800;
bool going = true;

bool button0_mouse = false;
float theta = 0.0;
float phi = 0.0;
float theta_before = 0.0;
float phi_before = 0.0;
float mouse_x_before = 0.0;
float mouse_y_before = 0.0;
float sens = 0.25;
float dist = 20.0;

SDL_TimerID timer(0);
Uint32 evolve(Uint32, void*);

*/


void Window::init(int width, int height, char* caption) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	surface = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_DOUBLEBUF );
	atexit(SDL_Quit);
		
	SDL_WM_SetCaption(caption, 0);

	glEnable(GL_DEPTH_TEST);    
		
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_BLEND) ;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
			
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// perspective
	gluPerspective(65.0, float(width)/float(height), 1.0, 1000.0);
	 
	// background
	glClearColor(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);
}

/*

bool events(){

SDL_Event event;   
SDL_PollEvent(&event); 
	
Uint8* keys = SDL_GetKeyState(NULL);
Uint8 mouse = SDL_GetMouseState(NULL,NULL);
if(event.type == SDL_QUIT || keys[SDLK_ESCAPE] == SDL_PRESSED){return false;}

if((&mouse)[0] == 1){if(button0_mouse==false){mouse_x_before=event.motion.x;mouse_y_before=event.motion.y;theta_before=theta;phi_before=phi;} button0_mouse=true;}
else{button0_mouse=false;}
if(event.type == SDL_MOUSEMOTION){
	if(button0_mouse == true){
		phi=phi_before - (mouse_y_before - event.motion.y)*sens;
		theta=theta_before - (mouse_x_before - event.motion.x)*sens;
		if(phi>90.0){phi=90.0;}
		if(phi<-90.0){phi=-90.0;}
	}
}



if(event.type == SDL_MOUSEBUTTONDOWN){
	if(event.button.button == SDL_BUTTON_WHEELUP){if(dist>2.0){dist-=1.0;}else{dist=2.0;}}
	if(event.button.button == SDL_BUTTON_WHEELDOWN)  {dist+=1.0;}
}
if (event.type == SDL_KEYDOWN){
    switch(event.key.keysym.sym){ 
    case SDLK_UP: 
	break;
    case SDLK_DOWN: 
	break;
    case SDLK_RIGHT:
    break;
    case SDLK_LEFT:
    break;
    case SDLK_PAGEUP:
    break;
    case SDLK_PAGEDOWN:
    break;
    case SDLK_HOME:
    break;
    case SDLK_LCTRL:
    break;
break;
	}
}
return true;	
};


*/
