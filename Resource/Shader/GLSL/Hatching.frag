#version 420 core
#define LIGHT_NUM 1

struct Light {
	float type;
	float range;
	float _pad0, _pad1;
	vec4 color;
	vec4 position;
	vec4 direction;
	float attenuation;
};


layout (binding = 0) uniform sampler2D tex0;

layout (std140, binding = 0) uniform UBO0 {
	mat4 view;
	mat4 proj;
	vec3 camera_position;
};

layout (std140, binding = 2) uniform UBO2 {
	Light lights[LIGHT_NUM];
};


in vec2 texcoord_VS_out;
in vec3 normal_VS_out;
in vec3 position_VS_out;

out vec4 color_FS_out;

void main() {
	//
	vec3 albedo = texture(tex0, texcoord_VS_out).rgb * 0.2;
	//
	vec3 final_color = vec3(0.0);
	//
	for (uint i = 0; i < LIGHT_NUM; ++i)
	{
		//
		vec3 ambient = lights[i].color.rgb * albedo.rgb;
		//
		vec3 normal = normalize(normal_VS_out);
		vec3 light_dir = normalize(lights[i].position.xyz - position_VS_out);
		vec3 diffuse = max(dot(normal, light_dir), 0.0) * lights[i].color.rgb * albedo;
		//
		vec3 view_dir = normalize(camera_position - position_VS_out);
		vec3 halfway_dir = normalize(light_dir + view_dir);
		vec3 specular = pow(max(dot(normal, halfway_dir), 0.0), 2.0) * lights[i].color.rgb * albedo;
		//
		final_color += (ambient + diffuse + specular);
	}
	//
	color_FS_out = vec4(final_color, 1.0);
}