import csv
import numpy as np
import matplotlib.pyplot as plt


csvfile = open("step_folder_stat.csv")
tab = csv.reader(csvfile, delimiter=',')
t = []
for row in tab:
    print(row)
    t.append(row)

nt = np.array(t)
types = nt[0, 1:]
tots = nt[len(nt)-1, 1:].astype(int)
tsum = np.sum(tots)
nb_files = len(nt) - 1
print(nb_files)
print(tsum)
print(types)

types_pres = []
for i in range(len(types)):
    propnz = np.count_nonzero(nt[1:-1, i+1].astype(int)) / nb_files
    print(propnz)
    types_pres.append(propnz)

fig = plt.figure(figsize = (25, 10))
subfig = fig.subfigures(2,1)
ax1 = subfig[0].subplots()
ax2 = subfig[1].subplots()
subfig[0].suptitle("Structures les plus utilisées")
ax1.pie(tots, labels=types, autopct='%1.2f%%')

ax2.bar(types, types_pres, width = 0.4)

plt.xlabel("type de structure")
subfig[1].suptitle("Taux de présence sur tous les fichiers Step : proportion avec laquelle la structure est présente (au moins une fois) dans les fichiers step")
plt.show()
