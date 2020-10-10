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



def main():
	#
	Utils.init("Test", 800, 800)
	Utils.clear_color(0.1, 0.1, 0.1)
	#
	Keyboard.instance().on_press.add_callback(mycallback)
	#
	uniforms = ConstantBufferPool()
	controller = CameraController()
	#
	quad = Geometry.create_quad(nene.COUNTER_CLOCK_WISE)
	#
	# shader = Shader(
	# 	os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Screen.vert"),
	# 	os.path.join(PROJECT_DIR, "Resource/Shader/GLSL/Screen.frag"),
	# 	nene.VertexFormat.POSITION_NORMAL_TEXTURE, True
	# )
	#
	while not Utils.window_should_close():
		#
		Utils.update()
		Utils.clear()
		#
		cc.update()
		cc.get_camera().use()
		#
		# quad.draw(shader)
		#
		NeneConstantBuffer.instance().per_frame.update(0)
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
