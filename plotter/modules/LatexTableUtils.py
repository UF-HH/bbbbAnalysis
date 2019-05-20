from __future__ import print_function
import collections

class LatexTable:
    def __init__(self):
        self.tab = []
        self.separator = ' & '
        self.newline = r' \\ '
        self.replaceZero = True
        self.colwidth = "{: >35}"
    def addRow (self, inList):
        self.tab.append(inList)

    def printTab (self):
        for ir, row in enumerate(self.tab):
            for ic, col in enumerate(row):
                last = self.separator if ic != len(row) -1 else self.newline
                # if self.replaceZero:
                #     print (col, ic)
                #     try: num = int(col)
                #     except ValueError:
                #         pass
                #     else:
                #         col = num
                # print (col, end=last)
                
                thistoformat = self.colwidth
                if '\\hline' in col or '\\cline' in col:
                    thistoformat = "{0}"
                    last = ''
                if 'begin{tabular}' in col or 'end{tabular}' in col:
                    last = ''
                    thistoformat = "{0}"
                print (thistoformat.format(col), end=last)
                # print("{: >20} {: >20} {: >20}".format(*row))
            print ('')