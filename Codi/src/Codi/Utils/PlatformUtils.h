#pragma once

#include <string>

namespace Codi {

class FileDialogs {
public:
    /// @brief 
    /// @param filter 
    /// @return empty string if cancelled
    static std::string OpenFile(const char* filter);
    
    /// @brief 
    /// @param filter 
    /// @return empty string if cancelled
    static std::string SaveFile(const char* filter);
};

} // namespace Codi
