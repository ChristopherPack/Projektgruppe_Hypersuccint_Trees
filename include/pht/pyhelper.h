#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_PYHELPER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_PYHELPER_H_

#include <functional>
#include <utility>

#include <Python.h>

#include <iostream>
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

namespace pht {
    class DLL_API PyHelper {
    public:
        static void pyCall(const std::string& file, const std::string& function, std::function<PyObject*()> getArguments, std::function<void(PyObject*)> deleteArguments) {
            Py_Initialize();
            PyObject* str = PyUnicode_FromString(file.c_str());
            PyObject* module = PyImport_Import(str);
            if(PyErr_Occurred()) {
                PyErr_Print();
            }
            Py_DECREF(str);

            PyObject* func = PyObject_GetAttrString(module, function.c_str());
            PyObject* args = getArguments();
            PyObject_CallObject(func, args);
            deleteArguments(args);
            Py_DECREF(func);

            Py_DECREF(module);
            Py_Finalize();
        }

        template<class T> static PyObject* toPyList(std::vector<T> vector, std::function<PyObject*(T)> convertEntry) {
            Py_ssize_t size = vector.size();
            PyObject* pyList = PyList_New(size);
            for(Py_ssize_t i = 0; i < size; i++) {
                PyList_SetItem(pyList, i, convertEntry(vector.at(i)));
            }
            return pyList;
        }

        static void deletePyList(PyObject* pyList, std::function<void(PyObject*)> deleteEntry) {
            Py_ssize_t size = PyList_GET_SIZE(pyList);
            for(Py_ssize_t i = 0; i < size; i++) {
                deleteEntry(PyList_GetItem(pyList, i));
            }
            Py_DECREF(pyList);
        }

        template<class K, class V> static PyObject* toPyDict(std::map<K,V> map, std::function<std::pair<PyObject*,PyObject*>(K,V)> convertEntry) {
            PyObject* pyDict = PyDict_New();
            for(const std::pair<K,V>& entry : map) {
                std::pair<PyObject*,PyObject*> pyEntry = convertEntry(entry.first, entry.second);
                PyDict_SetItem(pyDict, pyEntry.first, pyEntry.second);
            }
            return pyDict;
        }

        static void deletePyDict(PyObject* pyDict, std::function<void(PyObject*,PyObject*)> deleteEntry) {
            PyObject* pyDictItems = PyDict_Items(pyDict);
            Py_ssize_t pyDictItemsSize = PyList_GET_SIZE(pyDictItems);
            for(Py_ssize_t i = 0; i < pyDictItemsSize; i++) {
                PyObject* item = PyList_GetItem(pyDictItems, i);
                deleteEntry(PyTuple_GetItem(item, 0), PyTuple_GetItem(item, 1));
            }
            Py_DECREF(pyDictItems);
            Py_DECREF(pyDict);
        }
    };
}

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_PYHELPER_H_