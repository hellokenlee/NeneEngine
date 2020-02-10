# -*- coding:UTF-8 -*-
__author__ = "KenLee"
__email__ = "hellokenlee@163.com"

import os
import sys
import time
import math

PROJECT_DIR = os.path.abspath("../../../Project/")
PROJECT_BUILD_DIR = os.path.join(os.path.join(PROJECT_DIR, "x64"), "DebugGL")
sys.path.append(PROJECT_BUILD_DIR)

import nene

from nene import Vec3
from nene import Utils
from nene import Model
from nene import Shader
from nene import Camera
from nene import Keyboard
from nene import Geometry
from nene import CoordinateAxes
from nene import CameraController
from nene import NeneConstantBuffer
from nene import ConstantBufferPool


def mycallback(eve):
	if eve.code == int(nene.EventCode.ON_KEY_PRESS):
		if eve.key == int(nene.Key.ESCAPE):
			Utils.set_window_should_close(True)
		pass


def main():
	#
	Utils.init("Nene Python", 800, 600)
	Utils.clear_color(0.1, 0.1, 0.1)
	#
	Keyboard.instance().on_press.add_callback(mycallback)
	#
	cbp = ConstantBufferPool()
	cc = CameraController()
	ca = CoordinateAxes(100.0, 10.0)
	#
	quad = Geometry.create_quad(nene.COUNTER_CLOCK_WISE)
	shader0 = Shader(
		os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Screen.vert"),
		os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Screen.frag"),
		nene.VertexFormat.POSITION_NORMAL_TEXTURE, True
	)
	shader1 = Shader(
		os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Common.vert"),
		os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Common.frag"),
		nene.VertexFormat.POSITION_NORMAL_TEXTURE, True
	)
	#
	house = Model(os.path.join(PROJECT_DIR, "Resource/Mesh/house/house.obj"))
	#
	house.scale_to(0.001)
	#
	house.move_to(Vec3(1.0, 1.0, 1.0))
	#
	while not Utils.window_should_close():
		#
		Utils.update()
		Utils.clear()
		#
		cc.update()
		cc.get_camera().use()
		#
		NeneConstantBuffer.instance().per_frame.update(0)
		#
		ca.draw()
		#
		house.draw(shader1)
		#
		Utils.swap_buffers()
	#
	Utils.terminate()
	#
	os._exit(0)
	pass

if __name__ == '__main__':
	main()
	pass
