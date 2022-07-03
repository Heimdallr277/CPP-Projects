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
	���Ժ����ӿ�,�ú������Կ�ƽ̨����,ÿ�δ��뵱ǰ״̬,Ҫ�����������ӵ�,�����ӵ������һ��������Ϸ��������ӵ�,��Ȼ�Կ�ƽ̨��ֱ����Ϊ��ĳ�������
	
	input:
		Ϊ�˷�ֹ�ԶԿ�ƽ̨ά����������ɸ��ģ����д���Ĳ�����Ϊconst����
		M, N : ���̴�С M - ���� N - ���� ����0��ʼ�ƣ� ���Ͻ�Ϊ����ԭ�㣬����x��ǣ�����y���
		top : ��ǰ����ÿһ���ж���ʵ��λ��. e.g. ��i��Ϊ��,��_top[i] == M, ��i������,��_top[i] == 0
		_board : ���̵�һά�����ʾ, Ϊ�˷���ʹ�ã��ڸú����տ�ʼ���������Ѿ�����ת��Ϊ�˶�ά����board
				��ֻ��ֱ��ʹ��board���ɣ����Ͻ�Ϊ����ԭ�㣬�����[0][0]��ʼ��(����[1][1])
				board[x][y]��ʾ��x�С���y�еĵ�(��0��ʼ��)
				board[x][y] == 0/1/2 �ֱ��Ӧ(x,y)�� ������/���û�����/�г������,�������ӵ㴦��ֵҲΪ0
		lastX, lastY : �Է���һ�����ӵ�λ��, ����ܲ���Ҫ�ò�����Ҳ������Ҫ�Ĳ������ǶԷ�һ����
				����λ�ã���ʱ��������Լ��ĳ����м�¼�Է������ಽ������λ�ã�����ȫȡ�������Լ��Ĳ���
		noX, noY : �����ϵĲ������ӵ�(ע:��ʵ���������top�Ѿ����㴦���˲������ӵ㣬Ҳ����˵���ĳһ��
				������ӵ�����ǡ�ǲ������ӵ㣬��ôUI�����еĴ�����Ѿ������е�topֵ�ֽ�����һ�μ�һ������
				��������Ĵ�����Ҳ���Ը�����ʹ��noX��noY��������������ȫ��Ϊtop������ǵ�ǰÿ�еĶ�������,
				��Ȼ�������ʹ��lastX,lastY�������п��ܾ�Ҫͬʱ����noX��noY��)
		���ϲ���ʵ���ϰ����˵�ǰ״̬(M N _top _board)�Լ���ʷ��Ϣ(lastX lastY),��Ҫ���ľ�������Щ��Ϣ�¸������������ǵ����ӵ�
	output:
		������ӵ�Point
*/
extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	/*
		��Ҫ������δ���
	*/
	int x = -1, y = -1;//���ս�������ӵ�浽x,y��
	int** board = new int*[M];
	for(int i = 0; i < M; i++){
		board[i] = new int[N];
		for(int j = 0; j < N; j++){
			board[i][j] = _board[i * N + j];
		}
	}

	/*
		�������Լ��Ĳ������������ӵ�,Ҳ���Ǹ�����Ĳ�����ɶ�x,y�ĸ�ֵ
		�ò��ֶԲ���ʹ��û�����ƣ�Ϊ�˷���ʵ�֣�����Զ����Լ��µ��ࡢ.h�ļ���.cpp�ļ�
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
	

	int* t = new int[N];		//����top[]
	for (int i = 0; i < N; i++) {
		t[i] = top[i];
	}
	
	int* val = new int[N];		//val[i]���������������Ϊ������ʱ����ǰ�ߵ�j������Ĺ�ֵ

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

	//�ҳ���ֵ������һ��
	for (int j = 0; j < N; j++) {
		if (val[j] > val[columntochoose]) {
			columntochoose = j;
			if (val[columntochoose]==Max) {
				break;
			}
		}
	}

	//��ֵ��x,y
	x = t[columntochoose] - 1;
	y = columntochoose;

	//�������ͷ��ڴ�
	delete[] t;
	delete[] predfs;
	delete[] val;

	/*
		��Ҫ������δ���
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}


/*
	getPoint�������ص�Pointָ�����ڱ�dllģ���������ģ�Ϊ��������Ѵ���Ӧ���ⲿ���ñ�dll�е�
	�������ͷſռ䣬����Ӧ�����ⲿֱ��delete
*/
extern "C" __declspec(dllexport) void clearPoint(Point* p){
	delete p;
	return;
}

