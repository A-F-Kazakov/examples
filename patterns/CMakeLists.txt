cmake_minimum_required(VERSION 3.16)

project(${PROJECT_NAME}-patterns LANGUAGES CXX)

# creational
add_executable(${PROJECT_NAME}-abstract_factory abstract_factory.cpp)
add_executable(${PROJECT_NAME}-builder builder.cpp)
add_executable(${PROJECT_NAME}-factory_method factory_method.cpp)
add_executable(${PROJECT_NAME}-prototype prototype.cpp)

#stuctural
add_executable(${PROJECT_NAME}-adapter adapter.cpp)
add_executable(${PROJECT_NAME}-bridge bridge.cpp)
add_executable(${PROJECT_NAME}-composite composite.cpp)
add_executable(${PROJECT_NAME}-decorator decorator.cpp)
add_executable(${PROJECT_NAME}-facade facade.cpp)
add_executable(${PROJECT_NAME}-proxy proxy.cpp)

#behavioural
add_executable(${PROJECT_NAME}-command command.cpp)
add_executable(${PROJECT_NAME}-mediator mediator.cpp)
add_executable(${PROJECT_NAME}-observer observer.cpp)
add_executable(${PROJECT_NAME}-state state.cpp)
add_executable(${PROJECT_NAME}-strategy strategy.cpp)
add_executable(${PROJECT_NAME}-visitor visitor.cpp)
