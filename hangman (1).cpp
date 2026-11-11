#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace cfg {
constexpr int kMaxWrong = 6;      
constexpr int kAlphabetSize = 26; 
}  

struct WordEntry {
    string word;
    string category; 
};

class WordBank {
public:
    WordBank()
        : entries_{
              {"KUCING", "Hewan"},
              {"HARIMAU", "Hewan"},
              {"JERAPAH", "Hewan"},
              {"KELINCI", "Hewan"},
              {"GAJAH", "Hewan"},
              {"BUAYA", "Hewan"},
              {"BERUANG", "Hewan"},
              {"SEMANGKA", "Buah"},
              {"JERUK", "Buah"},
              {"MANGGA", "Buah"},
              {"NANAS", "Buah"},
              {"DURIAN", "Buah"},
              {"PISANG", "Buah"},
              {"ANGGUR", "Buah"},
              {"INDONESIA", "Negara"},
              {"JEPANG", "Negara"},
              {"BRAZIL", "Negara"},
              {"MESIR", "Negara"},
              {"KANADA", "Negara"},
              {"PERANCIS", "Negara"},
              {"AUSTRALIA", "Negara"},
              {"NORWEGIA", "Negara"},
              {"DOKTER", "Profesi"},
              {"GURU", "Profesi"},
              {"PILOT", "Profesi"},
              {"ARSITEK", "Profesi"},
              {"PETANI", "Profesi"},
              {"KOKI", "Profesi"},
              {"RENDANG", "Makanan"},
              {"BAKSO", "Makanan"},
              {"MARTABAK", "Makanan"},
              {"KETUPAT", "Makanan"},
              {"TEMPE", "Makanan"},
              {"BADMINTON", "Olahraga"},
              {"BASKET", "Olahraga"},
              {"RENANG", "Olahraga"},
              {"SEPAKBOLA", "Olahraga"},
              {"GITAR", "Alat Musik"},
              {"ANGKLUNG", "Alat Musik"},
              {"PIANO", "Alat Musik"},
              {"SERULING", "Alat Musik"},
              {"PAYUNG", "Benda"},
              {"SEPEDA", "Benda"},
              {"KOMPUTER", "Benda"},
              {"KACAMATA", "Benda"},
              {"LEMARI", "Benda"}},
          rng_(random_device{}()) {}

    const WordEntry& PickRandom() {
        uniform_int_distribution<size_t> dist(0, entries_.size() - 1);
        size_t index = dist(rng_);
        while (entries_.size() > 1 && index == lastIndex_) {
            index = dist(rng_);
        }
        lastIndex_ = index;
        return entries_[index];
    }

private:
    vector<WordEntry> entries_;
    mt19937 rng_;
    size_t lastIndex_ = static_cast<size_t>(-1);
};

enum class Phase { Playing, Won, Lost };
enum class GuessResult { Ignored, Correct, Wrong };

class GameState {
public:
    void StartRound(const WordEntry& entry) {
        word_ = entry.word;
        category_ = entry.category;
        guessed_.fill(false);
        wrongLetters_.clear();
        phase_ = Phase::Playing;
    }

    GuessResult Guess(char letter) {
        if (phase_ != Phase::Playing || letter < 'A' || letter > 'Z') {
            return GuessResult::Ignored;
        }
        const size_t idx = static_cast<size_t>(letter - 'A');
        if (guessed_[idx]) {
            return GuessResult::Ignored;
        }
        guessed_[idx] = true;

        if (word_.find(letter) != string::npos) {
            if (AllRevealed()) {
                phase_ = Phase::Won;
                ++wins_;
            }
            return GuessResult::Correct;
        }

        wrongLetters_.push_back(letter);
        if (wrongCount() >= cfg::kMaxWrong) {
            phase_ = Phase::Lost;
            ++losses_;
        }
        return GuessResult::Wrong;
    }

    bool IsGuessed(char letter) const {
        return letter >= 'A' && letter <= 'Z' &&
               guessed_[static_cast<size_t>(letter - 'A')];
    }

    Phase phase() const { return phase_; }
    const string& word() const { return word_; }
    const string& category() const { return category_; }
    int wrongCount() const { return static_cast<int>(wrongLetters_.size()); }
    int livesLeft() const { return cfg::kMaxWrong - wrongCount(); }
    const vector<char>& wrongLetters() const { return wrongLetters_; }
    int wins() const { return wins_; }
    int losses() const { return losses_; }

private:
    bool AllRevealed() const {
        return all_of(word_.begin(), word_.end(),
                           [this](char c) { return IsGuessed(c); });
    }

    string word_;
    string category_;
    array<bool, cfg::kAlphabetSize> guessed_{};
    vector<char> wrongLetters_;
    Phase phase_ = Phase::Playing;
    int wins_ = 0;
    int losses_ = 0;
};


class Renderer {
public:

