#pragma once

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <map>

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
    virtual bool isFunction() = 0;
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

    bool isFunction() {
        return false;
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
        if (value) {
            return new String("true");
        }
        return new String("false");
    }
};

class List : public Atom {
private:
    std::vector<Atom*>* members;

public:
    List() {
        members = new std::vector<Atom*>;
    }

    List(std::vector<Atom*>* members) {
        this->members = members;
    }

    void setValue(std::vector<Atom*>* members) {
        this->members = members;
    }

    std::vector<Atom*>* getValue() {
        return members;
    }

    bool isPrimitive() {
        return false;
    }

    bool isFunction() {
        return false;
    }

    String* toString() {
        std::string value = "(";
        for (std::vector<Atom*>::iterator it = members->begin(); it != members->end(); ++it) {
            value += (*it)->toString()->getValue().c_str();
            if (it != members->end() - 1) {
                value += " ";
            }
        }
        value += ")";
        return new String(value);
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

    bool isFunction() {
        return true;
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

class DefFunction : public NativeFunction {

public:
    DefFunction() : NativeFunction(new Symbol("def")){
        std::vector<Symbol*>* argv = new std::vector<Symbol*>;
        argv->push_back(new Symbol("name"));
        argv->push_back(new Symbol("value"));
        this->setArgv(argv);
    }

    Atom* call(std::vector<Atom*>* args) {
        if (args->size() == getArgv()->size()) {
            Symbol* name = dynamic_cast<Symbol*>(args->front());
            Atom* value = args->back();

            class Def : public Function{

            public:
                Atom* value;

                Def(Symbol* name, Atom* value) {
                    setName(name);
                    this->value = value;
                }

                Atom* call(std::vector<Atom*>* args) {
                    return value;
                }

            };

            Def* def = new Def(name, value);
            runtime->addFunction(name, def);
            return def;
        }
        throw Exception("Wrong number of arguments passed to def");
    }

};

class LambdaFunction : public NativeFunction {

public:

    LambdaFunction() : NativeFunction(new Symbol("lambda")){
        std::vector<Symbol*>* argv = new std::vector<Symbol*>;
        argv->push_back(new Symbol("args"));
        argv->push_back(new Symbol("func"));
        this->setArgv(argv);
    }

    Atom* call(std::vector<Atom*>* args) {
        if (args->size() == getArgv()->size()) {
            List* funcArgs = dynamic_cast<List*>(args->front());
            Atom* value = args->back();

            class Lambda : public Function {

            public:
                Atom* func;

                Lambda(List* args, Atom* func) {
                    this->func = func;
                }

                Atom* call(std::vector<Atom*>* args) {
                    return func;
                }

            };

            return new Lambda(funcArgs, value);
        }
        throw Exception("Wrong number of arguments passed to lambda");
    }

};

class ListFunction : public NativeFunction {
public:

    ListFunction() : NativeFunction(new Symbol("list")){}

    Atom* call(std::vector<Atom*>* args) {
        return new List(args);
    }
};

class FirstFunction : public NativeFunction {
public:
    FirstFunction() : NativeFunction(new Symbol("first")){
        std::vector<Symbol*>* argv = new std::vector<Symbol*>;
        argv->push_back(new Symbol("list"));
        this->setArgv(argv);
    }

    Atom* call(std::vector<Atom*>* args) {
        if (args->size() == getArgv()->size()) {
            List* list = dynamic_cast<List*>(args->front());
            if (list) {
                return list->getValue()->front();
            }
            return args->front();
        }
        throw Exception("Wrong number of arguments passed to first");
    }
};

class RestFunction : public NativeFunction {
public:
    RestFunction() : NativeFunction(new Symbol("rest")){
        std::vector<Symbol*>* argv = new std::vector<Symbol*>;
        argv->push_back(new Symbol("list"));
        this->setArgv(argv);
    }

    Atom* call(std::vector<Atom*>* args) {
        if (args->size() == getArgv()->size()) {
            List* list = dynamic_cast<List*>(args->front());
            if (list) {
                std::vector<Atom*>* rest = new std::vector<Atom*>;
                for (std::vector<Atom*>::iterator it = list->getValue()->begin() + 1; it != list->getValue()->end(); ++it) {
                    rest->push_back((*it));
                }
                return new List(rest);
            }
            return args->front();
        }
        throw Exception("Wrong number of arguments passed to first");
    }
};

class AddFunction : public NativeFunction {

public:

    AddFunction() : NativeFunction(new Symbol("+")){}

    Atom* call(std::vector<Atom*>* args) {
        double sum = 0;
        for(std::vector<Atom*>::iterator it = args->begin(); it != args->end(); ++it) {
            if ((*it)->isPrimitive()) {
                Double* value = dynamic_cast<Double*>((*it));
                if (value) {
                    sum += value->getValue();
                } else {
                    throw Exception("Value passed to function (+) was not a number");
                }
            }
        }
        return new Double(sum);
    }
};

class SubtractFunction : public NativeFunction {

public:

    SubtractFunction() : NativeFunction(new Symbol("-")){}

    Atom* call(std::vector<Atom*>* args) {
        if (args->size() > 0) {
            Double *initialValue = dynamic_cast<Double *>(*args->begin());
            double sum = initialValue->getValue();
            for (std::vector<Atom *>::iterator it = args->begin() + 1; it != args->end(); ++it) {
                if ((*it)->isPrimitive()) {
                    Double *value = dynamic_cast<Double *>((*it));
                    if (value) {
                        sum -= value->getValue();
                    } else {
                        throw Exception("Value passed to function (-) was not a number");
                    }
                }
            }
            return new Double(sum);
        }
        return new Double(0);
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