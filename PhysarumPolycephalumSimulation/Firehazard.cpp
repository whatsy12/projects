#include<bits/stdc++.h>
#include<random>
#include<chrono>
#include<thread>
using namespace std;
#define M_E 2.7182818284590452354
//점균이 화학 구배를 따라 움직이며 자연스럽게 미로를 푸는 과정을 수치 모델(화학주성 기반) -> Probability 함수의 수학적 뒷받침 (농도 기반으로 움직여서)
//https://arxiv.org/abs/1108.4956
//케모택시(화학주성)를 기반으로 군집 및 경로 선택 행동을 수학적으로 정리함. 군집도 계산 방법에 응용 가능 -> Model 함수의 ALPHA와 BETA 같은 점균의 속도 등으로 군집 설명
//https://arxiv.org/abs/1601.01137
//네트워크 형성과 군집의 생성 과정을 다중 단계로 분리하여 정량적으로 분석함. 중심 집중도나 군집 정량화 접근으로 참고 가능 -> 중심과의 거리를 계산하는 것을 ClustingIndex 함수에 씀
//https://www.nature.com/articles/s41598-022-05439-w

int MAP_SIZE;
int CUR_X;
int CUR_Y;
int MODEL_TYPE;
double ALPHA = 0.45;
double BETA = 0.3;
vector<vector<int>> MP;
vector<vector<int>> FOOD;
vector<int> ESCAPE_X;
vector<int> ESCAPE_Y;
int dx[5] = {0, 1, 0, -1, 0};
int dy[5] = {1, 0, -1, 0, 0};

class mold {
public :
    //지수, 멱승, 선형
    void Init() {
        switch (MODEL_TYPE) {
            case 1:
                ALPHA = -0.45;
                break;
            case 2:
                ALPHA = -1.75;
                break;
            default:
                ALPHA = 1;
                BETA = -0.45;
                break;
        }
    }
    void MPRandomize() {
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
                return ALPHA + BETA * (double)c;
        }
    }

    int Heuristic(int x, int y) {
        int res = INT_MAX;
        for (int i = 0; i < ESCAPE_X.size(); i++) {
            res = min(res, abs(ESCAPE_X[i] - x) + abs(ESCAPE_Y[i] - y));
        }
        return res;
    }

    double Probability(int DES_X, int DES_Y) {
        double funct = max((double)0, Model(MP[DES_X][DES_Y] + FOOD[DES_X][DES_Y] - Heuristic(DES_X, DES_Y)));
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
            divisor += max((double)0, Model(MP[x][y] + FOOD[x][y] - Heuristic(DES_X, DES_Y)));
        }
        if (divisor > 0 && flag) {
            return funct / divisor;
        } else if (flag) {
            return (double)1 / (double)SIZE;
        } else {
            return (double)0;
        }
    }

    double ClusteringIndex() {
        double total = 0;
        vector<pair<int, int>> coords;
        for (int i = 0; i < MAP_SIZE; ++i) {
            for (int j = 0; j < MAP_SIZE; ++j) {
                if (MP[i][j] <= 0) continue;
                for (int c = 0; c < MP[i][j]; ++c) {
                    coords.push_back({i, j});
                }
            }
        }
        if (coords.size() <= 1) return 1.0;
        double cx = 0, cy = 0;
        for (int i = 0; i < coords.size(); i++) {
            cx += coords[i].first;
            cy += coords[i].second;
        }
        cx /= coords.size();
        cy /= coords.size();
        double distSum = 0;
        for (int i = 0; i < coords.size(); i++) {
            int x = coords[i].first;
            int y = coords[i].second;
            distSum += sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
        }
        distSum /= coords.size();
        double norm = sqrt(MAP_SIZE * MAP_SIZE);
        return 1.0 - (distSum / norm);
    }

    void DisplayMP() {
        cout << "\n";
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                if (MP[i][j] == -1) {
                    cout << "[ ]";
                    continue;
                }
                cout << " " << MP[i][j] << " ";
            }
            cout << "\n";
        }
        double cluster = ClusteringIndex();
        cout << "\nClustering Index: " << cluster << "\n";
        cout << "=========================================\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    void Simulation(int t) {
        if (t <= 0) {
            return;
        }
        vector<vector<int>> CUR_MP(MAP_SIZE, vector<int>(MAP_SIZE));
        CUR_MP = MP;
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                CUR_X = i;
                CUR_Y = j;

                priority_queue<pair<int, pair<double, pair<int, int>>>> pq;
                if (MP[i][j] == -1) continue;
                for (int v = 0; v < 5; v++) {
                    int x = CUR_X + dx[v];
                    int y = CUR_Y + dy[v];
                    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) continue;
                    if (MP[x][y] == -1) continue;
                    double res = Probability(x, y);
                    if (res == 0) continue;
                    pq.push({-Heuristic(x, y), {res, {x, y}}});
                }
                int x = pq.top().second.second.first;
                int y = pq.top().second.second.second;
                CUR_MP[x][y] += MP[i][j];
                CUR_MP[i][j] -= MP[i][j];
                cout << i << ", " << j << " -> " << x << ", " << y << "\n";
            }
        }
        MP = CUR_MP;
        DisplayMP();
        Simulation(t - 1);
    }
};

int main(void) {
    srand(time(0));
    cout << "Simulation size:\n";
    cin >> MAP_SIZE;
    MP.resize(MAP_SIZE, vector<int>(MAP_SIZE));
    FOOD.resize(MAP_SIZE, vector<int>(MAP_SIZE));
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
        cout << "Please input the food map:\n";
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                cin >> FOOD[i][j];
                if (FOOD[i][j] == 1) {
                    FOOD[i][j] = INT_MIN;
                    ESCAPE_X.push_back(i);
                    ESCAPE_Y.push_back(j);
                }
            }
        }
    } else {
        m.MPRandomize();
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                FOOD[i][j] = rand() % 10;
                if (FOOD[i][j] == 1) {
                    cout << FOOD[i][j] << " ";
                    FOOD[i][j] = INT_MIN;
                    if (MP[i][j] == -1) {
                        MP[i][j] = 0;
                    }
                    ESCAPE_X.push_back(i);
                    ESCAPE_Y.push_back(j);
                } else {
                    FOOD[i][j] = 0;
                    cout << FOOD[i][j] << " ";
                }
            }
            cout << "\n";
        }
    }
    cout << "Please input the simulation model type. (1/2/3)\n";
    cin >> MODEL_TYPE;
    cout << "Please input the steps:\n";
    int steps;
    cin >> steps;
    m.Init();
    m.DisplayMP();
    m.Simulation(steps);
}