#include <unity.h>
#include <cstdint>

// Replicate IDisplayMode and ModeManager for native testing
class IDisplayMode {
public:
    virtual ~IDisplayMode() = default;
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void cleanup() = 0;
    virtual uint16_t frameDelayMs() const = 0;
};

class ModeManager {
public:
    static const int MAX_MODES = 64;
    IDisplayMode* modes_[MAX_MODES];
    IDisplayMode* currentMode_;
    int currentModeId_;

    ModeManager() : currentMode_(nullptr), currentModeId_(-1) {
        for (int i = 0; i < MAX_MODES; i++) modes_[i] = nullptr;
    }

    void registerMode(int id, IDisplayMode* mode) {
        if (id >= 0 && id < MAX_MODES) modes_[id] = mode;
    }

    void setMode(int id) {
        if (id < 0 || id >= MAX_MODES || modes_[id] == nullptr) return;
        if (id == currentModeId_) return;
        if (currentMode_ != nullptr) currentMode_->cleanup();
        currentModeId_ = id;
        currentMode_ = modes_[id];
        currentMode_->init();
    }

    int getMode() const { return currentModeId_; }
    void update() { if (currentMode_) currentMode_->update(); }
    uint16_t getFrameDelay() const { return currentMode_ ? currentMode_->frameDelayMs() : 20; }
};

class TestMode : public IDisplayMode {
public:
    int initCount = 0, updateCount = 0, cleanupCount = 0;
    void init() override { initCount++; }
    void update() override { updateCount++; }
    void cleanup() override { cleanupCount++; }
    uint16_t frameDelayMs() const override { return 10; }
};

void test_register_and_set_mode() {
    ModeManager mgr;
    TestMode mode1;
    mgr.registerMode(1, &mode1);
    mgr.setMode(1);
    TEST_ASSERT_EQUAL(1, mgr.getMode());
    TEST_ASSERT_EQUAL(1, mode1.initCount);
}

void test_update_calls_current_mode() {
    ModeManager mgr;
    TestMode mode1;
    mgr.registerMode(1, &mode1);
    mgr.setMode(1);
    mgr.update();
    mgr.update();
    TEST_ASSERT_EQUAL(2, mode1.updateCount);
}

void test_mode_transition_calls_cleanup_and_init() {
    ModeManager mgr;
    TestMode mode1, mode2;
    mgr.registerMode(1, &mode1);
    mgr.registerMode(2, &mode2);

    mgr.setMode(1);
    TEST_ASSERT_EQUAL(1, mode1.initCount);

    mgr.setMode(2);
    TEST_ASSERT_EQUAL(1, mode1.cleanupCount);
    TEST_ASSERT_EQUAL(1, mode2.initCount);
}

void test_set_same_mode_does_nothing() {
    ModeManager mgr;
    TestMode mode1;
    mgr.registerMode(1, &mode1);
    mgr.setMode(1);
    mgr.setMode(1);
    TEST_ASSERT_EQUAL(1, mode1.initCount);
}

void test_set_invalid_mode() {
    ModeManager mgr;
    TestMode mode1;
    mgr.registerMode(1, &mode1);
    mgr.setMode(1);
    mgr.setMode(99);
    TEST_ASSERT_EQUAL(1, mgr.getMode());
}

void test_frame_delay() {
    ModeManager mgr;
    TestMode mode1;
    mgr.registerMode(1, &mode1);
    mgr.setMode(1);
    TEST_ASSERT_EQUAL(10, mgr.getFrameDelay());
}

void test_no_mode_set() {
    ModeManager mgr;
    mgr.update();
    TEST_ASSERT_EQUAL(20, mgr.getFrameDelay());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_register_and_set_mode);
    RUN_TEST(test_update_calls_current_mode);
    RUN_TEST(test_mode_transition_calls_cleanup_and_init);
    RUN_TEST(test_set_same_mode_does_nothing);
    RUN_TEST(test_set_invalid_mode);
    RUN_TEST(test_frame_delay);
    RUN_TEST(test_no_mode_set);
    return UNITY_END();
}
