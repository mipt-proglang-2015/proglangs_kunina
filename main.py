from xml.dom.minidom import parse
import sys
import csv
import time
from collections import defaultdict
import numpy as np
import argparse
from graph_algorithm import FW

def parse_command_arguments():

    parser = argparse.ArgumentParser()
    parser.add_argument('--input_file', type = str, help = 'input xml file')
    parser.add_argument('--output_file', type = str, help = 'output csv file')
    args = parser.parse_args()
    return args

def parse_input_file(input_xml):

    dom_xml = parse(input_xml)
    schematics = dom_xml.childNodes[0]

    ids = idt = cur_res = cur_inv_res  = n = 0

    for k in schematics.childNodes:
        if k.attributes and k.nodeName == "net":
            n = n + 1

    resistance_matrix = []
    for i in range(n):
        resistance_matrix.append([])
        for j in range(n):
            if i ==j:  
                resistance_matrix[i].append(0.0)
            else:
                resistance_matrix[i].append(np.inf)

    for k in schematics.childNodes:
        if k.attributes and k.nodeName != "net":
            for (x, y) in k.attributes.items():
                if x == "resistance":
                    cur_res = float(y)
                elif x == "net_from":
                    ids = int(y) - 1
                elif x == "net_to":
                    idt = int(y) - 1
                elif x == "reverse_resistance":
                    cur_inv_res = float(y)
            if k.nodeName != "diode":
                cur_inv_res = cur_res
            resistance_matrix[ids][idt] = 1 / (1 / resistance_matrix[ids][idt] + 1 / cur_res)
            resistance_matrix[idt][ids] = 1 / (1 / resistance_matrix[idt][ids] + 1 / cur_inv_res)

    return (resistance_matrix, n)

# def floyd_warshall(resistance_matrix):

#     for m in range(n):
#         for i in range(n):
#             for j in range(n):
#                 if resistance_matrix[i][j] != 0 and (resistance_matrix[i][m] + resistance_matrix[m][j]) != 0:
#                     resistance_matrix[i][j] = 1 / (1 / resistance_matrix[i][j] + 1 / (resistance_matrix[i][m] + resistance_matrix[m][j]))

def full_output_csv(res, output_file):

    with open(output_file, "w") as f:
        for line_n in range(n):
            for column_n in range(n-1):
                f.write(str(round(resistance_matrix[line_n][column_n], 6)) + ", ")
            f.write(str(round(resistance_matrix[line_n][n-1], 6)) + "\n")

if __name__ == "__main__":

    start = time.process_time()

    args = parse_command_arguments()

    (resistance_matrix, n) = parse_input_file(args.input_file)

    resistance_matrix = FW(resistance_matrix)

    full_output_csv(resistance_matrix, args.output_file)

    end = time.process_time() 
    delta = end - start
    print("Python calculations time: {:.9f} ms".format(delta * 1000))