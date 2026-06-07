# 🌱 AI Agent Destekli Akıllı Sera Otomasyonu

Bu proje, Sakarya Uygulamalı Bilimler Üniversitesi Elektrik-Elektronik Mühendisliği Bölümü **Yapay Zekaya Giriş** dersi kapsamında geliştirilmiş otonom bir Karar Destek Sistemidir.

## 🎯 Projenin Amacı
Geleneksel kural tabanlı sera otomasyonlarının aksine, bu projede IoT sensör verileri (ESP32) ile Büyük Dil Modelleri (LLM) entegre edilmiştir. Sistem, CRISP-DM (Yapay Zeka Proje Yaşam Döngüsü) prensiplerine uygun olarak geliştirilmiş olup, hava durumu gibi bağlamsal verileri kullanarak kaynak optimizasyonu (su ve enerji tasarrufu) sağlamaktadır.

## 📁 Klasör Yapısı
Kılavuza uygun olarak proje dizin yapısı aşağıdaki gibidir:
* `/data`: Sensörlerden elde edilen örnek veri setleri (JSON/CSV).
* `/notebooks`: Keşifsel Veri Analizi (EDA) ve sınır testi scriptleri.
* `/src`: ESP32 mikrodenetleyicisi üzerinde çalışan C++ (.ino) kaynak kodları.
* `/api`: N8N otomasyon platformuna ait dışa aktarılmış Workflow (.json) dosyası.
* `/docs`: 15 Sayfalık Dönem Projesi Teknik Raporu (IEEE Format) ve kullanım belgeleri.

## 🛠 Kullanılan Teknolojiler
* **Donanım:** ESP32, DHT-22 (Sıcaklık/Nem), HL-69 (Toprak Nemi), Röle Modülleri.
* **Yapay Zeka & Otomasyon:** N8N Cloud, Gemini LLM (Zero-shot Inference).
* **Yazılım & Protokoller:** C++, HTTP POST/GET, RESTful API, JSON.

---
*Geliştirici: Ahmet Muhammet ÇALIŞKAN*
