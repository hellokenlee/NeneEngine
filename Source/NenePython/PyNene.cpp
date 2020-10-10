/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include <iostream>
#include <pybind11.h>
#include <embed.h>
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

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(nene, mod)
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


const char* script =
	"try:"
	"	import main"
	"	main.main()"
	"except Exception as e:"
	"	import traceback"
	"	traceback.print_exc()";


int main()
{
	//
	py::scoped_interpreter guard{};
	//
	py::exec(R"(
        try:
            import main
            main.main()
        except Exception as _e:
            import traceback
            traceback.print_exc()
    )");
	//
	return 0;
}
