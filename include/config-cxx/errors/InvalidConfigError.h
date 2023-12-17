#pragma once

#include <stdexcept>

namespace config::errors
{
struct InvalidConfigError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
}
