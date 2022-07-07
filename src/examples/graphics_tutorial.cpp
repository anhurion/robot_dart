#include <robot_dart/control/pd_control.hpp>
#include <robot_dart/robot_dart_simu.hpp>

#ifdef GRAPHIC
#include <robot_dart/gui/magnum/graphics.hpp>
#include <robot_dart/gui/magnum/gs/light.hpp>
#include <robot_dart/gui/magnum/gs/material.hpp>
#endif
#include <Magnum/GL/Texture.h>

inline std::shared_ptr<robot_dart::Robot> random_box(size_t num = 0)
{
    // random pose
    Eigen::Vector6d pose = Eigen::Vector6d::Random();
    // make sure it is above the ground
    pose(5) += 1.5;
    // random size
    Eigen::Vector3d size = Eigen::Vector3d::Random().array() * Eigen::Vector3d(0.1, 0.2, 0.1).array() + 0.3;
    return robot_dart::Robot::create_box(size, pose, "free", 1.,
        dart::Color::Red(1.0),
        "box_" + std::to_string(num));
}

inline std::shared_ptr<robot_dart::Robot> random_sphere(size_t num = 0)
{
    // random pose
    Eigen::Vector6d pose = Eigen::Vector6d::Random();
    // make sure it is above the ground
    pose(5) += 1.5;
    // random size
    Eigen::Vector3d size = Eigen::Vector3d::Random()[0] * Eigen::Vector3d(0.2, 0.2, 0.2).array() + 0.3;
    return robot_dart::Robot::create_ellipsoid(size, pose, "free", 1.,
        dart::Color::Blue(1.0),
        "sphere_" + std::to_string(num));
}

int main()
{
    std::srand(std::time(NULL));
    // choose time step of 0.001 seconds
    robot_dart::RobotDARTSimu simu(0.001);
#ifdef GRAPHIC
    auto graphics = std::make_shared<robot_dart::gui::magnum::Graphics>();
    simu.set_graphics(graphics);
    // set the camera at position (0, 3, 1) looking at the center (0, 0, 0)
    graphics->look_at({0., 3., 1.}, {0., 0., 0.});
#endif

    // add floor of square size of 10 meters and height of 0.2 meters
    simu.add_floor(10., 0.2);

    // add random objects to the world
    simu.add_robot(random_box(1));
    simu.add_robot(random_sphere(1));

    // add a simple arm
    auto arm_robot = std::make_shared<robot_dart::Robot>("arm.urdf");

    // pin the arm to world
    arm_robot->fix_to_world();
    arm_robot->set_position_enforced(true);
    simu.add_robot(arm_robot);
#ifdef GRAPHIC
    // @SHADOWS_GRAPHICS@
    // Disable shadows
    graphics->enable_shadows(false, false);
    simu.run(1.);
    // Enable non-transparent shadows
    graphics->enable_shadows(true, false);
    simu.run(1.);
    // Enable transparent shadows
    graphics->enable_shadows(true, true);
    simu.run(1.);
    // @SHADOWS_GRAPHICS_END@
    // @CLR_LIGHT@
    // Clear Lights
    graphics->clear_lights();
    // @CLR_LIGHT_END@
    simu.run(.2);
    // @LIGHT_MATERIAL@
    // Create Light material
    Magnum::Color4 ambient = {1.f, 1.f, 1.f, 1.f};
    Magnum::Color4 diffuse = {1.f, 1.f, 1.f, 1.f};
    Magnum::Color4 specular = {1.f, 1.f, 1.f, 1.f};
    Magnum::Float shininess = 1000.f;
    auto custom_material = robot_dart::gui::magnum::gs::Material(ambient, diffuse, specular, shininess);
    // @LIGHT_MATERIAL_END@
    // @POINT_LIGHT@
    // create point light
    Magnum::Vector3 position = {0.f, 0.f, 2.f};
    Magnum::Float intensity = 1.f;
    Magnum::Vector3 attenuation_terms = {1.f, 0.f, 0.f};
    auto point_light = robot_dart::gui::magnum::gs::create_point_light(position, custom_material, intensity, attenuation_terms);
    graphics->add_light(point_light);
    // @POINT_LIGHT_END@
    simu.run(1.);
    graphics->clear_lights();
    simu.run(.2);
    // @DIRECTIONAL_LIGHT@
    // create directional light
    Magnum::Vector3 direction = {-1.f, -1.f, -1.f};
    auto directional_light = robot_dart::gui::magnum::gs::create_directional_light(direction, custom_material);
    graphics->add_light(directional_light);
    // @DIRECTIONAL_LIGHT_END@
    simu.run(1.);
    graphics->clear_lights();
    simu.run(.2);
    // @SPOT_LIGHT@
    position = {0.f, 0.f, 1.f};
    Magnum::Float spot_exponent = M_PI;
    Magnum::Float spot_cut_off = M_PI / 8;
    auto spot_light = robot_dart::gui::magnum::gs::create_spot_light(position, custom_material, direction, spot_exponent, spot_cut_off, intensity, attenuation_terms);
    // @SPOT_LIGHT_END@
    graphics->add_light(spot_light);
    simu.run(1.);
#endif
    return 0;
}
