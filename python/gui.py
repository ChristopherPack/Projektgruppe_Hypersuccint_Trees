import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import math

import pht_hst

node_size = (40, 20)

def hsv_to_hex(H, S, V):
    h = math.floor(H/60.0)
    f = (H/60.0-h)
    p = V*(1-S)
    q = V*(1-S*f)
    t = V*(1-S*(1-f))

    return [
        "#"+hex(int(V*255))[2:].ljust(2,'0')+hex(int(t*255))[2:].ljust(2,'0')+hex(int(p*255))[2:].ljust(2,'0'),
        "#"+hex(int(q*255))[2:].ljust(2,'0')+hex(int(V*255))[2:].ljust(2,'0')+hex(int(p*255))[2:].ljust(2,'0'),
        "#"+hex(int(p*255))[2:].ljust(2,'0')+hex(int(V*255))[2:].ljust(2,'0')+hex(int(t*255))[2:].ljust(2,'0'),
        "#"+hex(int(p*255))[2:].ljust(2,'0')+hex(int(q*255))[2:].ljust(2,'0')+hex(int(V*255))[2:].ljust(2,'0'),
        "#"+hex(int(t*255))[2:].ljust(2,'0')+hex(int(p*255))[2:].ljust(2,'0')+hex(int(V*255))[2:].ljust(2,'0'),
        "#"+hex(int(t*255))[2:].ljust(2,'0')+hex(int(p*255))[2:].ljust(2,'0')+hex(int(V*255))[2:].ljust(2,'0'),
        "#"+hex(int(V*255))[2:].ljust(2,'0')+hex(int(t*255))[2:].ljust(2,'0')+hex(int(p*255))[2:].ljust(2,'0')][h]

micro_colors = [[hsv_to_hex((50*(u+10*m))%360, 1.0, 1.0) for u in range(10)] for m in range(10)]

mini_colors = [hsv_to_hex(i/10.0*360.0, 0.75, 0.75) for i in range(10)]

tree = None

def drawNode(content, x, y, startNode, resultNode):
    outline = "blue" if startNode else ("red" if resultNode else "black")
    background = "#add8e6" if startNode else ("#ff7f7f" if resultNode else "white")
    #hsv_to_hex(240, 0.5, 1)
    thickness = 2 if startNode or resultNode else 1
    canvas.create_oval(x-(node_size[0]/2.0), y-(node_size[1]/2.0), x+(node_size[0]/2.0), y+(node_size[1]/2.0), width=thickness, fill=background, outline=outline)
    canvas.create_rectangle(x-(node_size[0]/2.0), y-(node_size[1]/2.0), x+(node_size[0]/2.0), y+(node_size[1]/2.0), width=thickness, fill=background, outline=outline)
    canvas.create_text(x, y, text=content)

def drawSubtree(hstnode, x, width, y, depth, startNode, resultNode, mode):
    childCount = tree.degree(hstnode[0], hstnode[1], hstnode[2])
    identifiers = [hstnode]

    if mode == "background":
        if depth == 1:
            canvas.create_rectangle(x-width/2.0, y, x+width/2.0, y+99999, fill=mini_colors[0], width=0)

    for i in range(childCount):
        child = tree.child(hstnode[0], hstnode[1], hstnode[2], i)
        identifiers.append(tree.getParent(child[0], child[1], child[2]))
        if mode == "background":
            if child[0] != hstnode[0]:
                bg_x = x-width/2.0+width/childCount*(0.5+i)
                bg_w = width/childCount
                bg_y = 70*(depth+1)
                canvas.create_rectangle(bg_x-bg_w/2.0, bg_y, bg_x+bg_w/2.0, bg_y+99999, fill=mini_colors[(child[0]+1)%len(mini_colors)], width=0)

    
    node_data = "("+str(hstnode[0])+", "+str(hstnode[1])+", "+str(hstnode[2])+")"
    if mode == "nodes":
        if startNode != None:
            if resultNode != None:
                drawNode(node_data, x, y, startNode in identifiers, resultNode in identifiers)

            else:
                drawNode(node_data, x, y, startNode in identifiers, False)
        elif resultNode != None:
            if resultNode in identifiers:
                drawNode(node_data, x, y, False, True)
        else:
            drawNode(node_data, x, y, False, False)

    '''if not lines:
        drawNode("("+str(hstnode[0])+", "+str(hstnode[1])+", "+str(hstnode[2])+")", x, y, False, False)'''

    '''childCount = len(rawnode)
    for i, child in enumerate(rawnode):
        if lines:
            canvas.create_line(x, y, x-width/2.0+width/childCount*(0.5+i), 70*(depth+1))
        drawSubtree(child, x-width/2.0+width/childCount*(0.5+i), width/childCount, 70*(depth+1), depth+1, startNode, resultNode, lines)'''
    
    for i in range(childCount):
        child = tree.child(hstnode[0], hstnode[1], hstnode[2], i)
        if mode == "connections":
            if child[0] == hstnode[0] and child[1] == hstnode[1]:
                canvas.create_line(x, y, x-width/2.0+width/childCount*(0.5+i), 70*(depth+1), width=2, fill=micro_colors[(child[0])%len(mini_colors)][(child[1])%len(mini_colors[0])])
            else:
                canvas.create_line(x, y, x-width/2.0+width/childCount*(0.5+i), 70*(depth+1), fill="black", dash=(3,5))
        drawSubtree(child, x-width/2.0+width/childCount*(0.5+i), width/childCount, 70*(depth+1), depth+1, startNode, resultNode, mode)

def drawTree(startNode=None, resultNode=None):
    if tree == None:
        return
    canvas.delete("all")
    drawSubtree((0,0,0), window.winfo_width()/2.0, window.winfo_width(), 70, 1, startNode, resultNode, "background")
    drawSubtree((0,0,0), window.winfo_width()/2.0, window.winfo_width(), 70, 1, startNode, resultNode, "connections")
    drawSubtree((0,0,0), window.winfo_width()/2.0, window.winfo_width(), 70, 1, startNode, resultNode, "nodes")

def loadTree():
    global tree
    tree = pht_hst.PyHST(pathValue.get(), False)
    drawTree()

def executeQuery():
    global tree
    if tree == None:
        return

    try:
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
    except IndexError:
        messagebox.showerror("Error", "Index Out Of Bounds - Node probably does not exist.")

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