#include "my_core_libs.hpp"

PSP_MODULE_INFO("Engine Main", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

bool Debug = false;
int running = 1;

//Controles
PSP_CONTROLLER pad;

int main(){ 
	setup_callbacks();
	//configura o controle
	sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	//Inicia a GPU
	initGu();
	//init_perspective_2D();
	//init_perspective_3D();
	init_perspective_2D_Especial();

	CameraData Cam = Create_camera(Vec3(0, 0, 1), Vec3(0, 1, 0));

	while(running){
		//update
		readController(&pad, 1);
		calc_delta();
		if(pad.Buttons != 0){
			if(pad.Buttons & PSP_CTRL_UP){
				Cam = ProcessGamePad(Cam, UP, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_DOWN){
				Cam = ProcessGamePad(Cam, DOWN, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_LEFT){
				Cam = ProcessGamePad(Cam, LEFT, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_RIGHT){
				Cam = ProcessGamePad(Cam, RIGHT, delta_time, 5.0f);
			}
		}
		//render
		startFrame(); 
		clear(GU_RGBA(255, 255, 255, 255));
		should_blend(true);
        sceGuEnable(GU_DEPTH_TEST);

		//Code goes Here
		SendCamData(Cam);

		reset_translate(0, 0, 1.0f);
        scale(2, 2, 1);
        draw_square2D(1, 1, 0xFF0000FF);

		reset_translate(10, 0, 1.0f);
        scale(2, 2, 1);
        draw_square2D(1, 1, 0xFF0000FF);

		draw_square(0, 0, 10, 10, 0xFF00FFFF);
		
		//reset_translate(1, 0, 1.0f);
		//scale(1000, 1000, 1000);
		//draw_Circle3D(3, 0xFF0000FF);

		endFrame();
	}
	endGu();
	sceKernelExitGame();
	return 0;
}
