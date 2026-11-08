import re
import json
from pathlib import Path
from enum import Enum, EnumMeta
import sys
import os
import struct


namespace_re = re.compile(
    r'namespace\s+(\w+(?:::\w+)*)\s*\{(.*?)\}',
    re.DOTALL
)
constexpr_re = re.compile(
    r'constexpr\s+\w+\s+(\w+)\s*=\s*([0-9]+)\s*;'
)

def parse_header(*paths):
    mapping = {}
    for path in paths:
        with open(path, "r", encoding="utf-8") as f:
            text = f.read()
            for ns_name, ns_body in namespace_re.findall(text):
                for name, value in constexpr_re.findall(ns_body):
                    mapping[f"{ns_name}::{name}"] = int(value)
    return mapping

def replace_values(obj, mapping):
    if isinstance(obj, dict):
        return {k: replace_values(v, mapping) for k, v in obj.items()}
    if isinstance(obj, list):
        return [replace_values(v, mapping) for v in obj]
    if isinstance(obj, str):
        return mapping.get(obj, obj)
    return obj


json_input_file_path = sys.argv[1]
package_output_file_path = sys.argv[2]
data_src_file_path = sys.argv[3]
cpp_file_path = sys.argv[4]
mapping = parse_header(cpp_file_path + "/src/engine/file/types.h", cpp_file_path + "/src/game/types.h")

def get_value(element, name, default, throw_on_missing):
    if name not in element:
        if throw_on_missing:
            raise ValueError(f"Missing key value: '{name}'")
        return default
    try:
        return int(element[name])
    except(ValueError, TypeError):
        return element[name]


def pack_color(color: dict) -> int:
    r = color["r"]
    g = color["g"]
    b = color["b"]
    a = color.get("a", 255)
    return ((r << 24) | (g << 16) | (b << 8) | a)



mesh_data_skip_size = 20

total_file_size = 0
def process_package_header(data, output_file):
    global total_file_size

    buffer_count = 0
    mesh_count = 0
    image_count = 0
    image_sampler_count = 0
    material_count = 0
    shader_count = 0
    pipeline_count = 0

    if "buffer" in json_input_data:
        buffer_count = len(json_input_data["buffer"])
    if "mesh" in json_input_data:
        mesh_count = len(json_input_data["mesh"])
    if "image" in json_input_data:
        image_count = len(json_input_data["image"])
    if "image_sampler" in json_input_data:
        image_sampler_count = len(json_input_data["image_sampler"])
    if "material" in json_input_data:
        material_count = len(json_input_data["material"])
    if "shader" in json_input_data:
        shader_count = len(json_input_data["shader"])
    if "pipeline" in json_input_data:
        pipeline_count = len(json_input_data["pipeline"])



    buffer_setup_size = buffer_count * int(mapping["resource_metadata::buffer_setup_size"])
    mesh_setup_size = mesh_count * int(mapping["resource_metadata::mesh_setup_size"])
    image_setup_size = image_count * int(mapping["resource_metadata::image_setup_size"])
    image_sampler_setup_size = image_sampler_count * int(mapping["resource_metadata::image_sampler_setup_size"])
    material_setup_size = material_count * int(mapping["resource_metadata::material_setup_size"])
    shader_setup_size = shader_count * int(mapping["resource_metadata::shader_setup_size"])
    pipeline_setup_size = pipeline_count * int(mapping["resource_metadata::pipeline_setup_size"])
    header_size = int(mapping["resource_metadata::package_header_size"])

    total_file_size = (buffer_setup_size +
                       mesh_setup_size +
                       image_setup_size +
                       image_sampler_setup_size +
                       material_setup_size +
                       shader_setup_size +
                       pipeline_setup_size +
                       header_size)

    buffer_setup_offset = header_size
    mesh_setup_offset = buffer_setup_offset + buffer_setup_size
    image_setup_offset = mesh_setup_offset + mesh_setup_size
    image_sampler_setup_offset = image_setup_offset + image_setup_size
    material_setup_offset = image_sampler_setup_offset + image_sampler_setup_size
    shader_setup_offset = material_setup_offset + material_setup_size
    pipeline_setup_offset = shader_setup_offset + shader_setup_size


    output_file.write(struct.pack('<H', buffer_count))
    output_file.write(struct.pack('<H', mesh_count))
    output_file.write(struct.pack('<H', image_count))
    output_file.write(struct.pack('<H', image_sampler_count))
    output_file.write(struct.pack('<H', material_count))
    output_file.write(struct.pack('<H', shader_count))
    output_file.write(struct.pack('<H', pipeline_count))
    output_file.write(struct.pack('<H', 65535))

    output_file.write(struct.pack('<I', buffer_setup_offset))
    output_file.write(struct.pack('<I', mesh_setup_offset))
    output_file.write(struct.pack('<I', image_setup_offset))
    output_file.write(struct.pack('<I', image_sampler_setup_offset))
    output_file.write(struct.pack('<I', material_setup_offset))
    output_file.write(struct.pack('<I', shader_setup_offset))
    output_file.write(struct.pack('<I', pipeline_setup_offset))