    void DrawGallows(const GameState& g) const {
        const int w = g.wrongCount();
        const bool lost = g.phase() == Phase::Lost;
        const string head = (w < 1) ? "     " : (lost ? "(x_x)" : "( ^ )");
        const string lArm = (w >= 3) ? "/" : " ";
        const string body = (w >= 2) ? "|" : " ";
        const string rArm = (w >= 4) ? "\\" : " ";
        const string lLeg = (w >= 5) ? "/" : " ";
        const string rLeg = (w >= 6) ? "\\" : " ";

        cout << "\n   +-----+\n"
                  << "   |     |\n"
                  << "   |   " << (w >= 1 ? head : "     ") << "\n"
                  << "   |    " << lArm << body << rArm << "\n"
                  << "   |    " << lLeg << " " << rLeg << "\n"
                  << "   |\n"
                  << "  =====\n\n";
    }

    void DrawWord(const GameState& g) const {
        cout << "   Kata: ";
        for (const char c : g.word()) {
            if (g.IsGuessed(c)) {
                cout << c << ' ';
            } else if (g.phase() == Phase::Lost) {
                cout << '[' << c << ']';
            } else {
                cout << "_ ";
            }
        }
        cout << "\n";
    }

    void DrawStatus(const GameState& g) const {
        cout << "   Kategori   : " << g.category() << "\n"
                  << "   Sisa nyawa : " << g.livesLeft() << " / "
                  << cfg::kMaxWrong << "  ";
        for (int i = 0; i < cfg::kMaxWrong; ++i) {
            cout << (i < g.livesLeft() ? "<3 " : "x  ");
        }
        cout << "\n   Huruf salah: ";
        if (g.wrongLetters().empty()) {
            cout << "-";
        }
        for (const char c : g.wrongLetters()) {
            cout << c << ' ';
        }
        cout << "\n   Skor sesi  : Menang " << g.wins() << "  |  Kalah "
                  << g.losses() << "\n";
    }

    void DrawKeyboard(const GameState& g) const {
        cout << "\n   Papan huruf:\n   ";
        for (int i = 0; i < cfg::kAlphabetSize; ++i) {
            const char c = static_cast<char>('A' + i);
            if (!g.IsGuessed(c)) {
                cout << ' ' << c << ' ';
            } else if (g.word().find(c) != string::npos) {
                cout << '[' << c << ']';
            } else {
                cout << '(' << c << ')';
            }
            if (i == 12) cout << "\n   ";
        }
        cout << "\n";
    }

    void DrawScreen(const GameState& g) const {
        cout << "\n=========================================\n"
                  << "                HANGMAN\n"
                  << "=========================================\n";
        DrawGallows(g);
        DrawWord(g);
        cout << "\n";
        DrawStatus(g);
        if (g.phase() == Phase::Playing) {
            DrawKeyboard(g);
        }
    }

    void DrawResult(const GameState& g) const {
        if (g.phase() == Phase::Won) {
            cout << "\n   *** SELAMAT, KAMU MENANG! ***\n";
        } else {
            cout << "\n   *** YAH, KAMU KALAH! ***\n"
                      << "   Katanya: " << g.word() << "\n";
        }
    }
};

static bool ReadLine(string& line) {
    return static_cast<bool>(getline(cin, line));
}

static char ParseLetter(const string& line) {
    string trimmed;
    for (const char c : line) {
        if (!isspace(static_cast<unsigned char>(c))) trimmed += c;
    }
    if (trimmed.size() != 1) return 0;
    const unsigned char u = static_cast<unsigned char>(trimmed[0]);
    if (!isalpha(u)) return 0;
    return static_cast<char>(toupper(u));
}

static bool AskPlayAgain() {
    string line;
    while (true) {
        cout << "\n   Main lagi? (y = ya, n = keluar): " << flush;
        if (!ReadLine(line)) return false;
        const char c = ParseLetter(line);
        if (c == 'Y') return true;
        if (c == 'N') return false;
        cout << "   Ketik y atau n.\n";
    }
}


int main() {
    WordBank bank;
    GameState game;
    const Renderer renderer;

    bool running = true;
    while (running) {
        game.StartRound(bank.PickRandom());
        string message;

        while (game.phase() == Phase::Playing) {
            renderer.DrawScreen(game);
            if (!message.empty()) cout << "\n   " << message << "\n";

            cout << "\n   Tebak satu huruf: " << flush;
            string line;
            if (!ReadLine(line)) {
                running = false;
                break;
            }

            const char letter = ParseLetter(line);
            if (letter == 0) {
                message = "Masukkan satu huruf A-Z saja.";
                continue;
            }
            switch (game.Guess(letter)) {
                case GuessResult::Ignored:
                    message = string("Huruf ") + letter +
                              " sudah pernah ditebak (nyawa tidak berkurang).";
                    break;
                case GuessResult::Correct:
                    message = string("Benar! Huruf ") + letter + " ada.";
                    break;
                case GuessResult::Wrong:
                    message = string("Salah! Huruf ") + letter +
                              " tidak ada.";
                    break;
            }
        }
        if (!running) break;

        renderer.DrawScreen(game);
        renderer.DrawResult(game);
        running = AskPlayAgain();
    }

    cout << "\n   Sampai jumpa! Skor akhir: Menang " << game.wins()
              << ", Kalah " << game.losses() << ".\n";
    return 0;
}