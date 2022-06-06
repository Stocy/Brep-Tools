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
types = [t.split("_")[-1] for t in nt[0, 1:]]
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

fig = plt.figure(figsize = (16, 9))
subfig = fig.subfigures(2,1)
ax1 = subfig[0].subplots()
ax2 = subfig[1].subplots()
# mettre en ordre
ordre = tots.argsort()
tots_sorted = np.sort(tots)[::-1]
types_sorted = [types[i] for i in ordre[::-1]]
# enlever les petites valeurs
thresholdInd= next(i for i, c in enumerate(tots_sorted) if c/tsum<0.02)
others_count = sum(tots_sorted[thresholdInd:])
types_sorted = types_sorted[0:thresholdInd]
tots_sorted = tots_sorted[0:thresholdInd]
# ajouter une catégorie qui regroupe les autres
types_sorted.append('Others')
tots_sorted = np.append(tots_sorted,others_count)



explodeArray = [0]*len(types_sorted)
explodeArray[0] = 0.1
subfig[0].suptitle("Structures les plus utilisées")
ax1.pie(tots_sorted, labels=types_sorted, autopct='%1.2f%%', rotatelabels=False, explode=explodeArray)
subfig[1].suptitle("Taux de présence sur tous les fichiers Step : proportion avec laquelle la structure est présente (au moins une fois) dans les fichiers step")
dict_type_typeProp = {types[i] : types_pres[i] for i in range(len(types))}
sorted_dic = sorted(dict_type_typeProp.items(), key=lambda x: x[1])
ax2.barh([i[0] for i in sorted_dic], [i[1] for i in sorted_dic])

plt.ylabel("Type de structure (Geom_*)")


plt.show()