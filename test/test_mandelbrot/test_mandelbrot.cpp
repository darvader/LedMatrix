#include <unity.h>

// Replicate the pure computation function from MandelbrotMode
static int computeIterations(float cr, float ci, int maxIterations) {
    float x = 0.0, y = 0.0, xx;
    int iteration = 0;
    while ((x * x + y * y <= 4) && (iteration < maxIterations)) {
        xx = x * x - y * y + cr;
        y = 2.0 * x * y + ci;
        x = xx;
        iteration++;
    }
    return iteration;
}

void test_inside_mandelbrot_set() {
    int iters = computeIterations(0.0f, 0.0f, 1000);
    TEST_ASSERT_EQUAL(1000, iters);
}

void test_origin_inside() {
    int iters = computeIterations(-0.5f, 0.0f, 1000);
    TEST_ASSERT_EQUAL(1000, iters);
}

void test_outside_mandelbrot_set() {
    int iters = computeIterations(2.0f, 2.0f, 1000);
    TEST_ASSERT_TRUE(iters < 1000);
    TEST_ASSERT_TRUE(iters < 10);
}

void test_boundary_point() {
    int iters = computeIterations(0.25f, 0.0f, 1000);
    TEST_ASSERT_TRUE(iters > 100);
}

void test_clearly_outside() {
    int iters = computeIterations(3.0f, 0.0f, 1000);
    TEST_ASSERT_EQUAL(1, iters);
}

void test_negative_one() {
    int iters = computeIterations(-1.0f, 0.0f, 1000);
    TEST_ASSERT_EQUAL(1000, iters);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_inside_mandelbrot_set);
    RUN_TEST(test_origin_inside);
    RUN_TEST(test_outside_mandelbrot_set);
    RUN_TEST(test_boundary_point);
    RUN_TEST(test_clearly_outside);
    RUN_TEST(test_negative_one);
    return UNITY_END();
}
