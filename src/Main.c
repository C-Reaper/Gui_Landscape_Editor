#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Cube.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mathlib.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mesh.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_World3D.h"
#include "/home/codeleaded/System/Static/Library/MarchingCubes.h"
#include "/home/codeleaded/System/Static/Library/PerlinNoise.h"
#include "/home/codeleaded/System/Static/Library/RayCast.h"
#include "/home/codeleaded/System/Static/Library/Circle.h"

#define FIELDX	100
#define FIELDY	100
#define FIELDZ	100

Camera cam;
World3D world;
int Mode = 0;
int Menu = 0;

float Speed = 4.0f;
float radius = 1.0f;
float* landscape = NULL;

char Landscape_Bounce(void* world,Vec3 pos){
	const float* ls = (const float*)world;
	const uint32_t px = (uint32_t)pos.x;
	const uint32_t py = (uint32_t)pos.y;
	const uint32_t pz = (uint32_t)pos.z;
	if(px >= FIELDX || py >= FIELDY || pz >= FIELDZ) return 0;
	return 1;
}
float Landscape_Get(void* world,Vec3 pos){
	const float* ls = (const float*)world;
	const uint32_t px = (uint32_t)pos.x;
	const uint32_t py = (uint32_t)pos.y;
	const uint32_t pz = (uint32_t)pos.z;
	if(!Landscape_Bounce(world,pos)) return -1.0;
	return ls[px + FIELDX * (py + FIELDY * pz)];
}
void Landscape_Set(void* world,Vec3 pos,float v){
	float* ls = (float*)world;
	const uint32_t px = (uint32_t)pos.x;
	const uint32_t py = (uint32_t)pos.y;
	const uint32_t pz = (uint32_t)pos.z;
	if(!Landscape_Bounce(world,pos)) return;
	ls[px + FIELDX * (py + FIELDY * pz)] = v;
}
char Landscape_IsVoid(void* world,Vec3 pos){
	const float* ls = (const float*)world;
	const uint32_t px = (uint32_t)pos.x;
	const uint32_t py = (uint32_t)pos.y;
	const uint32_t pz = (uint32_t)pos.z;
	if(!Landscape_Bounce(world,pos)) return 1;
	return ls[px + FIELDX * (py + FIELDY * pz)] < 0.0;
}

