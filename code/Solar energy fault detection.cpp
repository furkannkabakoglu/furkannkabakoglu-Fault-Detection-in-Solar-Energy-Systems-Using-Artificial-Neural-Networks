#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>

using namespace std;

// Sigmoid fonksiyonu
double sigmoid(double deger)
{
    return 1 / (1 + exp(-deger));
}

// Sigmoid fonksiyonunun türevi
double sigmoidTurevi(double deger)
{
    return deger * (1 - deger);
}

// Ağırlıkları ve biasları güncelleme fonksiyonu
void agirlikVeBiasGuncelle(double &parametre, double hata, double ogrenmeOrani)
{
    parametre += ogrenmeOrani * hata;
}

// İleri yayılım fonksiyonu
void ileriYayilim(double girisler[], double &h1, double &h2, double &cikti, double w[], double b[])
{
    h1 = sigmoid(w[0] * girisler[0] + w[1] * girisler[1] + w[2] * girisler[2] + b[0]);
    h2 = sigmoid(w[3] * girisler[0] + w[4] * girisler[1] + w[5] * girisler[2] + b[1]);
    cikti = sigmoid(w[6] * h1 + w[7] * h2 + b[2]);
}

// Geri yayılım fonksiyonu
void geriYayilim(double girisler[], double h1, double h2, double beklenenCikti, double &cikti, double w[], double b[], double ogrenmeOrani)
{
    // Hata hesaplama
    double ciktiHata = beklenenCikti - cikti;
    double ciktiTurevi = ciktiHata * sigmoidTurevi(cikti);

    // Çıktı katmanı için geri yayılım
    agirlikVeBiasGuncelle(w[6], ciktiTurevi * h1, ogrenmeOrani);
    agirlikVeBiasGuncelle(w[7], ciktiTurevi * h2, ogrenmeOrani);
    agirlikVeBiasGuncelle(b[2], ciktiTurevi, ogrenmeOrani);

    // Gizli katman için geri yayılım
    double h1Turevi = ciktiTurevi * w[6] * sigmoidTurevi(h1);
    double h2Turevi = ciktiTurevi * w[7] * sigmoidTurevi(h2);
    agirlikVeBiasGuncelle(w[0], h1Turevi * girisler[0], ogrenmeOrani);
    agirlikVeBiasGuncelle(w[1], h1Turevi * girisler[1], ogrenmeOrani);
    agirlikVeBiasGuncelle(w[2], h1Turevi * girisler[2], ogrenmeOrani);
    agirlikVeBiasGuncelle(w[3], h2Turevi * girisler[0], ogrenmeOrani);
    agirlikVeBiasGuncelle(w[4], h2Turevi * girisler[1], ogrenmeOrani);
    agirlikVeBiasGuncelle(w[5], h2Turevi * girisler[2], ogrenmeOrani);
}

// Tahmin fonksiyonu
void tahminEt(double giris1, double giris2, double giris3, double agrlk[], double bs[])
{
    double h1, h2, cikti;
    double girisler[3] = {giris1, giris2, giris3};
    ileriYayilim(girisler, h1, h2, cikti, agrlk, bs);
    cout << "Girdi: " << giris1 << ", " << giris2 << ", " << giris3;
    cout << " Tahmin edilen cikis: " << cikti << " (1: Ariza Var, 0: Ariza Yok)" << endl;
}

int main()
{
    srand(time(0)); // Rastgele sayı üreticisini başlat

    // Ağırlıklar ve biaslar
    double w[8];
    double b[3];

    // Rastgele ağırlık ve bias değerlerinin atanması
    for (int i = 0; i < 9; ++i)
    {
        w[i] = (double)rand() / RAND_MAX; // 0 ile 1 arasında rastgele ağırlık değerleri
    }
    for (int i = 0; i < 3; ++i)
    {
        b[i] = (double)rand() / RAND_MAX; // 0 ile 1 arasında rastgele bias değerleriS
    }

    double ogrenmeOrani = 0.5;

    // Eğitim verileri: Girişler ve beklenen çıktılar
    double girisler[13][3] = {
        {0.186, 0.059, 0.215}, {0.221, 0.041, 0.304}, {0.215, 0.043, 0.289},{0.178, 0.065, 0.223}, {0.223, 0.039, 0.312}, {0.198, 0.056, 0.221}, {0.205, 0.055, 0.240}, {0.021, 0.041, 0.304}, {0.180, 0.060, 0.220}, {0.199, 0.044, 0.272},{0.218, 0.057, 0.244},{0.181, 0.053, 0.225},{0.192, 0.052, 0.305},
    };
         
    double beklenenCiktilar[13] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1 };

    // Hata değerlerini saklamak için dizi
    double hatalar[100000]; // 100000 eğitim döngüsü için hata dizisi

    // Eğitim döngüsü
    for (int epoch = 0; epoch < 100000; ++epoch)
    {
        double toplamHata = 0;
        for (int i = 0; i < 6; ++i)
        {
            double h1, h2
, cikti;
            ileriYayilim(girisler[i], h1, h2, cikti, w, b);
            geriYayilim(girisler[i], h1, h2, beklenenCiktilar[i], cikti, w, b, ogrenmeOrani);
            toplamHata += abs(beklenenCiktilar[i] - cikti);
        }
        
        hatalar[epoch] = toplamHata / 6; // Ortalama hatayı kaydet
    }

    // Hata verilerini bir dosyaya yaz
    ofstream dataFile("hatalar.dat");
    if (!dataFile)
    {
        cerr << "Dosya açılamadı!" << endl;
        return 1;
    }
    for (int i = 0; i < 100000; ++i)
    {
        dataFile << i << " " << hatalar[i] << endl;
    }
    dataFile.close();

    // Gnuplot'u çağır ve grafiği çiz
    string command = "gnuplot -p -e \"set terminal wxt; plot 'hatalar.dat' with lines\"";
    system(command.c_str());

    // Test
    cout << "Test sonuclari:" << endl;
    for (int i = 0; i < 13; ++i)
    {
        double h1, h2, cikti;
        ileriYayilim(girisler[i], h1, h2, cikti, w, b);
        cout << "Gunes panelleri voltaji: " << girisler[i][0] << ", gunes panelleri akimi: " << girisler[i][1] << ", hava sicakligi: " << girisler[i][2];
        cout << ", Ariza Tahmini: " << cikti << " (1: Ariza Var, 0: Ariza Yok)" << endl;
    }

    // Kullanıcıdan giriş alarak tahmin yap (tekrar eden döngü)
    while (true)
    {
        double sayi1, sayi2, sayi3;
        cout << "Gunes Panelleri Voltaji: ";
        cin >> sayi1;
        if (sayi1 == -1)
            break;
        cout << "Gunes Panelleri Akimi: ";
        cin >> sayi2;
        cout << "Hava Sicakligi: ";
        cin >> sayi3;
        tahminEt(sayi1, sayi2, sayi3, w, b);
    }

    return 0;
}
