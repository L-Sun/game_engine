#include <iostream>
#include "MyTest.hpp"
#include "SceneManager.hpp"
#include "DebugManager.hpp"
#include <chrono>
#include <thread>
#include "MyPhysicsManager.hpp"

using namespace My;

int MyTest::Initialize() {
    int result;

    std::cout << "My Game Logic Initialize" << std::endl;
    std::cout << "Start Loading Game Scene" << std::endl;
    result = g_SceneManager->LoadScene("Asset/Scene/test.fbx");
    m_Clock.Initialize();
    m_Clock.Start();
    return result;
}

void MyTest::Finalize() { std::cout << "MyTest Game Logic Finalize" << std::endl; }

void MyTest::Tick() {}

void MyTest::OnLeftKey() {
    auto node_weak_ptr = g_SceneManager->GetSceneGeometryNode(selectedNode[i]);

    if (auto node_ptr = node_weak_ptr.lock()) {
        node_ptr->Move(-1.0f, 0.0f, 0.0f);
        g_hysicsManager->UpdateRigidBodyTransform(*node_ptr);
    }
}
void MyTest::OnRightKey() {
    auto node_weak_ptr = g_SceneManager->GetSceneGeometryNode(selectedNode[i]);

    if (auto node_ptr = node_weak_ptr.lock()) {
        node_ptr->Move(1.0f, 0.0f, 0.0f);
        g_hysicsManager->UpdateRigidBodyTransform(*node_ptr);
    }
}
void MyTest::OnUpKey() {
    auto node_weak_ptr = g_SceneManager->GetSceneGeometryNode(selectedNode[i]);

    if (auto node_ptr = node_weak_ptr.lock()) {
        node_ptr->Move(0.0f, 0.0f, 1.0f);
        g_hysicsManager->UpdateRigidBodyTransform(*node_ptr);
    }
}
void MyTest::OnDownKey() {
    auto node_weak_ptr = g_SceneManager->GetSceneGeometryNode(selectedNode[i]);

    if (auto node_ptr = node_weak_ptr.lock()) {
        node_ptr->Move(0.0f, 0.0f, -1.0f);
        g_hysicsManager->UpdateRigidBodyTransform(*node_ptr);
    }
}

void MyTest::OnCKey() { i = (i + 1) % selectedNode.size(); }
