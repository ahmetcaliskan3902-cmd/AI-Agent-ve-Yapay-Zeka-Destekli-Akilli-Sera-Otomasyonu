import json
import pandas as pd
import numpy as np

# 1. Veri Yükleme (Data Loading)
print("--- Veri yukleme baslatildi ---")
with open('../data/sensor_data.json', 'r') as file:
    data = json.load(file)

df = pd.DataFrame(data)
print("Ham Veri Seti:")
print(df.to_string())

# 2. Veri Ön İşleme ve Temizleme (Data Preprocessing)
print("\n--- Veri On Isleme ve Eksik Veri Kontrolu ---")
# Eksik veya hatali NaN degerlerin kontrolu
if df.isnull().values.any():
    print("Eksik veri tespit edildi, temizleniyor...")
    df = df.dropna()
else:
    print("Eksik veya hatali veri bulunmadi. Veri seti temiz.")

# 3. Keşifsel Veri Analizi (EDA) & İstatistiksel Özet
print("\n--- Istatisliksel Ozet Bilgiler ---")
print(df[['sicaklik', 'nem', 'toprak_nemi']].describe())

# 4. Kritik Eşik ve Sınır Analizi (Sera Limit Kontrolleri)
print("\n--- Kritik Esik Deger Analizi ---")
kritik_sicaklik = df[df['sicaklik'] > 30]
print(f"30 derece uzerindeki kritik kayit sayisi: {len(kritik_sicaklik)}")

kritik_toprak = df[df['toprak_nemi'] < 40]
print(f"%%40 altindaki kritik toprak nemi kayit sayisi: {len(kritik_toprak)}")

print("\n--- EDA Islemleri Basariyla Tamamlandi ---")
