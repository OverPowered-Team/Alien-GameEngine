#include "Globals.h"
#include "Application.h"
#include "ModuleNavigation.h"
#include "ModuleRenderer3D.h"
#include "ModuleObjects.h"
#include "Time.h"
#include "Alien.h"
#include "Optick/include/optick.h"
#include "ResourceMesh.h"

// Recast/detour/detourcrowd ----
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "RecastDump.h"
//#include "DetourNavMesh.h"
//#include "DetourNavMeshBuilder.h"
#include "DetourDebugDraw.h"


ModuleNavigation::ModuleNavigation(bool start_enabled) : Module(start_enabled)
{
	resetCommonSettings();
	drawModes[(int)NavDrawMode::DRAWMODE_NAVMESH] = true;
}

ModuleNavigation::~ModuleNavigation()
{
	LOG_ENGINE("Destroying and cleaning module navigation");
	
}

void ModuleNavigation::LoadConfig(JSONfilepack*& config)
{

}
void ModuleNavigation::SaveConfig(JSONfilepack*& config)
{

}

bool ModuleNavigation::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Init module navigation ");

	bool ret = true;
	

	return ret;
}

bool ModuleNavigation::Start()
{
	LOG_ENGINE("starting module navigation");

	return true;
}

// ---------------------------------------------------------
update_status ModuleNavigation::PreUpdate(float dt)
{
	OPTICK_EVENT();
	

	return UPDATE_CONTINUE;
}

update_status ModuleNavigation::Update(float dt)
{
	OPTICK_EVENT();


	return UPDATE_CONTINUE;
}

update_status ModuleNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleNavigation::CleanUp()
{
	return true;
}
// -----------------------------------------------------------

void ModuleNavigation::DebugDrawNavMeshes(NavDrawMode drawMode)
{
	glDisable(GL_LIGHTING);
	//glDepthMask(GL_FALSE);

	if (solid)
	{
		if(drawMode == NavDrawMode::DRAWMODE_VOXELS)
			duDebugDrawHeightfieldSolid(&dd, *solid);
	}
	if (cset)
	{
		if(drawMode == NavDrawMode::DRAWMODE_CONTOURS)
			duDebugDrawContours(&dd, *cset);
	}
	if (chf && cset)
	{
		if (drawMode == NavDrawMode::DRAWMODE_REGION_CONNECTIONS)
		{
			duDebugDrawCompactHeightfieldRegions(&dd, *chf);
			duDebugDrawRegionConnections(&dd, *cset);
		}
	}
	if (pmesh)
	{
		if (drawMode == NavDrawMode::DRAWMODE_POLYMESH)
			duDebugDrawPolyMesh(&dd, *pmesh);
		if (drawMode == NavDrawMode::DRAWMODE_POLYMESH_DETAIL)
			duDebugDrawPolyMeshDetail(&dd, *dmesh);
	}

	//glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
}

