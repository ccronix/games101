// clang-format off
#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"

constexpr double MY_PI = 3.1415926;

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

Eigen::Matrix4f get_model_matrix(float rotation_x, float rotation_y, float rotation_z)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    float rotation_x_arc = degree_to_arc(rotation_x);
    float rotation_y_arc = degree_to_arc(rotation_y);
    float rotation_z_arc = degree_to_arc(rotation_z);

    Eigen::Matrix4f rotate_x, rotate_y, rotate_z;
    Eigen::Matrix4f translate_2z, translate_2z_inv;

    translate_2z << 1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, -2,
                    0, 0, 0, 1,

    translate_2z_inv << 1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 2,
                        0, 0, 0, 1,

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
    
    model = translate_2z * rotate_x * rotate_y * translate_2z_inv * rotate_z * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // TODO: Copy-paste your implementation from the previous assignment.
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float half_fov_arc = degree_to_arc(eye_fov / 2);
    float top = -zNear * tan(half_fov_arc);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = - right;

    Eigen::Matrix4f scale, translate, perspective;
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

    projection = translate * scale * perspective;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle_x = 0, angle_y = 0, angle_z = 0, angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc == 2)
    {
        command_line = true;
        filename = std::string(argv[1]);
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0,0,8};


    std::vector<Eigen::Vector3f> pos
            {
                    {2, 0, -2},
                    {0, 2, -2},
                    {-2, 0, -2},
                    {3.5, -1, -3},
                    {2.5, 1.5, -3},
                    {-1, 0.5, -3}
            };

    std::vector<Eigen::Vector3i> ind
            {
                    {0, 1, 2},
                    {3, 4, 5}
            };

    std::vector<Eigen::Vector3f> cols
            {
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0}
            };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle_x, angle_y, angle_z));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
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
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
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
// clang-format on