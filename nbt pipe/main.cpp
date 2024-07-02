#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdint>

void printFileContent(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    // Read and print each byte of the file in hexadecimal format
    std::ostringstream oss;
    int byteCount = 0;
    char byte;
    while (file.get(byte)) {
        if (byteCount % 16 == 0) {
            if (byteCount > 0) {
                std::cout << "  " << oss.str() << "\n";
                oss.str("");
            }
            std::cout << std::setw(8) << std::setfill('0') << std::hex << byteCount << "  ";
        } else if (byteCount % 8 == 0) {
            std::cout << " ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(static_cast<uint8_t>(byte)) << " ";
        if (byte >= 32 && byte <= 126) {
            oss << byte;
        } else {
            oss << ".";
        }
        byteCount++;
    }

    // Print any remaining characters in the last line
    if (!oss.str().empty()) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << byteCount << "  " << std::setw(2) << std::setfill('0') << std::hex << "   ";
        std::cout << std::left << std::setw(48) << std::setfill(' ') << oss.str() << "\n";
    }
}

int main() {
    std::string filename = "plank.mcstructure";

    printFileContent(filename);

    return 0;
}
