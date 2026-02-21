#include "my_core_libs.hpp"

PSP_MODULE_INFO("Engine Main", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

//Physics
#define QNT_PARTICLE 20
#define MAX_FORCES 20

ForceRegistration registy[MAX_FORCES];
Particle particulas[QNT_PARTICLE];

bool Debug = false;
int running = 1;
//---------------------

//Controles
PSP_CONTROLLER pad;

void renderParticle(Particle particle, unsigned int cor){
	reset_translate(particle.position.x, particle.position.y, particle.position.z);
	draw_square2D(cor);
}

void renderParticles(Particle* particles, int qnt, unsigned int cor){
	for(int i = 0; i < qnt; i++){
		renderParticle(particles[i], cor);
	}
}


int main(){ 
	/***Start the Callback Functions****/
	setup_callbacks();
	/****Setup the controller****/
	sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	/****starts the GPU****/
	initGu();
	/****starts the perspective*****/
	//init_perspective_2D();
	init_perspective_3D();
	//init_perspective_2D_Especial();
	/**Create the Cam**/
	CameraData Cam = Create_camera(Vec3(0, 0, 70), Vec3(0, 1, 0));
	bool shouldRender = false;
	bool puxou = false;
	Texture* font_text = load_texture("resources/UI/Fonte.png", GU_TRUE, 0);
	Font* main_font = create_font(font_text, 16, 16, 16, 8);
	/*ForceGenerator forceApp = Create_GravityGenerator(vec4{0, -9.81, 0, 0}); /*{ vec4{0, -9.81, 0, 0}, 
							 FG_GRAVITY};*/

	
	/*for(int i = 0; i < QNT_PARTICLE; i++){
		int x = (rand() % 40) - 5; // [0, 10]
		int y = (rand() % 40) - 5; // [0, 10];
		particulas[i] = Create_Particle(vec4{float(x), float(y), 0, 0}, 0.5f, 0.99f, vec4{0, 0, -15.0f, 0}, vec4{0, 0, 1, 0});
	}*/

	/*for(int i = 0; i < MAX_FORCES; i++){
		registy[i].p  = &particulas[i];
		registy[i].fg = &forceApp;
	}*/
	particulas[0] = Create_Particle(vec4{0, 10, 0, 0}, 0, 0.99f, vec4{0,0,0,0}, vec4{0,0,0,0});
	//particulas[2] = Create_Particle(vec4{2, 10, 0, -5}, 0, 0.99f, vec4{0,0,0,0}, vec4{0,0,0,0});
	particulas[1] = Create_Particle(vec4{0, 0, 0, 0}, 1, 0.90f, vec4{0,0,0,0}, vec4{0,0,0,0});
	//force Generators
	ForceGenerator springGen = Create_BuoyancyGenerator(2, 1, 0);//Create_SpringGenerator(&particulas[0].position, 15.0f, 10.0f);
	//ForceGenerator springGen2 = Create_SpringGenerator(&particulas[2].position, 15.0f, 10.0f);
	ForceGenerator gravityGen = Create_GravityGenerator(vec4{0, -9.81, 0, 0});

	//ponto fixo
	registy[0].p  = &particulas[0];
	registy[0].fg = &gravityGen;
	registy[1].p  = &particulas[0];
	registy[1].fg = &springGen;
	//movel
	registy[2].p  = &particulas[1];
	registy[2].fg = &gravityGen;
	registy[3].p  = &particulas[1];
	registy[3].fg = &springGen;
	//
	//registy[4].p  = &particulas[1];
	//registy[4].fg = &springGen2;

	//for(int i = 0; i < 2; i++){
	//	registy[i].p  = &particulas[i];
		//registy[i].fg = &forceApp;
	//}
	/***The main loop***/
	while(running){
		//update
		readController(&pad, 1);
		calc_delta();
		if(pad.Buttons != 0){
			if(pad.Buttons & PSP_CTRL_UP){
				particulas[0].position += vec4{0, 5.0f * delta_time, 0, 0};
				//Cam = ProcessGamePad(Cam, UP, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_DOWN){
				particulas[0].position += vec4{0, -5.0f * delta_time, 0, 0};
				//Cam = ProcessGamePad(Cam, DOWN, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_LEFT){
				particulas[0].position += vec4{-5.0f * delta_time, 0, 0, 0};
				//Cam = ProcessGamePad(Cam, LEFT, delta_time, 5.0f);
			}
			if(pad.Buttons & PSP_CTRL_RIGHT){
				particulas[0].position += vec4{5.0f * delta_time, 0, 0, 0};
				//Cam = ProcessGamePad(Cam, RIGHT, delta_time, 5.0f);
			}
			//------------------------------------------------------
			if(pad.Buttons & PSP_CTRL_TRIANGLE){
				
			}
			if(pad.Buttons & PSP_CTRL_CROSS){
				if(!shouldRender){
					shouldRender = true;
					particulas[1].position += vec4{-0.5f, 0, 0, 0};
				}
			}
			if(pad.Buttons & PSP_CTRL_SQUARE){
				if(!puxou){
					puxou = true;
					particulas[1].position += vec4{0, -0.5f, 0, 0};
				}
			}
			if(pad.Buttons & PSP_CTRL_CIRCLE){
				//Cam = ProcessGamePad(Cam, BACKWARD, delta_time, 5.0f);
			}
		}
		//render
		startFrame(); 
		clear(GU_RGBA(125, 125, 125, 255));
		//Render Goes Down Here
		should_blend(true);
        sceGuEnable(GU_DEPTH_TEST);
		//Code goes Here
		//Cam.Position.y = Get_Position(obj).y;
		SendCamData(Cam);
		//ponto inicial
		reset_translate(0, 0, 0);
		draw_square2D(GU_RGBA(0, 0, 0, 255));

		//if(shouldRender){
		for(int i = 0; i < 4; i++){
			updateForce(registy[i].p, registy[i].fg, delta_time);
		}
		for(int i = 0; i < 2; i++){
			Integrate(&particulas[i], delta_time);
		}
		//for(int i = 0; i < QNT_PARTICLE; i++){
			//AddForce(&particulas[i], vec4{gravidade.x, gravidade.y, gravidade.z, 0.0f});
			//Integrate(&particulas[i], delta_time);
		//}
		renderParticles(particulas, 2, GU_RGBA(255, 0, 0, 255));
		//}
		/*if(puxou){
			puxou = false;
		}
		if(shouldRender){
			shouldRender = false;
		}*/
		//reset_translate(0, 0, 0);
		//draw_square2D(GU_RGBA(0, 0, 0, 255));
		//Cam.Position.x = Get_Position(obj).x;
		
		//sceGuEnable(GU_TEXTURE_2D);
		
		//for(int i = 0; i < 1; i++)
		//draw_sprite(caracteres, 10, 10);
		//draw_sprite(font_text, 10, 10);
		//draw_sprite(font_text, 10, 10);
		//sceGuDisable(GU_TEXTURE_2D);
		//draw_caracter(main_font, '0', GU_RGBA(255, 0, 0, 255), 0, 0, 16, 16);
		//char* stringer = ParaString(obj.linearVelocity.y);
		//draw_string(main_font, stringer, qnt_digitos(obj.linearVelocity.y),  
		//			GU_RGBA(255, 0, 0, 255), 0, 0, 16, 16);
		//free(stringer);

	
		//reset_translate(s1.xcenter, s1.ycenter, -1.0f);
		//scale(s1.sizex, s1.sizey, 1);
		//draw_square2D(0xFF00FFFF);

		/**
		reset_translate(s1.xcenter, s1.ycenter, -10.0f);
		scale(s1.sizex, s1.sizey, 1);
		draw_square2D(0xFF00FFFF);

		reset_translate(s2.xcenter, s2.ycenter, -10.0f);
		scale(s2.sizex, s2.sizey, 1);
		draw_square2D(0xFFFF00FF);
		*/
		//reset_translate(10, 0, 1.0f);
		//scale(2, 2, 1);
		//draw_square2D(1, 1, 0xFF0000FF);
		
		//draw_square(0, 0, 10, 10, 0xFF00FFFF);
		/**
		reset_translate(s1.xcenter, s1.ycenter, s1.zcenter);
        scale(s1.radius, s1.radius, 1);
        draw_Circle3D(1, 0xFFFF00FF);

		reset_translate(s2.xcenter, s2.ycenter, s2.zcenter);
        scale(s2.radius, s2.radius, 1);
        draw_Circle3D(1, 0xFF00FFFF);
		*/
		//Render Goes Up Here
		endFrame();
	}
	endGu();
	destroy_texture(font_text);
	destroy_Font(main_font);

	sceKernelExitGame();
	return 0;
}
