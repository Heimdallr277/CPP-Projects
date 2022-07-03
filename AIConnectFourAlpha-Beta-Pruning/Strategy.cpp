#include <iostream>
#include "Point.h"
#include "Strategy.h"
#include "Judge.h"
#include <conio.h>
#include <atlstr.h>
#include <algorithm>

using namespace std;

const int Max =  99999999;
const int Min = -99999999;
const int maxdepth = 6;

int level[4] = {0, 1, 4, 50};

int compare(pair<int,int> a, pair<int,int> b) {
	return (a.second - b.second);
}

/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误
	
	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:其实这里给出的top已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/
extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	/*
		不要更改这段代码
	*/
	int x = -1, y = -1;//最终将你的落子点存到x,y中
	int** board = new int*[M];
	for(int i = 0; i < M; i++){
		board[i] = new int[N];
		for(int j = 0; j < N; j++){
			board[i][j] = _board[i * N + j];
		}
	}

	/*
		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
	*/
	//Add your own code below
	/*
     //a naive example
	for (int i = N-1; i >= 0; i--) {
		if (top[i] > 0) {
			x = top[i] - 1;
			y = i;
			break;
		}
	}
    */
	

	int* t = new int[N];		//备份top[]
	for (int i = 0; i < N; i++) {
		t[i] = top[i];
	}
	
	int* val = new int[N];		//val[i]储存搜索树中深度为最大深度时，当前走第j列情况的估值

	pair<int, int>* predfs = new pair<int, int>[N];
	for (int k = 0; k < N; k++) {
		if (t[k]==0)
			predfs[k] = make_pair(k, Min);
		else
			predfs[k] = make_pair(k, dfs(M, N, t, board, noX, noY, 2, 1, 3, k, Min, Max));
	}

	sort(predfs, predfs+N, compare);

	int columntochoose = 0;
	for (int k = 0; k < N; k++) {
		int i = predfs[k].first;
		if (t[i]==0)
			val[i] = Min;
		else {
			val[i] = dfs(M, N, t, board, noX, noY, 2, 1, maxdepth-1, i, Min, Max);
			columntochoose = i;
		}
	}

	//找出估值最大的那一列
	for (int j = 0; j < N; j++) {
		if (val[j] > val[columntochoose]) {
			columntochoose = j;
			if (val[columntochoose]==Max) {
				break;
			}
		}
	}

	//赋值给x,y
	x = t[columntochoose] - 1;
	y = columntochoose;

	//结束，释放内存
	delete[] t;
	delete[] predfs;
	delete[] val;

	/*
		不要更改这段代码
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}


/*
	getPoint函数返回的Point指针是在本dll模块中声明的，为避免产生堆错误，应在外部调用本dll中的
	函数来释放空间，而不应该在外部直接delete
*/
extern "C" __declspec(dllexport) void clearPoint(Point* p){
	delete p;
	return;
}

/*
	清除top和board数组
*/
void clearArray(int M, int N, int** board){
	for(int i = 0; i < M; i++){
		delete[] board[i];
	}
	delete[] board;
}


/*
	添加你自己的辅助函数，你可以声明自己的类、函数，添加新的.h .cpp文件来辅助实现你的想法
*/

