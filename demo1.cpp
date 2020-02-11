#include <iostream>
#include "sTaskFlow/sTaskFlow.hpp" // all codes are written in header files.

int main() {
    // use stf::make_task_node to create a node.
    // the first parameter is this node's task.
    auto n1 = stf::make_task_node([]() {
        std::cout << "Node1" << std::endl;
        return 1;
    });
    // the second parameter is this node's predecessor
    // each predecessor's return value will be this task's input parameter
    auto n2 = stf::make_task_node([](auto x) {
        std::cout << "Node2, x: " << x << std::endl;
        return 2.0;
    }, n1);
    // you can use auto in lambda's parameter
    // its type will be its predecessor's return type
    // in this case, the type of x is int.
    auto n3 = stf::make_task_node([](auto x) {
        std::cout << "Node3, x: " << x << std::endl;
        return '3';
    }, n1);
    // each node can have several predecessors.
    // two predecessors mean this task will receive two input parameters
    // the types of two parameters will be the predecessors' return type respectively.
    // in this case, the type of x is double, the type of y is char.
    auto n4 = stf::make_task_node([](auto x, auto y) {
        std::cout << "Node4, x: " << x << ", y:" << y << std::endl;
        return 4.f;
    }, n2, n3);
    // n1, n2, n3, n4 is a std::shared_ptr
    // so use -> to call its member function
    // n4->run() will run all nodes which have direct or indirect connection with n4
    // in this case, all nodes have connection with n4, so every node will be execute
    // run() will return a std::future
    auto f4 = n4->run();
    // get() will block the main, until the task is finished.
    f4.get();
    // n4->reset_task() should be called, before run() call again.
    // otherwise, nodes will not be execute
    n4->reset_task();
    // you can just execute part of this task graph
    // in this case, n3->run() will only execute nodes have connection with n3
    // so n2, n4 will not be execute
    auto f3 = n3->run();
    f3.get();
    // if you run n4->run() without n3->reset_task()
    // the nodes which have been executed will not execute again
    // in this case, n4->run() will only execute n2, n4
    // because n1, n3 has been execute but not reset_task()
    auto f4_2 = n4->run();
    f4_2.get();
    return 0;
}