/*
	���top��board����
*/
void clearArray(int M, int N, int** board){
	for(int i = 0; i < M; i++){
		delete[] board[i];
	}
	delete[] board;
}


/*
	������Լ��ĸ�������������������Լ����ࡢ����������µ�.h .cpp�ļ�������ʵ������뷨
*/

int calcvalue(const int M, const int N, int* top, int** board, const int noX, const int noY, int player) {
	//���ݵ�ǰ���ƣ������Ӧ��ҵĹ���ֵ�����ع���ֵ
	int ans = 0;	//��¼�ܹ���ֵ
	int mine = 0;	//��¼�ҷ����Ӹ���
	int notminex, notminey;	//��¼�����ҷ��������ڵ�λ��
	int upx, lowx, lefty, righty;
	int NWx, NWy, SEx, SEy, SWx, SWy, NEx, NEy;
	for (int x = 0; x < M; x++) {
		for (int y = 0; y < N; y++) {
			if (board[x][y]==player) {	//�����ǰλ�����Ƕ�Ӧ��Ҳſ��Ǽ���

				//��ֱ������
				//�Ե�ǰxΪ����������������
				upx = x - 2;
				if (upx < 0)
					upx = 0;
				lowx = x + 2;
				if (lowx >= M)
					lowx = M-1;
				for (int start = lowx; start >= upx + 2; start--) {	//��x����Ϊstart��ʼ���Ͽ���start��ֵ��С
					mine = 0;	//��¼�ҷ����Ӹ���
					for (int i = 0; i < 3; i++) {	//��������������
						if (board[start-i][y]==player) {
							mine++;
						}
					}
					//�������������������м����ҷ����ӣ�Ȼ�������ֵ
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
							//����������Ϸ����л�����һ���Ļ�
							ans += level[3];
						}
						break;
					default:
						break;
					}
				}
				//��ֱ�����ϼ������

				//ˮƽ������
				//��yΪ��������������������
				lefty = y-3;
				if (lefty<0)
					lefty=0;
				righty = y+3;
				if (righty>=N)
					righty = N-1;
				for (int start = lefty; start <= righty-3; start++) {//��y����Ϊstart��ʼ���ҿ���start��ֵ����
					mine = 0;	 //��¼�ҷ����Ӹ���
					notminey = 0;//��¼�²����ҷ����ӵ�λ�ã������ڳ�������ʱ���ж�
					for (int i = 0; i < 4; i++) { //�����������ĸ�
						if (board[x][start+i]==player) {
							mine++;
						} else {
							//����ֻ����mine==3��ʱ��Ż��õ����ֵ
							notminey = start+i;
						}
					}
					//�����������ĸ������м����ҷ����ӣ�Ȼ�������ֵ
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
						//�������ĸ��������������ҷ��������������һ����������һ���Ǳ�����2+1
						//����������Ļ�����notminey�ض������ĸ����ӵ����������
						if (notminey == start) { //���������
							if (x!=noX&&start!=noY&&board[x][start]==0) { //�����������߿�����һ��
								ans += level[3];
							}
							if (x!=noX&&(start+4<N)&&(start+4!=noY)&&board[x][start+4]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
						} else if (notminey == start+3) { //���������
							if (x!=noX&&(start-1>=0)&&(start-1!=noY)&&board[x][start-1]==0) { //�����������߿�����һ��
								ans += level[3];
							}
							if (x!=noX&&(notminey!=noY)&&board[x][notminey]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
						} else { //���û�������������ϳ�2+1
							//������ϵ����Ǹ��ط������µĻ�
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
				//ˮƽ����������

				//���ϵ����·���
				//��x,yΪ���������߶���������
				NWx = x - 3;	//���Ϸ���NorthWest
				SEx = x + 3;	//���·���SouthEast
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
					//����ˮƽ����ļ��㷽��
					//�����������ĸ������м����ҷ����ӣ�Ȼ�������ֵ
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
						//�������ĸ��������������ҷ��������������һ����������һ���Ǳ�����2+1
						//����������Ļ�����Ǽ����ض������ĸ����ӵ������ϻ�������
						if (notminex==startx && notminey==starty) { //�����������
							if (startx!=noX&&starty!=noY&&board[startx][starty]==0) { //�����������߿�����һ��
								ans += level[3];
							}
							if ((startx+4<M)&&(startx+4!=noX)&&(starty+4<N)&&(starty+4!=noY)&&board[startx+4][starty+4]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
						} else if ((notminex==startx+3)&&(notminey==starty+3)) { //���������
							if ((startx-1>=0)&&(startx-1!=noX)&&(starty-1>=0)&&(starty-1!=noY)&&board[startx-1][starty-1]==0) { //�����������߿�����һ��
								ans += level[3];
							}
							if ((notminex!=noX)&&(notminey!=noY)&&board[notminex][notminey]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
						} else { //���û�������������ϳ�2+1
							//������ϵ����Ǹ��ط������µĻ�
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
				//���ϵ����·���������

				//���ϵ����·���
				//��x,yΪ���������߶������������������ϵ����µļ��㷽��
				SWx = x + 3;	//���·���SouthWest
				NEx = x - 3;	//���Ϸ���NorthEast
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
					//����ˮƽ����ļ��㷽��
					//�����������ĸ������м����ҷ����ӣ�Ȼ�������ֵ
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
						//�������ĸ��������������ҷ��������������һ����������һ���Ǳ�����2+1
						//����������Ļ�����Ǽ����ض������ĸ����ӵ������ϻ�������
						if (notminex==startx && notminey==starty) { //�����������
							if (startx!=noX&&starty!=noY&&board[startx][starty]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
							if ((startx+4<M)&&(starty-4>=0)&&(startx+4!=noX)&&(starty-4!=noY)&&board[startx+4][starty-4]==0) { //�����������߿�����һ��
								ans += level[3];
							}
						} else if ((notminex==startx+3)&&(notminey==starty-3)) { //�����������
							if ((startx-1>=0)&&(startx-1!=noX)&&(starty+1<N)&&(starty+1!=noY)&&board[startx-1][starty+1]==0) { //����������ұ߿�����һ��
								ans += level[3];
							}
							if ((notminex!=noX)&&(notminey!=noY)&&board[notminex][notminey]==0) { //�����������߿�����һ��
								ans += level[3];
							}
						} else { //���û�������������ϳ�2+1
							//������ϵ����Ǹ��ط������µĻ�
							if (notminex!=noX&&notminey!=noY&&board[notminex][notminey]==0) {
								ans += level[3];
							}
						}
						break;
					default:
						break;
					}
				}
				//���ϵ����·���������
			}
		}
	}
	return ans;
}



int dfs(const int M, const int N, int* top, int** board, 
		const int noX, const int noY, int crtplayer, int otherplayer, 
		int leftdepth, int columntoplay, int alpha, int beta) {
	//������������ĳһ�߲���Ӧ��ֵ������ֵΪ���ֵ
	//�����ֱ�Ϊ���̴�С��ÿ�ж��������̡��ϴ��ߵ�λ�á������ߵ�λ�á���ǰ��ң���ʼ�ǻ�����2����������ң���ʼ������1��
	//�Լ���ʣ�µ�������ȡ�Ҫ�µ���һ�С�alpha��betaֵ����ʼ��ʱ��ֱ�ΪMin��Max��

	//�������
	top[columntoplay]--;
	board[top[columntoplay]][columntoplay] = crtplayer;

	//�����һ�����Ծ���ʤ���Ļ�
	if (crtplayer == 1) {
		if (userWin(top[columntoplay], columntoplay, M, N, board)) {//����˿���Ӯ�������䣩����ֱ�ӷ���Min
			//��ԭ���
			board[top[columntoplay]][columntoplay] = 0;
			top[columntoplay]++;
			return Min;
		}
	} else if (crtplayer == 2) {
		if (machineWin(top[columntoplay], columntoplay, M, N, board)) {//�����������Ӯ��ֱ�ӷ���Max
			//��ԭ���
			board[top[columntoplay]][columntoplay] = 0;
			top[columntoplay]++;
			return Max;
		}
	}

	//���������ʤ��������Ҫ�����ݹ�����

	//�ȴ��������Ժ�ʹ����top������һ��Ϊ������λ�õ����
	if (columntoplay==noY && top[columntoplay]-1==noX) {
		top[columntoplay]--;
	}

	//����Ѿ�����ݹ���ȣ�����Ҫ���ݵ�ǰ��������������ֵ
	if (leftdepth <= 0) {
		int tmpval = calcvalue(M,N,top,board,noX,noY,2)-calcvalue(M,N,top,board,noX,noY,1);
		//��ԭ���
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return tmpval;
	}

	//���û�е���ݹ����������ݹ�
	if (crtplayer == 1)	{	//��������ߣ���ǰΪ��С��
		for (int i = 0; i < N; i++) {
			if (top[i] != 0) {	//���topû�дﵽ�������������
				int tmp = dfs(M, N, top, board, noX, noY, otherplayer, crtplayer, leftdepth-1, i, alpha, beta);
				//�иı�ľ��ǵ�ǰ�������һ��˸�λ�á�ʣ��������ȼ��˸�1�����ͦ�ֵ���Ǽ̳и��׽ڵ��
				
				//�ݹ�����õ���ֵ��tmp�������ڵ�,Ҳ���Ǹú������ڴ���Ľڵ㣬Ϊ��С��

				//alpha�൱�ڵ�ǰtmp�е����ֵ
				if (tmp > alpha) {
					alpha = tmp;
				}

				//�ж��Ƿ���Լ�֦����ǰ�Ǽ�С�㣬����alpha��֦ 
				if (beta <= alpha) {
					break;
				}
			}
		}

		//��ԭ���
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return alpha;

	} else if (crtplayer == 2) {	//����ǻ����ߣ���ǰΪ�����
		for (int i = 0; i < N; i++) {
			if (top[i] != 0) {	//���topû�дﵽ�������������
				int tmp = dfs(M, N, top, board, noX, noY, otherplayer, crtplayer, leftdepth-1, i, alpha, beta);
				//�иı�ľ��ǵ�ǰ�������һ��˸�λ�á�ʣ��������ȼ��˸�1�����ͦ�ֵ���Ǽ̳и��׽ڵ��
				
				//�ݹ�����õ���ֵ��tmp�������ڵ�,Ҳ���Ǹú������ڴ���Ľڵ㣬Ϊ�����

				//beta�൱�ڵ�ǰtmp�е���Сֵ
				if (tmp < beta) {
					beta = tmp;
				}

				//�ж��Ƿ���Լ�֦����ǰ�Ǽ���㣬����beta��֦ 
				if (beta <= alpha) {
					break;
				}
			}
		}
		//��ԭ���
		if (columntoplay==noY && top[columntoplay]==noX) {
			top[columntoplay]++;
		}
		board[top[columntoplay]][columntoplay] = 0;
		top[columntoplay]++;
		return beta;
	}

}