void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window);
	}
	
	Menu = m;
}
void Setup(AlxWindow* w){
	ResizeAlxFont(32,32);
	Menu_Set(1);

	cam = Camera_Make(
		(Vec3D){ FIELDX * 0.5f,FIELDY * 0.8f,FIELDZ * 0.5f,1.0f },
		(Vec3D){ 0.0f,0.0f,0.0f,1.0f },
		90.0f
	);

	world = World3D_Make(
		Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }),
		Matrix_MakePerspektive(cam.p,cam.up,cam.a),
		Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f)
	);
	//world.normal = WORLD3D_NORMAL_NONE;
	world.normal = WORLD3D_NORMAL_CAP;

	PerlinNoise_Persistance_Set(PerlinNoise_Persistance_Get() * 10.0);
	PerlinNoise_Offset_Set(PerlinNoise_Offset_Get() * 10.0);
	landscape = (float*)malloc(sizeof(float) * FIELDX * FIELDY * FIELDZ);

	for(int i = 0;i<FIELDZ;i++){
		for(int k = 0;k<FIELDX;k++){
			const float h = (float)PerlinNoise_2D_Get(k,i);
			const uint32_t height = (uint32_t)(0.5f * (h + 1.0f) * FIELDY);

			for(int j = 0;j<height;j++)			landscape[(i * FIELDY + j) * FIELDX + k] = 1.0f;
			for(int j = height;j<FIELDY;j++)	landscape[(i * FIELDY + j) * FIELDX + k] = -1.0f;
		}
	}

	Vector_Clear(&world.trisIn);
	MarchingCubes_Render3D_FP(landscape,FIELDX,FIELDY,FIELDZ,&world.trisIn,0.0f,0.0f,0.0f,WHITE);
}
void Update(AlxWindow* w){
	if(Menu==1){
		Camera_Focus(&cam,GetMouseBefore(),GetMouse(),GetScreenRect().d);
		Camera_Update(&cam);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	
	if(Stroke(ALX_KEY_ESC).PRESSED) Menu_Set(!Menu);
	if(Stroke(ALX_KEY_Z).PRESSED) Mode = Mode < 2 ? Mode+1 : 0;

	if(Stroke(ALX_KEY_W).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.ld,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_S).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.ld,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_A).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.sd,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_D).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.sd,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_R).DOWN)
		cam.p.y += Speed * w->ElapsedTime;
	if(Stroke(ALX_KEY_F).DOWN)
		cam.p.y -= Speed * w->ElapsedTime;

	if(Stroke(ALX_KEY_UP).DOWN)
		PerlinNoise_Persistance_Set(PerlinNoise_Persistance_Get() * 1.01);
	else if(Stroke(ALX_KEY_DOWN).DOWN)
		PerlinNoise_Persistance_Set(PerlinNoise_Persistance_Get() * 0.99);

	if(Stroke(ALX_KEY_LEFT).DOWN)
		radius *= 1.01;
	else if(Stroke(ALX_KEY_RIGHT).DOWN)
		radius *= 0.99;

	if(Stroke(ALX_MOUSE_L).DOWN){
		Vec3 intersec_pos = (Vec3){ 0.0f,0.0f,0.0f };
		RayCast_TileMap_N(
			landscape,
			Landscape_IsVoid,
			(Vec3){ cam.p.x,cam.p.y,cam.p.z },
			(Vec3){ cam.ld.x,cam.ld.y,cam.ld.z },
			0.01f,
			100.0f,
			&intersec_pos
		);

		if(Landscape_Bounce(landscape,intersec_pos)){
			//Landscape_Set(landscape,intersec_pos,1.0);
			//Circle3_RenderX(landscape,FIELDX,FIELDY,FIELDZ,intersec_pos,radius,1.0f);
			Circle3_RenderXWire(landscape,FIELDX,FIELDY,FIELDZ,intersec_pos,radius,1.0f);
		}
	}else if(Stroke(ALX_MOUSE_R).DOWN){
		Vec3 intersec_pos = (Vec3){ 0.0f,0.0f,0.0f };
		RayCast_TileMap(
			landscape,
			Landscape_IsVoid,
			(Vec3){ cam.p.x,cam.p.y,cam.p.z },
			(Vec3){ cam.ld.x,cam.ld.y,cam.ld.z },
			0.01f,
			100.0f,
			&intersec_pos
		);

		if(Landscape_Bounce(landscape,intersec_pos)){
			//Landscape_Set(landscape,intersec_pos,-1.0);
			//Circle3_RenderX(landscape,FIELDX,FIELDY,FIELDZ,intersec_pos,radius,-1.0);
			Circle3_RenderXWire(landscape,FIELDX,FIELDY,FIELDZ,intersec_pos,radius,-1.0f);
		}
	}

	World3D_Set_Model(&world,Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }));
	World3D_Set_View(&world,Matrix_MakePerspektive(cam.p,cam.up,cam.a));
	World3D_Set_Proj(&world,Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f));
	
	Vector_Clear(&world.trisIn);
	MarchingCubes_Render3D_FP(landscape,FIELDX,FIELDY,FIELDZ,&world.trisIn,0.0,0.0,0.0,WHITE);

	Clear(LIGHT_BLUE);
	World3D_Update(&world,cam.p,(Vec2){ GetWidth(),GetHeight() });

	for(int i = 0;i<world.trisOut.size;i++){
		Tri3D* t = (Tri3D*)Vector_Get(&world.trisOut,i);
		const Pixel c = Pixel_Mulf(t->c.c,t->c.l);

		if(Mode==0)
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
		if(Mode==1)
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c,1.0f);
		if(Mode==2){
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),WHITE,1.0f);
		}
	}

	CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0,0,RED,"X: %f, Y: %f, Z: %f | R: %f",cam.p.x,cam.p.y,cam.p.z,radius);
	CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0,GetAlxFont()->CharSizeY + 1,RED,"SizeIn: %d, SizeBuff: %d, SizeOut: %d",world.trisIn.size,world.trisBuff.size,world.trisOut.size);
}
void Delete(AlxWindow* w){
	if(landscape) free(landscape);
	landscape = NULL;

	World3D_Free(&world);
	AlxWindow_Mouse_SetVisible(&window);
}

int main(){
	if(Create("Marching Cubes for Landscape Generation and Edit",2500,1440,1,1,Setup,Update,Delete))
        Start();
    return 0;
}