#ifndef _MODULE_NAVIGATION_H__
#define _MODULE_NAVIGATION_H__

#include "Module.h"
#include "Recast.h"
#include "RecastDump.h"
#include "DebugDraw.h"

class GameObject;

enum PolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

class BuildContext : public rcContext
{
	j1PerfTimer timers[RC_MAX_TIMERS];
	
	static const int MAX_MESSAGES = 1000;
	const char* messages[MAX_MESSAGES];
	int messageCount;
	static const int TEXT_POOL_SIZE = 8000;
	char textPool[TEXT_POOL_SIZE];
	int textPoolSize;

public:
	BuildContext();

	const char* getLogText(const int i) const;
	int getLogCount() const;

protected:
	virtual void doLog(const rcLogCategory category, const char* msg, const int len);
	virtual void doStartTimer(const rcTimerLabel label);
	virtual int doGetAccumulatedTime(const rcTimerLabel label) const;
};

class DebugDrawGL : public duDebugDraw
{
public:
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();

	virtual unsigned int areaToCol(unsigned int area);
};

class ModuleNavigation : public Module
{
	friend class ModuleObjects;
	friend class PanelNavigation;

public:
	ModuleNavigation(bool start_enabled = true);

	~ModuleNavigation();

	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);
	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	bool Bake();
	void DrawPolyMesh();
	void DrawHeightMesh();

private:
	void resetCommonSettings();

public:
	bool show_navmesh = true;
	bool show_heightmesh = false;
	
private:

	BuildContext ctx;
	DebugDrawGL dd;
	bool keepInterResults = true;

	unsigned char* triareas = nullptr;
	rcHeightfield* solid = nullptr;
	rcCompactHeightfield* chf = nullptr;
	rcContourSet* cset = nullptr;
	rcPolyMesh* pmesh = nullptr;
	rcPolyMeshDetail* dmesh = nullptr;

	float cellSize;
	float cellHeight;
	float agentHeight;
	float agentRadius;
	float agentMaxClimb;
	float agentMaxSlope;
	float regionMinSize;
	float regionMergeSize;
	float edgeMaxLen;
	float edgeMaxError;
	float vertsPerPoly;
	float detailSampleDist;
	float detailSampleMaxError;
	int partitionType;

	bool filterLowHangingObstacles;
	bool filterLedgeSpans;
	bool filterWalkableLowHeightSpans;


};




#endif // !_MODULE_NAVIGATION_H__

