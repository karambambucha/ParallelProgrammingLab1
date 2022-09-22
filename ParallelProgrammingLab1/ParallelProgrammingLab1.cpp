#include <iostream>
#include <iomanip>
#include <chrono>
#include "pthread.h"
#include "sched.h"
#include "semaphore.h"
#pragma comment(lib,"pthreadVCE2.lib")
using namespace std;
int koef[10][10] = { 1,0,0,0,0,0,0,0,0,0,
                        1,1,0,0,0,0,0,0,0,0,
                        1,4,1,0,0,0,0,0,0,0,
                        1,3,3,1,0,0,0,0,0,0,
                        7,32,12,32,7,0,0,0,0,0,
                        19,75,50,50,75,19,0,0,0,0,
                        41,216,27,272,27,216,41,0,0,0,
                        751,3577,1323,2989,2989,1323,3577,751,0,0,
                        989,5888,-928,10496,-4540,10496,-928,5888,989,0,
                        2857,15741,1080,19344,5778,5778,19344,1080,15741,2857
};
double mltp[10] = { 1,(1.0 / 2),(1.0 / 3),(3.0 / 8),(2.0 / 45),(5.0 / 288),(1.0 / 140),(7.0 / 17280),(4.0 / 14175),(9.0 / 89600) };

double function(double x)
{
    return (1 + sqrt(x)) / (1 + 4 * x + 3 * x * x);
}

double GetValue() //ввод double, проверка и возвращение double
{
    while (true)
    {
        double a;
        cin >> a;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(32767, '\n');
            cout << "Неверный ввод, попробуйте ещё раз.\n";
        }
        else
            return a;
    }
}

double Integrate(double from, double h, int n, int power) {
    double sum = 0, partSum;
    for (int j = 0; j < n; j++)
    {
        partSum = 0;
        for (int i = 0; i <= power; i++)
            partSum += koef[power][i] * function(from + (i + j * h));
        sum += mltp[power] * partSum * h;
    }
    return sum;
}

struct IntegrateStruct {
    double from;
    double h;
    int n;
    int power;
    double res;
};

void *integrateThread(void* data) {
    struct IntegrateStruct* task = (struct IntegrateStruct*)data;
    task->res = Integrate(task->from, task->h, task->n, task->power);
    pthread_exit(&data);
    return data;
}

void MethodNewtonCotes(double a, double b, int n, int power, int threadsNum)
{
    cout << "---МЕТОД НЬЮТОН-КОТЕСА " << power << " СТЕПЕНИ-- - \n";
    cout << "Левая граница: " << a << ", правая граница: " << b << ", количество шагов: " << n << ", количество потоков: " << threadsNum << "\n";
    double s1 = 0;
    if ((power < 0) || (power > 9))
    {
        cout << "Неверное значение степени!\n";
        return;
    }
    double h = (b - a) / (n * 1.0);
    pthread_t *threads = new pthread_t[threadsNum];
    struct IntegrateStruct *tasks = new IntegrateStruct[threadsNum];
    double distance = (b - a) / (double)threadsNum;
   
    for (int i = 0; i < threadsNum; ++i) { // создаем задания и потоки
        tasks[i].from = a + (double)i * distance;
        tasks[i].h = h; 
        tasks[i].n = n/threadsNum;
        tasks[i].power = power;
        pthread_create(&threads[i], NULL, integrateThread, (void*)&tasks[i]);
    }
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threadsNum; ++i) {
        pthread_join(threads[i], NULL);
        s1 += tasks[i].res;
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        elapsed).count();
    cout << "Вычисленный интеграл = " << fixed << setprecision(10) << s1 << ", " << microseconds << "мкс\n\n";
}

int main()
{
    setlocale(LC_ALL, "");
    double a, b;
    int n;
    int threads;
    int power = 3;
    cout << "Введите a:\n";
    a = GetValue();
    cout << "Введите b:\n";
    b = GetValue();
    cout << "Введите количество шагов:\n";
    n = GetValue();
    cout << "Введите количество потоков:\n";
    threads = GetValue();
    if (a > b)
    {
        double temp = b;
        b = a;
        a = temp;
        cout << "Неверные значения пределов интеграла! Значения пределов были сменены местами!\n";
    }
    MethodNewtonCotes(a, b, n, power, threads);
    return 0;
}
