// SceneManager.hpp
#pragma once
#include "Scene.hpp"
#include <memory>
#include <stack>

class SceneManager {
private:
    std::stack<std::unique_ptr<Scene>> scenes;

public:
    void push(std::unique_ptr<Scene> scene) {
        scenes.push(std::move(scene));
    }

    void pop() {
        if (!scenes.empty()) scenes.pop();
    }

    Scene* current() {
        return scenes.empty() ? nullptr : scenes.top().get();
    }
};
