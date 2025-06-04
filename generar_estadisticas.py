import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Leer CSV generado por C++
df = pd.read_csv("output_batch/estadisticasLote.csv")

# Mostrar los primeros datos
print(df.head())

# Crear gráfico de caja (Boxplot)
plt.figure(figsize=(10, 6))
sns.boxplot(data=df[["Media", "Minimo", "Maximo"]])
plt.title("Distribución de Intensidades en Tumor (por Slice)")
plt.ylabel("Intensidad (0-255)")
plt.grid(True)
plt.tight_layout()

# Guardar imagen
plt.savefig("output_batch/boxplot_estadisticas.png")
plt.show()

plt.figure(figsize=(10, 4))
sns.lineplot(x=range(len(df)), y="Area", data=df, marker="o")
plt.title("Área del Tumor por Corte")
plt.xlabel("Slice")
plt.ylabel("Área (número de pixeles)")
plt.grid(True)
plt.tight_layout()
plt.savefig("output_batch/area_tumor.png")

# Cargar intensidades individuales
data = pd.read_csv("output_batch/intensidades_puro.csv", header=None, names=["Intensidad"])

# Boxplot completo
plt.figure(figsize=(8, 5))
sns.boxplot(data["Intensidad"])
plt.title("Distribución de Intensidades del Tumor (Todos los Slices)")
plt.xlabel("Intensidad (0-255)")
plt.grid(True)
plt.tight_layout()
plt.show()


