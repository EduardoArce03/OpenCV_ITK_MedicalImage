import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Leer CSV generado por C++
df = pd.read_csv("cmake-build-debug/output_batch/estadisticas.csv")

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
#plt.savefig("output_batch/boxplot_estadisticas.png")
plt.show()
