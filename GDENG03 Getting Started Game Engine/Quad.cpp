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
	colors = new vec3[vertexCount];

	float halfLength = length / 2.0f;
	this->position[0] = { -halfLength, -halfLength, 0.0f };
	this->position[1] = { -halfLength,  halfLength, 0.0f };
	this->position[2] = { halfLength, -halfLength, 0.0f };
	this->position[3] = { halfLength,  halfLength, 0.0f };

	// Set default colors
	this->colors[0] = { 1.0f, 0.0f, 0.0f };  // Red
	this->colors[1] = { 0.0f, 1.0f, 0.0f };  // Green
	this->colors[2] = { 0.0f, 0.0f, 1.0f };  // Blue
	this->colors[3] = { 1.0f, 1.0f, 0.0f };  // Yellow
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
	for (int i = 0; i < vertexCount; i++)  // FIXED: use vertexCount not sizeof
	{
		position[i] += vec3{ x, y, 0 };
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
