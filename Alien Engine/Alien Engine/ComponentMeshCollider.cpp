#include "ComponentMeshCollider.h"
#include "ComponentMesh.h"
#include "ComponentPhysics.h"
#include "ResourceMesh.h"
#include "ModulePhysX.h"
#include "Application.h"

ComponentMeshCollider::ComponentMeshCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Mesh Collider");
	type = ComponentType::MESH_COLLIDER;
	shape = CreateMeshShape();
	InitCollider();
}

void ComponentMeshCollider::SetSize(const float3 size)
{
}

void ComponentMeshCollider::ScaleChanged()
{
	BeginUpdateShape();
	shape->setGeometry(PxTriangleMeshGeometry(triangle_mesh, PxMeshScale(F3_TO_PXVEC3(physics->scale))));
	EndUpdateShape();
}

void ComponentMeshCollider::DrawSpecificInspector()
{
}

void ComponentMeshCollider::SaveComponent(JSONArraypack* to_save)
{
}

void ComponentMeshCollider::LoadComponent(JSONArraypack* to_load)
{
}

void ComponentMeshCollider::Clone(Component* clone)
{
}

void ComponentMeshCollider::Reset()
{
}

PxShape* ComponentMeshCollider::CreateMeshShape()
{
	if (shape) {
		shape->release();
		shape = nullptr;
	}

	const ComponentMesh* mesh = GetMesh();
	if (!mesh) {
		return nullptr;
	}

	uint nverts = mesh->mesh->num_vertex;
	uint ntris = mesh->mesh->num_faces;
	uint nindex = mesh->mesh->num_index;


	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = nverts;
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = mesh->mesh->vertex;

	meshDesc.triangles.count = ntris;
	meshDesc.triangles.stride = 3 * sizeof(uint);
	meshDesc.triangles.data = mesh->mesh->index;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = App->physx->px_cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return nullptr;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	triangle_mesh = App->physx->px_physics->createTriangleMesh(readBuffer);
	return App->physx->CreateShape(PxTriangleMeshGeometry(triangle_mesh, PxMeshScale(F3_TO_PXVEC3(physics->scale))), *material);
}
