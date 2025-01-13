#include "parser.h"

#include <algorithm>
#include <stdexcept>

ArgParser::ArgParser() : parsed_arguments_(false) {
}

void ArgParser::AddArgument(const std::string& name, Action action) {
    if (name.empty()) {
        throw std::invalid_argument("Argument name cannot be empty\n");
    }
    if (IsPositional(name) && action == Action::Flag) {
        throw std::invalid_argument("Positional argument " + name + " can not have Action::Flag\n");
    }
    if (action == Action::Flag) {
        arguments_[name] = Holder(action);
    } else {
        arguments_[name] = Holder(action);
    }
    if (IsPositional(name)) {
        positional_arguments_.push_back(name);
    }
}

void ArgParser::ParseArguments(int32_t argc, char** argv) {
    if (parsed_arguments_) {
        throw std::runtime_error("Arguments are already parsed\n");
    }
    parsed_arguments_ = true;
    std::reverse(positional_arguments_.begin(), positional_arguments_.end());
    for (size_t i = 1; i < argc; ++i) {
        std::string name;
        Holder* current = nullptr;
        if (!arguments_.contains(argv[i])) {
            if (!positional_arguments_.empty()) {
                name = positional_arguments_.back();
                current = &arguments_[positional_arguments_.back()];
                positional_arguments_.pop_back();
            } else {
                ThrowUnrecognized(argc, argv, name, i);
            }
        } else {
            name = argv[i];
            current = &arguments_[name];
            ++i;
            if (i == argc && arguments_[name].action != Action::Flag) {
                throw std::runtime_error("Argument " + name + ": expected one argument\n");
            }
        }
        called_arguments_.insert(name);
        switch (current->action) {
            case Action::StoreValue:
                current->value = {argv[i]};
                break;
            case Action::Extend:
                i = ParseExtend(argc, argv, i, name);
                --i;
                break;
            case Action::Flag:
                --i;
                break;
            default:;
        }
    }
    CheckRequiredArgumentsParsed();
}

bool ArgParser::CheckParsed(const std::string& name) {
    return CheckArgumentPositioning(name) != ArgumentCheckResult::NotInArguments;
}

std::string ArgParser::GetArgument(const std::string& name) {
    return CheckForPresence(name)[0];
}

std::vector<std::string> ArgParser::GetExtendedArgument(const std::string& name) {
    return CheckForPresence(name);
}

std::vector<std::string> ArgParser::GetAllParsedArguments() const {
    return std::vector<std::string>(called_arguments_.begin(), called_arguments_.end());
}

bool ArgParser::IsPositional(const std::string& name) {
    return name[0] != NonPositionalArgumentSign;
}

ArgParser::ArgumentCheckResult ArgParser::CheckArgumentPositioning(const std::string& name) const {
    if (called_arguments_.contains(name)) {
        return ArgumentCheckResult::Positional;
    }
    if (called_arguments_.contains(NonPositionalArgumentSign + name)) {
        return ArgumentCheckResult::NonPositional;
    }
    return ArgumentCheckResult::NotInArguments;
}

std::vector<std::string> ArgParser::CheckForPresence(const std::string& name) {
    ArgumentCheckResult argument_check_result = CheckArgumentPositioning(name);
    if (argument_check_result == ArgumentCheckResult::NotInArguments) {
        throw std::runtime_error("Argument " + name + " has not been parsed\n");
    }
    std::string positioning;
    if (argument_check_result == ArgumentCheckResult::NonPositional) {
        positioning = NonPositionalArgumentSign;
    }
    return arguments_[positioning + name].value;
}

size_t ArgParser::ParseExtend(int32_t argc, char** argv, size_t i, const std::string& name) {
    for (; i < argc; ++i) {
        if (!arguments_.contains(argv[i])) {
            arguments_[name].value.push_back(argv[i]);
        } else {
            break;
        }
    }
    return i;
}

void ArgParser::ThrowUnrecognized(int32_t argc, char** argv, const std::string& name, size_t i) {
    std::string unrecognized;
    for (; i < argc; ++i) {
        unrecognized += std::string(argv[i]) + " ";
    }
    throw std::runtime_error("Unrecognized arguments: " + unrecognized + "\n");
}

void ArgParser::CheckRequiredArgumentsParsed() {
    if (!positional_arguments_.empty()) {
        std::string required_arguments;
        while (!positional_arguments_.empty()) {
            required_arguments += positional_arguments_.back() + " ";
            positional_arguments_.pop_back();
        }
        throw std::runtime_error("Following arguments are required arguments: " + required_arguments + "\n");
    }
}