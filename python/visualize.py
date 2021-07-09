from ete3 import Tree, faces, AttrFace, TreeStyle, NodeStyle

def layout(node):
    #if node.is_leaf():
    N = AttrFace("name", fsize=10)
    N.background.color = "White"
    faces.add_face_to_node(N, node, 0, position="float")

def createTree(currentNode, descendants):
    nst = NodeStyle()
    nst["bgcolor"] = "LightSteelBlue"
    nst["size"] = 10
    node = Tree(name=currentNode)
    node.dist = 2
    node.set_style(nst)

    for descendant in descendants[currentNode]:
        node.add_child(createTree(descendant,descendants), dist=2)

    return node


def visualize(nodes, root, ancestors, decendants):
    '''i = 0
    for node in nodes:
        print("node "+str(i)+": "+str(node))
        i = i+1
    print("root: "+str(root))
    for ancestorPair in ancestors.items():
        print("ancPair: ("+str(ancestorPair[0])+", "+str(ancestorPair[1])+")")
    for descendantPair in decendants.items():
        print("descPair: ("+str(descendantPair[0])+", "+str(descendantPair[1])+")")'''

    # Set dashed blue lines in all leaves

    tree = createTree(root,descendants=decendants)

    ts = TreeStyle()
    ts.layout_fn = layout
    ts.force_topology = False
    ts.min_leaf_separation = 50
    ts.show_leaf_name = False
    ts.mode = "r"
    #t.render("node_background.png", w=400, tree_style=ts)
    tree.show(tree_style=ts)

#visualize([],None,{},{})