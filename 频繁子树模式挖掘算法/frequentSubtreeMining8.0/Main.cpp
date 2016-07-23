#define HOME

#ifdef HOME
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>
#include <stdexcept>
using namespace std;


// �ڵ�
struct Node
{
	size_t id;	// Ψһ��ʶ��Ҳ�����������е�λ��
	int father;	// ���ڵ�Ψһ��ʶ��Ҳ�����������и��ڵ���±�
	vector<string> properties;	// �����±꼴id����������
	vector<size_t> sonNode;	// ���ӽڵ㣬Ҳ�����������к��ӽڵ���±�

	size_t orderID;	// �ýڵ��ھ������е�˳��ID

	// �����������е� ��Tģʽ���� �ڵ�
	string uuidByMatchProperty; // ���ýڵ���Ϊ��ģʽ�����еĽڵ�ʱ�����Լ���
	int availableFather;	// �����ǰ��available�����º�ĸ��ڵ�
};


// �ڵ㣨ģʽ��T��
struct NodeForT
{
	size_t id;	// Ψһ��ʶ
	int father;	// ���ڵ�Ψһ��ʶ��Ҳ�����������и��ڵ���±�

	string uuidByMatchProperty;	// ���Լ���

	unsigned long long propertyFlag = 0;	// ��ǽڵ��б�ѡȡ�����ԣ�Ĭ�ϴӵ�һ�����Կ�ʼȡ��ֱ����������ȡ�꣩
};

// ������������T
struct SubTree
{
	size_t itemNo;	// �������������������
	size_t root;	// ���ڵ�λ��
	size_t rml;	// ����Ҷ�ڵ�λ��
	unordered_map<size_t, int> toFatherID;	// �������ж�Ӧ��ģʽ��T�ĸ��ڵ�
};

// ������Ϣ
struct TreeMessage
{
	int nodeCnt = 0;	// �ڵ���Ŀ
};


// ��Ҫ���������
vector<vector<Node>> Data;	// ��������ݼ�
unsigned propertyNum;	// ���Եĸ���
vector<size_t> toMatchProperties;	// ���άƵ������ʱ����Ҫ��ƥ������Լ�
unsigned int minsupp;	// ��С֧�ֶ���ֵ


// ͳ����Ŀ
int intputDataNum = 0;	// ����������Ŀ
int frequentSubtreeNum = 0;	// Ƶ����������


// ������Ϣ
struct PropertyMeg
{
	unsigned long long propertyFlag = 0;
	unsigned cnt = 0;
};

unordered_map<string, PropertyMeg> frequentProperty;	//��Ƶ�����Լ��ϡ������Ϣ

TreeMessage TtreeMeg;	// ģʽ���ڵ��Լ�������Ϣ

vector<SubTree> itemToSubTr;	// ÿ���������м�¼����Tλ��

// ģʽ���ĸ��ڵ������Ҷ�ڵ��ʶ
string Troot, Trml;

vector<size_t> visitDataPos;	// ��Ҫ���ʵ����������±�

// ͳ����Ŀ
inline int &calDataNum(int &num)
{
	++num;
	return num;
}


/********************************************* �������� ************************************************/
// �������Եĸ���
inline void setPropertyNum()
{
	propertyNum = 6;
}


// �������άƵ������ʱ����Ҫ��ƥ������Լ�
inline void setToMatchProperties()
{
	//vector<size_t> matchProperty = { 0, 1, 2, 5 };
	//vector<size_t> matchProperty = { 1, 2, 5 };
	vector<size_t> matchProperty = { 1, 2 };
	//vector<size_t> matchProperty = { 1 };
	for (size_t i = 0; i != matchProperty.size(); ++i)
	{
		toMatchProperties.push_back(matchProperty[i]);
	}
	sort(toMatchProperties.begin(), toMatchProperties.end());
}

// ������С֧�ֶ���ֵ
inline void setMinsupp()
{
	minsupp = 2;
}


// ��������
inline void inputData()
{
	int iid;
	vector<Node> Item;
	//std::ios::sync_with_stdio(false);
	while (cin >> iid)
	{
		if (!iid && !Item.empty())
		{
			calDataNum(intputDataNum);
			//cout << "intputDataNum :" << calDataNum(intputDataNum) << endl;
			Data.push_back(Item);
			Item.clear();
		}
		Node nd;
		nd.id = iid;
		nd.properties.push_back(std::to_string(iid));
		string strTmp;
		for (size_t i = 1; i < propertyNum; ++i)
		{
			cin >> strTmp;
			nd.properties.push_back(strTmp);
		}
		nd.father = std::stoi(nd.properties[propertyNum - 2]);
		Item.push_back(nd);
	}
	if (!Item.empty())
	{
		cout << "intputDataNum :" << calDataNum(intputDataNum) << endl;
		Data.push_back(Item);
		Item.clear();
	}
}

