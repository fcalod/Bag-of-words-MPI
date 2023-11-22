import csv
import pandas as pd
import numpy as np
import re
import nltk
import matplotlib.pyplot as plt
from sklearn.feature_extraction.text import CountVectorizer

etiquetas = ["0_shakespeare_the_merchant_of_venice",
             "1_shakespeare_romeo_juliet",
             "2_shakespeare_hamlet",
             "3_dickens_a_christmas_carol",
             "4_dickens_oliver_twist",
             "5_dickens_a_tale_of_two_cities"]
corpus = []

for etiqueta in etiquetas:
  archivo = open("sample_data/" + etiqueta + ".txt", "r")
  corpus.append(archivo.read())
  archivo.close()

corpus = np.array(corpus)
df_corpus = pd.DataFrame({"documento": corpus, "categoria": etiquetas})
print(df_corpus)


# bolsa de palabras en matriz dispersa
count_vectorizer = CountVectorizer(min_df=0.0, max_df=1.0)
matriz_conteo = count_vectorizer.fit_transform(corpus)
print(matriz_conteo)
# ver valores diferentes de cero en la matriz dispersa
print(matriz_conteo)
# ver la representación densa
matriz_conteo = matriz_conteo.toarray()
print(matriz_conteo)

# obten todas las palabras únicas del corpus
vocabulario = count_vectorizer.get_feature_names_out()
# muestra los vectores de características del documento
df = pd.DataFrame(matriz_conteo, columns=vocabulario)
# guarda el vocabulario en un csv
df.to_csv("vocab.csv")

# Tamaño del vocabulario
print(len(vocabulario), vocabulario)

# Escribe el tamaño del vocabulario
with open("vocab_size.csv", 'w') as file:
	writer = csv.writer(file)
	writer.writerow([len(vocabulario)])

