#pragma once

#include <stdexcept>

namespace config::filesystem::errors
{
struct FileNotFoundError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
}
