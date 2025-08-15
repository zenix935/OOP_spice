#pragma once
#include <stdexcept>
#include <string>

class CircuitError : public std::runtime_error
{
public:
    explicit CircuitError(const std::string& message) : std::runtime_error("Circuit Error: "+message) {}
};

class DuplicateNameError : public CircuitError
{
public:
    explicit DuplicateNameError(const std::string& name,const std::string& type)
        : CircuitError("'"+name+"' ("+type+") already exists. Please choose a unique name.") {
    }
};

class ElementNotFoundError : public CircuitError
{
public:
    explicit ElementNotFoundError(const std::string& name) : CircuitError("Element '"+name+"' not found in the circuit.") {}
};

class NodeNotFoundError : public CircuitError
{
public:
    explicit NodeNotFoundError(const std::string& name) : CircuitError("Node '"+name+"' not found in the circuit.") {}
};

class UnsupportedTypeError : public CircuitError
{
public:
    explicit UnsupportedTypeError(const std::string& type) : CircuitError("Unsupported element type: '"+type+"'.") {}
};

class InvalidValueError : public CircuitError
{
public:
    explicit InvalidValueError(const std::string& message) : CircuitError("Invalid value: "+message) {}
};

class MatrixError : public CircuitError
{
public:
    explicit MatrixError(const std::string& message) : CircuitError("Matrix Error: "+message) {}
};

class SyntaxError : public CircuitError
{
public:
    explicit SyntaxError(const std::string& message) : CircuitError("Syntax Error: "+message) {}
};