bool ModuleNavigation::Bake()
{
	bool ret = true;

	// get all flagged navigation static gameobjects that has mesh -----

	GameObject* scene_root = App->objects->GetRoot(true);

	std::vector<GameObject*> gos_with_mesh;
	std::stack<GameObject*> stacked_go;

	stacked_go.push(scene_root);
	while (!stacked_go.empty()) {
		GameObject* go = stacked_go.top();
		stacked_go.pop();

		if (go->GetComponent(ComponentType::MESH) != nullptr && go->nav_data.nav_static)
			gos_with_mesh.push_back(go);

		std::vector<GameObject*> child_gos = go->GetChildren();
		for (uint i = 0; i < child_gos.size(); ++i)
			stacked_go.push(child_gos[i]);
	}

	if (gos_with_mesh.empty())
		return false;

	// get min and max aabb
	GameObject* go = gos_with_mesh.front();
	ComponentMesh* mesh = static_cast<ComponentMesh*>(go->GetComponent(ComponentType::MESH));
	AABB aabb = mesh->GetLocalAABB();

	// get vertex data
	const int nverts = mesh->mesh->num_vertex;
	const float* verts = mesh->mesh->vertex;
	const int ntris = mesh->mesh->num_faces;
	const uint* tris = mesh->mesh->index;

	// Init build configuration from TODO: UNHARDCODE TEST, and make from gui or something ---------------

	rcConfig rc_conf;

	rc_conf.cs = cellSize;
	rc_conf.ch = cellHeight;
	rc_conf.walkableSlopeAngle = agentMaxSlope;
	rc_conf.walkableHeight = (int)ceilf(agentHeight / rc_conf.ch);
	rc_conf.walkableClimb = (int)floorf(agentMaxClimb / rc_conf.ch);
	rc_conf.walkableRadius = (int)ceilf(agentRadius / rc_conf.cs);
	rc_conf.maxEdgeLen = (int)(edgeMaxLen / rc_conf.cs);
	rc_conf.maxSimplificationError = edgeMaxError;
	rc_conf.minRegionArea = (int)rcSqr(regionMinSize);
	rc_conf.mergeRegionArea = (int)rcSqr(regionMergeSize);
	rc_conf.maxVertsPerPoly = vertsPerPoly;
	rc_conf.detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
	rc_conf.detailSampleMaxError = cellHeight * detailSampleMaxError;

	// area where the navigation will be build
	rcVcopy(rc_conf.bmin, (float*)&aabb.minPoint);
	rcVcopy(rc_conf.bmax, (float*)&aabb.maxPoint);
	rcCalcGridSize(rc_conf.bmin, rc_conf.bmax, rc_conf.cs, &rc_conf.width, &rc_conf.height);

	ctx.resetTimers();
	ctx.startTimer(RC_TIMER_TOTAL);
	ctx.log(RC_LOG_PROGRESS, "Building navigation:");
	ctx.log(RC_LOG_PROGRESS, " - %d x %d cells", rc_conf.width, rc_conf.height);
	ctx.log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

	// Rasterize polygon soup ----------------------------------------------------------------------------

	solid = rcAllocHeightfield();
	if (!solid)
	{
		ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(&ctx, *solid, rc_conf.width, rc_conf.height, rc_conf.bmin, rc_conf.bmax, rc_conf.cs, rc_conf.ch))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not create solid heightfield.");
		return false;
	}

	triareas = new unsigned char[ntris];
	if (!triareas)
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Out of memory 'triareas' (%d)", ntris);
		return false;
	}

	memset(triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(&ctx, rc_conf.walkableSlopeAngle, verts, nverts, (int*)tris, ntris, triareas);

	if (!rcRasterizeTriangles(&ctx, verts, nverts, (int*)tris, triareas, ntris, *solid, rc_conf.walkableClimb))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not rasterize triangles.");
		return false;
	}

	if (!keepInterResults)
	{
		delete[] triareas;
		triareas = nullptr;
	}

	//// Filter walkable surfaces --------------------------------------------------------------------------

	if (filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(&ctx, rc_conf.walkableClimb, *solid);
	if (filterLedgeSpans)
		rcFilterLedgeSpans(&ctx, rc_conf.walkableHeight, rc_conf.walkableClimb, *solid);
	if (filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(&ctx, rc_conf.walkableHeight, *solid);

	//// Partition walkable surface to simple regions -------------------------------------------------------

	chf = rcAllocCompactHeightfield();
	if (!chf)
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(&ctx, rc_conf.walkableHeight, rc_conf.walkableClimb, *solid, *chf))
	{
		LOG_ENGINE("build Navigation: Could not build compact data.");
		return false;
	}

	if (!keepInterResults)
	{
		rcFreeHeightField(solid);
		solid = nullptr;
	}

	if (!rcErodeWalkableArea(&ctx, rc_conf.walkableRadius, *chf))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not erode.");
		return false;
	}

	// partition the heighfield so that we can use a simple algorithm later to triangulate the walkable areas
	// INFO: using Watershed partitioning (classic recast)

	if (!rcBuildDistanceField(&ctx, *chf))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not build distance field.");
		return false;
	}

	// partitioning walkable surface into simple regions without holes

	if (!rcBuildRegions(&ctx, *chf, 0, rc_conf.minRegionArea, rc_conf.mergeRegionArea))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not build watershed regions.");
		return false;
	}

	// Trace and simplify region contours ---------------------------------------------------------------------

	cset = rcAllocContourSet();
	if (!cset)
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Out of memory 'contour set'.");
		return false;
	}
	if (!rcBuildContours(&ctx, *chf, rc_conf.maxSimplificationError, rc_conf.maxEdgeLen, *cset))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not create contours.");
		return false;
	}

	// Build polygons mesh from contours ----------------------------------------------------------------------

	pmesh = rcAllocPolyMesh();
	if (!pmesh)
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(&ctx, *cset, rc_conf.maxVertsPerPoly, *pmesh))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not triangulate contours.");
	}

	// Create detail mesh -------------------------------------------------------------------------------------

	dmesh = rcAllocPolyMeshDetail();
	if (!dmesh)
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Out of memory 'polymesh detail'.");
		return false;
	}
	if (!rcBuildPolyMeshDetail(&ctx, *pmesh, *chf, rc_conf.detailSampleDist, rc_conf.detailSampleMaxError, *dmesh))
	{
		ctx.log(RC_LOG_ERROR, "build Navigation: Could not build detail mesh.");
		return false;
	}

	if (!keepInterResults)
	{
		rcFreeCompactHeightfield(chf);
		chf = nullptr;
		rcFreeContourSet(cset);
		cset = nullptr;
	}

	// TODO: Create detour data from recast poly mesh 








	// SHOW BUILD INFO -----------------------------------------------------------------------

	ctx.stopTimer(RC_TIMER_TOTAL);
	duLogBuildTimes(ctx, ctx.getAccumulatedTime(RC_TIMER_TOTAL));

	ctx.log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", pmesh->nverts, pmesh->npolys);

	for (int i = 0; i < ctx.getLogCount(); ++i)
	{
		LOG_ENGINE("%s", ctx.getLogText(i));
	}


	return ret;

}

