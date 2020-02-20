#include "Particle.h"
#include "ParticleSystem.h"
#include "Billboard.h"
#include "Application.h"
#include "Camera.h"
#include "GL/gl.h"

Particle::Particle(ParticleSystem* owner, ParticleInfo info, ParticleMutableInfo endInfo) : owner(owner), particleInfo(info), startInfo(info), endInfo(endInfo)
{
	owner->sourceFactor = GL_SRC_ALPHA;
	owner->destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
}

Particle::~Particle()
{
}

void Particle::PreUpdate(float dt)
{
	currentLifeTime += dt;
	if (currentLifeTime > particleInfo.maxLifeTime)
		to_delete = true;

}

void Particle::Update(float dt)
{
	// Apply forces
	particleInfo.velocity += particleInfo.force * dt;

	// Move
	particleInfo.position += particleInfo.velocity * dt;
}

void Particle::PostUpdate(float dt)
{
	if (particleInfo.changeOverLifeTime)
		InterpolateValues(dt);
}

void Particle::Draw()
{
	glColor4f(particleInfo.color.x, particleInfo.color.y, particleInfo.color.z, particleInfo.color.w);

	float4x4 particleLocal = float4x4::FromTRS(particleInfo.position, particleInfo.rotation, float3(1.f, 1.f, 1.f));
	float4x4 particleGlobal = particleLocal;

	if (!particleInfo.globalTransform)
	{
		float4x4 parentGlobal = owner->emmitter.GetGlobalTransform();
		particleGlobal = parentGlobal * particleLocal;
	}

	glPushMatrix();
	glMultMatrixf((GLfloat*)&(particleGlobal.Transposed()));


	// -------------------------------------------- Blending Options --------------------------------------------
	glEnable(GL_BLEND);
	switch (owner->funcBlendSource)
	{
	case FunctionBlendType::ZERO: owner->sourceFactor = GL_ZERO; break;
	case FunctionBlendType::ONE: owner->sourceFactor = GL_ONE; break;
	case FunctionBlendType::SRC_COLOR: owner->sourceFactor = GL_SRC_COLOR; break;
	case FunctionBlendType::ONE_MINUS_SRC_COLOR: owner->sourceFactor = GL_ONE_MINUS_SRC_COLOR; break;
	case FunctionBlendType::SRC_ALPHA: owner->sourceFactor = GL_SRC_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_SRC_ALPHA: owner->sourceFactor = GL_ONE_MINUS_SRC_ALPHA; break;
	case FunctionBlendType::DST_ALPHA: owner->sourceFactor = GL_DST_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_DST_ALPHA: owner->sourceFactor = GL_ONE_MINUS_DST_ALPHA; break;
	case FunctionBlendType::DST_COLOR: owner->sourceFactor = GL_DST_COLOR; break;
	case FunctionBlendType::ONE_MINUS_DST_COLOR: owner->sourceFactor = GL_ONE_MINUS_DST_COLOR; break;
	case FunctionBlendType::CONSTANT_COLOR: owner->sourceFactor = GL_CONSTANT_COLOR; break;
	case FunctionBlendType::ONE_MINUS_CONSTANT_COLOR: owner->sourceFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
	case FunctionBlendType::CONSTANT_ALPHA: owner->sourceFactor = GL_CONSTANT_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_CONSTANT_ALPHA: owner->sourceFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
	}
	switch (owner->funcBlendDest)
	{
	case FunctionBlendType::ZERO: owner->destinationFactor = GL_ZERO; break;
	case FunctionBlendType::ONE: owner->destinationFactor = GL_ONE; break;
	case FunctionBlendType::SRC_COLOR: owner->destinationFactor = GL_SRC_COLOR; break;
	case FunctionBlendType::ONE_MINUS_SRC_COLOR: owner->destinationFactor = GL_ONE_MINUS_SRC_COLOR; break;
	case FunctionBlendType::SRC_ALPHA: owner->destinationFactor = GL_SRC_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_SRC_ALPHA: owner->destinationFactor = GL_ONE_MINUS_SRC_ALPHA; break;
	case FunctionBlendType::DST_ALPHA: owner->destinationFactor = GL_DST_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_DST_ALPHA: owner->destinationFactor = GL_ONE_MINUS_DST_ALPHA; break;
	case FunctionBlendType::DST_COLOR: owner->destinationFactor = GL_DST_COLOR; break;
	case FunctionBlendType::ONE_MINUS_DST_COLOR: owner->destinationFactor = GL_ONE_MINUS_DST_COLOR; break;
	case FunctionBlendType::CONSTANT_COLOR: owner->destinationFactor = GL_CONSTANT_COLOR; break;
	case FunctionBlendType::ONE_MINUS_CONSTANT_COLOR: owner->destinationFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
	case FunctionBlendType::CONSTANT_ALPHA: owner->destinationFactor = GL_CONSTANT_ALPHA; break;
	case FunctionBlendType::ONE_MINUS_CONSTANT_ALPHA: owner->destinationFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
	}

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(owner->sourceFactor, owner->destinationFactor);

	switch (owner->eqBlend)
	{
	case EquationBlendType::FUNC_ADD: glBlendEquation(GL_FUNC_ADD); break;
	case EquationBlendType::FUNC_SUBTRACT: glBlendEquation(GL_FUNC_SUBTRACT); break;
	case EquationBlendType::FUNC_REVERSE_SUBTRACT: glBlendEquation(GL_FUNC_REVERSE_SUBTRACT); break;
	}

	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);

	// -----------------------------------------------------------------------------------------------------------------

	/*if (owner->resMat != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, owner->resMat->gpu_id);
	}
	else
		glBindTexture(GL_TEXTURE_2D, 0);*/

	glBegin(GL_TRIANGLES);

	// -------------------

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f * particleInfo.size, -0.5f * particleInfo.size, 0.f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f * particleInfo.size, 0.5f * particleInfo.size, 0.f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.5f * particleInfo.size, 0.5f * particleInfo.size, 0.f);


	// -------------------

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f * particleInfo.size, -0.5f * particleInfo.size, 0.f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f * particleInfo.size, -0.5f * particleInfo.size, 0.f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f * particleInfo.size, 0.5f * particleInfo.size, 0.f);

	// -------------------

	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glColor4f(1.f, 1.f, 1.f, 1.f);

}

void Particle::Orientate(ComponentCamera* camera)
{
	switch (owner->GetBillboardType())
	{
	case BillboardType::SCREEN:
		particleInfo.rotation = Billboard::AlignToScreen(camera);
		break;

	case BillboardType::WORLD:
		particleInfo.rotation = Billboard::AlignToWorld(camera, particleInfo.position);
		break;

	case BillboardType::AXIS:
		particleInfo.rotation = Billboard::AlignToAxis(camera, particleInfo.position);

		break;

	case BillboardType::NONE:

		break;

	default:
		break;
	}
}

void Particle::InterpolateValues(float dt)
{

	rateToLerp = 1.f / particleInfo.maxLifeTime;
	if (t <= 1)
	{
		t += rateToLerp * dt;
		particleInfo.color = float4::Lerp(startInfo.color, endInfo.color, t);
		particleInfo.size = Lerp(startInfo.size, endInfo.size, t);
		particleInfo.force = float3::Lerp(startInfo.force, endInfo.force, t);
	}

}


float3 Particle::GetPosition() const
{
	return particleInfo.position;
}

float Particle::Lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}