// �������ݿ��������������Ľڵ�˳��˳�㱣��ÿ���ڵ�ĺ��ӽڵ㣩
inline void setDataTreeOrderID()
{
	for (size_t i = 0; i != Data.size(); ++i)
	{
		vector<Node> &Item = Data[i];

		vector<bool> visited(Item.size());
		fill(visited.begin(), visited.end(), false);

		size_t orderID = 0;

		queue<size_t> queNode;

		for (size_t j = 0; j != Item.size(); ++j)
		{
			Node &nd = Item[j];
			if (nd.father < 0)
			{
				nd.orderID = orderID;
				++orderID;
				visited[j] = true;
				queNode.push(j);
			}
		}

		/*
			�����п�������趨˳���ԭ��
				�����ݿ��еĶ�ά������ڵ������±꼴Ϊ�ڵ�ID�����߿��Ժ϶�Ϊһ����
		*/
		while (!queNode.empty())
		{
			size_t fatherID = queNode.front();
			queNode.pop();

			for (size_t j = 0; j != Item.size(); ++j)
			{
				if (!visited[j])
				{
					Node &nd = Item[j];
					if (nd.father == fatherID)
					{
						Item[nd.father].sonNode.push_back(j);
						nd.orderID = orderID;
						++orderID;
						visited[j] = true;
						queNode.push(nd.id);
					}
				}
			}
		}
	}
}
/********************************************* �������� ************************************************/


/********************************************* ������� ************************************************/
// ���Ƶ�����Լ���
inline void outputFrequentProperty()
{
	int propertyStrNum = 0;
	for (unordered_map<string, PropertyMeg>::const_iterator cit = frequentProperty.begin();
		cit != frequentProperty.end(); ++cit)
	{
		++propertyStrNum;
		cout << propertyStrNum << ": " << (*cit).first << endl;
	}
}

// ���Ƶ�������������Ľ����
inline void outputFrequentSubtree(const vector<NodeForT> &Ttree)
{
	cout << "frequentSubtreeNum : " << calDataNum(frequentSubtreeNum) << " : " << endl;
	for (vector<NodeForT>::size_type i = 0; i != Ttree.size(); ++i)
	{
		cout << Ttree[i].uuidByMatchProperty << endl;
		//cout << Ttree[i].id << " " << Ttree[i].uuidByMatchProperty
		//	<< " " << Ttree[i].father << endl;
	}
	cout << endl;
}

/********************************************* ������� ************************************************/



/********************************************* ��ȡ�������Լ��� ************************************************/
// ��ȡ�������Գ��ִ���
inline void getSinglePropertySetWithCnt(unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			for (size_t k = 0; k != toMatchProperties.size(); ++k)
			{
				const string &propertyStr = nd.properties[toMatchProperties[k]];
				if (siglePropertySet.find(propertyStr) == siglePropertySet.end())
				{
					siglePropertySet[propertyStr].cnt = 1;
					siglePropertySet[propertyStr].propertyFlag = static_cast<unsigned long long>(1 << toMatchProperties[k]);
				}
				else
				{
					try
					{
						if (siglePropertySet[propertyStr].propertyFlag != static_cast<unsigned long long>(1 << toMatchProperties[k]))
						{
							throw runtime_error("���Լ�û�а�Ҫ��˳�����루�������Լ����໥������");
						}
					}
					catch (runtime_error err)
					{
						cerr << "Error: " << __FILE__
							<< " : in function " << __func__
							<< " at line " << __LINE__ << endl
							<< "     Comiled on " << __DATE__
							<< " at " << __TIME__ << endl
							<< err.what() << endl;
					}
					++siglePropertySet[propertyStr].cnt;
				}
			}
		}
	}
}


/*
����һ��ö�����Լ���+��֦(����Ƶ��������)
�����ڣ����Ը������٣�ֱ��ö�����Եĸ��Ӷ�С�ڱ������ݿ�ĸ��Ӷ�
*/

// ��ȡ�������Լ���
inline void getAllPropertySet(size_t num, size_t pos, const size_t &K, const Node &nd,
	const unordered_map<string, PropertyMeg> &siglePropertySet,
	string strProperty, unsigned long long propertyFlag)
{
	if (num == K)
	{
		if (frequentProperty.find(strProperty) == frequentProperty.end())
		{
			frequentProperty[strProperty].cnt = 1;
			frequentProperty[strProperty].propertyFlag = propertyFlag;
		}
		else
		{
			try
			{
				if (frequentProperty[strProperty].propertyFlag != propertyFlag)
				{
					throw runtime_error("���Լ�û�а�Ҫ��˳�����루�������Լ����໥������");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}
			++frequentProperty[strProperty].cnt;
		}
	}
	else
	{
		for (size_t i = pos; i != toMatchProperties.size(); ++i)
		{
			if (num + (toMatchProperties.size() - i) >= K)
			{
				const string &sigleProperty = nd.properties[toMatchProperties[i]];
				if (siglePropertySet.at(sigleProperty).cnt >= minsupp)
				{
					getAllPropertySet(num + 1, i + 1, K, nd, siglePropertySet,
						strProperty + sigleProperty,
						propertyFlag | static_cast<unsigned long long>(1 << toMatchProperties[i]));
				}
			}
			else
			{
				break;
			}
		}
	}
}

/*
	��ȡ�������Լ��ϳ��ִ��������õ������Գ��ִ���������֦�����������Ƶ�����������Լ��ϣ�,
	���ջᱣ�棺����Ƶ�����Լ��� + һЩ��Ƶ��������
	���ԣ���Ҫ�Ƴ���Ƶ�����Լ��ϡ�
*/
inline void removeInfrequentPropertySet()
{
	vector<string> ptyStr_del;
	for (unordered_map<string, PropertyMeg>::iterator it = frequentProperty.begin();
		it != frequentProperty.end(); ++it)
	{
		if ((*it).second.cnt < minsupp)
		{
			ptyStr_del.push_back(it->first);
		}
	}
	for (size_t i = 0; i != ptyStr_del.size(); ++i)
	{
		frequentProperty.erase(ptyStr_del[i]);
	}
}

inline void getAllPropertySetWithCnt_1(const unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			for (size_t k = 1; k != toMatchProperties.size() + 1; ++k)
			{
				getAllPropertySet(0, 0, k, nd, siglePropertySet, "", 0);
			}
		}
	}
	removeInfrequentPropertySet();
}


