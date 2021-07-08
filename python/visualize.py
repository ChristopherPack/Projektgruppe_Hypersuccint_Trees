def visualize(nodes, root, ancestors, decendants):
    i = 0
    for node in nodes:
        print("node "+str(i)+": "+str(node))
        i = i+1
    print("root: "+str(root))
    for ancestorPair in ancestors.items():
        print("ancPair: ("+str(ancestorPair[0])+", "+str(ancestorPair[1])+")")
    for descendantPair in decendants.items():
        print("descPair: ("+str(descendantPair[0])+", "+str(descendantPair[1])+")")