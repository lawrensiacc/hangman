#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
using namespace std;

int main() {

    string kata[15] = {
        "KUCING", "HARIMAU", "JERAPAH", "KELINCI", "GAJAH",
        "BUAYA", "BERUANG", "SEMANGKA", "JERUK", "MANGGA",
        "NANAS", "DURIAN", "PISANG", "ANGGUR", "INDONESIA"
    };

    srand(time(0));

    int pilih;

    cout << "===== HANGMAN =====" << endl;
    cout << "1. Mulai Permainan" << endl;
    cout << "2. Keluar" << endl;
    cout << "Pilih: ";
    cin >> pilih;

    if (pilih == 1) {

        string secret = kata[rand() % 15];
        string jawaban = "";
        string salah = "";
        char huruf;
        int kesempatan = 6;
        bool menang = false;

        for (int i = 0; i < secret.length(); i++) {
            jawaban += "_ ";
        }

        while (kesempatan > 0) {

            cout << "\n===== HANGMAN =====" << endl;
            cout << "Kata: " << jawaban << endl;
            cout << "Huruf salah: " << salah << endl;
            cout << "Kesempatan: " << kesempatan << endl;

            cout << "Masukkan huruf: ";
            cin >> huruf;
            huruf = toupper(huruf);

            bool benar = false;

            for (int i = 0; i < secret.length(); i++) {
                if (secret[i] == huruf) {
                    jawaban[i * 2] = huruf;
                    benar = true;
                }
            }

            if (benar == false) {

                bool pernah = false;

                for (int i = 0; i < salah.length(); i++) {
                    if (salah[i] == huruf) {
                        pernah = true;
                    }
                }

                if (pernah == false) {
                    salah += huruf;
                    kesempatan--;
                }
                else {
                    cout << "Huruf sudah pernah ditebak!" << endl;
                }
            }

            string hasil = "";

            for (int i = 0; i < jawaban.length(); i++) {
                if (jawaban[i] != ' ') {
                    hasil += jawaban[i];
                }
            }

            if (hasil == secret) {
                menang = true;
                break;
            }
        }

        if (menang == true) {
            cout << "\nSELAMAT! Kamu menang!" << endl;
            cout << "Kata: " << secret << endl;
        }
        else {
            cout << "\nGAME OVER! Kamu kalah!" << endl;
            cout << "Kata yang benar: " << secret << endl;
        }
    }

    else if (pilih == 2) {
        cout << "Terima kasih telah bermain!" << endl;
    }

    else {
        cout << "Pilihan tidak valid!" << endl;
    }

    return 0;
}
