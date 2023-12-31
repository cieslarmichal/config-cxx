#include "config-cxx/Config.h"

int main()
{
    config::Config config;

    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";
    const std::string awsRegionKey = "aws.region";
    const std::string authExpiresInKey = "auth.expiresIn";
    const std::string authEnabledKey = "auth.enabled";
    const std::string authRolesKey = "auth.roles";

    const auto dbHostValue = config.get<std::string>(dbHostKey);                    // "localhost"
    const auto dbPortValue = config.get<int>(dbPortKey);                            // 2000
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);        // "0987654321"
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);      // "321"
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);              // "eu-central-2"
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);              // 7200
    const auto authEnabledValue = config.get<bool>(authEnabledKey);                 // false
    const auto authRolesValue = config.get<std::vector<std::string>>(authRolesKey); // ["anonymous", "user"]

    return 0;
}
