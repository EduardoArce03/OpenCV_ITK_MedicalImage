import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Leer CSV generado por C++
df = pd.read_csv("output_batch/estadisticasLote.csv")

# Mostrar los primeros datos
print(df.head())
# --- Gráfico Media/Mediana vs Slice ---
# Verificamos si existe la columna 'Mediana' (si la exportaste desde C++)
if "Mediana" in df.columns:
    plt.figure(figsize=(10, 6))
    plt.plot(df["Slice"], df["Media"], marker="o", label="Media")
    plt.plot(df["Slice"], df["Mediana"], marker="s", linestyle="--", label="Mediana")
    plt.title("Media y Mediana de Intensidad del Tumor por Slice")
    plt.xlabel("Slice")
    plt.ylabel("Intensidad (0-255)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("output_batch/media_mediana_vs_slice.png")
    plt.show()
else:
    print("⚠️ La columna 'Mediana' no está en estadisticasLote.csv — revisa si la exportaste desde C++.")

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

# grafica_memoria.py
import pandas as pd
import matplotlib.pyplot as plt

# Cargar CSV
df = pd.read_csv("output_batch/uso_memoria.csv")

# Filtrar solo el slice 1 (el que tiene crecimiento)
df_filtrado = df[df["mem_consumida"] > 0]

# Si no hay filas con mem_consumida > 0, tomar igual la primera fila
if df_filtrado.empty:
    df_filtrado = df.iloc[[0]]

# Plot
plt.figure(figsize=(6, 4))
plt.bar(df_filtrado["slice"], df_filtrado["mem_consumida"], color="skyblue")
plt.title("Memoria consumida durante el procesamiento inicial")
plt.xlabel("Slice")
plt.ylabel("Memoria consumida (KB)")
plt.xticks(df_filtrado["slice"])

# Comentario adicional en la gráfica
plt.text(df_filtrado["slice"].values[0], df_filtrado["mem_consumida"].values[0] + 200,
         "Restante slices: sin crecimiento", ha='center', fontsize=9, color='gray')

plt.tight_layout()
plt.savefig("output_batch/grafica_memoria_consumida.png")
plt.show()

# --- Detección de Outliers usando IQR ---
print("Generando CSV de outliers...")

Q1 = data["Intensidad"].quantile(0.25)
Q3 = data["Intensidad"].quantile(0.75)
IQR = Q3 - Q1

lower_bound = Q1 - 1.5 * IQR
upper_bound = Q3 + 1.5 * IQR

outliers = data[(data["Intensidad"] < lower_bound) | (data["Intensidad"] > upper_bound)]
outliers["Tipo"] = outliers["Intensidad"].apply(lambda x: "Bajo" if x < lower_bound else "Alto")

# Exportar a CSV
outliers.to_csv("output_batch/outliers.csv", index=False)
print(f"✅ Outliers encontrados: {len(outliers)}")
print("CSV guardado en output_batch/outliers.csv")

# --- Gráfico de cantidad de outliers por tipo ---
plt.figure(figsize=(6, 4))
sns.countplot(x="Tipo", data=outliers, palette="Set2")
plt.title("Cantidad de Outliers por Tipo")
plt.xlabel("Tipo de Outlier")
plt.ylabel("Cantidad")
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("output_batch/grafica_outliers.png")
plt.show()


