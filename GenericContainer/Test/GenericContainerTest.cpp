#include <iostream>
#include "../GenericContainer.h"

int main()
{
    GenericContainer<Slot<int>, Slot<std::string>> container;

    container.set(12);
    container.set<std::string>("Allora");

    std::cout << container.get<int>() << std::endl;
    std::cout << container.get<std::string>() << std::endl;
}