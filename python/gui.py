import tkinter as tk
from tkinter import ttk

import xml.etree.ElementTree as ET

import pht_hst

layer_height = 20
node_radius = 20

rawtree = None
tree = None

def drawNode(content, x, y, startNode, resultNode):
    if startNode:
        canvas.create_oval(x-(layer_height/2.0), y-(layer_height/2.0), x+(layer_height/2.0), y+(layer_height/2.0), fill="white", outline="blue")
    elif resultNode:
        canvas.create_oval(x-(layer_height/2.0), y-(layer_height/2.0), x+(layer_height/2.0), y+(layer_height/2.0), fill="white", outline="red")
    else:
        canvas.create_oval(x-(layer_height/2.0), y-(layer_height/2.0), x+(layer_height/2.0), y+(layer_height/2.0), fill="white", outline="black")
    canvas.create_text(x, y, text=content)

def drawSubtree(hstnode, x, width, y, depth, startNode, resultNode, lines):
    '''
    if not lines:
        if startNode != None:
            if resultNode != None:
                pass #drawNode(rawnode.tag, x, y, rawnode == startNode, rawnode == resultNode)
            if False: #rawnode == startNode
                drawNode(rawnode.tag, x, y, True, False)
        elif resultNode != None:
            if False: #rawnode == resultNode
                drawNode(rawnode.tag, x, y, False, True)
        else:
            drawNode(rawnode.tag, x, y, False, False)
    '''
    if not lines:
        drawNode("("+str(hstnode[0])+", "+str(hstnode[1])+", "+str(hstnode[2])+")", x, y, False, False)

    '''childCount = len(rawnode)
    for i, child in enumerate(rawnode):
        if lines:
            canvas.create_line(x, y, x-width/2.0+width/childCount*(0.5+i), 70*(depth+1))
        drawSubtree(child, x-width/2.0+width/childCount*(0.5+i), width/childCount, 70*(depth+1), depth+1, startNode, resultNode, lines)'''
    childCount = tree.degree(hstnode[0], hstnode[1], hstnode[2])
    for i in range(childCount):
        if lines:
            canvas.create_line(x, y, x-width/2.0+width/childCount*(0.5+i), 70*(depth+1))
        child = tree.child(hstnode[0], hstnode[1], hstnode[2], i)
        drawSubtree(child, x-width/2.0+width/childCount*(0.5+i), width/childCount, 70*(depth+1), depth+1, startNode, resultNode, lines)

def drawTree(startNode=None, resultNode=None):
    if rawtree == None:
        return
    if tree == None:
        return
    canvas.delete("all")
    drawSubtree((0,0,0), window.winfo_width()/2.0, window.winfo_width(), 70, 1, startNode, resultNode, True)
    drawSubtree((0,0,0), window.winfo_width()/2.0, window.winfo_width(), 70, 1, startNode, resultNode, False)

def loadTree():
    global rawtree, tree
    rawtree = ET.parse(pathValue.get())
    tree = pht_hst.PyHST(pathValue.get(), False, 12, 4)
    drawTree()

