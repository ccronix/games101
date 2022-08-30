#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char* argv[])
{

    // Change the definition here to change resolution
    Scene scene(1024, 1024);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.75f, 0.25f, 0.25f);
    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.25f, 0.25f, 0.75f);
    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.75, 0.75, 0.75);
    Material* light = new Material(DIFFUSE, (42.0 * Vector3f(0.9, 0.75, 0.6)));
    light->Kd = Vector3f(0.65f);

    MeshTriangle front("cornell/pFront.obj", white);
    MeshTriangle floor("cornell/pBack.obj", white);
    MeshTriangle shortbox("cornell/boxSmall.obj", white);
    MeshTriangle tallbox("cornell/boxTall.obj", white);
    MeshTriangle left("cornell/pLeft.obj", red);
    MeshTriangle right("cornell/pRight.obj", green);
    MeshTriangle light_("cornell/light.obj", light);

    scene.Add(&front);
    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene, 8);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}