def process_image_sampler_setups(array, output_file):
    global total_file_size
    return


def process_image_setups(array, output_file):
    global total_file_size
    return


def process_material_setups(array, output_file):
    global total_file_size

    default_color = {'r': 255, 'g':255, 'b':255, 'a':255}

    for element in array:
        id = get_value(element, "id", 0, 1)
        color = get_value(element, "color", default_color, 0)

        output_file.write(struct.pack('<H', id)) #2
        output_file.write(struct.pack('<H', 65535)) #4
        output_file.write(struct.pack('<I', pack_color(color))) #8

    return


def process_buffer_setups(array, output_file):
    global total_file_size

    for element in array:
        vram_memory = get_value(element, "vram_memory", 0, 1)
        id = get_value(element, "id", 0, 1)
        size = get_value(element, "size", 0, 1)
        usage = get_value(element, "usage", 0, 1)

        output_file.write(struct.pack('<H', id)) #2
        output_file.write(struct.pack('<B', vram_memory)) #3
        output_file.write(struct.pack('<B', 255)) #4
        output_file.write(struct.pack('<I', size)) #8
        output_file.write(struct.pack('<I', usage)) #12

    return


def process_mesh_setups(array, output_file):
    global total_file_size


    for element in array:
        src_file = get_value(element, "src_file", "", 1)
        id = get_value(element, "id", 0, 1)
        vertex_count = 0
        index_count = 0
        position_vertex_size = 0
        attribute_vertex_size = 0
        index_size = 0
        index_buffer_id = get_value(element, "index_buffer_id", 0, 1)
        position_vertex_buffer_id = get_value(element, "position_vertex_buffer_id", 0, 1)
        attribute_vertex_buffer_id = get_value(element, "attribute_vertex_buffer_id", 0, 1)

        data_size = os.path.getsize(data_src_file_path + element["src_file"])
        package_data_offset = total_file_size
        total_file_size = total_file_size + data_size - mesh_data_skip_size

        with open(data_src_file_path + element["src_file"], "rb") as src:
            vertex_count = struct.unpack("<I", src.read(4))[0]
            index_count = struct.unpack("<I", src.read(4))[0]
            position_vertex_size = struct.unpack("<I", src.read(4))[0]
            attribute_vertex_size = struct.unpack("<I", src.read(4))[0]
            index_size = struct.unpack("<I", src.read(4))[0]


        output_file.write(struct.pack('<H', id)) #2
        output_file.write(struct.pack('<B', position_vertex_size)) #3
        output_file.write(struct.pack('<B', attribute_vertex_size)) #4
        output_file.write(struct.pack('<H', position_vertex_buffer_id)) #6
        output_file.write(struct.pack('<H', attribute_vertex_buffer_id)) #8
        output_file.write(struct.pack('<H', index_buffer_id)) #10
        output_file.write(struct.pack('<B', index_size)) #11
        output_file.write(struct.pack('<B', 255)) #12
        output_file.write(struct.pack('<H', vertex_count)) #16
        output_file.write(struct.pack('<H', index_count)) #20
        output_file.write(struct.pack('<I', package_data_offset)) #24


    return


def process_shader_setups(array, output_file):
    global total_file_size

    for element in array:
        src_file = get_value(element, "src_file", "", 1)
        id = get_value(element, "id", 0, 1)
        code_size = os.path.getsize(data_src_file_path + element["src_file"])
        package_data_offset = total_file_size

        total_file_size = total_file_size + code_size

        output_file.write(struct.pack('<H', id)) #2
        output_file.write(struct.pack('<H', 65535)) #4
        output_file.write(struct.pack('<I', code_size)) #8
        output_file.write(struct.pack('<I', package_data_offset)) #12

    return


