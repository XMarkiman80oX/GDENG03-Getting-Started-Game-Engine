#include "Quad.h"

Quad::Quad()
{
	/* IF CENTER
	* //X - Y - Z
		{-0.5f,-0.5f,0.0f...}, 
		{-0.5f,0.5f,0.0f...}, 
		{ 0.5f,-0.5f,0.0f...},
		{ 0.5f,0.5f,0.0f...}
	*/
	this->position = new vec3[vertexCount];
	this->colors = new vec3[vertexCount];

	if (this->vertexCount == 4) {
		position[0] = { -0.25f, -0.25f, 0.0f };
		position[1] = { -0.25f,  0.25f, 0.0f };
		position[2] = { 0.25f, -0.25f, 0.0f };
		position[3] = { 0.25f,  0.25f, 0.0f };
	}
}

Quad::~Quad()
{
	delete[] position;
	delete[] colors;
}

//void Quad::setPosition(Vector2 position)
//{
//	this->setPosition(position.x, position.y);
//}

void Quad::setPosition(float x, float y)
{
	for (int i = 0; i < sizeof(this->vertexCount); i++) 
	{
		this->position[i] += vec3{ x, y, 0 };
	}
}

void Quad::setColors(Vector3 color1, Vector3 color2, Vector3 color3, Vector3 color4)
{
	if (this->vertexCount >= 4) {
		colors[0] = color1;
		colors[1] = color2;
		colors[2] = color3;
		colors[3] = color4;
	}
}

int Quad::getVertexCount()
{
	return this->vertexCount;
}

Vector3* Quad::getVertexPositions()
{
	return this->position;
}

Vector3* Quad::getVertexColors()
{
	return this->colors;
}
