#include <iostream>
#include <opencv2/opencv.hpp>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"
#include "Shader.hpp"

double degree_to_arc(double degree)
{
    double arc = degree * MY_PI / 180;
    return arc;
}

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float angle_x, float angle_y, float angle_z)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    float rotation_x_arc = degree_to_arc(angle_x);
    float rotation_y_arc = degree_to_arc(angle_y);
    float rotation_z_arc = degree_to_arc(angle_z);

    Eigen::Matrix4f rotate_x, rotate_y, rotate_z;
    Eigen::Matrix4f translate_2z, translate_2z_inv;

    rotate_x << 1, 0, 0, 0,
                0, cos(rotation_x_arc), -sin(rotation_x_arc), 0,
                0, sin(rotation_x_arc),  cos(rotation_x_arc), 0,
                0, 0, 0, 1;

    rotate_y << cos(rotation_y_arc),  0, sin(rotation_y_arc), 0,
                0, 1, 0, 0,
                -sin(rotation_y_arc), 0, cos(rotation_y_arc), 0,
                0, 0, 0, 1;

    rotate_z << cos(rotation_z_arc), -sin(rotation_z_arc), 0, 0,
                sin(rotation_z_arc),  cos(rotation_z_arc), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    
    model = rotate_x * rotate_y * rotate_z * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // TODO: Use the same projection matrix from the previous assignments
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float half_fov_arc = degree_to_arc(eye_fov / 2);
    float top = -zNear * tan(half_fov_arc);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = - right;

    Eigen::Matrix4f scale, translate, perspective, mirror;
    scale << 2 / (right - left), 0, 0, 0,
             0, 2 / (top - bottom), 0, 0,
             0, 0, 2 / (zNear - zFar), 0,
             0, 0, 0, 1;
    
    translate << 1, 0, 0, -(right + left) / 2,
                 0, 1, 0, -(top + bottom) / 2,
                 0, 0, 1, -(zNear + zFar) / 2,
                 0, 0, 0, 1;

    perspective << zNear, 0, 0, 0,
                   0, zNear, 0, 0,
                   0, 0, zNear + zFar, - zNear * zFar,
                   0, 0, 1, 0;
    
    // mirror << 1, 0,  0, 0,
    //           0, 1,  0, 0,
    //           0, 0, -1, 0,
    //           0, 0,  0, 1;

    projection = translate * scale * perspective;
    return projection;

}

int main(int argc, const char** argv)
{
    std::vector<Triangle*> TriangleList;

    float angle_x = 0, angle_y = 140, angle_z = 0;
    bool command_line = false;

    std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "./models/spot/";

    // Load .obj File
    bool loadout = Loader.LoadFile("./models/spot/spot_triangulated_good.obj");
    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            Triangle* t = new Triangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700);

    auto texture_path = "hmap.jpg";
    r.set_diffuse(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

    if (argc >= 2)
    {
        command_line = true;
        filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path = "spot_texture.png";
            r.set_diffuse(Texture(obj_path + texture_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,3};

    active_shader = texture_fragment_shader;
    texture_path = "spot_texture.png";

    r.set_diffuse(Texture(obj_path + texture_path));
    
    std::vector<std::string> shader_types;

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle_x, angle_y, angle_z));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle_x, angle_y, angle_z));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        // cv::imwrite(filename, image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'w') {
            angle_x += 10;
        }
        else if (key == 's') {
            angle_x -= 10;
        }
        else if (key == 'a') {
            angle_y += 10;
        }
        else if (key == 'd') {
            angle_y -= 10;
        }
        else if (key == 'q') {
            angle_z += 10;
        }
        else if (key == 'e') {
            angle_z -= 10;
        }
        else if (key == 'r') {
            angle_x = 0;
            angle_y = 0;
            angle_z = 0;
        }

    }
    return 0;
}
