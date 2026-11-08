import bpy
import json
import struct
import os
import math
import bmesh
import mathutils
from mathutils import Vector, Matrix


output_dir = os.path.dirname(bpy.data.filepath)


def make_mesh(obj):
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    bm.to_mesh(mesh)
    bm.free()

    vertices = []
    indices = []
    vertex_map = {}

    mesh.calc_loop_triangles()

    for triangle in mesh.loop_triangles:
        for loop_index in triangle.loops:
            loop = mesh.loops[loop_index]
            vertex = mesh.vertices[loop.vertex_index]

            position = tuple(vertex.co)
            normal = tuple(loop.normal)
            uv = (0.0, 0.0)

            if mesh.uv_layers.active:
                uv_data = mesh.uv_layers.active.data[loop_index]
                uv = tuple(uv_data.uv)

            key = (position,normal,uv)

            if key not in vertex_map:
                index = len(vertices)
                vertex_map[key] = index
                vertices.append({"position": position, "normal": normal, "uv": uv})
            else:
                index = vertex_map[key]

            indices.append(index)

    return vertices, indices


def write_mesh(obj, vertices, indices):
    global output_dir

    filename = os.path.join(
        output_dir,
        obj.name + ".mesh"
    )
    
    f32_size = 4
    position_vertex_size = 3 * f32_size
    attribute_vertex_size = 3 * f32_size + 2 * f32_size
    index_size = 2

    with open(filename, "wb") as f:
        f.write(struct.pack("<I", len(vertices)))
        f.write(struct.pack("<I", len(indices)))
        f.write(struct.pack("<I", position_vertex_size))
        f.write(struct.pack("<I", attribute_vertex_size))
        f.write(struct.pack("<I", index_size))


        f.write(b"".join(
            struct.pack(
                "<3f",
                v["position"][0],
                v["position"][2],
                -v["position"][1]
            )
            for v in vertices
        ))
        
        f.write(b"".join(
            struct.pack(
                "<5f",
                *v["normal"],
                *v["uv"]
            )
            for v in vertices
        ))
        
        f.write(b"".join(
            struct.pack("<H", i)
            for i in indices
        ))


    return



for obj in bpy.context.scene.objects:
    if obj.type != 'MESH':
        continue
    if "SM_" not in obj.name:
        continue

    vertices, indices = make_mesh(obj)

    write_mesh(obj, vertices, indices)