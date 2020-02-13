/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"

class Renderer
{
public:
	static Renderer& Instance();

	inline void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	inline std::shared_ptr<Camera> GetCamera() { return m_camera; }

	void UpdateConstantBuffer();
protected:
	std::shared_ptr<Camera> m_camera;

protected:
	Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator = (const Renderer&) = delete;
};

#endif // RENDERER_H
