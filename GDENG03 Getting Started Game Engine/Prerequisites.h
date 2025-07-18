#pragma once
#include <memory>
#include "Vector2D.h"
#include "Vector3D.h"

class SwapChain;
class DeviceContext;
class VertexBuffer;
class ConstantBuffer;
class VertexShader;
class PixelShader;
class IndexBuffer;
class DepthBuffer;
class RenderSystem;
class GraphicsEngine;
class Resource;
class RecourceManager;
class Texture;
class TextureManager;
class Mesh;
class MeshManager;
class BaseGameObject;

struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};

typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<DeviceContext> DeviceContextPtr;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<Resource> ResourcePtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Mesh> MeshPtr;
typedef std::shared_ptr<DepthBuffer> DepthBufferPtr;