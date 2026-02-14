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
	while(running){
		//update
		readController(&pad, 1);
		if(pad.Buttons != 0){
			
		}
		//render
		startFrame(); 
		clear(GU_RGBA(0, 0, 0, 255));
		endFrame();
	}
	endGu();
	sceKernelExitGame();
	return 0;
}
