import re

import numpy as np
import scipy.optimize

import matplotlib
import matplotlib.pyplot as plt

file_names = [
    "../results/testResults1.csv",
    "../results/testResults2.csv",
    "../results/testResults3.csv",
    "../results/testResults4.csv",
    "../results/testResults5.csv",
    "../results/testResults6.csv",
    "../results/testResults7.csv",
    "../results/testResults8.csv",
    "../results/testResults9.csv",
    "../results/testResults10.csv",
    "../results/dblp1.csv",
    "../results/dblp3.csv",
    "../results/dblp4.csv",
    "../results/dblp5.csv",
    "../results/dblpWR10.csv"
]
interfile_comparison_labels = [
    "Factory::create", 
    "Factory::create Huffman", 
    "Factory::create NoQueries", 
    "Factory::create Huffman+NoQueries", 
    "Factory::create WriteToFile", 
    "Factory::create ReadFromFile"
]
intrafile_comparison_labels = [
    "size", 
    "child", 
    "degree", 
    "leftmost_leaf", 
    "rightmost_leaf", 
    "child_rank", 
    "subtree_size", 
    "depth", 
    "height", 
    "isDummyAncestorWithinMiniTree", 
    "isDummyAncestorWithinMicroTree", 
    "leaf_size", 
    "leaf_rank"
]
tree_sizes = {
    "TreeNath.xml": 77, 
    "TreeNath2.xml": 837, 
    "TreeNath3.xml": 9197, 
    "TreeNath4.xml": 101157, 
    "TreeNath5.xml": 1112717, 
    "DBLP.xml": 7146530
}



def get_step(min, max, log=False):
    if not log:
        range = max-min
        num = range/np.power(10.0, np.floor(np.log10(range)))
        step = 1.0 if num >= 5.0 else (0.5 if num >= 2.5 else 0.25)
        step *= np.power(10.0, np.floor(np.log10(range)))
        return np.maximum(0.0, np.floor((min-range/8.0)/step)*step), np.ceil((max+range/8.0)/step)*step, step
    else:
        return np.floor(np.log10(min)), np.ceil(np.log10(max)), 1

