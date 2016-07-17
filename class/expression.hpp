#pragma once

#include <stdio.h>
#include <vector>
#include "runtime.hpp"

class ExpressionList;
extern Runtime* runtime;

class Expression {
private:
    Atom* atom;
    std::vector<Expression*> *members;
    bool isList;

public:
    Expression(){};

    Expression(Atom* atom) {
        this->atom = atom;
        isList = false;
    }

    Expression(std::vector<Expression*> *members) {
        this->members = members;
        isList = true;
    }

    Atom* getAtom() {
        return atom;
    }

    void dump() {
        if (!isList) {
            printf("%s", atom->toString()->getValue().c_str());
        } else {
            printf("(");
            for (std::vector<Expression*>::iterator it = members->begin(); it != members->end(); ++it) {
                (*it)->dump();
            }
            printf(")\n");
        }
    }

    Atom* eval() {
        if (!isList) {
            return atom;
        } else {
            std::vector<Atom*>* args = new std::vector<Atom*>;
            for (std::vector<Expression*>::iterator it = members->begin(); it != members->end(); ++it) {
                if (it != members->begin()) {
                    if ((*it)->isList) {
                        args->push_back((*it)->eval());
                    } else {
                        args->push_back((*it)->getAtom());
                    }
                }
            }
            if (members->front()->getAtom()->isFunction()) {
                Function* func = (Function*)(members->front()->getAtom());
                return func->call(args);
            }

            Symbol* symbol = (Symbol*)(members->front()->getAtom());
            if (runtime->functionExists(symbol)) {
                return runtime->getFunction(symbol)->call(args);
            }

            throw Exception("Coud not run function");
        }
    }
};

class ExpressionList {
private:
    std::vector<Expression*> *expressions;

public:
    ExpressionList(std::vector<Expression*> *expressions) {
        this->expressions = expressions;
    }

    std::vector<Expression*>* getExpressions() {
        return expressions;
    }

    void dump() {
        for (std::vector<Expression*>::iterator it = expressions->begin(); it != expressions->end(); ++it) {
            (*it)->dump();
        }
    }

    void eval() {
        for (std::vector<Expression*>::iterator it = expressions->begin(); it != expressions->end(); ++it) {
            (*it)->eval();
        }
    }
};