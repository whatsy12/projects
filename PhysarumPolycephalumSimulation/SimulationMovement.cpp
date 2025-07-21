#include<bits/stdc++.h>
using namespace std;
#define M_E 2.7182818284590452354
//https://claude.ai/public/artifacts/542a8337-ede9-4285-b851-1ea2be4cffa2

int MAP_SIZE;
int CUR_X;
int CUR_Y;
vector<vector<int>> MP;
int dx[5] = {0, 1, 0, -1, 0};
int dy[5] = {1, 0, -1, 0, 0};

class mold {
private:
    enum {
        ALPHA = 1,
        BETA = 1,
        MODEL_TYPE = 1
    };
public :
    double MPRandomize() {
        srand(time(0));
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                MP[i][j] = rand() % 11;
                if (MP[i][j] == 10) {
                    MP[i][j] = -1;
                }
            }
        }
        MP[CUR_X][CUR_Y] = max(1, MP[CUR_X][CUR_Y]);
    }

    double Model(int c) {
        switch (MODEL_TYPE) {
            case 1:
                return pow(M_E, (double)(ALPHA * c));
            case 2:
                return pow((double)c, ALPHA);
            default:
                return ALPHA + BETA * c;
        }
    }

    double Probability(int DES_X, int DES_Y) {
        double funct = Model(MP[DES_X][DES_Y]);
        double divisor = 0;
        int SIZE = 0;
        bool flag = false;
        for (int i = 0; i < 5; i++) {
            int x = CUR_X + dx[i];
            int y = CUR_Y + dy[i];
            if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) continue;
            if (MP[x][y] == -1) continue;
            if (DES_X == x && DES_Y == y) {
                flag = true;
            }
            divisor += Model(MP[x][y]);
        }
        if (divisor > 0 && flag) {
            return funct / divisor;
        } else if (flag) {
            return (double)1 / (double)SIZE;
        } else {
            return (double)0;
        }
    }

    void DisplayMP() {
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (MP[i][j] == -1) {
                    cout << "[ ]";
                } else if (i == CUR_X && j == CUR_Y) {
                    cout << "(" << MP[i][j] << ")";
                } else {
                    cout << " " << MP[i][j] << " ";
                }
            }
            cout << "\n";
        }
    }

    void Test() {
        for (int i = 0; i < 5; i++) {
            int x = CUR_X + dx[i];
            int y = CUR_Y + dy[i];
            cout << "P(" << x << ", " << y << ") = " << Probability(x, y) << "\n";
        }
    }
};

//한국어 깨져서 영어씀
int main(void) {
    cout << "Simulation size:\n";
    cin >> MAP_SIZE;
    cout << "Coordinates:\n";
    cin >> CUR_X >> CUR_Y;
    MP.resize(MAP_SIZE, vector<int>(MAP_SIZE));
    mold m;
    cout << "Do you want the map randomized? (y/n)\n";
    char ans;
    cin >> ans;
    if (ans == 'n') {
        cout << "Please input the simulation map:\n";
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                cin >> MP[i][j];
            }
        }
    } else {
        m.MPRandomize();
    }
    m.DisplayMP();
    m.Test();
}