/*
	������������ͶӰ��֧��˼�룬ֱ������Ƶ�����Լ��ϣ�����Ҫ��α������ݿ�
	�����ڣ����Ը����϶ֱ࣬��ö�����Եĸ��Ӷȴ��ڱ������ݿ�ĸ��Ӷ�
*/

// ����propertyFlag�������ö�����λ��¼��ʹ�õ����ԣ���������Լ���
inline void getPropertyStr(string &ptyStr, unsigned long long ptyPos, const Node &nd)
{
	unsigned bitNum = 0;
	while (ptyPos)
	{
		if (ptyPos & 1)
		{
			//ptyStr += nd.properties[bitNum];
			ptyStr.append(nd.properties[bitNum]);
		}
		ptyPos >>= 1;
		++bitNum;
	}
}

// ��ȡһ���ڵ���ٽ�����
inline void getOneNodeAdjoinProperty(const unsigned long long &propertyPos, const Node &nd,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (vector<size_t>::const_reverse_iterator cit = toMatchProperties.crbegin();
		cit != toMatchProperties.crend(); ++cit)
	{
		unsigned long long shiftRel = static_cast<unsigned long long>(1 << (*cit));
		if (!(shiftRel & propertyPos))
		{
			const string &pty = nd.properties[*cit];
			if (adjoinProperty.find(pty) == adjoinProperty.end())
			{
				adjoinProperty[pty].cnt = 1;
				adjoinProperty[pty].propertyFlag = shiftRel;
			}
			else
			{
				try
				{
					if (shiftRel != adjoinProperty[pty].propertyFlag)
					{
						throw runtime_error("���Լ�û�а�Ҫ��˳�����루�������Լ����໥������");
					}
				}
				catch (runtime_error err)
				{
					cerr << "Error: " << __FILE__
						<< " : in function " << __func__
						<< " at line " << __LINE__ << endl
						<< "     Comiled on " << __DATE__
						<< " at " << __TIME__ << endl
						<< err.what() << endl;
				}
				++adjoinProperty[pty].cnt;
			}
		}
		else
		{
			return;
		}
	}
}

// ��ȡ�ٽ�����
inline void getAdjoinProperty(const unsigned long long &propertyPos, const string &propertyStr,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			string ptyStr;
			getPropertyStr(ptyStr, propertyPos, nd);
			if (propertyStr == ptyStr)
			{
				getOneNodeAdjoinProperty(propertyPos, nd, adjoinProperty);
			}
		}
	}
}

