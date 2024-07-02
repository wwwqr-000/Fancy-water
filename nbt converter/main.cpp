#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

// Example struct to store coordinates
struct Coordinates {
    int32_t x;
    int32_t y;
    int32_t z;
};

// Function to read and parse NBT file
bool readNBTFile(const std::string& filename, std::vector<Coordinates>& coords) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    try {
        // Assuming your NBT file structure contains x, y, z as int32_t types
        while (!file.eof()) {
            int32_t x, y, z;
            file.read(reinterpret_cast<char*>(&x), sizeof(int32_t));
            file.read(reinterpret_cast<char*>(&y), sizeof(int32_t));
            file.read(reinterpret_cast<char*>(&z), sizeof(int32_t));
            coords.push_back({x, y, z});
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading NBT file: " << e.what() << std::endl;
        return false;
    }

    file.close();
    return true;
}

int main() {
    std::vector<Coordinates> coordinates;

    // Replace with your actual file path
    std::string filename = "house.nbt";

    // Read and parse the NBT file
    if (!readNBTFile(filename, coordinates)) {
        return 1;
    }

    // Output the coordinates
    for (const auto& coord : coordinates) {
        std::cout << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")" << std::endl;
    }

    return 0;
}
