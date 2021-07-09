#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_VISUALIZE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_VISUALIZE_H_

#include <memory>
#include <tuple>
#include <iostream>

#include <Python.h>

#include "pht/unordered_tree.h"
#include "pht/pyhelper.h"

namespace pht {
    class Visualize {
    public:
        template<class T> static void visualize(std::shared_ptr<pht::UnorderedTree<T>> tree) {
            PyHelper::pyCall("visualize", "visualize", [&tree](){

                std::map<std::shared_ptr<Node<T>>, PyObject*> nodeToPyNodeMap;

                uint32_t i = 0;
                PyObject* pyNodes = PyHelper::toPyList<std::shared_ptr<Node<T>>>(tree->nodes, [&nodeToPyNodeMap, &i](std::shared_ptr<Node<T>> node){
                    PyObject* pyNode = PyUnicode_FromString(std::to_string(i++).c_str());
                    nodeToPyNodeMap.insert({node, pyNode});
                    return pyNode;
                });

                
                PyObject* pyAncestors = PyHelper::toPyDict<std::shared_ptr<Node<T>>,std::shared_ptr<Node<T>>>(tree->ancestors, [&nodeToPyNodeMap](std::shared_ptr<Node<T>> key, std::shared_ptr<Node<T>> value){
                    PyObject* pyKey = nodeToPyNodeMap.at(key);
                    PyObject* pyValue = value==nullptr?Py_None:nodeToPyNodeMap.at(value);
                    Py_INCREF(pyKey);
                    Py_INCREF(pyValue);
                    return std::pair<PyObject*,PyObject*>(pyKey, pyValue);
                });

                
                PyObject* pyDescendants = PyHelper::toPyDict<std::shared_ptr<Node<T>>,std::vector<std::shared_ptr<Node<T>>>>(tree->descendants, [&nodeToPyNodeMap](std::shared_ptr<Node<T>> key, std::vector<std::shared_ptr<Node<T>>> value){
                    PyObject* pyKey = nodeToPyNodeMap.at(key);
                    PyObject* pyValue = PyHelper::toPyList<std::shared_ptr<Node<T>>>(value, [&nodeToPyNodeMap](std::shared_ptr<Node<T>> node){
                        PyObject* pyNode = nodeToPyNodeMap.at(node);
                        Py_INCREF(pyNode);
                        return pyNode;
                    });
                    Py_INCREF(pyKey);
                    return std::pair<PyObject*,PyObject*>(pyKey, pyValue);
                });


                return PyTuple_Pack(4, pyNodes, nodeToPyNodeMap.at(tree->root), pyAncestors, pyDescendants);
            },[](PyObject* args){
                PyHelper::deletePyDict(PyTuple_GetItem(args, 3), [](PyObject* key, PyObject* value){
                    Py_DECREF(key);
                    PyHelper::deletePyList(value, [](PyObject* entry){Py_DECREF(entry);});
                });
                PyHelper::deletePyDict(PyTuple_GetItem(args, 2), [](PyObject* key, PyObject* value){Py_DECREF(key);Py_DECREF(value);});
                PyHelper::deletePyList(PyTuple_GetItem(args, 0), [](PyObject* entry){Py_DECREF(entry);});
            });
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_VISUALIZE_H_
