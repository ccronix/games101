//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <eigen3/Eigen/Eigen>
#include "Texture.hpp"


struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        diffuse = nullptr;
        specular = nullptr;
        bump = nullptr;
        displacement = nullptr;
    }

    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,const Eigen::Vector2f& tc, Texture* diff, Texture* spec, Texture* bmp, Texture* disp) :
         color(col), normal(nor), tex_coords(tc), diffuse(diff), specular(spec), bump(bmp), displacement(disp) {}


    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coords;
    Texture* diffuse;
    Texture* specular;
    Texture* bump;
    Texture* displacement;
};

struct vertex_shader_payload
{
    Eigen::Vector3f position;
};

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

#endif //RASTERIZER_SHADER_H

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload);

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload);

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload);

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload);

Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload);

Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload);

Eigen::Vector3f hybrid_fragment_shader(const fragment_shader_payload& payload);

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis);