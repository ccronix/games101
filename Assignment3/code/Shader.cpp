#include "Shader.hpp"


Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {700, 700, 700}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = (light.position - point).normalized();
        Eigen::Vector3f v = (eye_pos - point).normalized();
        Eigen::Vector3f h = (v + l).normalized();

        float r2 = (light.position - point).squaredNorm();
        float cos_d = std::max(0.f, normal.dot(l));
        float cos_s = std::pow(std::max(0.f, normal.dot(h)), p);

        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / r2) * cos_d;
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / r2) * cos_s; 
        
        result_color += ambient + diffuse + specular;
    }

    return result_color * 255.f;
}

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.diffuse)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        return_color = payload.diffuse->getColorBilinear(payload.tex_coords[0], payload.tex_coords[1]);

    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = (light.position - point).normalized();
        Eigen::Vector3f v = (eye_pos - point).normalized();
        Eigen::Vector3f h = (v + l).normalized();

        float r2 = (light.position - point).squaredNorm();
        float cos_d = std::max(0.f, normal.dot(l));
        float cos_s = std::pow(std::max(0.f, normal.dot(h)), p);
        
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / r2) * cos_d;
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / r2) * cos_s;
        
        result_color += ambient + diffuse + specular;

    }

    return result_color * 255.f;
}

Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;


    float kh = 0.2, kn = 0.1;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)
    float x = normal.x(), y = normal.y(), z = normal.z();
    Eigen::Vector3f t(x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z));
    Eigen::Vector3f b = t.cross(normal);
    Eigen::Matrix3f bump_TBN;
    bump_TBN << t, b, normal;
    auto u = payload.tex_coords.x();
    auto v = payload.tex_coords.y();
    auto w = payload.bump->width;
    auto h = payload.bump->height;
    float bmp_dU = kh * kn * (payload.bump->getColor(u + 1.f / w, v).norm() - payload.bump->getColor(u, v).norm());
    float bmp_dV = kh * kn * (payload.bump->getColor(u, v + 1.f / h).norm() - payload.bump->getColor(u, v).norm());
    Eigen::Vector3f bmp_ln(-bmp_dU, -bmp_dV, 1.f);
    Eigen::Vector3f bmp_normal;
    bmp_normal = (bump_TBN * bmp_ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};
    result_color = bmp_normal;

    return result_color * 255.f;
}

Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = Eigen::Vector3f(0.5, 0.5, 0.5);
    // Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)
    float x = normal.x(), y = normal.y(), z = normal.z();
    Eigen::Vector3f t(x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z));
    Eigen::Vector3f b = normal.cross(t);
    Eigen::Matrix3f disp_TBN;
    disp_TBN << t, b, normal;
    auto u = payload.tex_coords.x();
    auto v = payload.tex_coords.y();
    auto w = payload.displacement->width;
    auto h = payload.displacement->height;
    float disp_dU = kh * kn * (payload.displacement->getColor(u + 1.f / w, v).norm() - payload.displacement->getColor(u, v).norm());
    float disp_dV = kh * kn * (payload.displacement->getColor(u, v + 1.f / h).norm() - payload.displacement->getColor(u, v).norm());
    Eigen::Vector3f disp_ln(-disp_dU, -disp_dV, 1.f);
    Eigen::Vector3f disp_point = point + kn * normal * payload.displacement->getColor(u, v).norm();
    Eigen::Vector3f disp_normal = (disp_TBN * disp_ln).normalized();


    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = (light.position - disp_point).normalized();
        Eigen::Vector3f v = (eye_pos - disp_point).normalized();
        Eigen::Vector3f h = (v + l).normalized();

        float r2 = (light.position - disp_point).squaredNorm();
        float cos_d = std::max(0.f, disp_normal.dot(l));
        float cos_s = std::pow(std::max(0.f, disp_normal.dot(h)), p);
        
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / r2) * cos_d;
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / r2) * cos_s;
        
        result_color += ambient + diffuse + specular;

    }

    return result_color * 255.f;
}

Eigen::Vector3f hybrid_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f diffuse_color;
    Eigen::Vector3f specular_color;
    Eigen::Vector3f bump_value;
    Eigen::Vector3f displacement_value;

    
    if (payload.diffuse) {
        diffuse_color = payload.diffuse->getColorBilinear(payload.tex_coords[0], payload.tex_coords[1]);
    }
    else {
        diffuse_color = {0.5, 0.5, 0.5};
        diffuse_color = diffuse_color * 255;
    }
    if (payload.specular) {
        specular_color = payload.specular->getColorBilinear(payload.tex_coords[0], payload.tex_coords[1]);
    }
    else {
        specular_color = {0.8, 0.8, 0.8};
        specular_color = specular_color * 255;
    }

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = diffuse_color / 255.f;
    Eigen::Vector3f ks = specular_color / 255.f;

    auto l1 = light{{20, 20, 20}, {800, 800, 800}};
    auto l2 = light{{-20, 20, 0}, {600, 600, 600}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = diffuse_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    float x = normal.x(), y = normal.y(), z = normal.z();
    Eigen::Vector3f t(x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z));
    Eigen::Vector3f b = t.cross(normal);
    
    auto u = payload.tex_coords.x();
    auto v = payload.tex_coords.y();

    Eigen::Vector3f disp_point;
    Eigen::Vector3f disp_normal;
    Eigen::Vector3f bmp_normal;

    if (payload.displacement) {
        Eigen::Matrix3f disp_TBN;
        disp_TBN << t, b, normal;
        auto d_w = payload.displacement->width;
        auto d_h = payload.displacement->height;
        float disp_dU = kh * kn * (payload.displacement->getColor(u + 1.f / d_w, v).norm() - payload.displacement->getColor(u, v).norm());
        float disp_dV = kh * kn * (payload.displacement->getColor(u, v + 1.f / d_h).norm() - payload.displacement->getColor(u, v).norm());
        Eigen::Vector3f disp_ln(-disp_dU, -disp_dV, 1.f);
        disp_point = point + kn * normal * payload.displacement->getColor(u, v).norm();
        disp_normal = (disp_TBN * disp_ln).normalized();
    }
    else {
        disp_point = point;
        disp_normal = normal;
    }

    if (payload.bump) {
        Eigen::Matrix3f bump_TBN;
        bump_TBN << t, b, disp_normal;
        auto b_w = payload.bump->width;
        auto b_h = payload.bump->height;
        float bmp_dU = kh * kn * (payload.bump->getColor(u + 1.f / b_w, v).norm() - payload.bump->getColor(u, v).norm());
        float bmp_dV = kh * kn * (payload.bump->getColor(u, v + 1.f / b_h).norm() - payload.bump->getColor(u, v).norm());
        Eigen::Vector3f bmp_ln(-bmp_dU, -bmp_dV, 1.f);
        bmp_normal = (bump_TBN * bmp_ln).normalized();
    }
    else {
        bmp_normal = disp_normal;
    }

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        Eigen::Vector3f l = (light.position - point).normalized();
        Eigen::Vector3f v = (eye_pos - point).normalized();
        Eigen::Vector3f h = (v + l).normalized();

        float r2 = (light.position - point).squaredNorm();
        float cos_d = std::max(0.f, bmp_normal.dot(l));
        float cos_s = std::pow(std::max(0.f, bmp_normal.dot(h)), p);
        
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / r2) * cos_d;
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / r2) * cos_s;
        
        result_color += ambient + diffuse + specular;

    }

    return result_color * 255.f;
}