def process_pipeline_setups(array, output_file):
    global total_file_size

    for element in array:
        primitive_topology = 0
        polygon_mode = 0
        cull_mode = 0
        front_face = 0
        id = get_value(element, "id", 0, 1)
        vertex_shader_id = 0
        compute_shader_id = 0
        fragment_shader_id = 0
        is_compute = get_value(element, "is_compute", 0, 0)
        primitive_restart_enabled = 0
        layout_idx = get_value(element, "layout_idx", 0, 1)
        depth_test_enabled = 0
        depth_write_enabled = 0

        if is_compute == 0:
            primitive_restart_enabled = get_value(element, "primitive_restart_enabled", 0, 0)
            primitive_topology = get_value(element, "primitive_topology", 0, 1)
            polygon_mode = get_value(element, "polygon_mode", 0, 0)
            cull_mode = get_value(element, "cull_mode", 0, 1)
            front_face = get_value(element, "front_face", 0, 0)
            depth_test_enabled = get_value(element, "depth_test_enabled", 0, 0)
            depth_write_enabled = get_value(element, "depth_write_enabled", 0, 0)
            vertex_shader_id = get_value(element, "vertex_shader_id", 0, 1)
            fragment_shader_id = get_value(element, "fragment_shader_id", 0, 1)
        else:
            compute_shader_id = get_value(element, "compute_shader_id", 0, 1)


        output_file.write(struct.pack('<H', id)) #2
        output_file.write(struct.pack('<B', is_compute)) #3
        output_file.write(struct.pack('<B', primitive_restart_enabled)) #4
        if is_compute != 0:
            output_file.write(struct.pack('<H', compute_shader_id)) #6
            output_file.write(struct.pack('<H', 65535)) #8
        else:
            output_file.write(struct.pack('<H', vertex_shader_id)) #6
            output_file.write(struct.pack('<H', fragment_shader_id)) #8

        output_file.write(struct.pack('<I', primitive_topology)) #12
        output_file.write(struct.pack('<I', polygon_mode)) #16
        output_file.write(struct.pack('<I', cull_mode)) #20
        output_file.write(struct.pack('<I', front_face)) #24

        output_file.write(struct.pack('<B', depth_test_enabled)) #25
        output_file.write(struct.pack('<B', depth_write_enabled)) #26
        output_file.write(struct.pack('<B', layout_idx)) #27
        output_file.write(struct.pack('<B', 255)) #28


    return


def process_mesh_data(array, output_file):
    for element in array:
        with open(data_src_file_path + element["src_file"], "rb") as src:
            src.read(mesh_data_skip_size)
            output_file.write(src.read())


def process_shader_data(array, output_file):
    for element in array:
        with open(data_src_file_path + element["src_file"], "rb") as src:
            output_file.write(src.read())


def process(data):
    with open(package_output_file_path, "wb") as output_file:
        process_package_header(data, output_file)

        if "buffer" in json_input_data:
            process_buffer_setups(json_input_data["buffer"], output_file)
        if "mesh" in json_input_data:
            process_mesh_setups(json_input_data["mesh"], output_file)
        if "image" in json_input_data:
            process_image_setups(json_input_data["image"], output_file)
        if "image_sampler" in json_input_data:
            process_image_sampler_setups(json_input_data["image_sampler"], output_file)
        if "material" in json_input_data:
            process_material_setups(json_input_data["material"], output_file)
        if "shader" in json_input_data:
            process_shader_setups(json_input_data["shader"], output_file)
        if "pipeline" in json_input_data:
            process_pipeline_setups(json_input_data["pipeline"], output_file)


        if "mesh" in json_input_data:
            process_mesh_data(json_input_data["mesh"], output_file)
        if "shader" in json_input_data:
            process_shader_data(json_input_data["shader"], output_file)


with open(json_input_file_path, 'r', encoding='utf-8') as json_input_file:
    json_input_data = json.load(json_input_file)
    json_input_data = replace_values(json_input_data, mapping)
    process(json_input_data)