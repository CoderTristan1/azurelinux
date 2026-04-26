#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct Workload {
    std::string name;
    size_t sizeMB;
};

class GPUMemoryManager {
public:
    GPUMemoryManager(size_t totalMemoryMB)
        : totalMemoryMB(totalMemoryMB), usedMemoryMB(0) {}

    bool allocate(const std::string& workloadName, size_t sizeMB) {
        if (sizeMB == 0 || sizeMB + usedMemoryMB > totalMemoryMB) {
            return false;
        }
        workloads.push_back({workloadName, sizeMB});
        usedMemoryMB += sizeMB;
        return true;
    }

    bool free(const std::string& workloadName) {
        auto it = std::find_if(workloads.begin(), workloads.end(),
            [&](const Workload& w) { return w.name == workloadName; });
        if (it == workloads.end()) {
            return false;
        }
        usedMemoryMB -= it->sizeMB;
        workloads.erase(it);
        return true;
    }

    void status() const {
        std::cout << "Azure GPU Memory Manager\n";
        std::cout << "Total GPU memory: " << totalMemoryMB << " MB\n";
        std::cout << "Used GPU memory:  " << usedMemoryMB << " MB\n";
        std::cout << "Free GPU memory:  " << totalMemoryMB - usedMemoryMB << " MB\n";
        std::cout << "Active workloads:\n";
        if (workloads.empty()) {
            std::cout << "  (none)\n";
        } else {
            for (const auto& w : workloads) {
                std::cout << "  " << w.name << " - " << w.sizeMB << " MB\n";
            }
        }
    }

private:
    size_t totalMemoryMB;
    size_t usedMemoryMB;
    std::vector<Workload> workloads;
};

int main() {
    GPUMemoryManager manager(24576); // 24 GB GPU memory

    manager.allocate("AzureAI-Training-Job", 8192);
    manager.allocate("AzureAI-Inference-Task", 4096);
    manager.allocate("Data-Preprocessing", 2048);

    manager.status();

    std::cout << "\nFreeing inference task...\n";
    manager.free("AzureAI-Inference-Task");

    manager.status();

    return 0;
}
