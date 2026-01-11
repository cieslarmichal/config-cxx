#include <cmath>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace details
{
template <typename T>
int findDecimal(T const& t)
{
    if (t == static_cast<T>(static_cast<int>(t)))
    {
        return 0; // No decimal places
    }

    // Use string representation to count decimal places accurately
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(15) << t;
    std::string str = oss.str();

    size_t dotPos = str.find('.');
    if (dotPos == std::string::npos)
    {
        return 0;
    }

    // Count non-zero trailing digits after decimal point
    size_t lastNonZero = str.find_last_not_of('0');
    if (lastNonZero <= dotPos)
    {
        return 0;
    }

    return static_cast<int>(lastNonZero - dotPos);
}

template <typename T>
std::optional<std::string> to_string(T const& t)
{
    if constexpr (std::is_same_v<T, std::nullptr_t>)
    {
        return "nullptr";
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        return t;
    }
    else if constexpr (std::is_same_v<T, std::vector<std::string>>)
    {
        std::string result = "[";
        for (const auto& str : t)
        {
            result += "\"" + str + "\", ";
        }
        if (!t.empty())
        {
            result.pop_back();
            result.pop_back(); // Remove the trailing comma and space
        }
        result += "]";
        return result;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        // Calculate precision dynamically based on the input value
        int precision = findDecimal(t);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << t;
        return oss.str();
    }
    else
    {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }
}

template <typename T>
std::optional<std::vector<std::string>> to_vector(const T& t)
{
    if constexpr (std::is_same_v<T, std::vector<std::string>>)
        return t;

    std::vector<std::string> result;

    auto strOption = details::to_string(t);

    if (strOption.has_value())
    {
        result.push_back(*strOption);
        return result;
    }

    else
    {
        return std::nullopt;
    }
}

template <typename T, typename U>
std::optional<T> to_optional(U const& u)
{
    if constexpr (std::is_constructible_v<T, U const&>)
        return static_cast<T>(u);
    else
        return std::nullopt;
}
}

namespace config
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

template <typename T>
std::optional<T> cast(ConfigValue const& cv)
{
    return std::visit([](auto const& value) { return details::to_optional<T>(value); }, cv);
}

template <>
inline std::optional<std::string> cast<std::string>(ConfigValue const& cv)
{
    return std::visit([](auto const& value) { return details::to_string(value); }, cv);
}

template <>
inline std::optional<std::vector<std::string>> cast<std::vector<std::string>>(ConfigValue const& cv)
{
    return std::visit([](auto const& value) { return details::to_vector(value); }, cv);
}

} // namespace config
