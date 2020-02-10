/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include <iostream>
#include <pybind11.h>
#include "PyTypes.h"
#include "PyUtils.h"
#include "PyShape.h"
#include "PyShader.h"
#include "PyCamera.h"
#include "PyGeometry.h"
#include "PyCoordinateAxes.h"
#include "PyCameraController.h"
#include "PyConstantBuffer.h"
#include "PyConstantBufferPool.h"
#include "PyTexture.h"
#include "PyEvent.h"
#include "PyObserver.h"
#include "PyMouse.h"
#include "PyKeyboard.h"


PYBIND11_MODULE(nene, mod)
{
	mod.doc() = "Nene Python Binding";

	BindTypes(mod);
	BindUtils(mod);
	BindShader(mod);
	BindCamera(mod);
	BindShape(mod);
	BindTexture(mod);
	BindGeometry(mod);
	BindCoordinateAxes(mod);
	BindCameraController(mod);
	BindConstantBuffer(mod);
	BindConstantBufferPool(mod);
	BindEvent(mod);
	BindObserver(mod);
	BindMouse(mod);
	BindKeyboard(mod);
}
