#include "Mesh.h"
#include "GraphicsEngine.h"
#include "VertexMesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <locale>
#include <codecvt>

Mesh::~Mesh()
{
}

Mesh::Mesh(const wchar_t* full_path) : Resource(full_path)
{
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warnings;
	std::string errors;

	std::string inputfile;
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, full_path, -1, nullptr, 0, nullptr, nullptr);
	if (size_needed > 0)
	{
		inputfile.resize(size_needed - 1); // Exclude null terminator
		WideCharToMultiByte(CP_UTF8, 0, full_path, -1, &inputfile[0], size_needed, nullptr, nullptr);
	}
	else
	{
		throw std::exception("Failed to convert wide string to UTF-8");
	}

	bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, inputfile.c_str());

	if(!errors.empty())
	{
		throw std::exception(errors.c_str());
	}
	if(!res)
	{
		throw std::exception("Mesh not created successfully");
	}

	if(shapes.size() == 0)
	{
		throw std::exception("Mesh has no shapes");
	}	

	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;

	for(size_t s = 0; s< shapes.size(); s++)
	{
		size_t index_offset = 0;
		// We reserve to speed up allocation
		list_vertices.reserve(shapes[s].mesh.indices.size());
		list_indices.reserve(shapes[s].mesh.indices.size());

		for(size_t v = 0; v < shapes[s].mesh.num_face_vertices.size(); v++)
		{
			unsigned char num_face_vertices = shapes[s].mesh.num_face_vertices[v];

			for(unsigned char f = 0; f < num_face_vertices; f++)
			{
				tinyobj::index_t index = shapes[s].mesh.indices[index_offset + f];

				tinyobj::real_t vx = attrib.vertices[index.vertex_index * 3 + 0];
				tinyobj::real_t vy = attrib.vertices[index.vertex_index * 3 + 1];
				tinyobj::real_t vz = attrib.vertices[index.vertex_index * 3 + 2];

				tinyobj::real_t tx = attrib.texcoords[index.texcoord_index * 2 + 0];
				tinyobj::real_t ty = attrib.texcoords[index.texcoord_index * 2 + 1];

				VertexMesh vertex(Vector3D(vx, vy, vz), Vector2D(tx, ty));
				list_vertices.push_back(vertex);
				list_indices.push_back(index_offset + f);
			}
			index_offset += num_face_vertices;
		}
	}
	void* shader_byte_code = nullptr;
	size_t size_shader = 0; 
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(
		&list_vertices[0],
		sizeof(VertexMesh),
		list_vertices.size(),
		shader_byte_code,
		size_shader
	);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(
		&list_indices[0],
		(UINT)list_indices.size()
	);
}

const VertexBufferPtr& Mesh::getVertexBuffer()
{
	return this->m_vertex_buffer;
}

const IndexBufferPtr& Mesh::getIndexBuffer()
{
	return this->m_index_buffer;
}
