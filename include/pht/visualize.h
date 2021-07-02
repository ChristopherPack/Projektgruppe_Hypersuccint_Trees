//
// Created by User on 02.07.2021.
//

#ifndef PROJEKTSUCCINCTTREES_VISUALIZE_H
#define PROJEKTSUCCINCTTREES_VISUALIZE_H

#include "pht/hypersuccinct_tree.h"
#include <Python.h>

void visualize()
{
    PyObject *pName, *pModule, *pDict, *pFunc, *pValue;

    // Initialize the Python Interpreter
    Py_Initialize();

    // Build the name object
    pName = PyUnicode_FromString("main");

    // Load the module object
    pModule = PyImport_Import(pName);

    // pDict is a borrowed reference
    pDict = PyModule_GetDict(pModule);

    // pFunc is also a borrowed reference
    pFunc = PyDict_GetItemString(pDict, "print_hi");

    if (PyCallable_Check(pFunc))
    {
        PyObject_CallObject(pFunc, NULL);
    } else
    {
        PyErr_Print();
    }

    // Clean up
    Py_DECREF(pModule);
    Py_DECREF(pName);

    // Finish the Python Interpreter
    Py_Finalize();
}


#endif //PROJEKTSUCCINCTTREES_VISUALIZE_H
