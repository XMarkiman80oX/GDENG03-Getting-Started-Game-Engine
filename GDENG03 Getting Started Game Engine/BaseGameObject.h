#include "Matrix4x4.h"
#include "string"

class VertexShader;
class PixelShader;
class BaseGameObject
{
protected:
	struct vertex
	{
		Vector3D position;
		Vector3D color;
		Vector3D color1;
	};

	__declspec(align(16))
	struct constantBufferData
	{
		Matrix4x4 m_world;
		Matrix4x4 m_view;
		Matrix4x4 m_proj;

		unsigned int m_time;
	};

protected: //Inherited to other primitives
	std::string name;
	Vector3D localPosition;
	Vector3D localScale;
	Vector3D localRotation;
	Vector3D localMatrix;

public: //Constructors
	BaseGameObject(std::string name);
	~BaseGameObject();

public: //ABSTRACT CLASSES
	virtual void update(float deltaTime) = 0;
	virtual void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) = 0;

public: //Setters
	void setPosition(float x, float y, float z);
	void setPosition(Vector3D pos);
	Vector3D getLocalPosition();

	void setScale(float x, float y, float z);
	void setScale(Vector3D scale);
	Vector3D getLocalScale();

	void setRotation(float x, float y, float z);
	void setRotation(Vector3D rot);

public: //Getters
	Vector3D getLocalRotation();
	std::string getName();
};

