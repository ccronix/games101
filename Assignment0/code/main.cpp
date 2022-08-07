#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main()
{
    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a / b << std::endl;
    std::cout << std::sqrt(b) << std::endl;
    std::cout << std::acos(-1) << std::endl;
    std::cout << std::sin(30.0 / 180.0 * acos(-1)) << std::endl;

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f, 2.0f, 3.0f);
    Eigen::Vector3f w(1.0f, 0.0f, 0.0f);
    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    Eigen::Vector3f add_result;
    add_result = v + w;
    std::cout << add_result << std::endl;
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl;
    std::cout << 2.0f * v << std::endl;

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix3f i, j;
    i << 1.0, 2.0, 3.0, 
         4.0, 5.0, 6.0, 
         7.0, 8.0, 9.0;

    j << 2.0, 3.0, 1.0, 
         4.0, 6.0, 5.0, 
         9.0, 7.0, 8.0;

    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl;
    // matrix add i + j
    Eigen::Matrix3f mat_add_res;
    mat_add_res = i + j;
    std::cout << "Example of matrix add \n";
    std::cout << mat_add_res << std::endl;
    // matrix scalar multiply i * 2.0
    Eigen::Matrix3f mat_scalar_mul;
    mat_scalar_mul = i * 2.0;
    std::cout << "Example of matrix scalar mutiply \n";
    std::cout << mat_scalar_mul << std::endl;
    // matrix multiply i * j
    Eigen::Matrix3f mat_mul;
    mat_mul = i * j;
    std::cout << "Example of matrix mutiply \n";
    std::cout << mat_mul << std::endl;
    // matrix multiply vector i * v
    Eigen::Vector3f mat_mul_vec;
    mat_mul_vec = i * v;
    std::cout << "Example of matrix mutiply vector \n";
    std::cout << mat_mul_vec << std::endl;

    /* 
    * PA 0
    */
    // TO DO: Define point P
    // TO DO: Define rotation matrix M
    // TO DO: M * P
    return 0;
}