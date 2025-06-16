//
// Created by Mirco Theile on 17/5/25.
//

#ifndef TASK_H
#define TASK_H
#include <functional>

struct Task
{
    std::function<void()> task;
    bool running{false};
    bool finished{false};

    std::unordered_set<std::string> depends;
    std::unordered_set<std::string> provides;
};

#endif //TASK_H