def executeQuery():
    global tree
    if tree == None:
        return

    if queryValue.get() == "isDummyAncestorWithinMiniTree":
        resultValue.set(tree.isDummyAncestorWithinMiniTree(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "isDummyAncestorWithinMicroTree":
        resultValue.set(tree.isDummyAncestorWithinMicroTree(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "child":
        resultValue.set("---")
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), tree.child(int(miniValue.get()), int(microValue.get()), int(nodeValue.get()), int(indexValue.get())))
    elif queryValue.get() == "childRank":
        resultValue.set(tree.childRank(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "getParent":
        resultValue.set("---")
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), tree.getParent(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
    elif queryValue.get() == "degree":
        resultValue.set(tree.degree(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "subtreeSize":
        resultValue.set(tree.subtreeSize(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "depth":
        resultValue.set(tree.depth(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "height":
        resultValue.set(tree.height(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "leftmostLeaf":
        resultValue.set("---")
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), tree.leftmostLeaf(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
    elif queryValue.get() == "rightmostLeaf":
        resultValue.set("---")
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), tree.rightmostLeaf(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
    elif queryValue.get() == "leafSize":
        resultValue.set(tree.leafSize(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)
    elif queryValue.get() == "leafRank":
        resultValue.set(tree.leafRank(int(miniValue.get()), int(microValue.get()), int(nodeValue.get())))
        drawTree((int(miniValue.get()), int(microValue.get()), int(nodeValue.get())), None)

window = tk.Tk()
window.title("HST")

load = ttk.Frame(window, padding=15)
load.pack(side=tk.TOP)
pathValue = tk.StringVar()
pathValue.set("../resources/treeNath.xml")
path = ttk.Frame(load, padding=15)
path.pack(side=tk.LEFT)
ttk.Entry(path, width=150, textvariable=pathValue).pack()
button1 = ttk.Frame(load, padding=15)
button1.pack(side=tk.LEFT)
ttk.Button(button1, text="Load", command=loadTree).pack()

canvas = tk.Canvas(window, width=800, height=450, bg="white")
canvas.pack(expand=True, fill=tk.BOTH)

controls = ttk.Frame(window)
controls.pack(side=tk.BOTTOM)

query = ttk.Frame(controls, padding=15)
query.pack(side=tk.LEFT)
ttk.Label(query, text="Selected query:").pack(side=tk.TOP)
queryValue = tk.StringVar()
queryValue.set("child")
cbb = ttk.Combobox(query, width=35, exportselection=0, textvariable=queryValue, values=["isDummyAncestorWithinMiniTree", "isDummyAncestorWithinMicroTree", "child", "childRank", "getParent", "degree", "subtreeSize", "depth", "height", "leftmostLeaf", "rightmostLeaf", "leafSize", "leafRank"])
cbb.pack(side=tk.BOTTOM)
cbb.state(["readonly"])
queryValue.trace("w", lambda v,i,m: index.state(["!disabled" if queryValue.get() == "child" else "disabled"]))

inputs = tk.LabelFrame(controls, text="Parameters:")
inputs.pack(side=tk.LEFT)

inputs1 = ttk.Frame(inputs, padding=15)
inputs1.pack(side=tk.LEFT)
ttk.Label(inputs1, text="Node:").pack(side=tk.TOP)
miniValue = tk.StringVar()
miniValue.set("1")
microValue = tk.StringVar()
microValue.set("0")
nodeValue = tk.StringVar()
nodeValue.set("0")
ttk.Entry(inputs1, textvariable=miniValue).pack()
ttk.Entry(inputs1, textvariable=microValue).pack()
ttk.Entry(inputs1, textvariable=nodeValue).pack()

inputs2 = ttk.Frame(inputs, padding=15)
inputs2.pack(side=tk.RIGHT)
ttk.Label(inputs2, text="Index:").pack(side=tk.TOP)
indexValue = tk.StringVar()
indexValue.set("0")
index = ttk.Entry(inputs2, textvariable=indexValue)
index.pack()

button2 = ttk.Frame(controls, padding=15)
button2.pack(side=tk.LEFT)
ttk.Button(button2, text="Execute", command=executeQuery).pack()
ttk.Button(button2, text="Redraw", command=drawTree).pack()

outputs = ttk.Frame(controls, padding=15)
outputs.pack(side=tk.LEFT)
resultValue = tk.StringVar()
resultValue.set("---")
ttk.Label(outputs, text="Result:").pack(side=tk.TOP)
ttk.Label(outputs, textvariable=resultValue).pack()

window.mainloop()
'''
    def drawLine(self, x1,y1, x2,y2):
        self.canvas.create_line(x1,y1, x2,y2, tags = "line")    

    def display(self):
        self.canvas.delete("line")
        depth = int(self.depth.get())
        return self.paintBranch(depth, self.width/2, self.height, self.height/3, math.pi/2)

    def paintBranch(self, depth, x1, y1, length, angle):
        if depth >= 0:
            depth -= 1
            x2 = x1 + int(math.cos(angle) * length)
            y2 = y1 - int(math.sin(angle) * length)

            # Draw the line
            self.drawLine(x1,y1, x2,y2)

            # Draw the left branch
            self.paintBranch(depth, x2, y2, length * self.sizeFactor, angle + self.angleFactor  )
            # Draw the right branch
            self.paintBranch(depth, x2, y2, length * self.sizeFactor, angle - self.angleFactor )        
'''