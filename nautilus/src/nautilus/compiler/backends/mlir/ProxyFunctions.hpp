
#include <cstdint>
#include <cstdio>
#include <string>
#include <unordered_map>

inline __attribute__((always_inline)) void printValueFromMLIR(uint64_t) {
}

namespace NES {
class ProxyFunctions {
public:
	ProxyFunctions() {
		functionNameToAddressMap.emplace(std::pair {"printValueFromMLIR", (void*) &printValueFromMLIR});
	}
	~ProxyFunctions() = default;
	void* getProxyFunctionAddress(std::string name) {
		return functionNameToAddressMap[name];
	}

private:
	std::unordered_map<std::string, void*> functionNameToAddressMap;
};
} // namespace NES