void ModuleNavigation::resetCommonSettings()
{
	cellSize = 0.3f;
	cellHeight = 0.2f;
	agentHeight = 2.0f;
	agentRadius = 0.6f;
	agentMaxClimb = 0.9f;
	agentMaxSlope = 45.0f;
	regionMinSize = 8;
	regionMergeSize = 20;
	edgeMaxLen = 12.0f;
	edgeMaxError = 1.3f;
	vertsPerPoly = 6.0f;
	detailSampleDist = 6.0f;
	detailSampleMaxError = 1.0f;
	partitionType = 0; // PARTITION_WATERSHED
}

// --------------------------------------------------------------------------------------
// custom buildcontext for debug log

BuildContext::BuildContext() :
	messageCount(0),
	textPoolSize(0)
{
	memset(messages, 0, sizeof(char*) * MAX_MESSAGES);

	resetTimers();
}


void BuildContext::doStartTimer(const rcTimerLabel label)
{
		timers[label].Start();
}

void BuildContext::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		timers[i].Reset();
}

void BuildContext::doStopTimer(const rcTimerLabel label)
{
	timers[label].Pause();
}

int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return (int)timers[label].ReadMs() * 1000; // useconds
}

void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
	if (!len) return;
	if (messageCount >= MAX_MESSAGES)
		return;
	char* dst = &textPool[textPoolSize];
	int n = TEXT_POOL_SIZE - textPoolSize;
	if (n < 2)
		return;
	char* cat = dst;
	char* text = dst + 1;
	const int maxtext = n - 1;
	// Store category
	*cat = (char)category;
	// Store message
	const int count = rcMin(len + 1, maxtext);
	memcpy(text, msg, count);
	text[count - 1] = '\0';
	textPoolSize += 1 + count;
	messages[messageCount++] = dst;
}

const char* BuildContext::getLogText(const int i) const
{
	return messages[i] + 1;
}

int BuildContext::getLogCount() const
{
	return messageCount;
}

// DEBUG DRAW IMPLEMENTATION -------------------------------------------------------------------

void DebugDrawGL::depthMask(bool state)
{
	glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawGL::texture(bool state)
{
	/*if (state)
	{
		glEnable(GL_TEXTURE_2D);
		g_tex.bind();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}*/
}

void DebugDrawGL::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		glPointSize(size);
		glBegin(GL_POINTS);
		break;
	case DU_DRAW_LINES:
		glLineWidth(size);
		glBegin(GL_LINES);
		break;
	case DU_DRAW_TRIS:
		glBegin(GL_TRIANGLES);
		break;
	case DU_DRAW_QUADS:
		glBegin(GL_QUADS);
		break;
	};
}

void DebugDrawGL::vertex(const float* pos, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3f(x, y, z);
}

void DebugDrawGL::vertex(const float* pos, unsigned int color, const float* uv)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2fv(uv);
	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2f(u, v);
	glVertex3f(x, y, z);
}

void DebugDrawGL::end()
{
	glEnd();
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

unsigned int DebugDrawGL::areaToCol(unsigned int area)
{
	switch (area)
	{
		// Ground (0) : light blue
	case SAMPLE_POLYAREA_GROUND: return duRGBA(0, 192, 255, 255);
		// Water : blue
	case SAMPLE_POLYAREA_WATER: return duRGBA(0, 0, 255, 255);
		// Road : brown
	case SAMPLE_POLYAREA_ROAD: return duRGBA(50, 20, 12, 255);
		// Door : cyan
	case SAMPLE_POLYAREA_DOOR: return duRGBA(0, 255, 255, 255);
		// Grass : green
	case SAMPLE_POLYAREA_GRASS: return duRGBA(0, 255, 0, 255);
		// Jump : yellow
	case SAMPLE_POLYAREA_JUMP: return duRGBA(255, 255, 0, 255);
		// Unexpected : red
	default: return duRGBA(255, 0, 0, 255);
	}
}

// ---------------------------------------------------------------------------------------------