def get_unit_info(step):
    pow = int(np.floor(np.log10(step)))
    short = { -8: "y", -7: "z", -6: "a", -5: "f", -4: "p", -3: "n", -2: "μ", -1: "m", 0: "", 1: "k", 2: "M", 3: "G", 4: "T", 5: "P", 6: "E", 7: "Z", 8: "Y"}[pow//3]
    if pow%3 == 0:
        return short, pow
    return str(10**(pow%3))+short, pow

def clean_float_str(string):
    match = re.fullmatch(r"([^\.]+\.\d*[123456789])0{5,}\d*(e-\d+)?", string)
    if match != None:
        return match.group(1)+(match.group(2) if match.group(2) != None else "")
    match = re.fullmatch(r"([^\.]*[123456789]\.0)0{4,}\d*(e-\d+)?", string)
    if match != None:
        return match.group(1)+(match.group(2) if match.group(2) != None else "")
    match = re.fullmatch(r"([^\.]+\.\d*)([012345678])9{5,}\d*(e-\d+)?", string)
    if match != None:
        return match.group(1)+str(int(match.group(2))+1)+(match.group(3) if match.group(3) != None else "")
    match = re.fullmatch(r"([^\.]*[012345678])\.9{5,}\d*(e-\d+)?", string)
    if match != None:
        return str(int(match.group(1))+1)+(match.group(2) if match.group(2) != None else "")
    return string

def float_to_latex(value):
    parts = "{:.2e}".format(value).split("e")
    return parts[0]+("" if int(parts[1]) == 0 else "^{"+str(int(parts[1]))+"}")



#Read data
data = {}
for file_name in file_names:
    with open(file_name) as csv_file:
        for line in csv_file:
            entry = re.fullmatch(r"([^,]+),\s*([^,]+),\s*(\d\d):(\d\d):(\d\d).(\d{3})_(\d{3})", line[:-1])
            if entry == None:
                raise Exception("Invalid csv entry: \""+line+"\"")
            methods = data.setdefault(entry.group(1), {})
            times = methods.setdefault(entry.group(2), [])
            time = int(entry.group(3))*(60.0**2)+int(entry.group(4))*60.0+int(entry.group(5))+int(entry.group(6))/1000.0+int(entry.group(7))/(1000.0**2)
            times.append(time)

#Sort data
interfile_comparison_data = {}
intrafile_comparison_data = {}
for file, methods in data.items():
    for method, times in methods.items():
        if method in interfile_comparison_labels:
            interfile_comparison_data.setdefault(method, []).append((file, np.array(times).sum()/len(times)))
        elif method in intrafile_comparison_labels:
            intrafile_comparison_data.setdefault(file, {}).setdefault(method, list(times))
        else:
            raise Exception("Invalid method: \""+method+"\"")

#Render data
x_all = np.array(list(set([tree_sizes[file] for _, times in interfile_comparison_data.items() for file, _ in times])))
y_all = np.array([time for _, times in interfile_comparison_data.items() for _, time in times])
figure = plt.figure(figsize=(20,11))
axes = figure.add_subplot(1,1,1)
axes.set_title(method)
axes.set_xscale("log")
axes.set_xlabel("Nodes")
axes.set_yscale("log")
axes.set_ylabel("s")
axes.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, _: clean_float_str(str(x/(10**y_unit_info_pwr)))))
axes.grid(True, which="both")
xmin, xmax, xstep = get_step(x_all.min(), x_all.max(), True)
ymin, ymax, ystep = get_step(y_all.min(), y_all.max(), True)
y_unit_info_pwr = get_unit_info(np.power(10.0, ymax))[1]
axes.set(xlim=(np.power(10.0, xmin), np.power(10.0, xmax)), xticks=np.power(10.0, np.arange(xmin, xmax+xstep, xstep)), ylim=(np.power(10.0, ymin), np.power(10.0, ymax)), yticks=np.power(10.0, np.arange(ymin, ymax+ystep, ystep)))
for method, times in interfile_comparison_data.items():
    x = np.array([tree_sizes[file] for file, _ in times])
    y = np.array([time for _, time in times])
    function = lambda _x,_a,_b: _a*_x+_b #lambda _x,_a,_b,_c: _a*_x**2+_b*_x+_c#lambda _x,_a,_b,_c: _a*_x**2+_b*_x+_c #TODO
    params = scipy.optimize.curve_fit(function, x, y, maxfev=5000)
    plt.rcParams.update({"text.usetex": True})
    func_str = f"${float_to_latex(params[0][0])} x {'+' if params[0][1] > 0.0 else ''} {float_to_latex(params[0][1])}$"
    axes.scatter(x, y)
    axes.plot(np.power(10.0, np.linspace(xmin, xmax+xstep, 1000)), function(np.power(10.0, np.linspace(xmin, xmax+xstep, 1000)), *params[0]), label=method+" ("+func_str+")")
    plt.rcParams.update({"text.usetex": False})
axes.legend()
plt.savefig("file_compare.png")
plt.close(figure)

#Render data
for file, time_data in intrafile_comparison_data.items():
    raw = np.array([x for _, times in time_data.items() for x in times])
    x = np.array([[x for x in times] for _, times in time_data.items()], dtype=object)
    labels = np.array([method for method, _ in time_data.items()])
    labels[::2] = np.core.defchararray.add("\n",labels[::2])
    figure = plt.figure(figsize=(20,11))
    axes = figure.add_subplot(1,1,1)
    min, max, step = get_step(raw.min(), raw.max())
    unit_info = (get_unit_info(step)[0], get_unit_info(max)[1])
    axes.yaxis.set_minor_formatter(matplotlib.ticker.FuncFormatter(lambda x, _: clean_float_str(str(x/(10**unit_info[1])))))
    axes.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, _: clean_float_str(str(x/(10**unit_info[1])))))
    axes.set_title(file)
    axes.set_ylabel(unit_info[0]+"s")
    axes.grid(True, which="both", axis="y")
    axes.set(ylim=(min, max), yticks=np.arange(min, max+step, step))
    axes.boxplot(x, labels=labels, showfliers=True)
    plt.savefig(f"{file}.png")
    plt.close(figure)