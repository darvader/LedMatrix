#include "EllipseRenderer.h"

EllipseRenderer::EllipseRenderer(IDisplay* display)
    : display_(display), runners_(nullptr), initialized_(false) {}

EllipseRenderer::~EllipseRenderer() {
    cleanup();
}

void EllipseRenderer::init() {
    if (!initialized_) {
        runners_ = new runner_t[NUM_RUNNERS];
        for (int i = 0; i < NUM_RUNNERS; i++) {
            createRunner(&runners_[i]);
        }
        initialized_ = true;
    }
}

void EllipseRenderer::cleanup() {
    if (runners_ != nullptr) {
        delete[] runners_;
        runners_ = nullptr;
        initialized_ = false;
    }
}

void EllipseRenderer::createRunner(runner_t* runner) {
    float speed = random(100) * 0.01f + 0.2f;
    runner->speed = speed;
    runner->r = random(255) * speed;
    runner->g = random(255) * speed;
    runner->b = random(255) * speed;
    runner->distance = 0.0f;
    runner->position = 0.0f;
    runner->radius = 0.5f + 0.006f * random(100);
}

void EllipseRenderer::drawRunners() {
    const int width = 128;
    const int height = 64;
    const int a = 60;  // semi-major axis
    const int b = 30;  // semi-minor axis
    const int centerX = width / 2;
    const int centerY = height / 2;

    if (!initialized_) {
        init();
    }

    for (int i = 0; i < NUM_RUNNERS; i++) {
        runner_t* runner = &runners_[i];
        runner->position += (M_PI / 1000.0f) * runner->speed;

        float theta = runner->position;
        int x = centerX + a * cos(theta) * runner->radius;
        int y = centerY + b * sin(theta) * runner->radius;
        display_->drawPixelRGB888(x, y, runner->r, runner->g, runner->b);

        if (runner->position > M_TWOPI * 3) {
            createRunner(runner);
        }
    }
}
