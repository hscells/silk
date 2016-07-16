#pragma once

#include <stdlib.h>
#include <string>
#include <map>
#include "expression.hpp"
#include "runtime.hpp"

using namespace std;

class String;
class Double;
class Bool;
class Symbol;
class Function;
class Runtime;

class Atom {
public:
    virtual String* toString() = 0;
    virtual bool isPrimitive() = 0;
};

extern Runtime* runtime;

template <class T>
class Primitive : public Atom {
public:
    virtual T getValue() = 0;
    virtual void setValue(T value) = 0;

    bool isPrimitive() {
        return true;
    }
};

class String : public Primitive<std::string> {
private:
    std::string value;

public:
    String(std::string value) {
        this->value = value;
    }

    String() {
        this->value = std::string();
    }

    String(String* value) {
        this->value = value->getValue();
    }

    void setValue(std::string value) {
        this->value = value;
    }

    std::string getValue() {
        return value;
    }

    String* toString() {
        return this;
    }
};

class Symbol : public Primitive<std::string> {
private:
    std::string value;

public:
    Symbol(std::string value) {
        this->value = value;
    }

    void setValue(std::string value) {
        this->value = value;
    }

    std::string getValue() {
        return value;
    }

    String* toString() {
        return new String(value);
    }
};

class Double : public Primitive<double> {
private:
    double value;

public:
    Double(double value) {
        this->value = value;
    }

    Double() {
        this->value = 0.0;
    }

    void setValue(double value) {
        this->value = value;
    }

    double getValue() {
        return value;
    }

    String* toString() {
        return new String(std::to_string(value));
    }
};

class Bool : public Primitive<bool> {
private:
    bool value;

public:
    Bool(bool value) {
        this->value = value;
    }

    Bool() {
        this->value = 0.0;
    }

    void setValue(bool value) {
        this->value = value;
    }

    bool getValue() {
        return value;
    }

    String* toString() {
        return new String("todo");
    }
};

class Exception : public std::exception {
public:
    Exception(char* message) {
        printf("%s\n", message);
        exit(1);
    }
};

class Runtime {
private:
    std::map<std::string, Function*>* functions;
public:

    Runtime() {
        this->functions = new std::map<std::string, Function*>;
    }

    bool functionExists(Symbol* symbol) {
        std::map<std::string, Function*>::iterator it;
        it = functions->find(symbol->getValue());
        return it != functions->end();
    }

    Function* getFunction(Symbol* symbol) {
        std::map<std::string, Function*>::iterator it;
        it = functions->find(symbol->getValue());
        if (it != functions->end()) {
            return (*it).second;
        }
        throw Exception("function has not been declared");
    }

    void addFunction(Symbol* name, Function* func) {
        functions->insert(std::make_pair(name->getValue(), func));
    }

    long getFunctionSize() {
        return functions->size();
    }

};

class Function : public Atom {
private:
    std::vector<Symbol*> *argv;
    Symbol *name;

public:
    virtual Atom* call(std::vector<Atom*>* args) = 0;

    bool isPrimitive() {
        return false;
    }

    String* toString() {
        return new String(name->toString());
    }

    Symbol* getName() {
        return name;
    }

    void setName(Symbol* name){
        this->name = name;
    }

    std::vector<Symbol*>* getArgv() {
        return this->argv;
    }

    void setArgv(std::vector<Symbol*>* argv) {
        this->argv = argv;
    }
};

class NativeFunction : public Function {
public:
    NativeFunction(Symbol* name) {
        setName(name);
        runtime->addFunction(name, this);
    }

    Atom* call(std::vector<Atom*>* args) {
        return this->call(args);
    }
};

class AddFunction : public NativeFunction {

public:

    AddFunction() : NativeFunction(new Symbol("+")){}

    Atom* call(std::vector<Atom*>* args) {
        double sum;
        for(std::vector<Atom*>::iterator it = args->begin(); it != args->end(); ++it) {
            if ((*it)->isPrimitive()) {
                sum += dynamic_cast<Double*>((*it))->getValue();
            }
        }
        return new Double(sum);
    }
};

class PrintlnFunction : public NativeFunction {
public:

    PrintlnFunction() : NativeFunction(new Symbol("println")){}

    Atom* call(std::vector<Atom*>* args) {
        std::string value = "";
        for(std::vector<Atom*>::iterator it = args->begin(); it != args->end(); ++it) {
                value += (*it)->toString()->getValue();
        }
        printf("%s\n", value.c_str());
        return new String(value);
    }
};