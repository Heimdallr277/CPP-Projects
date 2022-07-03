#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cmath>
#include <QDebug>
#include <math.h>

#define NAX 0.000000001

using namespace std;

struct edge
{
   double v; //流速
   double leng; //管道长度
   int n1,n2; //边所连接的两个节点，默认从n1流向n2
};

struct node
{
   vector<int> elist;  //点所连接的边，按照顺时针的顺序存储下来

};

class Calculator
{
public:
    Calculator();
    vector<double> caluconspeed(int num, vector<double>&length,
                                int i1, int i2, int o1, int o2, int o3);
    //对外接口
    vector<double> speed;   //存下每条管道的流速

private:
    edge edges[200];
    node nodes[200];
    vector< vector<double> > rect;//存储方程组(行列式)
    bool fr[200]; //深度搜索时判断边是否遍历过
    int n, EDGESUM, NODESUM;  //记录网格边长
    void setedgelength(int x, double leng);
    void addrect(vector<double> &tmp);
    int getdirline(int x, int y, int dir);
    bool existdir(int x, int y, int dir);
    void recursionline(int x, int y, int dir, vector<double> &tmp,int end);
    bool recursionrect(int x, int y, vector<double> &tmp,int end);
    void findline(int x, int y,int t);
    void findrect(int x1);
    void initrect();
    int min (int a, int b);
    double GreatestCommonDivisor(double a, double b);
    double LeastCommonMultiple (double a, double b);
    void getans();
};

#endif // CALCULATOR_H