int calcvalue(const int M, const int N, int* top, int** board, const int noX, const int noY, int player) {
	//根据当前局势，计算对应玩家的估价值，返回估价值
	int ans = 0;	//记录总估价值
	int mine = 0;	//记录我方棋子个数
	int notminex, notminey;	//记录不是我方棋子所在的位置
	int upx, lowx, lefty, righty;
	int NWx, NWy, SEx, SEy, SWx, SWy, NEx, NEy;
	for (int x = 0; x < M; x++) {
		for (int y = 0; y < N; y++) {
			if (board[x][y]==player) {	//如果当前位置上是对应玩家才考虑计算

				//竖直方向上
				//以当前x为中心上下再延伸俩
				upx = x - 2;
				if (upx < 0)
					upx = 0;
				lowx = x + 2;
				if (lowx >= M)
					lowx = M-1;
				for (int start = lowx; start >= upx + 2; start--) {	//从x坐标为start开始往上看，start的值减小
					mine = 0;	//记录我方棋子个数
					for (int i = 0; i < 3; i++) {	//考察连续的三个
						if (board[start-i][y]==player) {
							mine++;
						}
					}
					//看这连续的三个里面有几个我方棋子，然后给出估值
					switch (mine) {
					case 0:
						ans += level[0];
						break;
					case 1:
						ans += level[1];
						break;
					case 2:
						ans += level[2];
						break;
					case 3:
						if ((start-3>=0)&&(start-3!=noX)&&y!=noY&&board[start-3][y]==0) {
							//如果三连的上方还有机会下一个的话
							ans += level[3];
						}
						break;
					default:
						break;
					}
				}
				//竖直方向上计算结束

				//水平方向上
				//以y为中心向左向右延伸三个
				lefty = y-3;
				if (lefty<0)
					lefty=0;
				righty = y+3;
				if (righty>=N)
					righty = N-1;
				for (int start = lefty; start <= righty-3; start++) {//从y坐标为start开始往右看，start的值增大
					mine = 0;	 //记录我方棋子个数
					notminey = 0;//记录下不是我方棋子的位置，有利于出现三连时的判断
					for (int i = 0; i < 4; i++) { //考察连续的四个
						if (board[x][start+i]==player) {
							mine++;
						} else {
							//后面只有在mine==3的时候才会用到这个值
							notminey = start+i;
						}
					}
					//看这连续的四个里面有几个我方棋子，然后给出估值
					switch (mine) {
					case 0:
						ans += level[0];
						break;
					case 1:
						ans += level[1];
						break;
					case 2:
						ans += level[2];
						break;
					case 3:
						//连续的四个里面有三个是我方，分两种情况，一种是三连，一种是被隔成2+1
						//如果是三连的话，则notminey必定在这四个格子的最左或最右
						if (notminey == start) { //如果在最左
							if (x!=noX&&start!=noY&&board[x][start]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
							if (x!=noX&&(start+4<N)&&(start+4!=noY)&&board[x][start+4]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
						} else if (notminey == start+3) { //如果在最右
							if (x!=noX&&(start-1>=0)&&(start-1!=noY)&&board[x][start-1]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
							if (x!=noX&&(notminey!=noY)&&board[x][notminey]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
						} else { //如果没成三连，即被断成2+1
							//如果被断掉的那个地方可以下的话
							if (x!=noX&&notminey!=noY&&board[x][notminey]==0) {
								ans += level[3];
								int btm = x;
								while (board[btm][notminey]==0)	{
									btm++;
									if (btm>=M)
										break;
								}
								ans -= (btm-x)/5;
							}
						}
						break;
					default:
						break;
					}
				}
				//水平方向计算结束

				//左上到右下方向
				//以x,y为中心向两边都延伸三个
				NWx = x - 3;	//左上方，NorthWest
				SEx = x + 3;	//右下方，SouthEast
				NWy = y - 3;	
				SEy = y + 3;
				while (NWx<0 || NWy<0) {
					NWx++;
					NWy++;
				}
				while (SEx>=M || SEy>=N) {
					SEx--;
					SEy--;
				}
				for (int startx = NWx, starty = NWy; startx <= SEx-3, starty <= SEy-3; startx++, starty++) {
					mine = 0;
					notminex = 0, notminey = 0;
					for (int i = 0; i < 4; i++) {
						if (board[startx+i][starty+i]==player) {
							mine++;
						} else {
							notminex = startx+i;
							notminey = starty+i;
						}
					}
					//仿照水平方向的计算方法
					//看这连续的四个里面有几个我方棋子，然后给出估值
					switch (mine) {
					case 0:
						ans += level[0];
						break;
					case 1:
						ans += level[1];
						break;
					case 2:
						ans += level[2];
						break;
					case 3:
						//连续的四个里面有三个是我方，分两种情况，一种是三连，一种是被隔成2+1
						//如果是三连的话，则非己方必定在这四个格子的最左上或最右下
						if (notminex==startx && notminey==starty) { //如果在最左上
							if (startx!=noX&&starty!=noY&&board[startx][starty]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
							if ((startx+4<M)&&(startx+4!=noX)&&(starty+4<N)&&(starty+4!=noY)&&board[startx+4][starty+4]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
						} else if ((notminex==startx+3)&&(notminey==starty+3)) { //如果在最右
							if ((startx-1>=0)&&(startx-1!=noX)&&(starty-1>=0)&&(starty-1!=noY)&&board[startx-1][starty-1]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
							if ((notminex!=noX)&&(notminey!=noY)&&board[notminex][notminey]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
						} else { //如果没成三连，即被断成2+1
							//如果被断掉的那个地方可以下的话
							if (notminex!=noX&&notminey!=noY&&board[notminex][notminey]==0) {
								ans += level[3];
								int btm = x;
								while (board[btm][notminey]==0)	{
									btm++;
									if (btm>=M)
										break;
								}
								ans -= (btm-x)/5;
							}
						}
						break;
					default:
						break;
					}
				}
				//左上到右下方向计算结束

				//右上到左下方向
				//以x,y为中心向两边都延伸三个，仿照左上到右下的计算方法
				SWx = x + 3;	//左下方，SouthWest
				NEx = x - 3;	//右上方，NorthEast
				SWy = y - 3;	
				NEy = y + 3;
				while (NEx<0 || NEy>=N) {
					NEx++;
					NEy--;
				}
				while (SWx>=M || SWy<0) {
					SWx--;
					SWy++;
				}
				for (int startx = NEx, starty = NEy; startx <= SWx-3, starty >= SWy+3; startx++, starty--) {
					mine = 0;
					notminex = 0, notminey = 0;
					for (int i = 0; i < 4; i++) {
						if (board[startx+i][starty-i]==player) {
							mine++;
						} else {
							notminex = startx+i;
							notminey = starty-i;
						}
					}
					//仿照水平方向的计算方法
					//看这连续的四个里面有几个我方棋子，然后给出估值
					switch (mine) {
					case 0:
						ans += level[0];
						break;
					case 1:
						ans += level[1];
						break;
					case 2:
						ans += level[2];
						break;
					case 3:
						//连续的四个里面有三个是我方，分两种情况，一种是三连，一种是被隔成2+1
						//如果是三连的话，则非己方必定在这四个格子的最右上或最左下
						if (notminex==startx && notminey==starty) { //如果在最右上
							if (startx!=noX&&starty!=noY&&board[startx][starty]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
							if ((startx+4<M)&&(starty-4>=0)&&(startx+4!=noX)&&(starty-4!=noY)&&board[startx+4][starty-4]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
						} else if ((notminex==startx+3)&&(notminey==starty-3)) { //如果在最左下
							if ((startx-1>=0)&&(startx-1!=noX)&&(starty+1<N)&&(starty+1!=noY)&&board[startx-1][starty+1]==0) { //如果三连的右边可以下一个
								ans += level[3];
							}
							if ((notminex!=noX)&&(notminey!=noY)&&board[notminex][notminey]==0) { //如果三连的左边可以下一个
								ans += level[3];
							}
						} else { //如果没成三连，即被断成2+1
							//如果被断掉的那个地方可以下的话
							if (notminex!=noX&&notminey!=noY&&board[notminex][notminey]==0) {
								ans += level[3];
							}
						}
						break;
					default:
						break;
					}
				}
				//右上到左下方向计算结束
			}
		}
	}
	return ans;
}



int dfs(const int M, const int N, int* top, int** board, 
		const int noX, const int noY, int crtplayer, int otherplayer, 
		int leftdepth, int columntoplay, int alpha, int beta) {
	//搜索函数，求某一走步对应估值，返回值为其估值
	//参数分别为棋盘大小、每列顶部、棋盘、上次走的位置、不可走的位置、当前玩家（开始是机器是2）、对手玩家（开始是人是1）
	//以及还剩下的搜索深度、要下的那一列、alpha和beta值（开始的时候分别为Min和Max）

	//走棋操作
	top[columntoplay]--;
	board[top[columntoplay]][columntoplay] = crtplayer;

	//如果这一步可以决出胜负的话
	if (crtplayer == 1) {
		if (userWin(top[columntoplay], columntoplay, M, N, board)) {//如果人可以赢（机器输），则直接返回Min
			//复原棋局
			board[top[columntoplay]][columntoplay] = 0;
			top[columntoplay]++;
			return Min;
		}
	} else if (crtplayer == 2) {
		if (machineWin(top[columntoplay], columntoplay, M, N, board)) {//如果机器可以赢，直接返回Max
			//复原棋局
			board[top[columntoplay]][columntoplay] = 0;
			top[columntoplay]++;
			return Max;
		}
	}

	//如果决不出胜负，则需要继续递归搜索

	//先处理走子以后使该列top的上面一格为不可下位置的情况
	if (columntoplay==noY && top[columntoplay]-1==noX) {
		top[columntoplay]--;
	}

	//如果已经到达递归深度，则需要根据当前棋盘情况来计算估值
	if (leftdepth <= 0) {
		int tmpval = calcvalue(M,N,top,board,noX,noY,2)-calcvalue(M,N,top,board,noX,noY,1);
		//复原棋局
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return tmpval;
	}

	//如果没有到达递归深度则继续递归
	if (crtplayer == 1)	{	//如果是人走，当前为极小层
		for (int i = 0; i < N; i++) {
			if (top[i] != 0) {	//如果top没有达到最顶，还可以下子
				int tmp = dfs(M, N, top, board, noX, noY, otherplayer, crtplayer, leftdepth-1, i, alpha, beta);
				//有改变的就是当前与对手玩家换了个位置、剩余搜索深度减了个1，α和β值还是继承父亲节点的
				
				//递归结束得到估值后将tmp传给父节点,也就是该函数正在处理的节点，为极小层

				//alpha相当于当前tmp中的最大值
				if (tmp > alpha) {
					alpha = tmp;
				}

				//判断是否可以剪枝，当前是极小层，发生alpha剪枝 
				if (beta <= alpha) {
					break;
				}
			}
		}

		//复原棋局
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return alpha;

	} else if (crtplayer == 2) {	//如果是机器走，当前为极大层
		for (int i = 0; i < N; i++) {
			if (top[i] != 0) {	//如果top没有达到最顶，还可以下子
				int tmp = dfs(M, N, top, board, noX, noY, otherplayer, crtplayer, leftdepth-1, i, alpha, beta);
				//有改变的就是当前与对手玩家换了个位置、剩余搜索深度减了个1，α和β值还是继承父亲节点的
				
				//递归结束得到估值后将tmp传给父节点,也就是该函数正在处理的节点，为极大层

				//beta相当于当前tmp中的最小值
				if (tmp < beta) {
					beta = tmp;
				}

				//判断是否可以剪枝，当前是极大层，发生beta剪枝 
				if (beta <= alpha) {
					break;
				}
			}
		}
		//复原棋局
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return beta;
	}

}
