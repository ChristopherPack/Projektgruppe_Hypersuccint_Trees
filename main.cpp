#include <iostream>
#include "TreeStructure.h"

/**
 * todo: CPP PLEASE
 * @return
 */
int main() {
    std::cout << "Hello, World!" << std::endl;

    //create a tree representing the tree showcased in Fig.1
    TreeElement root = new TreeElement(null);
    root.setIdentificator("a");
    TreeStructure testTree = new TreeStructure(root);
    TreeElement t1 = new TreeElement(root);
    t1.setIdentificator("b");
    testTree.addChild(root, t1);
    TreeElement t2 = new TreeElement(root);
    t2.setIdentificator("c");
    testTree.addChild(root, t2);
    TreeElement t3 = new TreeElement(root);
    t3.setIdentificator("d");
    testTree.addChild(root, t3);
    TreeElement t4 = new TreeElement(root);
    t4.setIdentificator("e");
    testTree.addChild(root, t4);
    TreeElement t10 = new TreeElement(root);
    t10.setIdentificator("f");
    testTree.addChild(root, t10);

    TreeElement t5 = new TreeElement(t1);
    t5.setIdentificator("g");
    testTree.addChild(t1,t5);
    TreeElement t6 = new TreeElement(t1);
    t6.setIdentificator("h");
    testTree.addChild(t1,t6);
    TreeElement t7 = new TreeElement(t1);
    t7.setIdentificator("i");
    testTree.addChild(t1,t7);
    TreeElement t8 = new TreeElement(t1);
    t8.setIdentificator("j");
    testTree.addChild(t1,t8);
    TreeElement t11 = new TreeElement(t3);
    t11.setIdentificator("k");
    testTree.addChild(t3,t11);

    TreeElement t9 = new TreeElement(t5);
    t9.setIdentificator("l");
    testTree.addChild(t5,t9);
    TreeElement t26 = new TreeElement(t7);
    t26.setIdentificator("m");
    testTree.addChild(t7,t26);
    TreeElement t12 = new TreeElement(t11);
    t12.setIdentificator("n");
    testTree.addChild(t11,t12);
    TreeElement t13 = new TreeElement(t11);
    t13.setIdentificator("o");
    testTree.addChild(t11,t13);
    TreeElement t14 = new TreeElement(t11);
    t14.setIdentificator("p");
    testTree.addChild(t11,t14);

    TreeElement t15 = new TreeElement(t13);
    t15.setIdentificator("q");
    testTree.addChild(t13,t15);
    TreeElement t16 = new TreeElement(t13);
    t16.setIdentificator("r");
    testTree.addChild(t13,t16);

    TreeElement t17 = new TreeElement(t15);
    t17.setIdentificator("s");
    testTree.addChild(t15,t17);
    TreeElement t18 = new TreeElement(t15);
    t18.setIdentificator("t");
    testTree.addChild(t15,t18);
    TreeElement t19 = new TreeElement(t15);
    t19.setIdentificator("u");
    testTree.addChild(t15,t19);
    TreeElement t20 = new TreeElement(t15);
    t20.setIdentificator("v");
    testTree.addChild(t15,t20);
    TreeElement t21 = new TreeElement(t15);
    t21.setIdentificator("w");
    testTree.addChild(t15,t21);
    TreeElement t23 = new TreeElement(t16);
    t23.setIdentificator("x");
    testTree.addChild(t16,t23);
    TreeElement t24 = new TreeElement(t16);
    t24.setIdentificator("y");
    testTree.addChild(t16,t24);
    TreeElement t25 = new TreeElement(t16);
    t25.setIdentificator("z");
    testTree.addChild(t16,t25);

    TreeElement t22 = new TreeElement(t18);
    t22.setIdentificator("alpha");
    testTree.addChild(t18,t22);
    TreeElement t27 = new TreeElement(t23);
    t27.setIdentificator("beta");
    testTree.addChild(t23,t27);

    //todo: IT IS TIME TO FAIL
    //this should provide the SAME solution as Fig.1
    ArrayList<TreeStructure> subTrees = new ArrayList<TreeStructure>();
    subTrees = FarzanMunro.decompose(root,5);
    System.out.println("EVALUATION");
    System.out.println(subTrees.size());
    for(TreeStructure subTree : subTrees) {
        System.out.println("\n");
        System.out.println(subTree.getRoot().getIdentificator());
        System.out.println(subTree.getElements().size());
        System.out.println(subTree.isFinalized());
    }

    return 0;
}
