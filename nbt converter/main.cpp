#include "classes/nbt.hpp"
#include <fstream>

using namespace nbt;

int main() {
    std::ifstream input("java.nbt");
    tags::compound_tag root;
    input >> contexts::java >> root;
    std::ofstream output("mojangson.txt");
    output << contexts::mojangson << root;
}
