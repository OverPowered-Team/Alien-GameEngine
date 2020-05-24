#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ResourceTexture.h"
#include "ComponentCamera.h"
#include "ModuleObjects.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/glew32.lib") 



ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name.assign("Renderer3D");
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG_ENGINE("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	glewInit();

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG_ENGINE("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ENGINE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ENGINE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ENGINE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_NORMALIZE);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}
	return ret;
}

bool ModuleRenderer3D::Start()
{
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	OPTICK_EVENT();
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	OPTICK_EVENT();
#ifndef GAME_VERSION
	App->ui->Draw(); // last draw UI!!!
#endif

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG_ENGINE("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	App->window->width = width;
	App->window->height = height;
#ifdef GAME_VERSION
	if (App->objects->game_viewport->GetCamera() != nullptr) {
		App->objects->game_viewport->SetSize(width, height);
	}
#endif
}


void ModuleRenderer3D::RenderGrid()
{
	OPTICK_EVENT();
	//TODO: Change this to buffers
	glLineWidth(line_grid_width);
	glColor3f(grid_color.r, grid_color.g, grid_color.b);
	glBegin(GL_LINES);
	for (float i = 0; i <= length_grid; i += grid_spacing)
	{
		glVertex3f(i - length_grid * 0.5, 0, -length_grid * 0.5);
		glVertex3f(i - length_grid * 0.5, 0, length_grid * 0.5);
		glVertex3f(-length_grid * 0.5, 0, i - length_grid * 0.5);
		glVertex3f(length_grid * 0.5, 0, i - length_grid * 0.5);
	}
	glEnd();
	glLineWidth(1);
}

void ModuleRenderer3D::UpdateCameraMatrix(ComponentCamera* camera)
{
	OPTICK_EVENT();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool ModuleRenderer3D::IsInsideFrustum(const ComponentCamera* camera, const AABB& aabb)
{
	float3 corners[8];
	aabb.GetCornerPoints(corners);

	Plane planes[6];
	camera->frustum.GetPlanes(planes);

	for (uint i = 0; i < 6; ++i)
	{
		uint point_inside_plane = 8;

		for (uint p = 0; p < 8; ++p)
		{
			if (planes[i].IsOnPositiveSide(corners[p]))
			{
				--point_inside_plane;
			}
		}

		if (point_inside_plane == 0)
		{
			return false;
		}
	}

	return true;
}

ComponentCamera* ModuleRenderer3D::GetCurrentMainCamera()
{
	ComponentCamera* currentMainCam = nullptr;

#ifndef GAME_VERSION
	// By default, we return the editor cam
	currentMainCam = scene_fake_camera;
#endif

	// If there is a game camera, we overwrite it
	if (actual_game_camera != nullptr)
		currentMainCam = actual_game_camera;

	return currentMainCam;
}
void ModuleRenderer3D::BeginDebugDraw(float4& color)
{
	float current_color[4];
	glGetFloatv(GL_CURRENT_COLOR, current_color);
	App->renderer3D->last_color.Set(current_color[0], current_color[1], current_color[2], current_color[3]);

	glDisable(GL_LIGHTING);
	glColor4fv(&color[0]);
	glLineWidth(2.f);
}

void ModuleRenderer3D::EndDebugDraw()
{
	glEnable(GL_LIGHTING);
	glColor4fv(&App->renderer3D->last_color[0]);
	glLineWidth(1.f);

}

void ModuleRenderer3D::RenderCircleAroundZ(const float& x, const float& y, const float& z, const float& radius, const float& line_width, const int& segments)
{
	BeginDebugDraw(float4(0.0f, 1.0f, 0.0f, 1.0f));
	glLineWidth(line_width);
	glBegin(GL_LINE_LOOP);
	for (int a = 0; a < 360; a += 360 / segments)
	{
		double heading = a * (double)math::pi / 180;
		glVertex3d(cos(heading) * radius + x, sin(heading) * radius + y, z);
	}
	glEnd();
	EndDebugDraw();
}

void ModuleRenderer3D::RenderCircleAroundX(const float& x, const float& y, const float& z, const float& radius, const float& line_width, const int& segments)
{
	BeginDebugDraw(float4(0.0f, 1.0f, 0.0f, 1.0f));
	glLineWidth(line_width);
	glBegin(GL_LINE_LOOP);
	for (int a = 0; a < 360; a += 360 / segments)
	{
		double heading = a * (double)math::pi / 180;
		glVertex3d(x, sin(heading) * radius + y, cos(heading) * radius + z);
	}
	glEnd();
	EndDebugDraw();
}

void ModuleRenderer3D::DebugDrawBox(const float4x4& transform, const float3& half_size, const float3& color) const
{
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	BeginDebugDraw(float4(color.x, color.y, color.z, 1.0f));

	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();

	EndDebugDraw();
	glPopMatrix();
}

void ModuleRenderer3D::DebugDrawSphere(const float4x4& transform, float radius, const float3& color) const
{
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	BeginDebugDraw(float4(color.x, color.y, color.z, 1.0f));

	float delta_amgle = 360.0f / CIRCLE_SIDES;  // Sphere sides
	float curr_angle = 0.f;

	// Circle X-Z --------------------
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)CIRCLE_SIDES; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), 0.0f, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	// Circle X-Y --------------------
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)CIRCLE_SIDES; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle), 0.0f);
	}
	glEnd();

	// Circle Y-Z --------------------
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)CIRCLE_SIDES; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle), radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();


	EndDebugDraw();
	glPopMatrix();
}

void ModuleRenderer3D::DebugDrawCapsule(const float4x4& transform, float radius, float half_height, const float3& color) const
{
	glPushMatrix();

	float4x4 final_trams = transform * Quat::RotateZ( DEGTORAD* -90);
	glMultMatrixf(final_trams.Transposed().ptr());
	BeginDebugDraw(float4(color.x, color.y, color.z, 1.0f));

	float delta_amgle = 360.0f / CIRCLE_SIDES;
	float half_delta_angle = 180.f / HALF_CIRCLE_SIDES;
	float curr_angle = 0.f;

	// Up ---------------------------------------

	// Circle X-Z  
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)CIRCLE_SIDES; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	// Circle X-Y 
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)HALF_CIRCLE_SIDES; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) + half_height, 0.0f);
	}
	glEnd();

	// Circle Y-Z 
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)HALF_CIRCLE_SIDES; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) + half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();


	// Down ---------------------------------------

	// Circle X-Z 
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)CIRCLE_SIDES; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), -half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	// Circle X-Y 
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)HALF_CIRCLE_SIDES; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) - half_height, 0.0f);
	}
	glEnd();

	// Circle Y-Z 
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)HALF_CIRCLE_SIDES; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) - half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();


	// Sides -----------------------------

	glBegin(GL_LINES);
	glVertex3f(0.f, half_height, -radius);
	glVertex3f(0.f, -half_height, -radius);
	glVertex3f(0.f, half_height, radius);
	glVertex3f(0.f, -half_height, radius);
	glVertex3f(-radius, half_height, 0.f);
	glVertex3f(-radius, -half_height, 0.f);
	glVertex3f(radius, half_height, 0.f);
	glVertex3f(radius, -half_height, 0.f);
	glEnd();

	EndDebugDraw();
	glPopMatrix();
}

void ModuleRenderer3D::DebugDrawMesh(const float4x4& transform, float* vertices, uint num_vertices, uint* indices, uint num_indices, const float3& color) const
{
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	BeginDebugDraw(float4(color.x, color.y, color.z, 1.0f));

	EndDebugDraw();
	glPopMatrix();
}


