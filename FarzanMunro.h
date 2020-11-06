//
// Created by Christopher Pack on 06.11.2020.
//

#ifndef PROJEKTSUCCINCTTREES_FARZANMUNRO_H
#define PROJEKTSUCCINCTTREES_FARZANMUNRO_H


#include "TreeElement.h"
#include "TreeStructure.h"
#include <vector>

/**
 * In Java hat das funktioniert. Muss ich nur noch umschreiben
 * todo: CPP PLEASE
 */
class FarzanMunro{
    public:
        static void decompose(TreeElement root, int size);
    private:
        static void greedilyPack(TreeElement root, std::vector<TreeElement> subElements, std::vector<TreeStructure> components, int size);
};


#endif //PROJEKTSUCCINCTTREES_FARZANMUNRO_H
