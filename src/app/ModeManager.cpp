#include "ModeManager.h"

ModeManager::ModeManager()
    : currentMode_(nullptr), currentModeId_(-1), modeChangeCallback_(nullptr) {
    for (int i = 0; i < MAX_MODES; i++) {
        modes_[i] = nullptr;
    }
}

void ModeManager::registerMode(int id, IDisplayMode* mode) {
    if (id >= 0 && id < MAX_MODES) {
        modes_[id] = mode;
    }
}

void ModeManager::setMode(int id) {
    if (id < 0 || id >= MAX_MODES || modes_[id] == nullptr) return;
    if (id == currentModeId_) return;

    if (currentMode_ != nullptr) {
        currentMode_->cleanup();
    }

    currentModeId_ = id;
    currentMode_ = modes_[id];
    currentMode_->init();

    if (modeChangeCallback_) {
        modeChangeCallback_(id);
    }
}

void ModeManager::update() {
    if (currentMode_ != nullptr) {
        currentMode_->update();
    }
}

uint16_t ModeManager::getFrameDelay() const {
    if (currentMode_ != nullptr) {
        return currentMode_->frameDelayMs();
    }
    return 20;
}
