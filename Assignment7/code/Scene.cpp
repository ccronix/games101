//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here

    // if not intersect, return directly.
    Intersection emit_from_camera = Scene::intersect(ray);
    if (! emit_from_camera.happened) {
        return Vector3f();
    }

    // if hit a light then return the light emission.
    if (emit_from_camera.m->hasEmission()) {
        return emit_from_camera.m->getEmission();
    }

    float EPLISON = 1e-4;
    Vector3f L_direct(0, 0, 0);
    Vector3f L_indirect(0, 0, 0);
    
    switch (emit_from_camera.m->getType()) {
        case DIFFUSE:
        {
            Intersection emit_from_light;
            float PDF_light = 0.0f;
            sampleLight(emit_from_light, PDF_light);    
            

            Vector3f camera_intersect = emit_from_camera.coords;
            Vector3f light_intersect = emit_from_light.coords;

            Vector3f object_to_light_direction = (light_intersect - camera_intersect).normalized();

            float object_to_light_distance = (light_intersect - camera_intersect).norm();


            Ray object_to_light_ray(camera_intersect, object_to_light_direction); 
            Intersection object_to_light_intersect = intersect(object_to_light_ray);
            // If the ray is not blocked in the middle
            if(object_to_light_intersect.distance - object_to_light_distance > - EPLISON) {
                L_direct = emit_from_light.emit
                    * emit_from_camera.m->eval(ray.direction, object_to_light_ray.direction, emit_from_camera.normal) 
                    * dotProduct(object_to_light_ray.direction, emit_from_camera.normal)
                    * dotProduct(- object_to_light_ray.direction, emit_from_light.normal)
                    / std::pow(object_to_light_distance, 2)
                    / PDF_light;
            }
            
            // Test Russian Roulette with probability RussianRoulette
            if(get_random_float() > RussianRoulette) {
                return L_direct;
            }

            // caculate indirect emission
            Vector3f object_to_next_object_direction = emit_from_camera.m->sample(ray.direction, emit_from_camera.normal).normalized();
            Ray object_to_next_object_ray(emit_from_camera.coords, object_to_next_object_direction);
            // If ray r hit a non-emitting object at next_object_intersection
            Intersection next_object_intersection = Scene::intersect(object_to_next_object_ray);
            if (next_object_intersection.happened && (! next_object_intersection.m->hasEmission())) {
                float PDF = emit_from_camera.m->pdf(ray.direction, object_to_next_object_ray.direction, emit_from_camera.normal);
                // if pdf near zero will cause a zero division then got white point
                if (PDF > EPLISON) {
                    L_indirect = castRay(object_to_next_object_ray, depth + 1) 
                        * emit_from_camera.m->eval(ray.direction, object_to_next_object_ray.direction, emit_from_camera.normal)
                        * dotProduct(object_to_next_object_ray.direction, emit_from_camera.normal)
                        / PDF
                        / RussianRoulette;
                }
            }
            break;
        }
        case MIRROR:
        {
            if(get_random_float() > RussianRoulette) {
                return L_direct;
            }
            Vector3f object_to_next_object_direction = emit_from_camera.m->sample(ray.direction, emit_from_camera.normal).normalized();
            Ray object_to_next_object_ray(emit_from_camera.coords, object_to_next_object_direction);
            // If ray r hit a non-emitting object at next_object_intersection
            Intersection next_object_intersection = Scene::intersect(object_to_next_object_ray);
            float PDF = emit_from_camera.m->pdf(ray.direction, object_to_next_object_ray.direction, emit_from_camera.normal);
            if (PDF > EPLISON) {
                L_indirect = castRay(object_to_next_object_ray, depth + 1) 
                    * emit_from_camera.m->eval(ray.direction, object_to_next_object_ray.direction, emit_from_camera.normal)
                    * dotProduct(object_to_next_object_ray.direction, emit_from_camera.normal)
                    / PDF
                    / RussianRoulette;
            }
            break;
        }
    }
    
    return L_direct + L_indirect;
}