// ������չ
inline void propertyExpand(const unsigned long long &propertyPos, const string &propertyStr,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = adjoinProperty.cbegin();
		cit != adjoinProperty.end(); ++cit)
	{
		const PropertyMeg &ptyMeg = (*cit).second;
		if (ptyMeg.cnt >= minsupp)
		{
			string ptyStr = propertyStr + (*cit).first;
			try
			{
				if (frequentProperty.find(ptyStr) != frequentProperty.end())
				{
					throw runtime_error("��ȡ�ڽ����Գ����ظ�");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			frequentProperty[ptyStr].cnt = ptyMeg.cnt;
			frequentProperty[ptyStr].propertyFlag = (propertyPos | ptyMeg.propertyFlag);
			unordered_map<string, PropertyMeg> adjoinPropertyNext;
			const unsigned long long ptyPos = (propertyPos | ptyMeg.propertyFlag);
			getAdjoinProperty(ptyPos, ptyStr, adjoinPropertyNext);
			propertyExpand(ptyPos, ptyStr, adjoinPropertyNext);
		}
	}
}


// ��ȡ�������Լ��ϳ��ִ�������չ��������������Ƶ�����Լ���
inline void getAllPropertySetWithCnt_2(const unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = siglePropertySet.cbegin();
		cit != siglePropertySet.end(); ++cit)
	{
		const PropertyMeg &ptyMeg = (*cit).second;
		if (ptyMeg.cnt >= minsupp)
		{
			try
			{
				if (frequentProperty.find((*cit).first) != frequentProperty.end())
				{
					throw runtime_error("��ȡ�ĵ������Գ����ظ�");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			frequentProperty[(*cit).first] = ptyMeg;
			unordered_map<string, PropertyMeg> adjoinProperty;
			const unsigned long long &propertyPos = ptyMeg.propertyFlag;
			getAdjoinProperty(propertyPos, (*cit).first, adjoinProperty);
			propertyExpand(propertyPos, (*cit).first, adjoinProperty);
		}
	}
}


// �������Ƶ�����Լ��ϣ����ݾ�����������ѡ�񷽷���
inline void getAllFrequentPropertySet()
{
	unordered_map<string, PropertyMeg> siglePropertySet;
	getSinglePropertySetWithCnt(siglePropertySet);


	if (toMatchProperties.size() < 20)
	{
		getAllPropertySetWithCnt_1(siglePropertySet);
	}
	else
	{
		getAllPropertySetWithCnt_2(siglePropertySet);
	}
	//outputFrequentProperty();
}
/********************************************* ��ȡ�������Լ��� ************************************************/



/*************************************** ����ͶӰ��֧�Ķ�άƵ�������ھ� ******************************************/

// ��ǡ�ģʽ�����Ľڵ���������������Ӧ�Ľڵ㣬���ж��Ƿ�ȫ���ҵ�
inline bool signAvailableNode(vector<Node> &Item, const vector<NodeForT> &Ttree,
	unordered_map<string, size_t> &availableNode)
{
	vector<size_t> visitPos;
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		visitPos.push_back(i);
	}

	for (size_t i = 0; i != Item.size(); ++i)
	{
		Node &nd = Item[i];
		for (size_t j = 0; j != visitPos.size(); ++j)
		{
			string ndPtyStr;
			const NodeForT &ndT = Ttree[visitPos[j]];
			getPropertyStr(ndPtyStr, ndT.propertyFlag, nd);
			if (ndPtyStr == ndT.uuidByMatchProperty)
			{
				nd.uuidByMatchProperty = ndPtyStr;
				availableNode[ndPtyStr] = i;
				visitPos.erase(visitPos.begin() + j);
				if (visitPos.empty())
				{
					return true;
				}
				break;
			}
		}
	}
	return false;
}

// ���¿��õ�ĸ��ڵ㣨��������ɿ��õ���ɵ�ɭ�֣�
inline void updateAvailableNodeFather(vector<Node> &Item,
	const unordered_map<string, size_t> &availableNode)
{
	unordered_set<size_t> visitNodePos;
	for (unordered_map<string, size_t>::const_iterator cit = availableNode.cbegin();
		cit != availableNode.cend(); ++cit)
	{
		visitNodePos.insert((*cit).second);
	}

	for (unordered_set<size_t>::const_iterator cit = visitNodePos.cbegin();
		cit != visitNodePos.cend(); ++cit)
	{
		Node &nd = Item[*cit];
		int father = nd.father;

		while ((father >= 0) &&
			(visitNodePos.find(static_cast<unsigned>(father)) == visitNodePos.end()))
		{
			father = Item[father].father;
		}
		nd.availableFather = father;
	}
}

// �ж�ģʽ�����������еġ�ģʽ�����ĸ��ӹ�ϵ�Ƿ�һ�£�ֱ���ų����������еġ�ģʽ������ɭ�ֵ������
inline bool judgeRelationshipOfFatherSonEqual(const vector<NodeForT> &Ttree,
	const vector<Node> &Item, const unordered_map<string, size_t> &availableNode)
{
	if (Ttree.size() != availableNode.size())
	{
		return false;
	}

	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		const NodeForT &ndT = Ttree[i];

		if (ndT.father >= 0)
		{
			const string &fatherPtySet = Ttree[ndT.father].uuidByMatchProperty;
			if (availableNode.find(ndT.uuidByMatchProperty) != availableNode.end())
			{
				int ndFather = Item[availableNode.at(ndT.uuidByMatchProperty)].availableFather;
				if (ndFather >= 0)
				{
					if (Item[ndFather].uuidByMatchProperty != fatherPtySet)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (availableNode.find(ndT.uuidByMatchProperty) != availableNode.end())
			{
				int ndFather = Item[availableNode.at(ndT.uuidByMatchProperty)].availableFather;
				if (ndFather >= 0)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}


// ����������еġ�ģʽ�����Ľڵ�˳��
inline void getTtreeInItemOrder(const vector<Node> &Item,
	const unordered_map<string, size_t> &availableNode, vector<string> &TtreeInItemOrder)
{
	size_t fatherID = availableNode.at(Troot);
	TtreeInItemOrder.push_back(Troot);

	// ����������еġ�ģʽ�����Ƿ���ʹ�
	unordered_map<size_t, bool> nodeNumVisited;
	nodeNumVisited[fatherID] = true;

	// ���������еġ�ģʽ�����Ľڵ��Ŵ�С��������
	vector<size_t> availableNodeNum;
	for (unordered_map<string, size_t>::const_iterator cit = availableNode.cbegin();
		cit != availableNode.cend(); ++cit)
	{
		availableNodeNum.push_back((*cit).second);
		nodeNumVisited[(*cit).second] = false;
	}
	sort(availableNodeNum.begin(), availableNodeNum.end());

	// �����������еġ�ģʽ���������б�ʾ�������ɵ�����ʽ��1.�㣻2.�ڵ�����IDֵ��
	queue<size_t> queNode;
	queNode.push(fatherID);

	while (!queNode.empty())
	{
		fatherID = queNode.front();
		queNode.pop();

		for(size_t i = 0; i != availableNodeNum.size(); ++i)
		{
			if (!nodeNumVisited[availableNodeNum[i]])
			{
				const Node &nd = Item[availableNodeNum[i]];
				if (nd.availableFather == fatherID)
				{
					TtreeInItemOrder.push_back(nd.uuidByMatchProperty);
					nodeNumVisited[availableNodeNum[i]] = true;
					queNode.push(availableNodeNum[i]);
				}
			}
		}
	}
}


// �ж�ģʽ�����������еġ�ģʽ�����Ľڵ�˳���Ƿ�һ��
inline bool judgeTreeNodeOrder(const vector<Node> &Item, const vector<NodeForT> &Ttree,
	const unordered_map<string, size_t> &availableNode, const vector<string> &TtreeOrder)
{
	//map<size_t, string> TtreeInItemOrder;
	vector<string> TtreeInItemOrder;
	getTtreeInItemOrder(Item, availableNode, TtreeInItemOrder);


	if (TtreeInItemOrder.size() != TtreeOrder.size())
	{
		return false;
	}

	for (size_t i= 0; i != TtreeInItemOrder.size(); ++i)
	{
		if (TtreeInItemOrder[i] != TtreeOrder[i])
		{
			return false;
		}
	}

	return true;
}

// �жϡ�ģʽ�����Ƿ��������������
inline bool judgeTtreeExist(vector<Node> &Item, const vector<NodeForT> &Ttree,
	unordered_map<string, size_t> &availableNode, const vector<string> &TtreeOrder)
{
	// ����������еġ�ģʽ��������ǽڵ㣩
	if (signAvailableNode(Item, Ttree, availableNode))
	{
		// ����������еġ�ģʽ�����ĸ��ڵ�
		updateAvailableNodeFather(Item, availableNode);

		// �ж�ģʽ�����������еġ�ģʽ�����ĸ��ӹ�ϵ�Ƿ�һ�£�ֱ���ų����������еġ�ģʽ������ɭ�ֵ������
		if (judgeRelationshipOfFatherSonEqual(Ttree, Item, availableNode))
		{
			// �ж�ģʽ�����������еġ�ģʽ�����Ľڵ�˳���Ƿ�һ��
			if (judgeTreeNodeOrder(Item, Ttree, availableNode, TtreeOrder))
			{
				return true;
			}
		}
	}
	return false;
}

// ��¼��ģʽ�������������е�λ��
inline void recordTtreePosInItem(const vector<Node> &Item, const size_t nowItemID,
	const unordered_map<string, size_t> &availableNode)
{
	SubTree subTr;
	subTr.itemNo = nowItemID;
	subTr.root = availableNode.at(Troot);
	subTr.rml = availableNode.at(Trml);

	unordered_map<size_t, int> &toFatherID = subTr.toFatherID;

	int fatherID = subTr.rml;
	while (fatherID >= 0)
	{
		toFatherID[static_cast<size_t>(fatherID)] = Item[fatherID].availableFather;
		fatherID = Item[fatherID].availableFather;
	}
	itemToSubTr.push_back(subTr);
}

// ��ͶӰ��֧
inline void getProjectedBranch(const vector<NodeForT> &Ttree, const vector<string> &TtreeOrder)
{
	for (size_t i = 0; i < visitDataPos.size(); )
	{
		vector<Node> &Item = Data[visitDataPos[i]];

		// �������ж�Ӧ�ġ�ģʽ�����Ľڵ㣨���Լ���--��ţ����߾�Ψһ��һһ��Ӧ��
		unordered_map<string, size_t> availableNode;

		// �жϵ�ǰ���������Ƿ���ڡ�ģʽ����
		if (judgeTtreeExist(Item, Ttree, availableNode, TtreeOrder))
		{
			// ��¼�������еġ�ģʽ������λ��
			recordTtreePosInItem(Item, visitDataPos[i], availableNode);
			++i;
		}else
		{
			visitDataPos.erase(visitDataPos.begin() + i);
		}
	}
}

// ���RMB(T)�Ķ��ӽڵ㣨����ͶӰ�ڵ㣩�ؼ����롾ע���ع������������Ŀ�ļ����ظ����롿
inline void getPBnodeRMBchildCore(vector<size_t> &PBnode,
	const vector<Node> &item, const int nowNode, const int standardNo)
{
	const Node &nd = item[nowNode];
	for (size_t i = 0; i != nd.sonNode.size(); ++i)
	{
		if (-1 != standardNo)
		{
			if (item[nd.sonNode[i]].orderID > standardNo)
			{
				PBnode.push_back(nd.sonNode[i]);
			}
		}
		else
		{
			PBnode.push_back(nd.sonNode[i]);
		}
	}
}


// ���RMB(T)�Ķ��ӽڵ㣨����ͶӰ�ڵ��һ���֣�
inline void getPBNodeRMBChild(unordered_map<string, vector<size_t>> &TnodeToPBnode,
	const vector<Node> &item, const SubTree &subTr, unordered_set<string> &notVisitNode)
{
	int nowNode = subTr.rml;
	int standardNo = -1;
	while (nowNode >= 0)
	{
		// ��֦1
		//�����������ܲ���ͶӰ��֧Ƶ���ڵ㡿�Ľڵ㣩
		/*
			����::����3��
				��TΪ���ݼ�TDB�е�Ƶ��������V��RMB(T)��V��TDB��ͶӰ��֧��û��ͶӰ��֧Ƶ���ڵ㣬
				��ģʽ��T��ͨ��T������չ�õ�����������T'���������ڽڵ�V������չ�õ��µ�Ƶ��������
		*/
		const string &nowNodeUuid = item[nowNode].uuidByMatchProperty;
		if (notVisitNode.find(nowNodeUuid) == notVisitNode.end())
		{
			if (TnodeToPBnode.find(nowNodeUuid) == TnodeToPBnode.end())
			{
				vector<size_t> PBnode;
				getPBnodeRMBchildCore(PBnode, item, nowNode, standardNo);
				TnodeToPBnode[nowNodeUuid] = PBnode;
			}
		}

		int nextNode = subTr.toFatherID.at(nowNode);
		if (nextNode >= 0)
		{
			while ((nowNode >= 0) && (nowNode != nextNode))
			{
				standardNo = item[nowNode].orderID;
				nowNode = item[nowNode].father;
			}
		}
		else
		{
			break;
		}
	}
}

// ���RMB(T)�����ֵܽڵ㣨����ͶӰ�ڵ��һ���֣�
inline void getPBNodeRMBRightBrother(unordered_map<string, vector<size_t>> &TnodeToPBnode,
	const vector<Node> &item, const SubTree &subTr)
{
	int nowNode = subTr.rml;
	int fatherNode = item[nowNode].father;
	int standardNo = item[nowNode].orderID;
	int availableFather = item[nowNode].availableFather;


	while ((fatherNode >= 0) && (availableFather >= 0))
	{
		if(fatherNode != availableFather)
		{
			const string &fatherNodeUuid = item[availableFather].uuidByMatchProperty;
			if (TnodeToPBnode.find(fatherNodeUuid) != TnodeToPBnode.end())
			{
				vector<size_t> &PBnode = TnodeToPBnode[fatherNodeUuid];
				getPBnodeRMBchildCore(PBnode, item, fatherNode, standardNo);
			}
		}

		nowNode = item[nowNode].father;
		if (nowNode >= 0)
		{
			fatherNode = item[nowNode].father;
			standardNo = item[nowNode].orderID;

			if (subTr.toFatherID.find(nowNode) != subTr.toFatherID.end())
			{
				availableFather = item[nowNode].availableFather;
			}
		}
		else
		{
			break;
		}
	}
}

// ��ȡһ���ڵ���������Լ���
inline void getOneNodePtySet(size_t num, size_t pos, const
	size_t &K, const Node &nd,
	string strProperty, unsigned long long propertyFlag,
	unordered_map<string, PropertyMeg> &PBnodeToMeg)
{
	if (num == K)
	{
		if (frequentProperty.find(strProperty) != frequentProperty.end())
		{
			if (PBnodeToMeg.find(strProperty) != PBnodeToMeg.end())
			{
				try
				{
					if (PBnodeToMeg[strProperty].propertyFlag != propertyFlag)
					{
						throw runtime_error("���Լ�û�а�Ҫ��˳�����루�������Լ����໥������");
					}
				}
				catch (runtime_error err)
				{
					cerr << "Error: " << __FILE__
						<< " : in function " << __func__
						<< " at line " << __LINE__ << endl
						<< "     Comiled on " << __DATE__
						<< " at " << __TIME__ << endl
						<< err.what() << endl;
				}
				++PBnodeToMeg[strProperty].cnt;

			}
			else
			{
				PBnodeToMeg[strProperty].cnt = 1;
				PBnodeToMeg[strProperty].propertyFlag = propertyFlag;
			}
		}
	}
	else
	{
		for (size_t i = pos; i != toMatchProperties.size(); ++i)
		{
			if (num + (toMatchProperties.size() - i) >= K)
			{
				const string &sigleProperty = nd.properties[toMatchProperties[i]];
				if (frequentProperty.find(sigleProperty) != frequentProperty.end())
				{
					getOneNodePtySet(num + 1, i + 1, K, nd,
						strProperty + sigleProperty,
						propertyFlag | static_cast<unsigned long long>(1 << toMatchProperties[i]),
						PBnodeToMeg);
				}
			}
			else
			{
				break;
			}
		}
	}
}

// ���RMB(T)�Ķ��ӽڵ�����ֵܽڵ�ĺ���ڵ㣨����ͶӰ�ڵ��һ���֣�
/*
���ˣ����ɻ������ͶӰ��֧�ڵ�
*/
inline void getPBNodeRMBGrandson(
	unordered_map<string, unordered_map<string, PropertyMeg>> &TnodeToPBnodeCnt,
	const unordered_map<string, vector<size_t>> &TnodeToPBnode, const vector<Node> &item)
{
	for (unordered_map<string, vector<size_t>>::const_iterator cit = TnodeToPBnode.cbegin();
		cit != TnodeToPBnode.cend(); ++cit)
	{
		if (!(*cit).second.empty())
		{
			const vector<size_t> &PBnode = (*cit).second;
			queue<size_t> toVisited;
			for (size_t i = 0; i != PBnode.size(); ++i)
			{
				toVisited.push(PBnode[i]);
			}

			unordered_map<string, PropertyMeg> PBnodeToMeg;
			while (!toVisited.empty())
			{
				const Node &nd = item[toVisited.front()];
				toVisited.pop();
				for (size_t k = 1; k != toMatchProperties.size() + 1; ++k)
				{
					getOneNodePtySet(0, 0, k, nd, "", 0, PBnodeToMeg);
				}
				for (size_t i = 0; i != nd.sonNode.size(); ++i)
				{
					toVisited.push(nd.sonNode[i]);
				}
			}
			// ͳ��ͶӰ�ڵ�
			if (TnodeToPBnodeCnt.find((*cit).first) != TnodeToPBnodeCnt.end())
			{
				unordered_map<string, PropertyMeg> &pbToMeg_Tmp = TnodeToPBnodeCnt.at((*cit).first);
				for (unordered_map<string, PropertyMeg>::const_iterator cit_PBToMeg = PBnodeToMeg.cbegin();
					cit_PBToMeg != PBnodeToMeg.cend(); ++cit_PBToMeg)
				{
					if (pbToMeg_Tmp.find((*cit_PBToMeg).first) != pbToMeg_Tmp.end())
					{
						pbToMeg_Tmp[(*cit_PBToMeg).first].cnt += (*cit_PBToMeg).second.cnt;
					}
					else
					{
						pbToMeg_Tmp[(*cit_PBToMeg).first] = (*cit_PBToMeg).second;
					}
				}
			}
			else
			{
				TnodeToPBnodeCnt[(*cit).first] = PBnodeToMeg;
			}
		}
	}

}

// ���ͶӰ�ڵ���ִ���
inline void getPBnodeCnt(unordered_map<string, unordered_map<string, PropertyMeg>> &TnodeToPBnodeCnt,
	const vector<Node> &item, const SubTree &subTr, unordered_set<string> &notVisitNode)
{
	unordered_map<string, vector<size_t>> TnodeToPBnode;
	getPBNodeRMBChild(TnodeToPBnode, item, subTr, notVisitNode);
	getPBNodeRMBRightBrother(TnodeToPBnode, item, subTr);
	getPBNodeRMBGrandson(TnodeToPBnodeCnt, TnodeToPBnode, item);
}

// ���ģʽ���ڵ�Ԫ��Ψһ��ʶ
inline void getTtreeNodeUuid(const vector<NodeForT> &Ttree, unordered_map<string, size_t> &TtreeNodeUuid)
{
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		TtreeNodeUuid[Ttree[i].uuidByMatchProperty] = i;
	}
}

// ��á�ģʽ�����Ľڵ�˳���Լ����ҽڵ�
inline void getTtreeOrder(const vector<NodeForT> &Ttree, vector<string> &TtreeOrder)
{

	vector<bool> visited(Ttree.size());
	fill(visited.begin(), visited.end(), false);

	size_t fatherID = 0;
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		if (Ttree[i].father < 0)
		{
			TtreeOrder.push_back(Ttree[i].uuidByMatchProperty);
			visited[i] = true;
			fatherID = Ttree[i].id;
			break;
		}
	}

	queue<size_t> queNode;
	queNode.push(fatherID);

	int rmlNode = fatherID;

	while (!queNode.empty())
	{
		fatherID = queNode.front();
		queNode.pop();
		int rmlTmp = -1;
		for (size_t i = 0; i != Ttree.size(); ++i)
		{
			if (!visited[i])
			{
				const NodeForT &ndT = Ttree[i];
				if (ndT.father == fatherID)
				{
					TtreeOrder.push_back(ndT.uuidByMatchProperty);
					visited[i] = true;
					queNode.push(ndT.id);

					if (rmlNode == fatherID)
					{
						rmlTmp = ndT.id;
					}
				}
			}
		}
		if (-1 != rmlTmp)
		{
			rmlNode = rmlTmp;
		}
	}
	Trml = Ttree[rmlNode].uuidByMatchProperty;
}

// ��ǰ��������ͶӰ��֧������չ
inline void subtreeExpandBasedOnProjectedBranch(vector<NodeForT> &Ttree,
	unordered_set<string> notVisitNodePre)
{
	if (!visitDataPos.empty())
	{
		unordered_map<string, unordered_map<string, PropertyMeg>> TnodeToPBnodeCnt;

		for (size_t i = 0; i != itemToSubTr.size(); ++i)
		{
			SubTree &subTr = itemToSubTr[i];
			vector<Node> &item = Data[subTr.itemNo];
			// ���ڵ�ǰ����ģʽ�����������ϣ���������ͶӰ��֧����ͬ�ڵ���ֵĴ���
			getPBnodeCnt(TnodeToPBnodeCnt, item, subTr, notVisitNodePre);
		}

		unordered_map<string, size_t> TtreeNodeUuid;
		getTtreeNodeUuid(Ttree, TtreeNodeUuid);


		unordered_set<string> notVisitNode(notVisitNodePre);	// ����Ҫ���ʵĽڵ㣨���Ķ���3����֦��
		for (unordered_map<string, unordered_map<string, PropertyMeg>>::const_iterator cit = TnodeToPBnodeCnt.cbegin();
			cit != TnodeToPBnodeCnt.cend(); ++cit)
		{
			bool flag = false;
			const unordered_map<string, PropertyMeg> &PBnodeToMeg = (*cit).second;
			for (unordered_map<string, PropertyMeg>::const_iterator cit_pbToMeg = PBnodeToMeg.cbegin();
				cit_pbToMeg != PBnodeToMeg.cend(); ++cit_pbToMeg)
			{
				if (TtreeNodeUuid.find((*cit_pbToMeg).first) == TtreeNodeUuid.end() &&
					(*cit_pbToMeg).second.cnt >= minsupp)
				{
					flag = true;
					break;
				}
			}
			if (!flag)
			{
				notVisitNode.insert(cit->first);
			}
		}


		for (unordered_map<string, unordered_map<string, PropertyMeg>>::const_iterator cit = TnodeToPBnodeCnt.cbegin();
			cit != TnodeToPBnodeCnt.cend(); ++cit)
		{
			if (notVisitNode.find(cit->first) != notVisitNode.end())
			{
				continue;
			}


			try
			{
				if (TtreeNodeUuid.find((*cit).first) == TtreeNodeUuid.end())
				{
					throw runtime_error("û���ҵ�ͶӰ�ڵ�ĸ��ڵ�");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			size_t pbNodeFatherPos = TtreeNodeUuid[(*cit).first];

			const unordered_map<string, PropertyMeg> &PBnodeToMeg = (*cit).second;

			for (unordered_map<string, PropertyMeg>::const_iterator cit_pbToMeg = PBnodeToMeg.cbegin();
				cit_pbToMeg != PBnodeToMeg.cend(); ++cit_pbToMeg)
			{
				if (TtreeNodeUuid.find((*cit_pbToMeg).first) == TtreeNodeUuid.end() &&
					(*cit_pbToMeg).second.cnt >= minsupp)
				{
					NodeForT ndT;
					ndT.father = pbNodeFatherPos;
					ndT.id = TtreeMeg.nodeCnt;
					ndT.propertyFlag = (*cit_pbToMeg).second.propertyFlag;
					ndT.uuidByMatchProperty = (*cit_pbToMeg).first;

					++TtreeMeg.nodeCnt;


					Ttree.push_back(ndT);

					outputFrequentSubtree(Ttree);
					//calDataNum(frequentSubtreeNum);

					if (!itemToSubTr.empty())
					{
						itemToSubTr.clear();
					}

					vector<string> TtreeOrder;

					string trmlTmp = Trml;

					// ��á�ģʽ�����Ľڵ�˳���Լ����ҽڵ�
					getTtreeOrder(Ttree, TtreeOrder);

					vector<size_t> vtDtPs_Tmp(visitDataPos.begin(), visitDataPos.end());

					// ��ͶӰ��֧
					getProjectedBranch(Ttree, TtreeOrder);

					// ��ǰ��������ͶӰ��֧������չ
					subtreeExpandBasedOnProjectedBranch(Ttree, notVisitNode);

					if (vtDtPs_Tmp.size() != visitDataPos.size())
					{
						visitDataPos.swap(vtDtPs_Tmp);
					}

					Trml = trmlTmp;

					--TtreeMeg.nodeCnt;
					Ttree.erase(Ttree.end() - 1);
				}
			}
		}
	}
}



// ����ͶӰ��֧�Ķ�άƵ�������ھ��㷨
inline void frequentMulpropertySubtreeMiningBasedOnProjectedBranch()
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = frequentProperty.cbegin();
		cit != frequentProperty.cend(); ++cit)
	{
		vector<NodeForT> Ttree;

		TtreeMeg.nodeCnt = 0;

		NodeForT tnd;
		tnd.father = -1;
		tnd.id = TtreeMeg.nodeCnt;
		tnd.propertyFlag = (*cit).second.propertyFlag;
		tnd.uuidByMatchProperty = (*cit).first;

		++TtreeMeg.nodeCnt;

		Ttree.push_back(tnd);

		outputFrequentSubtree(Ttree);
		//calDataNum(frequentSubtreeNum);


		if (!itemToSubTr.empty())
		{
			itemToSubTr.clear();
		}

		vector<string> TtreeOrder;
		// ��á�ģʽ�����Ľڵ�˳��
		getTtreeOrder(Ttree, TtreeOrder);


		// ��á�ģʽ�����ĸ��ڵ�������Ҷ�ڵ�
		Troot = (*cit).first;
		Trml = (*cit).first;

		// ��ʼ��Ҫ���ʵ��������±�
		if (!visitDataPos.empty())
		{
			visitDataPos.clear();
		}
		for (size_t i = 0; i != Data.size(); ++i)
		{
			visitDataPos.push_back(i);
		}

		// ��ͶӰ��֧
		getProjectedBranch(Ttree, TtreeOrder);
		unordered_set<string> notVisitNode;

		// ��ǰ��������ͶӰ��֧������չ
		subtreeExpandBasedOnProjectedBranch(Ttree, notVisitNode);
	}
}

/*************************************** ����ͶӰ��֧�Ķ�άƵ�������ھ� ******************************************/

int main()
{
#ifdef HOME
	time_t startTime, endTime;
	startTime = clock();

	std::freopen("200.txt", "r", stdin);
	std::freopen("200-2-2", "w", stdout);

	//std::freopen("in", "r", stdin);
	//std::freopen("out", "w", stdout);

#endif // HOME

	setPropertyNum();
	setToMatchProperties();
	setMinsupp();
	inputData();
	setDataTreeOrderID();

	getAllFrequentPropertySet();

	frequentMulpropertySubtreeMiningBasedOnProjectedBranch();

	cout << "��άƵ�������ĸ����ǣ�" << frequentSubtreeNum << endl;

#ifdef HOME
	//std::ios::sync_with_stdio(false);
	endTime = clock();
	cout << "Time elapsed: " << (endTime - startTime) << "ms" << endl;
	cout << "Time elapsed: " << clock() / CLOCKS_PER_SEC << "s" << endl;
	fclose(stdout);
	fclose(stdin);
	//fout.close();
	//fin.close();
#endif // HOME

}
