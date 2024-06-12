#include <sphere.h>

TEST(sphere_test, intersection_test) {
    sphere sphere(1.0, bardrix::point3(0.0, 0.0, 3.0));
    bardrix::ray ray(bardrix::point3(0, 0, 0), bardrix::vector3(0, 0, 1), 10);
    std::optional<bardrix::point3> intersection = sphere.intersection(ray);

    ASSERT_TRUE(intersection.has_value());

    ASSERT_EQ(intersection.value(), bardrix::point3(0, 0, 2));
}
