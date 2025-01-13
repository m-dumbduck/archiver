#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

enum class Action { StoreValue, Flag, Extend };

class ArgParser {
public:
    ArgParser();

    void AddArgument(const std::string& name, Action action);

    void ParseArguments(int32_t argc, char** argv);

    bool CheckParsed(const std::string& name);

    std::string GetArgument(const std::string& name);

    std::vector<std::string> GetExtendedArgument(const std::string& name);

    std::vector<std::string> GetAllParsedArguments() const;

private:
    static constexpr char NonPositionalArgumentSign = '-';

    enum class ArgumentCheckResult { NotInArguments, Positional, NonPositional };

    struct Holder {
        Action action;
        std::vector<std::string> value;
        Holder() : action(Action::StoreValue) {
        }
        explicit Holder(Action action) : action(action) {
        }
        Holder(Action action, const std::vector<std::string>& value) : action(action), value(value) {
        }
    };

    bool parsed_arguments_;
    std::unordered_set<std::string> called_arguments_;
    std::vector<std::string> positional_arguments_;
    std::unordered_map<std::string, Holder> arguments_;

    static bool IsPositional(const std::string& name);

    ArgumentCheckResult CheckArgumentPositioning(const std::string& name) const;

    std::vector<std::string> CheckForPresence(const std::string& name);

    size_t ParseExtend(int32_t argc, char** argv, size_t i, const std::string& name);

    static void ThrowUnrecognized(int32_t argc, char** argv, const std::string& name, size_t i);

    void CheckRequiredArgumentsParsed();
};
